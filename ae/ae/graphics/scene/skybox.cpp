#include "skybox.h"

namespace ae {

Skybox::Skybox() {}

const std::shared_ptr<Texture> &Skybox::getTexture() const
{
    return m_texture;
}

void Skybox::setTexture(const std::shared_ptr<Texture> &texture)
{
    m_texture = texture;
}

void Skybox::create(const std::shared_ptr<Texture> &texture)
{
    static const std::vector<SkyboxVertex> vertices = {
        // positions (x, y, z)
        {{-1.0f, 1.0f, -1.0f}},  // 0 — левый верхний зад
        {{-1.0f, -1.0f, -1.0f}}, // 1 — левый нижний зад
        {{1.0f, -1.0f, -1.0f}},  // 2 — правый нижний зад
        {{1.0f, 1.0f, -1.0f}},   // 3 — правый верхний зад
        {{-1.0f, 1.0f, 1.0f}},   // 4 — левый верхний перед
        {{-1.0f, -1.0f, 1.0f}},  // 5 — левый нижний перед
        {{1.0f, -1.0f, 1.0f}},   // 6 — правый нижний перед
        {{1.0f, 1.0f, 1.0f}},    // 7 — правый верхний перед
    };

    static const std::vector<uint32_t> indices = {// Задняя грань (z = -1)
                                                  0,
                                                  2,
                                                  1,
                                                  0,
                                                  3,
                                                  2,

                                                  // Передняя грань (z = +1)
                                                  4,
                                                  5,
                                                  6,
                                                  4,
                                                  6,
                                                  7,

                                                  // Левая грань (x = -1)
                                                  4,
                                                  1,
                                                  5,
                                                  4,
                                                  0,
                                                  1,

                                                  // Правая грань (x = +1)
                                                  3,
                                                  6,
                                                  2,
                                                  3,
                                                  7,
                                                  6,

                                                  // Верхняя грань (y = +1)
                                                  4,
                                                  3,
                                                  0,
                                                  4,
                                                  7,
                                                  3,

                                                  // Нижняя грань (y = -1)
                                                  1,
                                                  6,
                                                  5,
                                                  1,
                                                  2,
                                                  6};

    m_vertex_array.create(vertices, indices);
    m_texture = texture;
}

bool Skybox::isValid() const
{
    return m_vertex_array.isValid() && m_texture != nullptr;
}

void Skybox::destroy()
{
    m_vertex_array.destroy();
    m_texture = nullptr;
}

void Skybox::draw() const
{
    if (isValid())
        m_vertex_array.draw();
}

} // namespace ae
