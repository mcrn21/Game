#include "collisions.h"
#include "../geometry/geometry_utils.h"
#include "../system/log.h"

#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/norm.hpp>

#include <execution>

namespace ae {

std::queue<const TrianglesNode *> Collisions::m_queue;
std::mutex Collisions::m_result_mutex;

void Collisions::test(const Collider *collider1,
                      const vec3 &collider_1_position,
                      const Collider *collider2,
                      CollisionResult &result)
{
    if (collider1->type == Collider::BOX && collider2->type == Collider::MESH) {
        auto *box_collider = static_cast<const BoxCollider *>(collider1);
        auto *mesh_collider = static_cast<const MeshCollider *>(collider2);

        auto aabb = box_collider->aabb.move(collider_1_position);

        aabbVsTriangleNode(aabb, *mesh_collider->root, result);
    }
}

void Collisions::test(const Collider *collider1,
                      const vec3 &start_position,
                      const vec3 &velocity,
                      const Collider *collider2,
                      CollisionResult &result)
{
    if (collider1->type == Collider::BOX && collider2->type == Collider::MESH) {
        auto *box_collider = static_cast<const BoxCollider *>(collider1);
        auto *mesh_collider = static_cast<const MeshCollider *>(collider2);

        auto aabb = box_collider->aabb.move(start_position);
        auto swept_aabb = aabb.swept(velocity);

        sweptAABBVsTriangleNode(aabb, swept_aabb, velocity, *mesh_collider->root, result);
    }
}

void Collisions::aabbVsTriangleNode(const AABB &aabb,
                                    const TrianglesNode &root,
                                    CollisionResult &result)
{
    m_queue = std::queue<const TrianglesNode *>();
    m_queue.push(&root);

    while (!m_queue.empty()) {
        const TrianglesNode *node = m_queue.front();
        m_queue.pop();

        if (!node->aabb.intersects(aabb))
            continue;

        if (node->isLeaf()) {
            std::for_each(std::execution::par_unseq,
                          node->triangles.begin(),
                          node->triangles.end(),
                          [&](const Triangle &triangle) {
                              thread_local CollisionResult temp;

                              // Сброс, т.к. 1 переменная на весь поток.
                              // Могут быть остаточные данные.
                              temp.reset();
                              aabbVsTriangle(aabb, triangle, temp);

                              if (temp.hit) {
                                  std::scoped_lock lock{m_result_mutex};
                                  if (temp.depth < result.depth)
                                      result = temp;
                              }
                          });

        } else {
            if (node->left)
                m_queue.push(node->left.get());
            if (node->right)
                m_queue.push(node->right.get());
        }
    }
}

void Collisions::aabbVsTriangle(const AABB &aabb, const Triangle &triangle, CollisionResult &result)
{
    // Оси AABB
    const std::array<vec3, 3> aabb_axes = {vec3{1.0f, 0.0f, 0.0f},
                                           vec3{0.0f, 1.0f, 0.0f},
                                           vec3{0.0f, 0.0f, 1.0f}};

    // Ребра треугольника
    const std::array<vec3, 3> triangle_edges = {triangle.v1 - triangle.v0,
                                                triangle.v2 - triangle.v1,
                                                triangle.v0 - triangle.v2};

    // Нормаль треугольник
    vec3 triangle_normal = glm::cross(triangle.v1 - triangle.v0, triangle.v2 - triangle.v0);

    // Tреугольник вырожден
    if (glm::length2(triangle_normal) < 1e-6f)
        return;

    triangle_normal = glm::normalize(triangle_normal);

    float min_overlap = std::numeric_limits<float>::max();
    vec3 best_axis;

    auto test = [&](const vec3 &axis) -> bool {
        if (glm::length2(axis) < 1e-6f)
            return true;

        vec3 n_axis = glm::normalize(axis);

        auto proj_aabb = geometry_utils::projectAABBOnAxis(aabb, n_axis);
        auto proj_tri = geometry_utils::projectTriangleOnAxis(triangle, n_axis);

        float overlap = std::min(proj_aabb.max, proj_tri.max)
                        - std::max(proj_aabb.min, proj_tri.min);

        if (overlap < 0.0f)
            return false; // разделяющая ось найдена

        if (overlap < min_overlap) {
            min_overlap = overlap;
            best_axis = n_axis;
        }

        return true;
    };

    // Проверка по осям AABB
    for (int32_t i = 0; i < aabb_axes.size(); ++i) {
        if (!test(aabb_axes[i]))
            return;
    }

    // Проверка по нормали треугольника
    if (!test(triangle_normal))
        return;

    // Проверка перекрестных произведений
    for (int32_t i = 0; i < 3; ++i) {
        for (int32_t j = 0; j < 3; ++j) {
            if (!test(glm::cross(triangle_edges[i], aabb_axes[j])))
                return;
        }
    }

    result.hit = true;

    vec3 aabb_center = aabb.getCenter();
    vec3 tri_сenter = (triangle.v0 + triangle.v1 + triangle.v2) / 3.0f;
    vec3 closest_point = geometry_utils::closestPointOnTriangle(triangle, aabb_center);

    // Проверяем, не является ли best_axis вырожденным или некорректным
    vec3 contact_normal = best_axis;
    if (!glm::all(glm::isfinite(best_axis)) || glm::length2(best_axis) < 1e-6f) {
        // Fallback на нормаль треугольника
        contact_normal = triangle_normal;
    } else
        contact_normal = glm::normalize(best_axis);

    if (glm::dot(contact_normal, (aabb_center - tri_сenter)) < 0.0f)
        contact_normal = -contact_normal;

    result.normal = contact_normal;
    result.point = closest_point;

    auto plane = geometry_utils::planeFromPointNormal(closest_point, contact_normal);
    auto p_plane = geometry_utils::closestPointOnPlane(plane, aabb_center);
    float length = std::abs(glm::dot(contact_normal, aabb.getHalfSize()))
                   - glm::length(aabb_center - p_plane);

    vec3 half_size = aabb.getHalfSize();
    float projected_extent = std::abs(plane.normal.x) * half_size.x
                             + std::abs(plane.normal.y) * half_size.y
                             + std::abs(plane.normal.z) * half_size.z;

    result.depth = projected_extent - glm::length(aabb_center - p_plane);
    result.point = closest_point;

    result.v0 = triangle.v0;
    result.v1 = triangle.v1;
    result.v2 = triangle.v2;
}

void Collisions::sweptAABBVsTriangleNode(const AABB &aabb,
                                         const AABB &swept_aabb,
                                         const vec3 &velocity,
                                         const TrianglesNode &root,
                                         CollisionResult &result)
{
    m_queue = std::queue<const TrianglesNode *>();
    m_queue.push(&root);

    while (!m_queue.empty()) {
        const TrianglesNode *node = m_queue.front();
        m_queue.pop();

        if (!node->aabb.intersects(swept_aabb))
            continue;

        if (node->isLeaf()) {
            std::for_each(std::execution::par_unseq,
                          node->triangles.begin(),
                          node->triangles.end(),
                          [&](const Triangle &triangle) {
                              thread_local CollisionResult temp;

                              // Сброс, т.к. 1 переменная на весь поток.
                              // Могут быть остаточные данные.
                              temp.reset();
                              sweptAABBVsTriangle(aabb, velocity, triangle, temp);

                              if (temp.hit) {
                                  std::scoped_lock lock{m_result_mutex};
                                  if (temp.toi < result.toi)
                                      result = temp;
                              }
                          });

        } else {
            if (node->left)
                m_queue.push(node->left.get());
            if (node->right)
                m_queue.push(node->right.get());
        }
    }
}

void Collisions::sweptAABBVsTriangle(const AABB &aabb,
                                     const vec3 &velocity,
                                     const Triangle &triangle,
                                     CollisionResult &result)
{
    // Оси AABB
    const std::array<vec3, 3> aabb_axes = {vec3{1.0f, 0.0f, 0.0f},
                                           vec3{0.0f, 1.0f, 0.0f},
                                           vec3{0.0f, 0.0f, 1.0f}};

    // Ребра треугольника
    const std::array<vec3, 3> triangle_edges = {triangle.v1 - triangle.v0,
                                                triangle.v2 - triangle.v1,
                                                triangle.v0 - triangle.v2};

    // Нормаль треугольник
    vec3 triangle_normal = glm::cross(triangle.v1 - triangle.v0, triangle.v2 - triangle.v0);

    // Tреугольник вырожден
    if (glm::length2(triangle_normal) < 1e-6f)
        return;

    triangle_normal = glm::normalize(triangle_normal);

    float min_overlap = std::numeric_limits<float>::max();
    vec3 best_axis{0.0f};
    float global_t_enter = 0.0f;
    float global_t_exit = 1.0f;

    auto test = [&](const vec3 &axis) -> bool {
        if (glm::length2(axis) < 1e-6f)
            return true;

        vec3 n_axis = glm::normalize(axis);

        auto proj_aabb = geometry_utils::projectAABBOnAxis(aabb, n_axis);
        auto proj_tri = geometry_utils::projectTriangleOnAxis(triangle, n_axis);

        float v = glm::dot(velocity, n_axis);
        float t_enter, t_exit;

        if (std::abs(v) < 1e-6f) {
            // Объекты статичны вдоль этой оси
            if (proj_aabb.max < proj_tri.min || proj_tri.max < proj_aabb.min)
                return false; // Нет перекрытия

            return true;
        }

        // Вычисляем время входа и выхода вдоль оси
        float t0 = (proj_tri.min - proj_aabb.max) / v;
        float t1 = (proj_tri.max - proj_aabb.min) / v;

        t_enter = std::min(t0, t1);
        t_exit = std::max(t0, t1);

        // Обновление глобального окна времени столкновения
        if (t_enter > global_t_enter) {
            global_t_enter = t_enter;
            best_axis = n_axis; // ось с максимальным t_enter — ось столкновения
        }

        global_t_exit = std::min(global_t_exit, t_exit);

        if (global_t_enter > global_t_exit || global_t_exit < 0.0f)
            return false; // окна не пересекаются

        return true;
    };

    // Проверка по осям AABB
    for (int32_t i = 0; i < aabb_axes.size(); ++i) {
        if (!test(aabb_axes[i]))
            return;
    }

    // Проверка по нормали треугольника
    if (!test(triangle_normal))
        return;

    // Проверка перекрестных произведений
    for (int32_t i = 0; i < 3; ++i) {
        for (int32_t j = 0; j < 3; ++j) {
            if (!test(glm::cross(triangle_edges[i], aabb_axes[j])))
                return;
        }
    }

    result.hit = true;

    vec3 aabb_center = aabb.getCenter();
    vec3 moved_aabb_center = aabb_center + velocity * global_t_enter;
    vec3 contact_approx = moved_aabb_center - result.normal * 0.001f;
    vec3 closest_point = geometry_utils::closestPointOnTriangle(triangle, contact_approx);

    // Проверяем, не является ли best_axis вырожденным или некорректным
    vec3 contact_normal = triangle_normal;
    if (!glm::all(glm::isfinite(best_axis)) || glm::length2(best_axis) < 1e-6f) {
        // Fallback на нормаль треугольника
        contact_normal = triangle_normal;
    } else
        contact_normal = glm::normalize(best_axis);

    if (glm::dot(contact_normal, velocity) > 0.0f)
        contact_normal = -contact_normal;

    result.normal = contact_normal;
    result.toi = global_t_enter;
    result.point = closest_point;

    auto plane = geometry_utils::planeFromPointNormal(closest_point, contact_normal);
    auto p_plane = geometry_utils::closestPointOnPlane(plane, aabb_center);

    vec3 half_size = aabb.getHalfSize();
    float projected_extent = std::abs(plane.normal.x) * half_size.x
                             + std::abs(plane.normal.y) * half_size.y
                             + std::abs(plane.normal.z) * half_size.z;

    result.depth = projected_extent - glm::length(aabb_center - p_plane);

    result.v0 = triangle.v0;
    result.v1 = triangle.v1;
    result.v2 = triangle.v2;
}

void Collisions::raycast(const Ray &ray, const Collider *collider, RaycastResult &result)
{
    if (collider->type == Collider::MESH) {
        auto *mesh_collider = static_cast<const MeshCollider *>(collider);
        rayVsTriangleNode(ray, *mesh_collider->root, result);
    }
}

void Collisions::rayVsTriangleNode(const Ray &ray, const TrianglesNode &root, RaycastResult &result)
{
    m_queue = std::queue<const TrianglesNode *>();
    m_queue.push(&root);

    while (!m_queue.empty()) {
        const TrianglesNode *node = m_queue.front();
        m_queue.pop();

        if (!rayVsAABB(ray, node->aabb))
            continue;

        if (node->isLeaf()) {
            std::for_each(std::execution::par_unseq,
                          node->triangles.begin(),
                          node->triangles.end(),
                          [&](const Triangle &triangle) {
                              thread_local RaycastResult temp;

                              // Сброс, т.к. 1 переменная на весь поток.
                              // Могут быть остаточные данные.
                              temp.reset();
                              rayVsTriangle(ray, triangle, temp);

                              if (temp.hit) {
                                  std::scoped_lock lock{m_result_mutex};
                                  if (temp.t < result.t)
                                      result = temp;
                              }
                          });

        } else {
            if (node->left)
                m_queue.push(node->left.get());
            if (node->right)
                m_queue.push(node->right.get());
        }
    }
}

void Collisions::rayVsTriangle(const Ray &ray, const Triangle &triangle, RaycastResult &result)
{
    constexpr float EPSILON = 1e-6f;
    const vec3 &orig = ray.origin;
    const vec3 &dir = ray.direction;

    vec3 v0_v1 = triangle.v1 - triangle.v0;
    vec3 v0_v2 = triangle.v2 - triangle.v0;

    vec3 pvec = glm::cross(dir, v0_v2);
    float det = glm::dot(v0_v1, pvec);

    // Отбрасываем задние грани, если нужно (односторонний треугольник)
    if (det < EPSILON) {
        result.hit = false;
        return;
    }

    float inv_det = 1.0f / det;

    vec3 tvec = orig - triangle.v0;
    float u = glm::dot(tvec, pvec) * inv_det;
    if (u < 0.0f || u > 1.0f) {
        result.hit = false;
        return;
    }

    vec3 qvec = glm::cross(tvec, v0_v1);
    float v = glm::dot(dir, qvec) * inv_det;
    if (v < 0.0f || u + v > 1.0f) {
        result.hit = false;
        return;
    }

    float t = glm::dot(v0_v2, qvec) * inv_det;
    if (t < 0.0f) {
        result.hit = false;
        return; // треугольник позади луча
    }

    result.hit = true;
    result.t = t;
    result.point = ray.origin + t * ray.direction;
    result.normal = glm::normalize(glm::cross(v0_v1, v0_v2));
}

bool Collisions::rayVsAABB(const Ray &ray, const AABB &aabb)
{
    const vec3 inv_dir = 1.0f / ray.direction;

    vec3 t1 = (aabb.min - ray.origin) * inv_dir;
    vec3 t2 = (aabb.max - ray.origin) * inv_dir;

    vec3 tmin = glm::min(t1, t2);
    vec3 tmax = glm::max(t1, t2);

    float t_near = glm::compMax(tmin);
    float t_far = glm::compMin(tmax);

    if (t_near > t_far || t_far < 0.0f)
        return false;

    return true;
}

} // namespace ae
