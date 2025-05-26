#ifndef AE_CONSTS_H
#define AE_CONSTS_H

#include "../geometry/frustum.h"

#include <glm/glm.hpp>

using namespace glm;

namespace ae {

const Frustum INVALID_FRUSTUM;
constexpr mat4 INVALID_TRANSFORM{1.0f};
constexpr vec3 INVALID_VEC3{0.0f};

} // namespace ae

#endif // AE_CONSTS_H
