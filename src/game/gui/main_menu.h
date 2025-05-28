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

    MainMenu();
    ~MainMenu() = default;

    void setMainMenuButtons(bool gameplay = false);

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

    void setActiveFrame(const SharedPtr<gui::Control> &active_frame);

    SharedPtr<gui::Control> createSettingsFrame();

private:
    std::vector<std::pair<String, std::function<void()>>> m_all_buttons;
    std::vector<int32_t> m_main_menu_button_nums;
    std::vector<int32_t> m_gameplay_button_nums;

    std::vector<SharedPtr<MainMenuButton>> m_main_menu_buttons;
    SharedPtr<Dialog> m_exit_game_dialog;
    SharedPtr<Dialog> m_exit_main_menu_dialog;

    SharedPtr<gui::Control> m_active_frame;
};

#endif // MAIN_MENU_H
