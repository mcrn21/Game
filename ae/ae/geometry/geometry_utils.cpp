#include "geometry_utils.h"

#include <algorithm>

namespace ae::geometry_utils {

AABB aabbFromSphere(const Sphere &sphere)
{
    return {sphere.center - sphere.r, sphere.center + sphere.r};
}

AABB aabbFromTriangles(const std::vector<Triangle> &triangles)
{
    vec3 min(std::numeric_limits<float>::max());
    vec3 max(std::numeric_limits<float>::lowest());

    for (const auto &t : triangles) {
        for (auto &v : {t.v0, t.v1, t.v2}) {
            min = glm::min(min, v);
            max = glm::max(max, v);
        }
    }

    return {min - 0.01f, max + 0.01f};
}

std::unique_ptr<TrianglesNode> buildTrianglesTree(const std::vector<Triangle> &triangles, int depth)
{
    if (triangles.empty())
        return nullptr;

    auto node = std::make_unique<TrianglesNode>();

    node->aabb = aabbFromTriangles(triangles);

    if (triangles.size() <= 4 || depth > 16) {
        node->triangles = triangles;
        return node;
    }

    int32_t axis = largestAxis(node->aabb.max - node->aabb.min);
    auto sorted = triangles;
    std::sort(sorted.begin(), sorted.end(), [axis](const Triangle &a, const Triangle &b) {
        float ca = (a.v0[axis] + a.v1[axis] + a.v2[axis]) / 3.0f;
        float cb = (b.v0[axis] + b.v1[axis] + b.v2[axis]) / 3.0f;
        return ca < cb;
    });

    int32_t mid = sorted.size() / 2;
    std::vector<Triangle> left_triangles(sorted.begin(), sorted.begin() + mid);
    std::vector<Triangle> right_triangles(sorted.begin() + mid, sorted.end());

    node->left = buildTrianglesTree(left_triangles, depth + 1);
    node->right = buildTrianglesTree(right_triangles, depth + 1);

    return node;
}

int32_t largestAxis(const vec3 &v)
{
    if (v.x > v.y && v.x > v.z)
        return 0;
    if (v.y > v.z)
        return 1;
    return 2;
}

float magnitudeSq(const vec3 &v)
{
    return glm::dot(v, v);
}

Interval projectAABBOnAxis(const AABB &aabb, const vec3 &axis)
{
    vec3 corners[8] = {{aabb.min.x, aabb.min.y, aabb.min.z},
                       {aabb.min.x, aabb.min.y, aabb.max.z},
                       {aabb.min.x, aabb.max.y, aabb.min.z},
                       {aabb.min.x, aabb.max.y, aabb.max.z},
                       {aabb.max.x, aabb.min.y, aabb.min.z},
                       {aabb.max.x, aabb.min.y, aabb.max.z},
                       {aabb.max.x, aabb.max.y, aabb.min.z},
                       {aabb.max.x, aabb.max.y, aabb.max.z}};

    float min_proj = glm::dot(corners[0], axis);
    float max_proj = min_proj;

    for (int32_t i = 1; i < 8; ++i) {
        float proj = glm::dot(corners[i], axis);
        min_proj = std::min(min_proj, proj);
        max_proj = std::max(max_proj, proj);
    }

    return {min_proj, max_proj};
}

Interval projectTriangleOnAxis(const Triangle &triangle, const vec3 &axis)
{
    float min_proj = glm::dot(triangle.v0, axis);
    float max_proj = min_proj;

    float proj1 = glm::dot(triangle.v1, axis);
    float proj2 = glm::dot(triangle.v2, axis);

    min_proj = std::min(min_proj, std::min(proj1, proj2));
    max_proj = std::max(max_proj, std::max(proj1, proj2));

    return {min_proj, max_proj};

    // float min_proj = glm::dot(triangle.v0, axis);
    // float max_proj = min_proj;

    // for (int32_t i = 1; i < 3; ++i) {
    //     float proj = glm::dot(axis, triangle[i]);
    //     min_proj = std::min(min_proj, proj);
    //     max_proj = std::max(max_proj, proj);
    // }

    // return {min_proj, max_proj};
}

vec3 projectPointOnTrianglePlane(const Triangle &triangle, const vec3 &p)
{
    Plane plane = planeFromTriangle(triangle);
    return closestPointOnPlane(plane, p);
}

Plane planeFromTriangle(const Triangle &triangle)
{
    Plane result;
    result.normal = glm::normalize(glm::cross(triangle.v1 - triangle.v0, triangle.v2 - triangle.v0));
    result.d = glm::dot(result.normal, triangle.v0);
    return result;
}

Plane planeFromPointNormal(const vec3 &point, const vec3 &normal)
{
    Plane plane;
    plane.normal = glm::normalize(normal);
    plane.d = glm::dot(plane.normal, point);
    return plane;
}

bool pointInTriangle(const Triangle &triangle, const vec3 &p)
{
    // Move the triangle so that the point is
    // now at the origin of the triangle
    vec3 a = triangle.v0 - p;
    vec3 b = triangle.v1 - p;
    vec3 c = triangle.v2 - p;

    // The point should be moved too, so they are both
    // relative, but because we don't use p in the
    // equation anymore, we don't need it!
    // p -= p; // This would just equal the zero vector!

    vec3 norm_pbc = glm::cross(b, c); // Normal of PBC (u)
    vec3 norm_pca = glm::cross(c, a); // Normal of PCA (v)
    vec3 norm_pab = glm::cross(a, b); // Normal of PAB (w)

    // Test to see if the normals are facing
    // the same direction, return false if not
    if (glm::dot(norm_pbc, norm_pca) < 0.0f)
        return false;
    else if (glm::dot(norm_pbc, norm_pab) < 0.0f)
        return false;

    // All normals facing the same way, return true
    return true;
}

vec3 closestPointOnLine(const Line &line, const vec3 &p)
{
    // Line Vector
    vec3 l_vec = line.end - line.start;

    // Project "point" onto the "Line Vector", computing:
    // closest(t) = start + t * (end - start)
    // T is how far along the line the projected point is
    float t = glm::dot(p - line.start, l_vec) / glm::dot(l_vec, l_vec);

    // Clamp t to the 0 to 1 range
    t = std::max(t, 0.0f);
    t = std::min(t, 1.0f);

    // Return projected position of t
    return line.start + l_vec * t;
}

vec3 closestPointOnTriangle(const Triangle &triangle, const vec3 &p)
{
    Plane plane = planeFromTriangle(triangle);
    vec3 closest = closestPointOnPlane(plane, p);

    // Closest point was inside triangle
    if (pointInTriangle(triangle, closest))
        return closest;

    vec3 c1 = closestPointOnLine(Line(triangle.v0, triangle.v1), closest); // Line AB
    vec3 c2 = closestPointOnLine(Line(triangle.v1, triangle.v2), closest); // Line BC
    vec3 c3 = closestPointOnLine(Line(triangle.v2, triangle.v0), closest); // Line CA

    float mag_sq_1 = magnitudeSq(closest - c1);
    float mag_sq_2 = magnitudeSq(closest - c2);
    float mag_sq_3 = magnitudeSq(closest - c3);

    if (mag_sq_1 < mag_sq_2 && mag_sq_1 < mag_sq_3)
        return c1;
    else if (mag_sq_2 < mag_sq_1 && mag_sq_2 < mag_sq_3)
        return c2;

    return c3;
}

vec3 closestPointOnPlane(const Plane &plane, const vec3 &p)
{
    float distance = glm::dot(plane.normal, p) - plane.d;
    return p - plane.normal * distance;
}

} // namespace ae::geometry_utils
