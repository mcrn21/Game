#include "image.h"
#include "../engine.h"
#include "../assets/assets.h"
#include "../system/log.h"

namespace ae::gui {

Image::Image(EngineContext &engine_context)
    : Control{engine_context}
{}

void Image::onStateChanged(int32_t state)
{
    if (state & DEFAULT) {
        l_debug("Image::DEFAULT");
    }

    if (state & HOVERED) {
        l_debug("Image::HOVERED");
    }

    if (state & FOCUSED) {
        l_debug("Image::FOCUSED");
    }

    if (state & PRESSED) {
        l_debug("Image::PRESSED");
    }

    repaint();
}

void Image::onCursorEnter()
{
    l_debug("Image::onCursorEnter");
}

void Image::onCursorLeave()
{
    l_debug("Image::onCursorLeave");
}

void Image::onCursorMoved(const vec2 &position)
{
    // l_debug("Cursor moved: {} x {}", position.x, position.y);
}

void Image::onButtonPressed(ButtonCode button)
{
    l_debug("Image::onButtonPressed");
}

void Image::onButtonReleased(ButtonCode button)
{
    l_debug("Image::onButtonReleased");
}

void Image::drawControl(Batch2D &batch_2d)
{
    batch_2d.drawTextureFrameRect({0.0f, 0.0f},
                                  getSize(),
                                  vec4{12.0f, 12.0f, 64.0f, 12.0f},
                                  getEngineContext().getAssets()->get<Texture>("frame"));

    // batch_2d.drawTextureRect({0.0f, 0.0f},
    //                          {32.0f, 32.0f},
    //                          Application::getInstance().getAssets()->get<Texture>("target"));
}

} // namespace ae::gui
