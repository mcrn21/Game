#include "main_menu.h"
#include "gui_theme.h"
#include "settings_frame.h"

#include <ae/task.h>

#include <spdlog/spdlog.h>

MainMenu::MainMenu()
    : Control{}
{
    m_all_buttons = {{String{"NEW GAME"}, [&]() { new_game(); }},
                     {String{"SAVE GAME"}, [&]() {}},
                     {String{"LOAD GAME"}, [&]() {}},
                     {String{"RESUME"}, [&]() { resume(); }},
                     {String{"SETTINGS"},
                      [&]() {
                          std::make_shared<CallbackTask>([&]() {
                              setActiveFrame(createSettingsFrame());
                          })->run();
                      }},
                     {String{"EXIT TO MAIN MENU"}, [&]() { createExitToMainMenuDialog(); }},
                     {String{"EXIT"}, [&]() { createExitGameDialog(); }}};

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

    float button_width = 1.0f;
    for (int32_t button_num : *button_nums) {
        auto main_menu_button = gui::Control::create<MainMenuButton>();

        button_width = std::max(button_width,
                                main_menu_button->getFont()
                                    ->getFontPage(main_menu_button->getFontPixelSize())
                                    ->getTextSize(m_all_buttons[button_num].first)
                                    .x);

        main_menu_button->setString(m_all_buttons[button_num].first);
        main_menu_button->clicked.connect(m_all_buttons[button_num].second);
        main_menu_button->setParent(sharedFromThis());
        m_main_menu_buttons.push_back(main_menu_button);
    }

    for (auto &main_menu_button : m_main_menu_buttons)
        main_menu_button->setSize(
            vec2{button_width * 1.18f, GuiTheme::metrics.main_menu_button_height});

    updateMainMenuButtonPositions();
}

void MainMenu::onSizeChanged(const vec2 &)
{
    updateMainMenuButtonPositions();

    if (m_exit_game_dialog)
        m_exit_game_dialog->setCenter(getSize() / 2.0f);

    if (m_exit_main_menu_dialog)
        m_exit_main_menu_dialog->setCenter(getSize() / 2.0f);

    repaint();
}

void MainMenu::updateMainMenuButtonPositions()
{
    float offset_x = GuiTheme::metrics.main_menu_buttons_corner_offset;
    float offset_y = getSize().y
                     - (m_main_menu_buttons.size() * GuiTheme::metrics.main_menu_button_height
                        + (m_main_menu_buttons.size() - 1)
                              * GuiTheme::metrics.main_menu_buttons_spacing
                        + offset_x);

    for (auto &main_menu_button : m_main_menu_buttons) {
        main_menu_button->setPosition(vec2{offset_x, offset_y});
        offset_y += GuiTheme::metrics.main_menu_button_height
                    + GuiTheme::metrics.main_menu_buttons_spacing;
    }
}

void MainMenu::setEnbaleMainMenuButtons(bool enable)
{
    for (auto &main_menu_button : m_main_menu_buttons)
        main_menu_button->setEnable(enable);
}

void MainMenu::createExitGameDialog()
{
    if (m_exit_game_dialog)
        return;

    auto dialog = gui::Control::create<Dialog>();
    dialog->setParent(sharedFromThis());

    dialog->setCenter(getSize() / 2.0f);

    dialog->setString(String("Are you sure want to exit the game?"));

    dialog->setAcceptString(String("Accept"));
    dialog->setCancelString(String("Cancel"));

    dialog->accepted.connect([this]() { exit(); });

    dialog->canceled.connect([this]() {
        std::make_shared<CallbackTask>([&]() {
            m_exit_game_dialog->setParent(nullptr);
            m_exit_game_dialog.reset();
            setEnbaleMainMenuButtons(true);
        })->run();
    });

    m_exit_game_dialog = dialog;
    setEnbaleMainMenuButtons(false);
}

void MainMenu::createExitToMainMenuDialog()
{
    if (m_exit_main_menu_dialog)
        return;

    auto dialog = gui::Control::create<Dialog>();
    dialog->setParent(sharedFromThis());

    dialog->setCenter(getSize() / 2.0f);

    dialog->setString(String("Are you sure want to exit to main menu?"));

    dialog->setAcceptString(String("Accept"));
    dialog->setCancelString(String("Cancel"));

    dialog->accepted.connect([this]() { exit_to_main_menu(); });

    dialog->canceled.connect([this]() {
        std::make_shared<CallbackTask>([&]() {
            m_exit_main_menu_dialog->setParent(nullptr);
            m_exit_main_menu_dialog.reset();
            setEnbaleMainMenuButtons(true);
        })->run();
    });

    m_exit_main_menu_dialog = dialog;
    setEnbaleMainMenuButtons(false);
}

void MainMenu::setActiveFrame(const SharedPtr<Control> &active_frame)
{
    if (m_active_frame) {
        m_active_frame->setParent(nullptr);
        m_active_frame.reset();
        return;
    }

    m_active_frame = active_frame;
    m_active_frame->setParent(sharedFromThis());
    m_active_frame->setPosition(vec2{100.0f, 100.0f});
    m_active_frame->setSize(vec2{400.0f});

    auto delay = std::make_shared<DelayTask>(seconds(3.0f));
    auto destroy_task = std::make_shared<CallbackTask>([this]() {
        m_active_frame->setParent(nullptr);
        m_active_frame.reset();
    });
    auto chain = std::make_shared<TaskChain>();
    chain->addTask(delay);
    chain->addTask(destroy_task);
    chain->run();
}

SharedPtr<gui::Control> MainMenu::createSettingsFrame()
{
    auto settings_frame = gui::Control::create<SettingsFrame>();

    return settings_frame;
}

void MainMenu::drawControl(Batch2D &batch_2d)
{
    batch_2d.drawRect(vec2{0.0f}, vec2{getSize()}, GuiTheme::palette.main_menu_bg);
}
