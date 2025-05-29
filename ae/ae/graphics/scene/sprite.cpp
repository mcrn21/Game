#include "sprite.h"
#include "../core/color.h"
#include "../core/vertex.h"

namespace ae {

Sprite::Sprite()
    : m_need_update{false}
{}

Sprite::Sprite(const vec2 &size,
               const s_ptr<Material> &material,
               const ivec4 &texture_rect)
    : m_size{size}
    , m_material{material}
    , m_texture_rect{texture_rect}
    , m_need_update{true}
{}

s_ptr<Material> Sprite::getMaterial() const
{
    return m_material;
}

void Sprite::setMaterial(const s_ptr<Material> &material, const ivec4 &texture_rect)
{
    m_material = material;
    m_texture_rect = texture_rect;
    m_need_update = true;
}

const ivec4 &Sprite::getTextureRect() const
{
    return m_texture_rect;
}

void Sprite::setTextureRect(const ivec4 &texture_rect)
{
    m_texture_rect = texture_rect;
    m_need_update = true;
}

const vec2 &Sprite::getSize() const
{
    return m_size;
}

void Sprite::setSize(const vec2 &size)
{
    m_size = size;
    m_need_update = true;
}

const AABB &Sprite::getAABB() const {}

void Sprite::draw(const RenderState &render_state) const
{
    if (m_need_update)
        const_cast<Sprite *>(this)->update();

    if (m_material && m_material->diffuse_texture->isValid() && m_vertex_array.isValid()) {
        Texture::bind(*m_material->diffuse_texture);
        m_vertex_array.draw();
        Texture::unbind();
    }
}

void Sprite::update()
{
    m_need_update = false;

    m_vertex_array.destroy();

    vec4 uv;
    if (m_texture_rect.z == 0 || m_texture_rect.w == 0) {
        uv.x = 0.0f;
        uv.y = 0.0f;
        uv.z = 1.0f;
        uv.w = 1.0f;
    } else if (m_material && m_material->diffuse_texture) {
        uv = m_material->diffuse_texture->getUVRect(m_texture_rect);
    }

    std::vector<Vertex> verticies
        = {{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, Color::white.getColor(), {uv.x, uv.w}},
           {{m_size.x, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, Color::white.getColor(), {uv.z, uv.w}},
           {{m_size.x, m_size.y, 0.0f}, {0.0f, 0.0f, -1.0f}, Color::white.getColor(), {uv.z, uv.y}},
           {{0.0f, m_size.y, 0.0f}, {0.0f, 0.0f, -1.0f}, Color::white.getColor(), {uv.x, uv.y}}};

    std::vector<uint32_t> indicies = {0, 1, 3, 1, 2, 3};

    m_vertex_array.create(verticies, indicies);
}

} // namespace ae
