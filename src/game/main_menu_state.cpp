#include "main_menu_state.h"
#include "gameplay_state.h"
#include "gui/main_menu.h"
#include "gui/main_menu_button.h"

#include <ae/app.h>
#include <ae/gui/label.h>

#include <spdlog/spdlog.h>

MainMenuState::MainMenuState(bool in_gameplay)
    : m_gameplay{in_gameplay}
{
    m_render_quad.create();
}

void MainMenuState::onEnter()
{
    spdlog::debug("MainMenuState::onEnter");

    auto &app = App::getInstance();

    app.getGui()->push(createGui());
}

void MainMenuState::onExit()
{
    spdlog::debug("MainMenuState::onExit");

    auto &app = App::getInstance();

    app.getGui()->pop();
}

void MainMenuState::update(const Time &dt)
{
    auto &app = App::getInstance();

    // Обновляем сцену и интерфейс
    app.getGui()->update(dt);
}

void MainMenuState::draw() const
{
    auto &app = App::getInstance();

    // Сцена и интерфейс рендерятся в текстуры
    app.getGui()->draw();

    // Рендерем текстуры
    glDisable(GL_DEPTH_TEST);
    m_render_quad.draw(app.getGui()->getRenderTexture().getSampledTexture());
    glEnable(GL_DEPTH_TEST);
}

SharedPtr<gui::Control> MainMenuState::createGui()
{
    auto &app = App::getInstance();

    auto root = gui::Control::create<MainMenu>();

    root->new_game.connect([&]() {
        SharedPtr<CallbackTask>::create([&]() {
            auto gameplay_state = SharedPtr<GameplayState>::create();
            app.getGameStateStack()->replace(gameplay_state);
        })->run();
    });

    root->resume.connect([&]() { app.getGameStateStack()->pop(); });

    root->exit_to_main_menu.connect([&]() {
        SharedPtr<CallbackTask>::create([&]() {
            app.getGameStateStack()->pop();
            app.getGameStateStack()->pop();

            auto main_menu_state = SharedPtr<MainMenuState>::create();
            app.getGameStateStack()->push(main_menu_state);
        })->run();
    });

    root->exit.connect([&]() { app.exit(); });

    root->setMainMenuButtons(m_gameplay);

    return root;
}
