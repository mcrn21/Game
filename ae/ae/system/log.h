#ifndef AE_LOG_H
#define AE_LOG_H

#include "../geometry/primitives.h"

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

#include <iterator>

using namespace glm;

// glm
template<typename T>
constexpr const char *glm_base_type_name()
{
    if constexpr (std::is_same_v<T, float>)
        return "vec";
    else if constexpr (std::is_same_v<T, int>)
        return "ivec";
    else if constexpr (std::is_same_v<T, double>)
        return "dvec";
    else if constexpr (std::is_same_v<T, uint32_t>)
        return "uvec";
    else
        return "vec"; // fallback
}

template<glm::length_t L, typename T, glm::qualifier Q>
struct fmt::formatter<glm::vec<L, T, Q>>
{
    constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin())
    {
        return ctx.begin(); // No custom format options
    }

    template<typename FormatContext>
    auto format(const glm::vec<L, T, Q> &v, FormatContext &ctx) -> decltype(ctx.out())
    {
        auto out = ctx.out();
        out = fmt::format_to(out, "{}{}(", glm_base_type_name<T>(), L);
        for (glm::length_t i = 0; i < L; ++i) {
            if (i > 0) {
                out = fmt::format_to(out, ", ");
            }
            out = fmt::format_to(out, "{}", v[i]);
        }
        return fmt::format_to(out, ")");
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

namespace ae {

inline void l_debug_mode()
{
    spdlog::set_level(spdlog::level::debug);
}

template<typename... Args>
inline void l_debug(fmt::format_string<Args...> fmt, Args &&...args)
{
    spdlog::debug(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void l_info(fmt::format_string<Args...> fmt, Args &&...args)
{
    spdlog::info(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void l_warn(fmt::format_string<Args...> fmt, Args &&...args)
{
    spdlog::warn(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void l_error(fmt::format_string<Args...> fmt, Args &&...args)
{
    spdlog::error(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void l_critical(fmt::format_string<Args...> fmt, Args &&...args)
{
    spdlog::critical(fmt, std::forward<Args>(args)...);
}

} // namespace ae

#endif // AE_LOG_H
