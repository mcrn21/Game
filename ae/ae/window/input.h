#ifndef AE_INPUT_H
#define AE_INPUT_H

#include "../../3rd/sigslot/signal.hpp"

#include <cstdint>
#include <vector>

struct GLFWwindow;

namespace ae {

enum class KeyCode : int32_t {
    SPACE = 32,
    APOSTROPHE = 39,
    COMMA = 44,
    MINUS = 45,
    PERIOD = 46,
    SLASH = 47,
    NUM_0 = 48,
    NUM_1 = 49,
    NUM_2 = 50,
    NUM_3 = 51,
    NUM_4 = 52,
    NUM_5 = 53,
    NUM_6 = 54,
    NUM_7 = 55,
    NUM_8 = 56,
    NUM_9 = 57,
    SEMICOLON = 59,
    EQUAL = 61,
    A = 65,
    B = 66,
    C = 67,
    D = 68,
    E = 69,
    F = 70,
    G = 71,
    H = 72,
    I = 73,
    J = 74,
    K = 75,
    L = 76,
    M = 77,
    N = 78,
    O = 79,
    P = 80,
    Q = 81,
    R = 82,
    S = 83,
    T = 84,
    U = 85,
    V = 86,
    W = 87,
    X = 88,
    Y = 89,
    Z = 90,
    LEFT_BRACKET = 91,
    BACKSLASH = 92,
    RIGHT_BRACKET = 93,
    GRAVE_ACCENT = 96,
    ESCAPE = 256,
    ENTER = 257,
    TAB = 258,
    BACKSPACE = 259,
    INSERT = 260,
    DELETE = 261,
    LEFT = 263,
    RIGHT = 262,
    DOWN = 264,
    UP = 265,
    PAGE_UP = 266,
    PAGE_DOWN = 267,
    HOME = 268,
    END = 269,
    CAPS_LOCK = 280,
    NUM_LOCK = 282,
    PRINT_SCREEN = 283,
    PAUSE = 284,
    F1 = 290,
    F2 = 291,
    F3 = 292,
    F4 = 293,
    F5 = 294,
    F6 = 295,
    F7 = 296,
    F8 = 297,
    F9 = 298,
    F10 = 299,
    F11 = 300,
    F12 = 301,
    LEFT_SHIFT = 340,
    LEFT_CONTROL = 341,
    LEFT_ALT = 342,
    LEFT_SUPER = 343,
    RIGHT_SHIFT = 344,
    RIGHT_CONTROL = 345,
    RIGHT_ALT = 346,
    RIGHT_SUPER = 347,
    MENU = 348,
    UNKNOWN = -1
};

enum class ButtonCode : int32_t {
    BUTTON_0 = 0,
    BUTTON_1,
    BUTTON_2,
    BUTTON_3,
    BUTTON_4,
    BUTTON_5,
    BUTTON_6,
    BUTTON_7
};

class Input
{
    friend class Window;

public:
    Input();
    ~Input() = default;

    // Keyboard
    bool isKeyHeld(KeyCode keycode) const;
    bool isKeyPressed(KeyCode keycode) const;
    bool isKeyReleased(KeyCode keycode) const;

    // Mouse
    bool isButtonHeld(ButtonCode button) const;
    bool isButtonPressed(ButtonCode button) const;
    bool isButtonReleased(ButtonCode button) const;

    // Cursor
    int32_t getCursorX() const;
    int32_t getCursorY() const;

    int32_t getCursorDeltaX() const;
    int32_t getCursorDeltaY() const;

    // Scroll
    float getScrollX() const;
    void setScrollX(float xoffset);

    float getScrollY() const;
    void setScrollY(float yoffset);

private:
    void update();

    void setKeyPressed(KeyCode keycode, int32_t pressed);
    void setButtonPressed(ButtonCode button, int32_t pressed);
    void setCursorPosition(int32_t x, int32_t y);

public:
    sigslot::signal<KeyCode> keyPressed;
    sigslot::signal<KeyCode> keyReleased;
    sigslot::signal<KeyCode> keyHolded;

    sigslot::signal<ButtonCode> buttonPressed;
    sigslot::signal<ButtonCode> buttonReleased;

    sigslot::signal<int32_t, int32_t, int32_t, int32_t> cursorMoved;

    sigslot::signal<float, float> scrolled;

private:
    std::vector<std::pair<bool, int32_t>> m_states;

    int32_t m_cursor_x;
    int32_t m_cursor_y;
    int32_t m_cursor_delta_x;
    int32_t m_cursor_delta_y;
    bool m_cursor_dragged;

    float m_scroll_x;
    float m_scroll_y;

    uint32_t m_update_counter;
};

} // namespace ae

#endif // AE_INPUT_H
