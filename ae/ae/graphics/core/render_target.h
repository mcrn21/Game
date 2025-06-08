#ifndef AE_RENDER_TARGET_H
#define AE_RENDER_TARGET_H

#include "../../system/signal.h"
#include "color.h"

#include <glm/glm.hpp>

using namespace glm;

namespace ae {

class RenderTarget
{
public:
    RenderTarget();
    virtual ~RenderTarget() = default;

    const ivec2 &getSize() const;
    void setSize(const ivec2 &size);

    const ivec4 &getViewport() const;
    void setViewport(const ivec4 &viewport);

    const Color &getClearColor() const;
    void setClearColor(const Color &clear_color);

    virtual void clear() const;
    virtual void display() const = 0;

public:
    Signal<const ivec2 &> sizeChanged;

private:
    ivec2 m_size;
    ivec4 m_viewport;
    Color m_clear_color;
};

} // namespace ae

#endif // AE_RENDER_TARGET_H
