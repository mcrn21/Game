#include "main_menu_gui.h"
#include "settings_dialog.h"
#include "style.h"

#include <ae/engine.h>
#include <ae/gui/label.h>

MainMenuGui::MainMenuGui(EngineContext &engine_context, bool gameplay)
    : gui::Control{engine_context}
    , m_gameplay{gameplay}
{
    auto &ctx = getEngineContext();

    m_all_buttons = {
        {"New game", [&]() { new_game(); }}, // 0
        {"Save game", [&]() {}},             // 1
        {"Load game", [&]() {}},             // 2
        {"Resume", [&]() { resume(); }},     // 3
        {"Settings",                         // 4
         [&]() { ctx.runLater([&]() { createSettingsDialog(); }); }},
        {"Exit to main menu", // 5
         [&]() { ctx.runLater([&]() { createExitToMainMenuDialog(); }); }},
        {"Exit", [&]() { ctx.runLater([&]() { createExitGameDialog(); }); }}, // 6

        // Exit game buttons
        {"Accept", [&]() { exit(); }},                                          // 7
        {"Cancel", [&]() { ctx.runLater([&]() { m_fake_terminal->pop(); }); }}, // 8

        // Exit to main menu
        {"Accept", [&]() { exit_to_main_menu(); }}, // 9

        {"Back", [&]() { ctx.runLater([&]() { m_fake_terminal->pop(); }); }} // 10
    };
}

void MainMenuGui::back()
{
    auto &ctx = getEngineContext();

    if (m_fake_terminal->getPageCount() > 1) {
        ctx.runLater([&]() { m_fake_terminal->pop(); });
        return;
    }

    if (m_gameplay)
        resume();
    else
        ctx.runLater([&]() { createExitGameDialog(); });
}

void MainMenuGui::onCreated()
{
    m_fake_terminal = gui::Control::create<FakeTerminal>(getEngineContext());
    m_fake_terminal->setParent(sharedFromThis());
    m_fake_terminal->setPosition(vec2{Style::Metrics::main_fake_terminal_offset});
    m_fake_terminal->setSize(getSize() - vec2{Style::Metrics::main_fake_terminal_offset * 2.0f});

    m_fake_terminal->setFontPixelHeight(Style::Metrics::main_fake_terminal_font_pixel_height);
    m_fake_terminal->setColor(Style::Palette::primary);

    m_gameplay ? createGameplayMenu() : createMainMenu();
}

void MainMenuGui::onSizeChanged(const vec2 &size)
{
    m_fake_terminal->setSize(size - vec2{Style::Metrics::main_fake_terminal_offset * 2.0f});
}

void MainMenuGui::drawControl(Batch2D &batch_2d)
{
    batch_2d.drawRect(vec2{0.0f}, vec2{getSize()}, Style::Palette::main_menu_bg);
}

void MainMenuGui::createMainMenu()
{
    std::vector<size_t> button_nums = {0, 1, 2, 4, 6};
    m_fake_terminal->push("main_menu",
                          "Welcom to test_fps_game. Version 1.0.\nPlease select:",
                          nullptr,
                          indexed_view(m_all_buttons, button_nums));
}

void MainMenuGui::createGameplayMenu()
{
    std::vector<size_t> button_nums = {3, 1, 2, 0, 4, 5, 6};
    m_fake_terminal->push("pause",
                          "Please select:",
                          nullptr,

                          indexed_view(m_all_buttons, button_nums));
}

void MainMenuGui::createExitToMainMenuDialog()
{
    std::vector<size_t> button_nums = {9, 8};
    m_fake_terminal->push("exit_to_main_menu",
                          "Are you sure want to exit to main menu?",
                          nullptr,
                          indexed_view(m_all_buttons, button_nums));
}

void MainMenuGui::createExitGameDialog()
{
    std::vector<size_t> button_nums = {7, 8};
    m_fake_terminal->push("exit",
                          "Are you sure want to exit the game?",
                          nullptr,
                          indexed_view(m_all_buttons, button_nums));
}

void MainMenuGui::createSettingsDialog()
{
    auto &ctx = getEngineContext();

    std::vector<size_t> button_nums = {10};
    auto settings_frame = gui::Control::create<SettingsDialog>(ctx);
    settings_frame->setSize(vec2{600.0f, 450.0f});

    m_fake_terminal->push("settings", {}, settings_frame, indexed_view(m_all_buttons, button_nums));
}

// void MainMenuGui::createExitGameDialog()
// {
//     if (m_exit_game_dialog)
//         return;

//     auto dialog = gui::Control::create<Dialog>();
//     dialog->setParent(sharedFromThis());

//     dialog->setCenter(getSize() / 2.0f);

//     dialog->setString(String("Are you sure want to exit the game?"));

//     dialog->setAcceptString(String("Accept"));
//     dialog->setCancelString(String("Cancel"));

//     dialog->accepted.connect([this]() { exit(); });

//     dialog->canceled.connect([this]() {
//         auto &app = App::getInstance();
//         app.runLater([&]() {
//             m_exit_game_dialog->setParent(nullptr);
//             m_exit_game_dialog.reset();
//             setEnbaleMainMenuButtons(true);
//         });
//     });

//     m_exit_game_dialog = dialog;
//     setEnbaleMainMenuButtons(false);
// }

// void MainMenuGui::createExitToMainMenuDialog()
// {
//     if (m_exit_main_menu_dialog)
//         return;

//     auto dialog = gui::Control::create<Dialog>();
//     dialog->setParent(sharedFromThis());

//     dialog->setCenter(getSize() / 2.0f);

//     dialog->setString(String("Are you sure want to exit to main menu?"));

//     dialog->setAcceptString(String("Accept"));
//     dialog->setCancelString(String("Cancel"));

//     dialog->accepted.connect([this]() { exit_to_main_menu(); });

//     dialog->canceled.connect([this]() {
//         auto &app = App::getInstance();
//         app.runLater([&]() {
//             m_exit_main_menu_dialog->setParent(nullptr);
//             m_exit_main_menu_dialog.reset();
//             setEnbaleMainMenuButtons(true);
//         });
//     });

//     m_exit_main_menu_dialog = dialog;
//     setEnbaleMainMenuButtons(false);
// }
