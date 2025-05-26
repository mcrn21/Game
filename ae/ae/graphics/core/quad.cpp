#include "quad.h"
#include "color.h"
#include "default_shaders.h"
#include "vertex.h"

namespace ae {

Quad::Quad() {}

void Quad::create()
{
    std::vector<Vertex2D> verticies
        = {Vertex2D{vec2{-1.0f, -1.0f}, Color::white.getColor(), vec2{0.0f, 0.0f}},
           Vertex2D{vec2{1.0f, -1.0f}, Color::white.getColor(), vec2{1.0f, 0.0f}},
           Vertex2D{vec2{1.0f, 1.0f}, Color::white.getColor(), vec2{1.0f, 1.0f}},
           Vertex2D{vec2{-1.0f, 1.0f}, Color::white.getColor(), vec2{0.0f, 1.0f}}};

    std::vector<uint32_t> indicies = {0, 1, 3, 1, 2, 3};

    m_vertex_array.create(verticies, indicies);
}

bool Quad::isValid() const
{
    return m_vertex_array.isValid();
}

void Quad::destroy()
{
    m_vertex_array.destroy();
}

void Quad::draw(const Texture &texture) const
{
    Shader::use(*DefaultShaders::getScreenQuad());
    glDisable(GL_DEPTH_TEST);
    Texture::bind(texture);
    m_vertex_array.draw();
    Texture::unbind();
    glEnable(GL_DEPTH_TEST);
    Shader::unuse();
}

} // namespace ae
