#include "input_action_manager.h"
#include "common/utils.h"
#include "system/log.h"

#include <algorithm>
#include <ranges>

namespace ae {

InputActionManager::InputActionManager()
    : m_update_counter{0}
{}

void InputActionManager::bindKey(const std::string &action_name, const std::string &binding)
{
    if (action_name.empty() || binding.empty())
        return;

    std::vector<std::string> codes;
    for (auto &&part : std::views::split(binding, '+'))
        codes.emplace_back(utils::trimString(std::string{part.begin(), part.end()}));

    if (codes.empty())
        return;

    KeyBinding key_binding;

    KeyBinding::Type type = KeyBinding::NONE;
    int32_t value = -1;

    for (const auto &code_string : codes) {
        value = -1; // Reset value

        KeyCode keycode = input_utils::keyCodeFromString(code_string);
        if (keycode != KeyCode::UNKNOWN) {
            if (input_utils::isKeyModifier(keycode))
                key_binding.modifiers = key_binding.modifiers
                                        | input_utils::keyCodeToModifier(keycode);
            else {
                type = KeyBinding::KEYBOARD;
                value = static_cast<int32_t>(keycode);
            }

        } else {
            ButtonCode button = input_utils::buttonCodeFromString(code_string);
            if (button != ButtonCode::UNKNOWN) {
                type = KeyBinding::MOUSE;
                value = static_cast<int32_t>(button);
            } else {
                ScrollDirection scroll_direction = input_utils::scrollDirectionFromString(
                    code_string);
                if (scroll_direction != ScrollDirection::UNKNOWN) {
                    type = KeyBinding::SCROLL;
                    value = static_cast<int32_t>(scroll_direction);
                }
            }
        }

        if (value != -1) {
            if (key_binding.value != -1) {
                l_error("Bind key error: multiple input codes: {}", binding);
                return;
            }

            key_binding.type = type;
            key_binding.value = value;
        }
    }

    if (key_binding.value != -1)
        bindKey(action_name, key_binding);
}

void InputActionManager::bindKey(const std::string &action_name,
                                 KeyCode keycode,
                                 KeyModifier modifiers)
{
    bindKey(action_name, {KeyBinding::KEYBOARD, static_cast<int32_t>(keycode), modifiers});
}

void InputActionManager::bindKey(const std::string &action_name,
                                 ButtonCode button,
                                 KeyModifier modifiers)
{
    bindKey(action_name, {KeyBinding::KEYBOARD, static_cast<int32_t>(button), modifiers});
}

void InputActionManager::bindKey(const std::string &action_name,
                                 ScrollDirection scroll_direction,
                                 KeyModifier modifiers)
{
    bindKey(action_name, {KeyBinding::KEYBOARD, static_cast<int32_t>(scroll_direction), modifiers});
}

void InputActionManager::bindKey(const std::string &action_name, const KeyBinding &binding)
{
    if (action_name.empty() || binding.value == -1 || binding.type == KeyBinding::NONE)
        return;

    auto &action = m_actions[action_name];
    action.name = action_name;
    action.bindings.push_back(binding);
}

void InputActionManager::bindKeys(
    const std::unordered_map<std::string, std::vector<std::string> > &actions)
{
    for (auto &[action_name, bindings] : actions) {
        for (const auto &binding : bindings)
            bindKey(action_name, binding);
    }
}

void InputActionManager::bindKeys(
    const std::unordered_map<std::string, std::vector<KeyBinding> > &actions)
{
    for (auto &[action_name, bindings] : actions) {
        for (const auto &binding : bindings)
            bindKey(action_name, binding);
    }
}

void InputActionManager::unbindKey(const std::string &action_name, const KeyBinding &binding)
{
    auto it = m_actions.find(action_name);
    if (it == m_actions.end())
        return;
    auto &vec = it->second.bindings;
    vec.erase(std::remove(vec.begin(), vec.end(), binding), vec.end());
}

void InputActionManager::removeAction(const std::string &action_name)
{
    m_actions.erase(action_name);
}

void InputActionManager::clear()
{
    m_actions.clear();
}

bool InputActionManager::isActionJustActivated(const std::string &action_name) const
{
    auto it = m_actions.find(action_name);
    return (it != m_actions.end())
               ? (it->second.active && it->second.last_update == m_update_counter)
               : false;
}

bool InputActionManager::isActionActive(const std::string &action_name) const
{
    auto it = m_actions.find(action_name);
    return (it != m_actions.end()) ? it->second.active : false;
}

bool InputActionManager::isActionJustDeactivated(const std::string &action_name) const
{
    auto it = m_actions.find(action_name);
    return (it != m_actions.end())
               ? (!it->second.active && it->second.last_update == m_update_counter)
               : false;
}

void InputActionManager::update(const Input &input)
{
    ++m_update_counter;

    for (auto &[name, action] : m_actions) {
        for (const auto &binding : action.bindings) {
            bool is_active = isBindingActive(binding, input);
            if (is_active && !action.active) {
                action.active = true;
                action.last_update = m_update_counter;
                break;
            } else if (!is_active && action.active) {
                action.active = false;
                action.last_update = m_update_counter;
            }
        }
    }
}

bool InputActionManager::isBindingActive(const KeyBinding &binding, const Input &input) const
{
    switch (binding.type) {
    case KeyBinding::KEYBOARD:
        return input.isKeyJustDown(static_cast<KeyCode>(binding.value))
               && input.getModifiers() == binding.modifiers;

    case KeyBinding::MOUSE:
        return input.isButtonJustDown(static_cast<ButtonCode>(binding.value));

    case KeyBinding::SCROLL:
        switch (binding.value) {
        case static_cast<int32_t>(ScrollDirection::SCROLL_UP): // Up
            return input.getScroll().y > 0.0f;
        case static_cast<int32_t>(ScrollDirection::SCROLL_DOWN): // Down
            return input.getScroll().y < 0.0f;
        case static_cast<int32_t>(ScrollDirection::SCROLL_LEFT): // Left
            return input.getScroll().x < 0.0f;
        case static_cast<int32_t>(ScrollDirection::SCROLL_RIGHT): // Right
            return input.getScroll().x > 0.0f;
        default:
            return false;
        }

    default:
        return false;
    }
    return false;
}

} // namespace ae
