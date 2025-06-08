#include "first_state_gui.h"

FirstStateGui::FirstStateGui(EngineContext &engine_context)
    : gui::Control{engine_context}
{}

void FirstStateGui::setSplashTexture(const s_ptr<Texture> &splash_texture)
{
    m_splash_texture = splash_texture;
    repaint();
}

void FirstStateGui::createFakeTerminal()
{
    m_fake_terminal = gui::Control::create<FakeTerminal>(getEngineContext());
    m_fake_terminal->setParent(sharedFromThis());
    m_fake_terminal->setPosition(vec2{Style::Metrics::main_fake_terminal_offset});
    m_fake_terminal->setSize(getSize() - vec2{Style::Metrics::main_fake_terminal_offset * 2.0f});

    m_fake_terminal->setFontPixelHeight(Style::Metrics::main_fake_terminal_font_pixel_height);
    m_fake_terminal->setColor(Style::Palette::primary);

    m_progress_bar = gui::Control::create<ProgressBar>(getEngineContext());
    m_progress_bar->setProgress(0.0f);
    m_progress_bar->setColor(Style::Palette::secondary);
    m_progress_bar->setSize(vec2{400.0f, 18.f});

    m_fake_terminal->push("init", String{}, m_progress_bar, true);
}

void FirstStateGui::setProgress(float progress)
{
    if (m_progress_bar)
        m_progress_bar->setProgress(progress);
}

void FirstStateGui::appendStringToCmdOutput(const String &string)
{
    if (m_fake_terminal)
        m_fake_terminal->appendStringToTopCmdOutput(string);
}

void FirstStateGui::onSizeChanged(const vec2 &size)
{
    if (m_fake_terminal)
        m_fake_terminal->setSize(size - vec2{Style::Metrics::main_fake_terminal_offset * 2.0f});
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
