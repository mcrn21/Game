#ifndef AE_DRAWABLE_H
#define AE_DRAWABLE_H

#include "../../geometry/primitives.h"
#include "../core/render_state.h"

namespace ae {

class Drawable
{
public:
    Drawable();
    virtual ~Drawable() = default;

    virtual const AABB &getAABB() const;
    virtual bool isTransparent() const;
    virtual void draw([[maybe_unused]] const RenderState &render_state) const;
};

} // namespace ae

#endif // AE_DRAWABLE_H
