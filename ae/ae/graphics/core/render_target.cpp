#include "render_target.h"

#include <GL/glew.h>

namespace ae {

RenderTarget::RenderTarget()
    : m_size{1}
    , m_viewport{1}
    , m_clear_color{Color::black}
{}

const ivec2 &RenderTarget::getSize() const
{
    return m_size;
}

void RenderTarget::setSize(const ivec2 &size)
{
    m_size = size;
    sizeChanged.emit(size);
}

const ivec4 &RenderTarget::getViewport() const
{
    return m_viewport;
}

void RenderTarget::setViewport(const ivec4 &viewport)
{
    m_viewport = viewport;
}

const Color &RenderTarget::getClearColor() const
{
    return m_clear_color;
}

void RenderTarget::setClearColor(const Color &clear_color)
{
    m_clear_color = clear_color;
}

void RenderTarget::clear() const
{
    auto &viewport = getViewport();
    auto clear_color = getClearColor();

    glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
    glClearColor(clear_color.getRed(),
                 clear_color.getGreen(),
                 clear_color.getBlue(),
                 clear_color.getAlpha());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

} // namespace ae
