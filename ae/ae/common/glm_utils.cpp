#include "glm_utils.h"

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

namespace ae::glm_utils {

bool decomposeTransform(const mat4 &transform, vec3 &position, vec3 &rotation, vec3 &scale)
{
    static vec3 skew;
    static vec4 perspective;
    static quat rotation_quat;

    bool result = glm::decompose(transform, scale, rotation_quat, position, skew, perspective);
    if (result)
        rotation = glm::eulerAngles(rotation_quat);
    return result;
}

} // namespace ae::glm_utils
