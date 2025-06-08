#include "input.h"
#include "../system/log.h"

#include <unordered_map>

#define KEYS_COUNT 1024
#define BUTTONS_COUNT 8

namespace ae {

namespace input_utils {

std::string keyCodeToString(KeyCode keycode)
{
    static const std::unordered_map<KeyCode, std::string> strings
        = {{KeyCode::SPACE, "Space"},
           {KeyCode::APOSTROPHE, "'"},
           {KeyCode::COMMA, ","},
           {KeyCode::MINUS, "-"},
           {KeyCode::PERIOD, "."},
           {KeyCode::SLASH, "/"},
           {KeyCode::NUM_0, "Num 0"},
           {KeyCode::NUM_1, "Num 1"},
           {KeyCode::NUM_2, "Num 2"},
           {KeyCode::NUM_3, "Num 3"},
           {KeyCode::NUM_4, "Num 4"},
           {KeyCode::NUM_5, "Num 5"},
           {KeyCode::NUM_6, "Num 6"},
           {KeyCode::NUM_7, "Num 7"},
           {KeyCode::NUM_8, "Num 8"},
           {KeyCode::NUM_9, "Num 9"},
           {KeyCode::SEMICOLON, ";"},
           {KeyCode::EQUAL, "="},
           {KeyCode::A, "A"},
           {KeyCode::B, "B"},
           {KeyCode::C, "C"},
           {KeyCode::D, "D"},
           {KeyCode::E, "E"},
           {KeyCode::F, "F"},
           {KeyCode::G, "G"},
           {KeyCode::H, "H"},
           {KeyCode::I, "I"},
           {KeyCode::J, "J"},
           {KeyCode::K, "K"},
           {KeyCode::L, "L"},
           {KeyCode::M, "M"},
           {KeyCode::N, "N"},
           {KeyCode::O, "O"},
           {KeyCode::P, "P"},
           {KeyCode::Q, "Q"},
           {KeyCode::R, "R"},
           {KeyCode::S, "S"},
           {KeyCode::T, "T"},
           {KeyCode::U, "U"},
           {KeyCode::V, "V"},
           {KeyCode::W, "W"},
           {KeyCode::X, "X"},
           {KeyCode::Y, "Y"},
           {KeyCode::Z, "Z"},
           {KeyCode::LEFT_BRACKET, "["},
           {KeyCode::BACKSLASH, "\\"},
           {KeyCode::RIGHT_BRACKET, "]"},
           {KeyCode::GRAVE_ACCENT, "`"},
           {KeyCode::ESCAPE, "Escape"},
           {KeyCode::ENTER, "Enter"},
           {KeyCode::TAB, "Tab"},
           {KeyCode::BACKSPACE, "Backspace"},
           {KeyCode::INSERT, "Insert"},
           {KeyCode::DELETE, "Delete"},
           {KeyCode::LEFT, "Left"},
           {KeyCode::RIGHT, "Right"},
           {KeyCode::DOWN, "Down"},
           {KeyCode::UP, "Up"},
           {KeyCode::PAGE_UP, "Page up"},
           {KeyCode::PAGE_DOWN, "Page down"},
           {KeyCode::HOME, "Home"},
           {KeyCode::END, "End"},
           {KeyCode::CAPS_LOCK, "Capslock"},
           {KeyCode::NUM_LOCK, "Numlock"},
           {KeyCode::PRINT_SCREEN, "Print screen"},
           {KeyCode::PAUSE, "Pause"},
           {KeyCode::F1, "F1"},
           {KeyCode::F2, "F2"},
           {KeyCode::F3, "F3"},
           {KeyCode::F4, "F4"},
           {KeyCode::F5, "F5"},
           {KeyCode::F6, "F6"},
           {KeyCode::F7, "F7"},
           {KeyCode::F8, "F8"},
           {KeyCode::F9, "F9"},
           {KeyCode::F10, "F10"},
           {KeyCode::F11, "F11"},
           {KeyCode::F12, "F12"},
           {KeyCode::LEFT_SHIFT, "Left shift"},
           {KeyCode::LEFT_CONTROL, "Left ctrl"},
           {KeyCode::LEFT_ALT, "Left alt"},
           {KeyCode::LEFT_SUPER, "Left super"},
           {KeyCode::RIGHT_SHIFT, "Right shift"},
           {KeyCode::RIGHT_CONTROL, "Right ctrl"},
           {KeyCode::RIGHT_ALT, "Right alt"},
           {KeyCode::RIGHT_SUPER, "Right super"},
           {KeyCode::MENU, "Menu"},
           {KeyCode::UNKNOWN, "Unknow"}};
    auto found = strings.find(keycode);
    return found != strings.end() ? found->second : "Unknow";
}

KeyCode keyCodeFromString(const std::string &string)
{
    static const std::unordered_map<std::string, KeyCode> codes
        = {{"Space", KeyCode::SPACE},
           {"'", KeyCode::APOSTROPHE},
           {",", KeyCode::COMMA},
           {"-", KeyCode::MINUS},
           {".", KeyCode::PERIOD},
           {"/", KeyCode::SLASH},
           {"Num 0", KeyCode::NUM_0},
           {"Num 1", KeyCode::NUM_1},
           {"Num 2", KeyCode::NUM_2},
           {"Num 3", KeyCode::NUM_3},
           {"Num 4", KeyCode::NUM_4},
           {"Num 5", KeyCode::NUM_5},
           {"Num 6", KeyCode::NUM_6},
           {"Num 7", KeyCode::NUM_7},
           {"Num 8", KeyCode::NUM_8},
           {"Num 9", KeyCode::NUM_9},
           {";", KeyCode::SEMICOLON},
           {"=", KeyCode::EQUAL},
           {"A", KeyCode::A},
           {"B", KeyCode::B},
           {"C", KeyCode::C},
           {"D", KeyCode::D},
           {"E", KeyCode::E},
           {"F", KeyCode::F},
           {"G", KeyCode::G},
           {"H", KeyCode::H},
           {"I", KeyCode::I},
           {"J", KeyCode::J},
           {"K", KeyCode::K},
           {"L", KeyCode::L},
           {"M", KeyCode::M},
           {"N", KeyCode::N},
           {"O", KeyCode::O},
           {"P", KeyCode::P},
           {"Q", KeyCode::Q},
           {"R", KeyCode::R},
           {"S", KeyCode::S},
           {"T", KeyCode::T},
           {"U", KeyCode::U},
           {"V", KeyCode::V},
           {"W", KeyCode::W},
           {"X", KeyCode::X},
           {"Y", KeyCode::Y},
           {"Z", KeyCode::Z},
           {"[", KeyCode::LEFT_BRACKET},
           {"\\", KeyCode::BACKSLASH},
           {"]", KeyCode::RIGHT_BRACKET},
           {"`", KeyCode::GRAVE_ACCENT},
           {"Escape", KeyCode::ESCAPE},
           {"Enter", KeyCode::ENTER},
           {"Tab", KeyCode::TAB},
           {"Backspace", KeyCode::BACKSPACE},
           {"Insert", KeyCode::INSERT},
           {"Delete", KeyCode::DELETE},
           {"Left", KeyCode::LEFT},
           {"Right", KeyCode::RIGHT},
           {"Down", KeyCode::DOWN},
           {"Up", KeyCode::UP},
           {"Page up", KeyCode::PAGE_UP},
           {"Page down", KeyCode::PAGE_DOWN},
           {"Home", KeyCode::HOME},
           {"End", KeyCode::END},
           {"Capslock", KeyCode::CAPS_LOCK},
           {"Numlock", KeyCode::NUM_LOCK},
           {"Print screen", KeyCode::PRINT_SCREEN},
           {"Pause", KeyCode::PAUSE},
           {"F1", KeyCode::F1},
           {"F2", KeyCode::F2},
           {"F3", KeyCode::F3},
           {"F4", KeyCode::F4},
           {"F5", KeyCode::F5},
           {"F6", KeyCode::F6},
           {"F7", KeyCode::F7},
           {"F8", KeyCode::F8},
           {"F9", KeyCode::F9},
           {"F10", KeyCode::F10},
           {"F11", KeyCode::F11},
           {"F12", KeyCode::F12},
           {"Left shift", KeyCode::LEFT_SHIFT},
           {"Left ctrl", KeyCode::LEFT_CONTROL},
           {"Left alt", KeyCode::LEFT_ALT},
           {"Left super", KeyCode::LEFT_SUPER},
           {"Right shift", KeyCode::RIGHT_SHIFT},
           {"Right ctrl", KeyCode::RIGHT_CONTROL},
           {"Right alt", KeyCode::RIGHT_ALT},
           {"Right super", KeyCode::RIGHT_SUPER},
           {"Menu", KeyCode::MENU},
           {"Unknow", KeyCode::UNKNOWN}};
    auto found = codes.find(string);
    return found != codes.end() ? found->second : KeyCode::UNKNOWN;
}

bool isKeyModifier(KeyCode code)
{
    static const std::vector<KeyCode> modifiers = {KeyCode::LEFT_SHIFT,
                                                   KeyCode::LEFT_CONTROL,
                                                   KeyCode::LEFT_ALT,
                                                   KeyCode::LEFT_SUPER,
                                                   KeyCode::RIGHT_SHIFT,
                                                   KeyCode::RIGHT_CONTROL,
                                                   KeyCode::RIGHT_ALT,
                                                   KeyCode::RIGHT_SUPER};
    return std::find(modifiers.begin(), modifiers.end(), code) != modifiers.end();
}

KeyModifier keyCodeToModifier(KeyCode code)
{
    if (code == KeyCode::LEFT_SHIFT || code == KeyCode::RIGHT_SHIFT)
        return KeyModifier::SHIFT;
    else if (code == KeyCode::LEFT_CONTROL || code == KeyCode::RIGHT_CONTROL)
        return KeyModifier::CTRL;
    else if (code == KeyCode::LEFT_ALT || code == KeyCode::LEFT_ALT)
        return KeyModifier::ALT;
    else if (code == KeyCode::LEFT_SUPER || code == KeyCode::RIGHT_SUPER)
        return KeyModifier::SUPER;
    return KeyModifier::NONE;
}

bool hasModifier(KeyModifier modifiers, KeyModifier flag)
{
    return (modifiers & flag) == flag;
}

std::string buttonCodeToString(ButtonCode button)
{
    static const std::unordered_map<ButtonCode, std::string> strings
        = {{ButtonCode::BUTTON_0, "Button 0"},
           {ButtonCode::BUTTON_1, "Button 1"},
           {ButtonCode::BUTTON_2, "Button 2"},
           {ButtonCode::BUTTON_3, "Button 3"},
           {ButtonCode::BUTTON_4, "Button 4"},
           {ButtonCode::BUTTON_5, "Button 5"},
           {ButtonCode::BUTTON_6, "Button 6"},
           {ButtonCode::BUTTON_7, "Button 7"},
           {ButtonCode::UNKNOWN, "Unknow"}};
    auto found = strings.find(button);
    return found != strings.end() ? found->second : "Unknow";
}

ButtonCode buttonCodeFromString(const std::string &string)
{
    static const std::unordered_map<std::string, ButtonCode> buttons
        = {{"Button 0", ButtonCode::BUTTON_0},
           {"Button 1", ButtonCode::BUTTON_1},
           {"Button 2", ButtonCode::BUTTON_2},
           {"Button 3", ButtonCode::BUTTON_3},
           {"Button 4", ButtonCode::BUTTON_4},
           {"Button 5", ButtonCode::BUTTON_5},
           {"Button 6", ButtonCode::BUTTON_6},
           {"Button 7", ButtonCode::BUTTON_7},
           {"Unknow", ButtonCode::UNKNOWN}};
    auto found = buttons.find(string);
    return found != buttons.end() ? found->second : ButtonCode::UNKNOWN;
}

std::string scrollDirectionToString(ScrollDirection scroll_direction)
{
    static const std::unordered_map<ScrollDirection, std::string> strings
        = {{ScrollDirection::SCROLL_UP, "Scroll up"},
           {ScrollDirection::SCROLL_DOWN, "Scroll down"},
           {ScrollDirection::SCROLL_LEFT, "Scroll left"},
           {ScrollDirection::SCROLL_RIGHT, "Scroll right"},
           {ScrollDirection::UNKNOWN, "Unknow"}};
    auto found = strings.find(scroll_direction);
    return found != strings.end() ? found->second : "Unknow";
}

ScrollDirection scrollDirectionFromString(const std::string &string)
{
    static const std::unordered_map<std::string, ScrollDirection> scroll_directions
        = {{"Scroll up", ScrollDirection::SCROLL_UP},
           {"Scroll down", ScrollDirection::SCROLL_DOWN},
           {"Scroll left", ScrollDirection::SCROLL_LEFT},
           {"Scroll right", ScrollDirection::SCROLL_RIGHT},
           {"Unknow", ScrollDirection::UNKNOWN}};
    auto found = scroll_directions.find(string);
    return found != scroll_directions.end() ? found->second : ScrollDirection::UNKNOWN;
}

} // namespace input_utils

Input::Input()
    : m_states(KEYS_COUNT + BUTTONS_COUNT, {false, 0})
    , m_modifiers{KeyModifier::NONE}
    , m_cursor_position{0}
    , m_cursor_delta{0}
    , m_cursor_dragged{false}
    , m_scroll{0.0f}
    , m_codepoint{0}
    , m_update_counter{0}
{}

bool Input::isKeyDown(KeyCode keycode) const
{
    return m_states[static_cast<int32_t>(keycode)].first;
}

bool Input::isKeyJustDown(KeyCode keycode) const
{
    return m_states[static_cast<int32_t>(keycode)].first
           && m_states[static_cast<int32_t>(keycode)].second == m_update_counter;
}

bool Input::isKeyJustUp(KeyCode keycode) const
{
    return !m_states[static_cast<int32_t>(keycode)].first
           && m_states[static_cast<int32_t>(keycode)].second == m_update_counter;
}

void Input::setKeyPressed(KeyCode keycode, int32_t pressed)
{
    if (pressed && !m_states[static_cast<int32_t>(keycode)].first) {
        m_states[static_cast<int32_t>(keycode)].first = true;
        m_states[static_cast<int32_t>(keycode)].second = m_update_counter;
        keyJustDown.emit(keycode);

    } else if (pressed && m_states[static_cast<int32_t>(keycode)].first) {
        keyDown.emit(keycode);

    } else if (!pressed && m_states[static_cast<int32_t>(keycode)].first) {
        m_states[static_cast<int32_t>(keycode)].first = false;
        m_states[static_cast<int32_t>(keycode)].second = m_update_counter;
        keyJustUp.emit(keycode);
    }
}

bool Input::isButtonDown(ButtonCode button) const
{
    return m_states[KEYS_COUNT + static_cast<int32_t>(button)].first;
}

bool Input::isButtonJustDown(ButtonCode button) const
{
    return m_states[KEYS_COUNT + static_cast<int32_t>(button)].first
           && m_states[KEYS_COUNT + static_cast<int32_t>(button)].second == m_update_counter;
}

bool Input::isButtonJustUp(ButtonCode button) const
{
    return !m_states[KEYS_COUNT + static_cast<int32_t>(button)].first
           && m_states[KEYS_COUNT + static_cast<int32_t>(button)].second == m_update_counter;
}

void Input::setButtonPressed(ButtonCode button, int32_t pressed)
{
    if (pressed && !m_states[KEYS_COUNT + static_cast<int32_t>(button)].first) {
        m_states[KEYS_COUNT + static_cast<int32_t>(button)].first = true;
        m_states[KEYS_COUNT + static_cast<int32_t>(button)].second = m_update_counter;
        buttonJustDown.emit(button);

    } else if (!pressed && m_states[KEYS_COUNT + static_cast<int32_t>(button)].first) {
        m_states[KEYS_COUNT + static_cast<int32_t>(button)].first = false;
        m_states[KEYS_COUNT + static_cast<int32_t>(button)].second = m_update_counter;
        buttonJustUp.emit(button);
    }
}

KeyModifier Input::getModifiers() const
{
    return m_modifiers;
}

void Input::setModifiers(KeyModifier modifiers)
{
    m_modifiers = modifiers;
}

const ivec2 &Input::getCursorPosition() const
{
    return m_cursor_position;
}

const ivec2 &Input::getCursorDelta() const
{
    return m_cursor_delta;
}

void Input::setCursorPosition(const ivec2 &cursor_position)
{
    if (m_cursor_dragged)
        m_cursor_delta = cursor_position - m_cursor_position;
    else
        m_cursor_dragged = true;

    // l_debug("delta: {}", m_cursor_delta);

    m_cursor_position = cursor_position;
    cursorMoved.emit(cursor_position, m_cursor_delta);
}

const vec2 &Input::getScroll() const
{
    return m_scroll;
}

void Input::setScroll(const vec2 &scroll)
{
    m_scroll = scroll;
    scrolled.emit(scroll);
}

uint32_t Input::getCodepoint() const
{
    return m_codepoint;
}

void Input::setCodepoint(uint32_t codepoint)
{
    m_codepoint = codepoint;
    codepointInputed.emit(codepoint);
}

void Input::update()
{
    ++m_update_counter;
    m_cursor_delta = ivec2{0};
    m_scroll = vec2{0.0f};
    m_codepoint = 0;
}

} // namespace ae
