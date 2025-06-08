#include "engine.h"
#include "animation_manager.h"
#include "game_state_stack.h"
#include "input_action_manager.h"
#include "task_manager.h"
#include "window/input.h"
#include "window/window.h"

#include <GL/glew.h>

namespace ae {

Engine::Engine()
    : m_running{false}
{}

bool Engine::create(const Config &config)
{
    m_config = config;
    init(config);
    return true;
}

Config &Engine::getConfig()
{
    return m_config;
}

int32_t Engine::exec()
{
    if (!m_data.window)
        return 1;

    m_running = true;
    m_data.running_clock.restart();
    Clock loop_clock;
    Time accumulator;

    while (m_running) {
        m_data.elapsed_time = loop_clock.getElapsedTime();
        accumulator += m_data.elapsed_time;
        loop_clock.restart();

        m_data.fps = m_data.fps * (1.0f - m_data.fps_alpha)
                     + (1.0f / m_data.elapsed_time.asSeconds()) * m_data.fps_alpha;

        m_data.input->update();
        m_data.window->pollEvents();
        m_data.input_action_manager->update();

        while (accumulator >= m_data.tick_time) {
            m_data.animation_manager->update(m_data.tick_time);
            m_data.task_manager->update(m_data.tick_time);
            m_data.game_state_stack->update(m_data.tick_time);
            accumulator -= m_data.tick_time;
        }

        m_data.window->clear();
        m_data.game_state_stack->draw(m_data.elapsed_time);
        m_data.window->display();
    }

    return 1;
}

void Engine::exit()
{
    m_running = false;
}

} // namespace ae
