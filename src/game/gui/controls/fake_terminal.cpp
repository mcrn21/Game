#include "fake_terminal.h"

FakeTerminal::FakeTerminal(EngineContext &engine_context)
    : gui::Control{engine_context}
    , m_color{Color::white}
{}

void FakeTerminal::setColor(const Color &color)
{
    m_color = color;
}

void FakeTerminal::pop()
{
    if (!m_pages_stack.empty()) {
        m_pages_stack.back().page_control->setParent(nullptr);
        m_pages_stack.pop_back();
    }

    if (!m_pages_stack.empty())
        m_pages_stack.back().page_control->setParent(sharedFromThis());
}

int32_t FakeTerminal::getPageCount() const
{
    return m_pages_stack.size();
}

String FakeTerminal::getTopCmdOutputString() const
{
    if (m_pages_stack.empty())
        return {};
    return m_pages_stack.back().cmd_output_label->getString();
}

void FakeTerminal::setTopCmdOutputString(const String &string)
{
    if (m_pages_stack.empty())
        return;

    auto &page = m_pages_stack.back();

    page.cmd_output_label->setString(string);

    page.cmd_output_label->setSize(page.cmd_output_label->getImplicitSize());

    updatePageSize(page);
    updatePagePosition(page);
}

void FakeTerminal::appendStringToTopCmdOutput(const String &string)
{
    if (m_pages_stack.empty())
        return;

    auto &page = m_pages_stack.back();

    if (!page.cmd_output_label->getString().isEmpty())
        page.cmd_output_label->appendString("\n");
    page.cmd_output_label->appendString(string);

    page.cmd_output_label->setSize(page.cmd_output_label->getImplicitSize());

    updatePageSize(page);
    updatePagePosition(page);
}

void FakeTerminal::onSizeChanged(const vec2 &)
{
    for (auto &page : m_pages_stack) {
        if (page.control_fill_width)
            updatePageSize(page);
        updatePagePosition(page);
    }
}

void FakeTerminal::updatePageSize(Page &page)
{
    vec2 size{0.0f};

    const auto &children = page.page_control->getChildren();
    if (children.empty())
        return;

    float spacing = getFontPixelHeight() * 0.5;

    size = children.front()->getSize();

    for (const auto &control : children | std::views::drop(1)) {
        if (control != page.cmd_output_label)
            size.y += spacing;

        if (control == page.cmd_output_label && page.cmd_output_label
            && page.cmd_output_label->getString().isEmpty())
            continue;

        if (control == page.control) {
            auto control_size = control->getSize();
            control_size.x = getSize().x;
            control->setSize(control_size);
        }

        control->setPosition({0.0f, size.y});

        size.x = std::max(size.x, control->getSize().x);
        size.y += control->getSize().y;
    }

    page.page_control->setSize(size);
}

void FakeTerminal::updatePagePosition(Page &page)
{
    page.page_control->setPosition(vec2{0.0f, getSize().y - page.page_control->getSize().y});
}
