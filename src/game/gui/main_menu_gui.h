#ifndef MAIN_MENU_GUI_H
#define MAIN_MENU_GUI_H

#include "controls/fake_terminal.h"

#include <ae/gui/control.h>
#include <ae/gui/label.h>
#include <ae/system/string.h>

#include <functional>
#include <ranges>

using namespace ae;

class MainMenuGui : public gui::Control
{
    using ButtonsList = std::vector<std::pair<String, std::function<void()>>>;

public:
    MainMenuGui(EngineContext &engine_context, bool gameplay = false);
    ~MainMenuGui() = default;

    void back();

    void onCreated();
    void onSizeChanged(const vec2 &size);

public:
    sigslot::signal<> new_game;
    sigslot::signal<> resume;
    sigslot::signal<> exit_to_main_menu;
    sigslot::signal<> exit;

protected:
    void drawControl(Batch2D &batch_2d);

private:
    template<typename Container, typename IndexContainer>
    auto indexed_view(const Container &data, const IndexContainer &indices)
    {
        return indices | std::views::transform([&](auto i) -> decltype(auto) { return data[i]; });
    }

    void createMainMenu();
    void createGameplayMenu();
    void createExitToMainMenuDialog();
    void createExitGameDialog();
    void createSettingsDialog();

private:
    bool m_gameplay;
    ButtonsList m_all_buttons;
    s_ptr<FakeTerminal> m_fake_terminal;
};

#endif // MAIN_MENU_GUI_H
