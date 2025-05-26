#ifndef AE_GLM_UTILS_H
#define AE_GLM_UTILS_H

#include <glm/glm.hpp>

using namespace glm;

namespace ae::glm_utils {

// Move vec3
template<typename T, typename U>
T moveVec(const T &vec, const U &x, const U &y, const U &z)
{
    return vec + T{x, y, z};
}

template<typename T, typename U>
T moveVecX(const T &vec, const U &x)
{
    return vec + T{x, 0, 0};
}

template<typename T, typename U>
T moveVecY(const T &vec, const U &y)
{
    return vec + T{0, y, 0};
}

template<typename T, typename U>
T moveVecZ(const T &vec, const U &z)
{
    return vec + T{0, 0, z};
}

// Transform vec3
template<typename T>
vec<3, T> transformVec3(const vec<3, T> &v, const mat<4, 4, T> &transform)
{
    vec<4, T> transformed = transform * vec<4, T>(v, static_cast<T>(1));
    return vec<3, T>{transformed};
}

bool decomposeTransform(const mat4 &transform, vec3 &position, vec3 &rotation, vec3 &scale);

} // namespace ae::glm_utils

#endif // AE_GLM_UTILS_H
