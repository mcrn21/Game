#include "engine_context.h"

#include "animation_manager.h"
#include "assets/assets.h"
#include "audio/audio_device.h"
#include "game_state_stack.h"
#include "gui/gui.h"
#include "input_action_manager.h"
#include "scene/scene.h"
#include "task_manager.h"
#include "window/input.h"
#include "window/window.h"

namespace ae {

EngineContext::EngineContext()
{
    m_data.assets = createUnique<Assets>();
    m_data.tick_time = seconds(1.0f / 60.0f);
    m_data.fps = 0;
}

Window *EngineContext::getWindow() const
{
    return m_data.window.get();
}

Input *EngineContext::getInput() const
{
    return m_data.input.get();
}

Assets *EngineContext::getAssets() const
{
    return m_data.assets.get();
}

Scene *EngineContext::getScene() const
{
    return m_data.scene.get();
}

Gui *EngineContext::getGui() const
{
    return m_data.gui.get();
}

GameStateStack *EngineContext::getGameStateStack() const
{
    return m_data.game_state_stack.get();
}

TaskManager *EngineContext::getTaskManager() const
{
    return m_data.task_manager.get();
}

AnimationManager *EngineContext::getAnimationManager() const
{
    return m_data.animation_manager.get();
}

InputActionManager *EngineContext::getInputActionManager() const
{
    return m_data.input_action_manager.get();
}

AudioDevice *EngineContext::getAudioDevice() const
{
    return m_data.audio_device.get();
}

int32_t EngineContext::getFps() const
{
    return m_data.fps;
}

Time EngineContext::getRunningTime() const
{
    return m_data.running_clock.getElapsedTime();
}

Time EngineContext::getElapsedTime() const
{
    return m_data.elapsed_time;
}

void EngineContext::runLater(const std::function<void()> &callback)
{
    m_data.task_manager->run(createShared<CallbackTask>(callback));
}

void EngineContext::runLater(const Time &delay, const std::function<void()> &callback)
{
    auto task_chain = createShared<TaskChain>();
    task_chain->addTask(createShared<DelayTask>(delay));
    task_chain->addTask(createShared<CallbackTask>(callback));
    m_data.task_manager->run(task_chain);
}

void EngineContext::exit() {}

bool EngineContext::init(const Config &config)
{
    m_data.tick_time = seconds(1.0f / static_cast<float>(config.game_frame_rate));

    // Input
    m_data.input = createUnique<Input>();

    m_data.window = createUnique<Window>(*this);
    if (!m_data.window->create(ivec2{config.window_width, config.window_height},
                               config.window_title,
                               config.msaa))
        return false;

    // Audio device
    m_data.audio_device = createUnique<AudioDevice>();

    // Task manager
    m_data.task_manager = createUnique<TaskManager>();

    // Animation manager
    m_data.animation_manager = createUnique<AnimationManager>();

    // Scene
    m_data.scene = createUnique<Scene>(*this);
    m_data.scene->setRenderTextureSize(ivec2{config.window_width, config.window_height});
    m_data.window->sizeChanged.connect(&Scene::setRenderTextureSize, m_data.scene.get());

    // Gui
    m_data.gui = createUnique<Gui>(*this);
    m_data.gui->setRenderTextureSize(ivec2{config.window_width, config.window_height});
    m_data.window->sizeChanged.connect(&Gui::setRenderTextureSize, m_data.gui.get());

    m_data.input->buttonJustDown.connect(&Gui::onButtonPressed, m_data.gui.get());
    m_data.input->buttonJustUp.connect(&Gui::onButtonReleased, m_data.gui.get());
    m_data.input->cursorMoved.connect(&Gui::onCursorMoved, m_data.gui.get());
    m_data.input->keyJustDown.connect(&Gui::onKeyPressed, m_data.gui.get());
    m_data.input->keyDown.connect(&Gui::onKeyHeld, m_data.gui.get());
    m_data.input->keyJustUp.connect(&Gui::onKeyReleased, m_data.gui.get());
    m_data.input->codepointInputed.connect(&Gui::onCodepointInputed, m_data.gui.get());

    // Game state stack
    m_data.game_state_stack = createUnique<GameStateStack>();

    // Game input action manager
    m_data.input_action_manager = createUnique<InputActionManager>(*this);

    return true;
}

} // namespace ae
