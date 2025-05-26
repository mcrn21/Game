#ifndef AE_APP_H
#define AE_APP_H

#include "animation_manager.h"
#include "assets/asset_loaders.h"
#include "assets/assets.h"
#include "config.h"
#include "game_state_stack.h"
#include "gui/gui.h"
#include "scene/scene.h"
#include "system/clock.h"
#include "task_manager.h"
#include "window/glfw_manager.h"
#include "window/input.h"
#include "window/window.h"

#include "../3rd/sigslot/signal.hpp"

namespace ae {

class App
{
public:
    App();
    ~App() = default;

    static App &getInstance();

    bool create(const Config &config = {});

    Config &getConfig();

    Window *getWindow() const;
    Input *getInput() const;
    Assets *getAssets() const;
    Scene *getScene() const;
    Gui *getGui() const;
    GameStateStack *getGameStateStack() const;
    TaskManager *getTaskManager() const;
    AnimationManager *getAnimationManager() const;

    int32_t getFps() const;

    int32_t exec();
    void exit();

    Time getRunningTime() const;
    Time getElapsedTime() const;

public:
    sigslot::signal<> created;

private:
    static GLFWManager m_glfw_manager;

    Config m_config;

    bool m_running;
    Time m_tick_time;
    Clock m_running_clock;
    Time m_elapsed_time;

    int32_t m_fps;
    const float m_fps_alpha = 0.3f;

    std::unique_ptr<Window> m_window;
    std::unique_ptr<Assets> m_assets;
    std::unique_ptr<Scene> m_scene;
    std::unique_ptr<Gui> m_gui;
    std::unique_ptr<GameStateStack> m_game_state_stack;
    std::unique_ptr<TaskManager> m_task_manager;
    std::unique_ptr<AnimationManager> m_animation_manager;
};

} // namespace ae

#endif // AE_APP_H
