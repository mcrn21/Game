#ifndef AE_RENDER_STATE_H
#define AE_RENDER_STATE_H

#include "shader.h"

#include <glm/glm.hpp>

using namespace glm;

namespace ae {

struct RenderState
{
    Shader *shader = nullptr;
    mat4 transform{1.0f};
};

} // namespace ae

#endif // AE_RENDER_STATE_H
