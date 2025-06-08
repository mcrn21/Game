#include "main_menu_state.h"
#include "load_level_state.h"

#include <ae/engine_context.h>
#include <ae/game_state_stack.h>
#include <ae/graphics/core/default_shaders.h>
#include <ae/gui/gui.h>
#include <ae/gui/label.h>
#include <ae/input_action_manager.h>
#include <ae/system/log.h>

MainMenuState::MainMenuState(EngineContext &engine_context, bool in_gameplay)
    : GameState{engine_context}
    , m_gameplay{in_gameplay}
{
    m_render_quad.create();
}

void MainMenuState::onEnter()
{
    l_debug("MainMenuState::onEnter");

    setupKeyBindings();

    auto &ctx = getEngineContext();
    ctx.runLater([&]() {
        m_main_menu_gui = createGui();
        ctx.getGui()->push(m_main_menu_gui);
    });
}

void MainMenuState::onExit()
{
    l_debug("MainMenuState::onExit");
    auto &ctx = getEngineContext();
    ctx.getGui()->pop();
}

void MainMenuState::update(const Time &dt)
{
    auto &ctx = getEngineContext();

    if (m_main_menu_gui) {
        auto *input_actions = ctx.getInputActionManager();
        // При нажатии Escape закрываются диалоги
        if (input_actions->isActionJustActivated("back"))
            m_main_menu_gui->back();
    }

    // Обновляем сцену и интерфейс
    ctx.getGui()->update(dt);
}

void MainMenuState::draw(const Time &dt) const
{
    auto &ctx = getEngineContext();

    // Сцена и интерфейс рендерятся в текстуры
    ctx.getGui()->draw();

    // Рендерем текстуры
    glDisable(GL_DEPTH_TEST);
    m_render_quad.draw(ctx.getGui()->getRenderTexture().getSampledTexture(),
                       dt.asSeconds(),
                       DefaultShaders::getOldTerminalScreenQuad().get());
    glEnable(GL_DEPTH_TEST);
}

s_ptr<MainMenuGui> MainMenuState::createGui()
{
    auto &ctx = getEngineContext();
    auto root = gui::Control::create<MainMenuGui>(ctx, m_gameplay);

    root->new_game.connect([&]() {
        ctx.runLater([&]() {
            auto loading_level_state = createShared<LoadLevelState>(ctx);
            ctx.getGameStateStack()->replace(loading_level_state);
        });
    });

    root->resume.connect([&]() { ctx.runLater([&]() { ctx.getGameStateStack()->pop(); }); });

    root->exit_to_main_menu.connect([&]() {
        ctx.runLater([&]() {
            ctx.getGameStateStack()->pop();
            ctx.getGameStateStack()->pop();

            auto main_menu_state = createShared<MainMenuState>(ctx);
            ctx.getGameStateStack()->push(main_menu_state);
        });
    });

    root->exit.connect([&]() { ctx.exit(); });

    return root;
}

void MainMenuState::setupKeyBindings()
{
    auto &ctx = getEngineContext();

    auto *input_actions = ctx.getInputActionManager();
    input_actions->clear();

    input_actions->bindKeys({{"back", std::vector<std::string>{"Escape"}}});
}
