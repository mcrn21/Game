#include "load_level_gui.h"
#include "style.h"

LoadLevelGui::LoadLevelGui(EngineContext &engine_context)
    : gui::Control{engine_context}
{}

void LoadLevelGui::setProgress(float progress)
{
    if (m_progress_bar)
        m_progress_bar->setProgress(progress);
}

void LoadLevelGui::addString(const String &string)
{
    auto cmd_output_string = m_fake_terminal->getTopCmdOutputString();
    if (cmd_output_string.isEmpty())
        cmd_output_string += string;
    else
        cmd_output_string += "\n" + string;

    m_fake_terminal->setTopCmdOutputString(cmd_output_string);
}

void LoadLevelGui::onCreated()
{
    m_fake_terminal = gui::Control::create<FakeTerminal>(getEngineContext());
    m_fake_terminal->setParent(sharedFromThis());
    m_fake_terminal->setPosition(vec2{Style::Metrics::main_fake_terminal_offset});
    m_fake_terminal->setSize(getSize() - vec2{Style::Metrics::main_fake_terminal_offset * 2.0f});

    m_fake_terminal->setFontPixelHeight(Style::Metrics::main_fake_terminal_font_pixel_height);
    m_fake_terminal->setColor(Style::Palette::primary);

    m_progress_bar = gui::Control::create<ProgressBar>(getEngineContext());
    m_progress_bar->setProgress(0.0f);
    m_progress_bar->setSize(vec2{400.0f, 40.0f});

    m_fake_terminal->push("load_level", String{}, m_progress_bar);
}

void LoadLevelGui::onSizeChanged(const vec2 &size)
{
    m_fake_terminal->setSize(size - vec2{Style::Metrics::main_fake_terminal_offset * 2.0f});
}
