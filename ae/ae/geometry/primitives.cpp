#include "primitives.h"
#include "../common/glm_utils.h"

#include <stdexcept>

namespace ae {

const vec3 &Triangle::operator[](int32_t i) const
{
    switch (i) {
    case 0:
        return v0;
    case 1:
        return v1;
    case 2:
        return v2;
    default:
        throw std::out_of_range("Invalid index for Triangle vertex");
    }
}

vec3 Triangle::getNormal() const
{
    return glm::normalize(glm::cross(v1 - v0, v2 - v0));
}

const AABB AABB::invalid;

bool AABB::intersects(const AABB &other) const
{
    return (max.x > other.min.x && min.x < other.max.x)
           && (max.y > other.min.y && min.y < other.max.y)
           && (max.z > other.min.z && min.z < other.max.z);
}

bool AABB::intersects(const vec3 &center, float radius) const
{
    float dist_squared = 0.0f;

    for (int i = 0; i < 3; ++i) {
        if (center[i] < min[i])
            dist_squared += (min[i] - center[i]) * (min[i] - center[i]);
        else if (center[i] > max[i])
            dist_squared += (center[i] - max[i]) * (center[i] - max[i]);
    }

    return dist_squared <= radius * radius;
}

bool AABB::contains(const AABB &other) const
{
    return (min.x <= other.min.x && max.x >= other.max.x)
           && (min.y <= other.min.y && max.y >= other.max.y)
           && (min.z <= other.min.z && max.z >= other.max.z);
}

bool AABB::contains(const vec3 &v) const
{
    return v.x >= min.x && v.x <= max.x && v.y >= min.y && v.y <= max.y && v.z >= min.z
           && v.z <= max.z;
}

vec3 AABB::getHalfSize() const
{
    return (max - min) / 2.0f;
}

vec3 AABB::getCenter() const
{
    return (max + min) / 2.0f;
}

AABB AABB::extend(float ex) const
{
    return {min - ex, max + ex};
}

AABB AABB::extend(const vec3 &ex) const
{
    return {min - ex, max + ex};
}

AABB AABB::move(const vec3 &center) const
{
    vec3 half = getHalfSize();
    return {center - half, center + half};
}

AABB AABB::transform(const mat4 &transform) const
{
    vec3 corners[] = {glm_utils::transformVec3(min, transform),
                      glm_utils::transformVec3({min.x, min.y, max.z}, transform),
                      glm_utils::transformVec3({min.x, max.y, min.z}, transform),
                      glm_utils::transformVec3({max.x, min.y, min.z}, transform),

                      glm_utils::transformVec3({max.x, max.y, min.z}, transform),
                      glm_utils::transformVec3({min.x, max.y, max.z}, transform),
                      glm_utils::transformVec3({max.x, min.y, max.z}, transform),
                      glm_utils::transformVec3(max, transform)};

    AABB transformed_aabb{vec3{std::numeric_limits<float>::max()},
                          vec3{std::numeric_limits<float>::lowest()}};

    for (int32_t i = 0; i < 8; ++i) {
        transformed_aabb.min = glm::min(transformed_aabb.min, corners[i]);
        transformed_aabb.max = glm::max(transformed_aabb.max, corners[i]);
    }

    return transformed_aabb;
}

AABB AABB::merge(const AABB &other) const
{
    return {glm::min(min, other.min), glm::max(max, other.max)};
}

AABB AABB::swept(const vec3 &v) const
{
    vec3 new_min = glm::min(min, min + v);
    vec3 new_max = glm::max(max, max + v);
    return AABB(new_min, new_max);
}

} // namespace ae
