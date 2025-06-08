#ifndef FIRST_STATE_GUI_H
#define FIRST_STATE_GUI_H

#include <ae/graphics/core/texture.h>
#include <ae/gui/control.h>

using namespace ae;

class FirstStateGui : public gui::Control
{
public:
    FirstStateGui(EngineContext &engine_context);
    ~FirstStateGui() = default;

    void setSplashTexture(const s_ptr<Texture> &splash_texture);

protected:
    void drawControl(Batch2D &batch_2d);

private:
    s_ptr<Texture> m_splash_texture;
};

#endif // FIRST_STATE_GUI_H
