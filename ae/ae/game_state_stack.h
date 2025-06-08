#ifndef AE_GAME_STATE_STACK_H
#define AE_GAME_STATE_STACK_H

#include "game_state.h"
#include "system/memory.h"
#include "system/time.h"

#include <vector>

namespace ae {

class GameStateStack
{
public:
    GameStateStack();
    ~GameStateStack() = default;

    void push(const s_ptr<GameState> &state);
    void replace(const s_ptr<GameState> &state);
    void pop();
    void clear();

    void update(const Time &dt);
    void draw(const Time &dt) const;

private:
    std::vector<s_ptr<GameState>> m_states;
};

} // namespace ae

#endif // AE_GAME_STATE_STACK_H
