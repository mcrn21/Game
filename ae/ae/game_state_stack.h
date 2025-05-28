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

    void push(const SharedPtr<GameState> &state);
    void replace(const SharedPtr<GameState> &state);
    void pop();
    void clear();

    void update(const Time &dt);
    void draw() const;

private:
    std::vector<SharedPtr<GameState>> m_states;
};

} // namespace ae

#endif // AE_GAME_STATE_STACK_H
