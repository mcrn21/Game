#ifndef AE_GAME_STATE_H
#define AE_GAME_STATE_H

#include "engine_context_object.h"
#include "system/time.h"

namespace ae {

class GameState : public EngineContextObject
{
public:
    GameState(EngineContext &engine_context);
    virtual ~GameState() = default;

    virtual void onEnter();
    virtual void onExit();
    virtual void onPause();
    virtual void onResume();

    virtual void update(const Time &dt);
    virtual void draw(const Time &dt) const;

    // Нужно ли пропускать обновление ивентов для состояний под этим
    virtual bool isTransparent() const { return true; } // draw через
    virtual bool isTranslucent() const { return true; } // update через
};

} // namespace ae

#endif // AE_GAME_STATE_H
