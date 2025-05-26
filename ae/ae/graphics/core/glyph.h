#ifndef AE_GLYPH_H
#define AE_GLYPH_H

#include <glm/glm.hpp>

using namespace glm;

namespace ae {

struct Glyph
{
    vec2 size{0.0f};
    vec2 offset{0.0f};
    vec2 uv0{0.0f};
    vec2 uv1{0.0f};
    float advance = 0.0f;
};

} // namespace ae

#endif // AE_GLYPH_H
