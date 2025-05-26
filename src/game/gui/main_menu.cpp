#include "main_menu.h"
#include "gui_theme.h"

#include <ae/task.h>

#include <spdlog/spdlog.h>

MainMenu::MainMenu()
    : Control{}
{
    m_all_buttons = {{String{"NEW GAME"}, [&]() { new_game(); }},
                     {String{"SAVE GAME"}, [&]() {}},
                     {String{"LOAD GAME"}, [&]() {}},
                     {String{"RESUME"}, [&]() { resume(); }},
                     {String{"SETTINGS"}, [&]() {}},
                     {String{"EXIT TO MAIN MENU"}, [&]() { exit_to_main_menu(); }},
                     {String{"EXIT"}, [&]() {
                          createExitGameDialog(); /*exit();*/
                      }}};

    m_main_menu_button_nums = {0, 2, 4, 6};
    m_gameplay_button_nums = {3, 1, 2, 0, 4, 5, 6};
}

void MainMenu::setMainMenuButtons(bool gameplay)
{
    // Clear old buttons
    for (auto &main_menu_button : m_main_menu_buttons)
        main_menu_button->setParent(nullptr);
    m_main_menu_buttons.clear();

    // Add new buttons
    auto *button_nums = gameplay ? &m_gameplay_button_nums : &m_main_menu_button_nums;
    for (int32_t button_num : *button_nums) {
        auto main_menu_button = gui::Control::create<MainMenuButton>();
        main_menu_button->setString(m_all_buttons[button_num].first);
        main_menu_button->setSize(GuiTheme::main_menu_button_size);
        main_menu_button->clicked.connect(m_all_buttons[button_num].second);
        main_menu_button->setParent(shared_from_this());
        m_main_menu_buttons.push_back(main_menu_button);
    }

    updateMainMenuButtonPositions();
}

void MainMenu::onSizeChanged(const vec2 &)
{
    updateMainMenuButtonPositions();

    if (m_exit_game_dialog)
        m_exit_game_dialog->setCenter(getSize() / 2.0f);

    repaint();
}

void MainMenu::updateMainMenuButtonPositions()
{
    float offset_x = GuiTheme::main_menu_button_offset_x;
    float offset_y = getSize().y
                     - (m_main_menu_buttons.size() * GuiTheme::main_menu_button_size.y
                        + (m_main_menu_buttons.size() - 1) * GuiTheme::main_menu_button_spacing
                        + offset_x);

    for (auto &main_menu_button : m_main_menu_buttons) {
        main_menu_button->setPosition(vec2{offset_x, offset_y});
        offset_y += GuiTheme::main_menu_button_size.y + GuiTheme::main_menu_button_spacing;
    }
}

void MainMenu::createExitGameDialog()
{
    if (m_exit_game_dialog)
        return;

    auto dialog = gui::Control::create<Dialog>();

    dialog->setPosition((getSize() - GuiTheme::exit_dialog_size) / 2.0f);
    dialog->setParent(shared_from_this());

    dialog->setCenter(getSize() / 2.0f);

    dialog->setString(String("Are you sure want to exit the game?"));

    dialog->setAcceptString(String("Accept"));
    dialog->setCancelString(String("Cancel"));

    dialog->accepted.connect([this]() { exit(); });

    dialog->canceled.connect([this]() {
        std::make_shared<CallbackTask>([&]() {
            m_exit_game_dialog->setParent(nullptr);
            m_exit_game_dialog.reset();
        })->run();
    });

    m_exit_game_dialog = dialog;
}

void MainMenu::drawControl(Batch2D &batch_2d)
{
    batch_2d.drawRect(vec2{0.0f}, vec2{getSize()}, GuiTheme::main_menu_bg_color);
}
