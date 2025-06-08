#include "gameplay_state.h"
#include "main_menu_state.h"

#include <ae/engine_context.h>
#include <ae/game_state_stack.h>
#include <ae/gui/gui.h>
#include <ae/scene/scene.h>
#include <ae/system/log.h>
#include <ae/task.h>
#include <ae/window/window.h>

GameplayState::GameplayState(EngineContext &engine_context)
    : GameState{engine_context}
    , m_paused{false}
{
    m_render_quad.create();
}

void GameplayState::onEnter()
{
    l_debug("GameplayState::onEnter");
    auto &ctx = getEngineContext();
    ctx.getWindow()->setMouseEnabled(false);
}

void GameplayState::onExit()
{
    l_debug("GameplayState::onExit");
    auto &ctx = getEngineContext();
    ctx.getWindow()->setMouseEnabled(true);

    // Clear assets
    ctx.getAssets()->clear();
    // Clear scene
    ctx.getScene()->clear();
}

void GameplayState::onPause()
{
    l_debug("GameplayState::onPause");
    m_paused = true;
    auto &ctx = getEngineContext();
    ctx.getWindow()->setMouseEnabled(true);
}

void GameplayState::onResume()
{
    l_debug("GameplayState::onResume");
    m_paused = false;
    auto &ctx = getEngineContext();
    ctx.getWindow()->setMouseEnabled(false);
}

bool GameplayState::isTranslucent() const
{
    return !m_paused;
}

void GameplayState::update(const Time &dt)
{
    auto &ctx = getEngineContext();

    if (ctx.getInput()->isKeyJustDown(KeyCode::ESCAPE)) {
        ctx.runLater(
            [&]() { ctx.getGameStateStack()->push(createShared<MainMenuState>(ctx, true)); });
    }

    // Обновляем сцену и интерфейс
    ctx.getScene()->tickUpdate(dt);
    ctx.getGui()->update(dt);
}

void GameplayState::draw(const Time &dt) const
{
    auto &ctx = getEngineContext();

    // Сцена и интерфейс рендерятся в текстуры
    ctx.getScene()->draw();
    ctx.getGui()->draw();

    // Рендерем текстуры
    glDisable(GL_DEPTH_TEST);
    m_render_quad.draw(ctx.getScene()->getRenderTexture().getSampledTexture(), dt.asSeconds());
    m_render_quad.draw(ctx.getGui()->getRenderTexture().getSampledTexture(), dt.asSeconds());
    glEnable(GL_DEPTH_TEST);
}
