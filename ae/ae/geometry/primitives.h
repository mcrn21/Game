#ifndef AE_PRIMITIVES_H
#define AE_PRIMITIVES_H

#include <glm/glm.hpp>

using namespace glm;

namespace ae {

struct Interval
{
    float min{0.0f};
    float max{0.0f};
};

struct Line
{
    vec3 start{0.0f};
    vec3 end{0.0f};
};

struct Plane
{
    vec3 normal{1.0f, 0.0f, 0.0f};
    float d = 0.0f;
};

struct Triangle
{
    const vec3 &operator[](int32_t i) const;
    vec3 getNormal() const;

    vec3 v0{0.0f};
    vec3 v1{0.0f};
    vec3 v2{0.0f};
};

struct AABB
{
    AABB() {}
    AABB(const vec3 &min, const vec3 &max)
        : min{min}
        , max{max}
    {}
    ~AABB() = default;

    bool intersects(const AABB &other) const;
    bool intersects(const vec3 &center, float radius) const;
    bool contains(const AABB &other) const;
    bool contains(const vec3 &v) const;

    vec3 getHalfSize() const;
    vec3 getCenter() const;

    AABB extend(float ex) const;
    AABB extend(const vec3 &ex) const;

    AABB move(const vec3 &center) const;
    AABB transform(const mat4 &transform) const;
    AABB merge(const AABB &other) const;
    AABB swept(const vec3 &v) const;

    static const AABB invalid;

    vec3 min{0.0f};
    vec3 max{0.0f};
};

struct Sphere
{
    vec3 center;
    float r = 0.0f;
};

struct Ray
{
    vec3 origin;
    vec3 direction;
};

struct RaycastResult
{
    void reset()
    {
        point = vec3{0.0f};
        normal = vec3{0.0f};
        t = 1.0f;
        hit = false;
    }

    vec3 point{0.0f};
    vec3 normal{0.0f};
    float t = 1.0f;
    bool hit = false;
};

} // namespace ae

#endif // AE_PRIMITIVES_H
