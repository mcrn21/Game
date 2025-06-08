#ifndef GAMEPLAY_STATE_H
#define GAMEPLAY_STATE_H

#include <ae/game_state.h>
#include <ae/graphics/core/quad.h>

using namespace ae;

class GameplayState : public GameState
{
public:
    GameplayState(EngineContext &engine_context);
    ~GameplayState() = default;

    void onEnter();
    void onExit();
    void onPause();
    void onResume();

    bool isTranslucent() const;

    void update(const Time &dt);
    void draw(const Time &dt) const;

private:
    bool m_paused;
    Quad m_render_quad;
};

#endif // GAMEPLAY_STATE_H
