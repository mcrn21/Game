#include "main_menu_state.h"
#include "gameplay_state.h"

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

    setupKeyBindings();

    auto &app = App::getInstance();
    app.runLater([&]() {
        m_main_menu = createGui();
        app.getGui()->push(m_main_menu);
    });
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

    if (m_main_menu) {
        auto *input_actions = app.getInputActionManager();
        // При нажатии Escape закрываются диалоги
        if (input_actions->isActionJustActivated("back"))
            m_main_menu->back();
    }

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

s_ptr<MainMenu> MainMenuState::createGui()
{
    auto &app = App::getInstance();
    auto root = gui::Control::create<MainMenu>(m_gameplay);

    root->new_game.connect([&]() {
        app.runLater([&]() {
            auto gameplay_state = createShared<GameplayState>();
            app.getGameStateStack()->replace(gameplay_state);
        });
    });

    root->resume.connect([&]() { app.runLater([&]() { app.getGameStateStack()->pop(); }); });

    root->exit_to_main_menu.connect([&]() {
        app.runLater([&]() {
            app.getGameStateStack()->pop();
            app.getGameStateStack()->pop();

            auto main_menu_state = createShared<MainMenuState>();
            app.getGameStateStack()->push(main_menu_state);
        });
    });

    root->exit.connect([&]() { app.exit(); });

    return root;
}

void MainMenuState::setupKeyBindings()
{
    auto &app = App::getInstance();

    auto *input_actions = app.getInputActionManager();
    input_actions->clear();

    input_actions->bindKeys({{"back", std::vector<std::string>{"Escape"}}});
}
