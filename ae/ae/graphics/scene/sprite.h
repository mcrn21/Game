#ifndef AE_SPRITE_H
#define AE_SPRITE_H

#include "../core/material.h"
#include "../core/vertex_array.h"
#include "drawable.h"

namespace ae {

class Sprite : public Drawable
{
public:
    Sprite();
    Sprite(const vec2 &size,
           const s_ptr<Material> &material = nullptr,
           const ivec4 &texture_rect = ivec4{0});
    ~Sprite() = default;

    s_ptr<Material> getMaterial() const;
    void setMaterial(const s_ptr<Material> &material,
                     const ivec4 &texture_rect = ivec4{0});

    const ivec4 &getTextureRect() const;
    void setTextureRect(const ivec4 &texture_rect);

    const vec2 &getSize() const;
    void setSize(const vec2 &size);

    const AABB &getAABB() const;

    void draw(const RenderState &render_state) const;

private:
    void update();

private:
    s_ptr<Material> m_material;
    ivec4 m_texture_rect;
    vec2 m_size;
    VertexArray m_vertex_array;
    bool m_need_update;
};

} // namespace ae

#endif // AE_SPRITE_H
