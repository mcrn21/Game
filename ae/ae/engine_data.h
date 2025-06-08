#ifndef AE_ENGINE_DATA_H
#define AE_ENGINE_DATA_H

#include "system/clock.h"
#include "system/memory.h"
#include "system/time.h"

namespace ae {

class Window;
class Assets;
class Scene;
class Gui;
class GameStateStack;
class TaskManager;
class AnimationManager;
class InputActionManager;
class AudioDevice;
class Input;

struct EngineData
{
    u_ptr<Window> window;
    u_ptr<Assets> assets;
    u_ptr<Scene> scene;
    u_ptr<Gui> gui;
    u_ptr<GameStateStack> game_state_stack;
    u_ptr<TaskManager> task_manager;
    u_ptr<AnimationManager> animation_manager;
    u_ptr<InputActionManager> input_action_manager;
    u_ptr<AudioDevice> audio_device;

    Time tick_time;      // Интервал одно тика
    Clock running_clock; // Время сначала запуска
    Time elapsed_time;   // Время одного кадра

    int32_t fps;
    const float fps_alpha = 0.3f; // Для сглаживания fps
};

} // namespace ae

#endif // AE_ENGINE_DATA_H
