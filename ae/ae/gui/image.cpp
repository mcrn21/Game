#include "image.h"
#include "../app.h"

#include <spdlog/spdlog.h>

namespace ae::gui {

Image::Image()
    : Control{}
{}

void Image::onStateChanged(int32_t state)
{
    if (state & DEFAULT) {
        spdlog::debug("Image::DEFAULT");
    }

    if (state & HOVERED) {
        spdlog::debug("Image::HOVERED");
    }

    if (state & FOCUSED) {
        spdlog::debug("Image::FOCUSED");
    }

    if (state & PRESSED) {
        spdlog::debug("Image::PRESSED");
    }

    repaint();
}

void Image::onCursorEnter()
{
    spdlog::debug("Image::onCursorEnter");
}

void Image::onCursorLeave()
{
    spdlog::debug("Image::onCursorLeave");
}

void Image::onCursorMoved(const vec2 &position)
{
    // spdlog::debug("Cursor moved: {} x {}", position.x, position.y);
}

void Image::onButtonPressed(ButtonCode button)
{
    spdlog::debug("Image::onButtonPressed");
}

void Image::onButtonReleased(ButtonCode button)
{
    spdlog::debug("Image::onButtonReleased");
}

void Image::drawControl(Batch2D &batch_2d)
{
    batch_2d.drawTextureFrameRect({0.0f, 0.0f},
                                  getSize(),
                                  vec4{12.0f, 12.0f, 64.0f, 12.0f},
                                  App::getInstance().getAssets()->get<Texture>("frame"));

    // batch_2d.drawTextureRect({0.0f, 0.0f},
    //                          {32.0f, 32.0f},
    //                          Application::getInstance().getAssets()->get<Texture>("target"));
}

} // namespace ae::gui
