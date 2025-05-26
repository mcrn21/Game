#include "gameplay_state.h"

#include <ae/app.h>

#include <spdlog/spdlog.h>

GameplayState::GameplayState()
    : m_paused{false}
{
    m_render_quad.create();
}

void GameplayState::onEnter()
{
    spdlog::debug("GameplayState::onEnter");
}

void GameplayState::onExit()
{
    spdlog::debug("GameplayState::onExit");
}

void GameplayState::onPause()
{
    spdlog::debug("GameplayState::onPause");
    m_paused = true;
}

void GameplayState::onResume()
{
    spdlog::debug("GameplayState::onResume");
    m_paused = false;
}

bool GameplayState::isTranslucent() const
{
    return !m_paused;
}

void GameplayState::update(const Time &dt)
{
    auto &app = App::getInstance();

    // Обновляем сцену и интерфейс
    app.getScene()->tickUpdate(dt);
    app.getGui()->update(dt);
}

void GameplayState::draw() const
{
    auto &app = App::getInstance();

    // Сцена и интерфейс рендерятся в текстуры
    app.getScene()->draw();
    app.getGui()->draw();

    // Рендерем текстуры
    glDisable(GL_DEPTH_TEST);
    m_render_quad.draw(app.getScene()->getRenderTexture().getSampledTexture());
    m_render_quad.draw(app.getGui()->getRenderTexture().getSampledTexture());
    glEnable(GL_DEPTH_TEST);
}
