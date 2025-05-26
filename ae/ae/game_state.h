#ifndef AE_GAME_STATE_H
#define AE_GAME_STATE_H

#include "system/time.h"

namespace ae {

class GameState
{
public:
    GameState();
    virtual ~GameState() = default;

    virtual void onEnter();
    virtual void onExit();
    virtual void onPause();
    virtual void onResume();

    virtual void update(const Time &dt);
    virtual void draw() const;

    // Нужно ли пропускать обновление ивентов для состояний под этим
    virtual bool isTransparent() const { return true; } // draw через
    virtual bool isTranslucent() const { return true; } // update через        
};

} // namespace ae

#endif // AE_GAME_STATE_H
