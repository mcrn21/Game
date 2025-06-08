#ifndef AE_INPUT_ACTION_MANAGER_H
#define AE_INPUT_ACTION_MANAGER_H

#include "engine_context_object.h"
#include "window/input.h"

#include <stdint.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace ae {

// Mouse wheel
// 0 up
// 1 down
// 2 left
// 3 right

struct KeyBinding
{
    enum Type { NONE, KEYBOARD, MOUSE, SCROLL };

    bool operator==(const KeyBinding &other) const
    {
        return type == other.type && value == other.value && modifiers == other.modifiers;
    }

    Type type = NONE;
    int32_t value = -1;
    KeyModifier modifiers = KeyModifier::NONE;
};

class InputActionManager : public EngineContextObject
{
public:
    InputActionManager(EngineContext &engine_context);
    ~InputActionManager() = default;

    void bindKey(const std::string &action_name, const std::string &binding);
    void bindKey(const std::string &action_name,
                 KeyCode keycode,
                 KeyModifier modifiers = KeyModifier::NONE);
    void bindKey(const std::string &action_name,
                 ButtonCode button,
                 KeyModifier modifiers = KeyModifier::NONE);
    void bindKey(const std::string &action_name,
                 ScrollDirection scroll_direction,
                 KeyModifier modifiers = KeyModifier::NONE);
    void bindKey(const std::string &action_name, const KeyBinding &binding);

    void bindKeys(const std::unordered_map<std::string, std::vector<std::string>> &actions);
    void bindKeys(const std::unordered_map<std::string, std::vector<KeyBinding>> &actions);

    void unbindKey(const std::string &action_name, const KeyBinding &binding);
    void removeAction(const std::string &action_name);
    void clear();

    bool isActionJustActivated(const std::string &action_name) const;
    bool isActionActive(const std::string &action_name) const;
    bool isActionJustDeactivated(const std::string &action_name) const;

    void update();

    uint32_t getUpdateCounter() const { return m_update_counter; }

private:
    bool isBindingActive(const KeyBinding &binding) const;

private:
    struct InputAction
    {
        std::string name;
        std::vector<KeyBinding> bindings;
        bool active = false;
        uint32_t last_update = 0;
    };

    std::unordered_map<std::string, InputAction> m_actions;
    uint32_t m_update_counter;
};

} // namespace ae

namespace std {
template<>
struct hash<ae::KeyBinding>
{
    std::size_t operator()(const ae::KeyBinding &kb) const
    {
        return ((std::hash<int32_t>()(static_cast<int>(kb.type)) << 2)
                ^ (std::hash<int32_t>()(kb.value) << 1)
                ^ (std::hash<int32_t>()(static_cast<int>(kb.modifiers))));
    }
};
} // namespace std

#endif // AE_INPUT_ACTION_MANAGER_H
