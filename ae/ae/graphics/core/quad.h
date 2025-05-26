#ifndef AE_QUAD_H
#define AE_QUAD_H

#include "texture.h"
#include "vertex_array.h"

#include <glm/glm.hpp>

using namespace glm;

namespace ae {

class Quad
{
public:
    Quad();
    ~Quad() = default;

    void create();
    bool isValid() const;
    void destroy();

    void draw(const Texture &texture) const;

private:
    VertexArray m_vertex_array;
};

} // namespace ae

#endif // AE_QUAD_H
