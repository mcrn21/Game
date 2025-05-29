#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "dialog.h"
#include "main_menu_button.h"

#include <ae/gui/control.h>
#include <ae/system/string.h>

#include <functional>

using namespace ae;

class MainMenu : public gui::Control
{
public:
    struct ButtonData
    {
        String string;
        std::function<void()> callback;
    };

    MainMenu(bool gameplay = false);
    ~MainMenu() = default;

    void back();

    void onCreated();
    void onSizeChanged(const vec2 &);

public:
    sigslot::signal<> new_game;
    sigslot::signal<> resume;
    sigslot::signal<> exit_to_main_menu;
    sigslot::signal<> exit;

protected:
    void drawControl(Batch2D &batch_2d);

private:
    void createButtons(const std::vector<ButtonData> buttons,
                       const std::vector<int32_t> &button_nums);
    void updateMainMenuButtonPositions();
    void setEnbaleMainMenuButtons(bool enable);

    void createExitGameDialog();
    void createExitToMainMenuDialog();

    void setActiveFrame(const s_ptr<gui::Control> &active_frame);

    s_ptr<gui::Control> createSettingsFrame();

private:
    bool m_gameplay;

    std::vector<std::pair<String, std::function<void()>>> m_all_buttons;
    std::vector<int32_t> m_main_menu_button_nums;
    std::vector<int32_t> m_gameplay_button_nums;

    std::vector<s_ptr<MainMenuButton>> m_main_menu_buttons;
    s_ptr<Dialog> m_exit_game_dialog;
    s_ptr<Dialog> m_exit_main_menu_dialog;

    s_ptr<gui::Control> m_active_frame;
};

#endif // MAIN_MENU_H
