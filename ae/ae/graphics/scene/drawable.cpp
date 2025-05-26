#include "drawable.h"

namespace ae {

Drawable::Drawable() {}

const AABB &Drawable::getAABB() const
{
    static AABB aabb;
    return aabb;
}

bool Drawable::isTransparent() const
{
    return false;
}

void Drawable::draw(const RenderState &render_state) const {}

} // namespace ae
