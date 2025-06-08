#ifndef AE_ENGINE_CONTEXT_H
#define AE_ENGINE_CONTEXT_H

#include "config.h"
#include "engine_data.h"

#include <functional>

namespace ae {

class EngineContext
{
public:
    EngineContext();
    ~EngineContext() = default;

    Window *getWindow() const;
    Input *getInput() const;
    Assets *getAssets() const;
    Scene *getScene() const;
    Gui *getGui() const;
    GameStateStack *getGameStateStack() const;
    TaskManager *getTaskManager() const;
    AnimationManager *getAnimationManager() const;
    InputActionManager *getInputActionManager() const;
    AudioDevice *getAudioDevice() const;

    int32_t getFps() const;
    Time getRunningTime() const;
    Time getElapsedTime() const;

    // Создает CallbackTask и запускает
    void runLater(const std::function<void()> &callback);
    void runLater(const Time &delay, const std::function<void()> &callback);

    template<typename T>
    void deleteLater(const s_ptr<T> &value)
    {
        runLater([value]() {});
    }

    template<typename T>
    void deleteLater(const Time &delay, const s_ptr<T> &value)
    {
        runLater(delay, [value]() {});
    }

    virtual void exit();

protected:
    bool init(const Config &config);

protected:
    EngineData m_data;
};

} // namespace ae

#endif // AE_ENGINE_CONTEXT_H
