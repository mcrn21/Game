#include "frustum.h"

namespace ae {

Frustum::Frustum()
    : m_points{vec3{0.0f}}
{}

const std::array<vec3, 8> &Frustum::getPoints() const
{
    return m_points;
}

const AABB &Frustum::getAABB() const
{
    return m_aabb;
}

bool Frustum::intersectWithAABB(const AABB &other_aabb) const
{
    for (const auto &plane : m_planes) {
        // Вычисляем самую дальнюю и самую близкую точки
        vec3 positive_vertex = glm::vec3((plane.normal.x > 0) ? other_aabb.max.x : other_aabb.min.x,
                                         (plane.normal.y > 0) ? other_aabb.max.y : other_aabb.min.y,
                                         (plane.normal.z > 0) ? other_aabb.max.z : other_aabb.min.z);

        // Если самая дальняя точка за плоскостью, AABB вне фрустума
        if (glm::dot(plane.normal, positive_vertex) + plane.d < 0)
            return false;
    }

    return true; // AABB внутри фрустума
}

void Frustum::update(const mat4 &proj_view_mat)
{
    auto inv = glm::inverse(proj_view_mat);

    // points
    for (int32_t x = 0; x < 2; ++x) {
        for (int32_t y = 0; y < 2; ++y) {
            for (int32_t z = 0; z < 2; ++z) {
                vec4 pt = inv * vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                m_points[(x * 2 + y) * 2 + z] = pt / pt.w;
            }
        }
    }

    // aabb
    m_aabb.min = vec3{std::numeric_limits<float>::max()};
    m_aabb.max = vec3{std::numeric_limits<float>::lowest()};
    for (int32_t i = 0; i < m_points.size(); ++i) {
        m_aabb.min = glm::min(m_aabb.min, m_points[i]);
        m_aabb.max = glm::max(m_aabb.max, m_points[i]);
    }

    // planes
    // Левая плоскость
    m_planes[0].normal = vec3(proj_view_mat[0][3] + proj_view_mat[0][0],
                              proj_view_mat[1][3] + proj_view_mat[1][0],
                              proj_view_mat[2][3] + proj_view_mat[2][0]);
    m_planes[0].d = proj_view_mat[3][3] + proj_view_mat[3][0];

    // Правая плоскость
    m_planes[1].normal = vec3(proj_view_mat[0][3] - proj_view_mat[0][0],
                              proj_view_mat[1][3] - proj_view_mat[1][0],
                              proj_view_mat[2][3] - proj_view_mat[2][0]);
    m_planes[1].d = proj_view_mat[3][3] - proj_view_mat[3][0];

    // Нижняя плоскость
    m_planes[2].normal = vec3(proj_view_mat[0][3] + proj_view_mat[0][1],
                              proj_view_mat[1][3] + proj_view_mat[1][1],
                              proj_view_mat[2][3] + proj_view_mat[2][1]);
    m_planes[2].d = proj_view_mat[3][3] + proj_view_mat[3][1];

    // Верхняя плоскость
    m_planes[3].normal = vec3(proj_view_mat[0][3] - proj_view_mat[0][1],
                              proj_view_mat[1][3] - proj_view_mat[1][1],
                              proj_view_mat[2][3] - proj_view_mat[2][1]);
    m_planes[3].d = proj_view_mat[3][3] - proj_view_mat[3][1];

    // Передняя плоскость (near)
    m_planes[4].normal = vec3(proj_view_mat[0][3] + proj_view_mat[0][2],
                              proj_view_mat[1][3] + proj_view_mat[1][2],
                              proj_view_mat[2][3] + proj_view_mat[2][2]);
    m_planes[4].d = proj_view_mat[3][3] + proj_view_mat[3][2];

    // Задняя плоскость (far)
    m_planes[5].normal = vec3(proj_view_mat[0][3] - proj_view_mat[0][2],
                              proj_view_mat[1][3] - proj_view_mat[1][2],
                              proj_view_mat[2][3] - proj_view_mat[2][2]);
    m_planes[5].d = proj_view_mat[3][3] - proj_view_mat[3][2];

    // Нормализация плоскостей
    for (auto &plane : m_planes) {
        float length = glm::length(plane.normal);
        plane.normal /= length;
        plane.d /= length;
    }
}

} // namespace ae
