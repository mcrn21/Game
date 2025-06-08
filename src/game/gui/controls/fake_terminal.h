#ifndef FAKE_TERMINAL_H
#define FAKE_TERMINAL_H

#include "../style.h"
#include "fake_terminal_button.h"

#include <ae/assets/assets.h>
#include <ae/engine.h>
#include <ae/gui/control.h>
#include <ae/gui/label.h>
#include <ae/system/log.h>
#include <ae/system/string.h>

#include <ranges>
#include <vector>

using namespace ae;

class FakeTerminal : public gui::Control
{
public:
    FakeTerminal(EngineContext &engine_context);
    ~FakeTerminal() = default;

    void setColor(const Color &color);

    template<std::ranges::range Range>
    void push(const String &cmd,
              const String &cmd_output,
              const s_ptr<gui::Control> &control,
              bool control_fill_width,
              const Range &buttons);
    void push(const String &cmd,
              const String &cmd_output = {},
              const s_ptr<gui::Control> &control = nullptr,
              bool control_fill_width = false)
    {
        push(cmd,
             cmd_output,
             control,
             control_fill_width,
             std::views::empty<std::pair<String, std::function<void()>>>);
    }
    void pop();

    int32_t getPageCount() const;

    String getTopCmdOutputString() const;
    void setTopCmdOutputString(const String &string);
    void appendStringToTopCmdOutput(const String &string);

    void onSizeChanged(const vec2 &);

private:
    struct Page
    {
        s_ptr<gui::Control> page_control;
        s_ptr<gui::Control> control;
        s_ptr<gui::Label> cmd_output_label;
        bool control_fill_width = false;
    };

    template<std::ranges::range Range>
    s_ptr<gui::Control> createButtonsContainer(const Range &buttons);

    void updatePageSize(Page &page);
    void updatePagePosition(Page &page);

private:
    Color m_color;
    std::vector<Page> m_pages_stack;
};

template<std::ranges::range Range>
inline void FakeTerminal::push(const String &cmd,
                               const String &cmd_output,
                               const s_ptr<Control> &control,
                               bool control_fill_width,
                               const Range &buttons)
{
    if (cmd.isEmpty())
        return;

    auto &ctx = getEngineContext();

    Page page;

    auto page_control = gui::Control::create<gui::Control>(ctx);
    page.page_control = page_control;

    auto cmd_label = gui::Control::create<gui::Label>(ctx);
    cmd_label->setParent(page_control);

    cmd_label->setColor(Style::Palette::getInverseOn(m_color));
    cmd_label->setFont(getFont());
    cmd_label->setFontPixelHeight(getFontPixelHeight());
    cmd_label->setString("> " + cmd);
    cmd_label->setSize(cmd_label->getImplicitSize());

    auto cmd_output_label = gui::Control::create<gui::Label>(ctx);
    cmd_output_label->setParent(page_control);
    page.cmd_output_label = cmd_output_label;

    cmd_output_label->setColor(Style::Palette::getInverseOn(m_color));
    cmd_label->setFont(getFont());
    cmd_output_label->setFontPixelHeight(getFontPixelHeight());
    cmd_output_label->setString(cmd_output);
    cmd_output_label->setSize(cmd_output_label->getImplicitSize());

    if (control) {
        control->setParent(page_control);
        page.control = control;
        page.control_fill_width = control_fill_width;

        if (control_fill_width) {
            auto control_size = control->getSize();
            control_size.x = getSize().x;
            control->setSize(control_size);
        }
    }

    if (!buttons.empty()) {
        auto buttons_container = createButtonsContainer(buttons);
        buttons_container->setParent(page_control);
    }

    updatePageSize(page);
    updatePagePosition(page);

    if (!m_pages_stack.empty())
        m_pages_stack.back().page_control->setParent(nullptr);
    m_pages_stack.push_back(page);
    m_pages_stack.back().page_control->setParent(sharedFromThis());
}

template<std::ranges::range Range>
inline s_ptr<gui::Control> FakeTerminal::createButtonsContainer(const Range &buttons)
{
    auto &ctx = getEngineContext();

    auto buttons_container = gui::Control::create<gui::Control>(ctx);

    float button_width = 1.0f;

    auto click_sound_buffer = ctx.getAssets()->get<SoundBuffer>("ft_btn");

    for (const auto &button : buttons) {
        auto button_control = gui::Control::create<FakeTerminalButton>(ctx);
        button_control->setParent(buttons_container);

        button_control->setClickSound(click_sound_buffer);
        button_control->setFont(getFont());
        button_control->setFontPixelHeight(getFontPixelHeight());
        button_control->setColor(m_color);
        button_control->setString(button.first);
        button_control->clicked.connect(button.second);

        button_width
            = std::max(button_width,
                       getFont()->getFontPage(getFontPixelHeight())->getTextSize(button.first).x);
    }

    button_width *= 1.3f;
    float y_offset = 0.0f;

    float button_height = getFontPixelHeight() * 1.1f;
    float buttons_spacing = getFontPixelHeight() * 0.3f;

    for (auto &button_control : buttons_container->getChildren()) {
        button_control->setSize(vec2{button_width, button_height});
        button_control->setPosition(vec2{0.0f, y_offset + buttons_spacing});
        y_offset += buttons_spacing + button_height;
    }

    buttons_container->setSize(vec2{button_width, y_offset});

    return buttons_container;
}

#endif // FAKE_TERMINAL_H
