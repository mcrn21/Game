#include "first_state_gui.h"

FirstStateGui::FirstStateGui(EngineContext &engine_context)
    : gui::Control{engine_context}
{}

void FirstStateGui::setSplashTexture(const s_ptr<Texture> &splash_texture)
{
    m_splash_texture = splash_texture;
    repaint();
}

void FirstStateGui::drawControl(Batch2D &batch_2d)
{
    if (m_splash_texture) {
        batch_2d
            .drawTexture(vec4{0.0f, 0.0f, getSize().x, getSize().y},
                         ivec4{0, 0, m_splash_texture->getSize().x, m_splash_texture->getSize().y},
                         m_splash_texture);
    }
}
