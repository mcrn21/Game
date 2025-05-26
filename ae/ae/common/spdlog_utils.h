#ifndef AE_SPDLOG_UTILS_H
#define AE_SPDLOG_UTILS_H

#include "../geometry/primitives.h"

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

using namespace glm;

// glm
template<>
struct fmt::formatter<vec3> : fmt::formatter<std::string>
{
    auto format(const vec3 &v, format_context &ctx) -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "vec3({}, {}, {})", v.x, v.y, v.z);
    }
};

template<>
struct fmt::formatter<vec2> : fmt::formatter<std::string>
{
    auto format(const vec3 &v, format_context &ctx) -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "vec2({}, {})", v.x, v.y);
    }
};

// ae
template<>
struct fmt::formatter<ae::AABB> : fmt::formatter<std::string>
{
    auto format(const ae::AABB &aabb, format_context &ctx) -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(),
                              "AABB(vec3({}, {}, {}), vec3({}, {}, {}))",
                              aabb.min.x,
                              aabb.min.y,
                              aabb.min.z,
                              aabb.max.x,
                              aabb.max.y,
                              aabb.max.z);
    }
};

#endif // AE_SPDLOG_UTILS_H
