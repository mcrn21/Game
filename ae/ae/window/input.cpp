#include "input.h"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#define KEYS_COUNT 1024
#define BUTTONS_COUNT 8

namespace ae {

Input::Input()
    : m_states(KEYS_COUNT + BUTTONS_COUNT, {false, 0})
    , m_cursor_x{0}
    , m_cursor_y{0}
    , m_cursor_delta_x{0}
    , m_cursor_delta_y{0}
    , m_cursor_dragged{false}
    , m_scroll_x{0.0f}
    , m_scroll_y{0.0f}
    , m_codepoint{0}
    , m_update_counter{0}
{}

bool Input::isKeyHeld(KeyCode keycode) const
{
    return m_states[static_cast<int32_t>(keycode)].first;
}

bool Input::isKeyPressed(KeyCode keycode) const
{
    return m_states[static_cast<int32_t>(keycode)].first
           && m_states[static_cast<int32_t>(keycode)].second == m_update_counter;
}

bool Input::isKeyReleased(KeyCode keycode) const
{
    return !m_states[static_cast<int32_t>(keycode)].first
           && m_states[static_cast<int32_t>(keycode)].second == m_update_counter;
}

bool Input::isButtonHeld(ButtonCode button) const
{
    return m_states[KEYS_COUNT + static_cast<int32_t>(button)].first;
}

bool Input::isButtonPressed(ButtonCode button) const
{
    return m_states[KEYS_COUNT + static_cast<int32_t>(button)].first
           && m_states[KEYS_COUNT + static_cast<int32_t>(button)].second == m_update_counter;
}

bool Input::isButtonReleased(ButtonCode button) const
{
    return !m_states[KEYS_COUNT + static_cast<int32_t>(button)].first
           && m_states[KEYS_COUNT + static_cast<int32_t>(button)].second == m_update_counter;
}

int32_t Input::getCursorX() const
{
    return m_cursor_x;
}

int32_t Input::getCursorY() const
{
    return m_cursor_y;
}

int32_t Input::getCursorDeltaX() const
{
    return m_cursor_delta_x;
}

int32_t Input::getCursorDeltaY() const
{
    return m_cursor_delta_y;
}

float Input::getScrollX() const
{
    return m_scroll_x;
}

void Input::setScrollX(float xoffset)
{
    m_scroll_x = xoffset;
    scrolled(m_scroll_x, m_scroll_y);
}

float Input::getScrollY() const
{
    return m_scroll_y;
}

void Input::setScrollY(float yoffset)
{
    m_scroll_y = yoffset;
    scrolled(m_scroll_x, m_scroll_y);
}

uint32_t Input::getCodepoint() const
{
    return m_codepoint;
}

void Input::setCodepoint(uint32_t codepoint)
{
    m_codepoint = codepoint;
    codepointInputed(codepoint);
}

void Input::update()
{
    ++m_update_counter;
    m_cursor_delta_x = 0;
    m_cursor_delta_y = 0;
    m_scroll_x = 0.0f;
    m_scroll_y = 0.0f;
    m_codepoint = 0;
}

void Input::setKeyPressed(KeyCode keycode, int32_t pressed)
{
    if (pressed && !m_states[static_cast<int32_t>(keycode)].first) {
        m_states[static_cast<int32_t>(keycode)].first = true;
        m_states[static_cast<int32_t>(keycode)].second = m_update_counter;
        keyPressed(keycode);

    } else if (pressed && m_states[static_cast<int32_t>(keycode)].first) {
        keyHolded(keycode);

    } else if (!pressed && m_states[static_cast<int32_t>(keycode)].first) {
        m_states[static_cast<int32_t>(keycode)].first = false;
        m_states[static_cast<int32_t>(keycode)].second = m_update_counter;
        keyReleased(keycode);
    }
}

void Input::setButtonPressed(ButtonCode button, int32_t pressed)
{
    if (pressed && !m_states[KEYS_COUNT + static_cast<int32_t>(button)].first) {
        m_states[KEYS_COUNT + static_cast<int32_t>(button)].first = true;
        m_states[KEYS_COUNT + static_cast<int32_t>(button)].second = m_update_counter;
        buttonPressed(button);

    } else if (!pressed && m_states[KEYS_COUNT + static_cast<int32_t>(button)].first) {
        m_states[KEYS_COUNT + static_cast<int32_t>(button)].first = false;
        m_states[KEYS_COUNT + static_cast<int32_t>(button)].second = m_update_counter;
        buttonReleased(button);
    }
}

void Input::setCursorPosition(int32_t x, int32_t y)
{
    if (m_cursor_dragged) {
        m_cursor_delta_x = x - m_cursor_x;
        m_cursor_delta_y = y - m_cursor_y;

    } else
        m_cursor_dragged = true;

    m_cursor_x = x;
    m_cursor_y = y;

    cursorMoved(m_cursor_x, m_cursor_y, m_cursor_delta_x, m_cursor_delta_y);
}

} // namespace ae
