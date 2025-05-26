#include "app.h"

#include <GL/glew.h>
#include <spdlog/spdlog.h>

namespace ae {

GLFWManager App::m_glfw_manager;

App::App()
    : m_running{false}
    , m_tick_time{seconds(1.0f / 60.0f)} // Default 60 fps
    , m_fps{0}
    , m_assets{std::make_unique<Assets>()}
{}

App &App::getInstance()
{
    static App instance;
    return instance;
}

bool App::create(const Config &config)
{
    m_config = config;

    m_tick_time = seconds(1.0f / static_cast<float>(config.game_frame_rate));

    m_window = std::make_unique<Window>();
    if (!m_window->create(config.window_width, config.window_height, config.window_title))
        return false;

    // Task manager
    m_task_manager = std::make_unique<TaskManager>();

    // Animation manager
    m_animation_manager = std::make_unique<AnimationManager>();

    m_scene = std::make_unique<Scene>();
    m_scene->setRenderTextureSize(ivec2{config.window_width, config.window_height});
    m_window->sizeChanged.connect(&Scene::setRenderTextureSize, m_scene.get());

    m_gui = std::make_unique<Gui>();
    m_gui->setRenderTextureSize(ivec2{config.window_width, config.window_height});
    m_window->sizeChanged.connect(&Gui::setRenderTextureSize, m_gui.get());

    // Input
    m_window->getInput().buttonPressed.connect(&Gui::onButtonPressed, m_gui.get());
    m_window->getInput().buttonReleased.connect(&Gui::onButtonReleased, m_gui.get());
    m_window->getInput().cursorMoved.connect(&Gui::onCursorMoved, m_gui.get());

    m_game_state_stack = std::make_unique<GameStateStack>();

    return true;
}

Config &App::getConfig()
{
    return m_config;
}

Window *App::getWindow() const
{
    return m_window.get();
}

Input *App::getInput() const
{
    if (!m_window)
        return nullptr;

    return &m_window->getInput();
}

Assets *App::getAssets() const
{
    return m_assets.get();
}

Scene *App::getScene() const
{
    return m_scene.get();
}

Gui *App::getGui() const
{
    return m_gui.get();
}

GameStateStack *App::getGameStateStack() const
{
    return m_game_state_stack.get();
}

TaskManager *App::getTaskManager() const
{
    return m_task_manager.get();
}

AnimationManager *App::getAnimationManager() const
{
    return m_animation_manager.get();
}

int32_t App::getFps() const
{
    return m_fps;
}

int32_t App::exec()
{
    if (!m_window)
        return 1;

    m_running = true;
    m_running_clock.restart();
    Clock loop_clock;
    Time accumulator;

    created();

    while (m_running) {
        m_elapsed_time = loop_clock.getElapsedTime();
        accumulator += m_elapsed_time;
        loop_clock.restart();

        m_fps = m_fps * (1.0f - m_fps_alpha) + (1.0f / m_elapsed_time.asSeconds()) * m_fps_alpha;

        m_window->pollEvents();

        while (accumulator >= m_tick_time) {
            m_animation_manager->update(m_tick_time);
            m_task_manager->update(m_tick_time);
            m_game_state_stack->update(m_tick_time);
            accumulator -= m_tick_time;
        }

        m_window->clear();
        m_game_state_stack->draw();
        m_window->display();
    }

    return 1;
}

void App::exit()
{
    m_running = false;
}

Time App::getRunningTime() const
{
    if (!m_running)
        return {0};
    return m_running_clock.getElapsedTime();
}

Time App::getElapsedTime() const
{
    return m_elapsed_time;
}

} // namespace ae
