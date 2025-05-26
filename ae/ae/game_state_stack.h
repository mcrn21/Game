#ifndef AE_GAME_STATE_STACK_H
#define AE_GAME_STATE_STACK_H

#include "game_state.h"
#include "system/time.h"

#include <memory>
#include <vector>

namespace ae {

class GameStateStack
{
public:
    GameStateStack();
    ~GameStateStack() = default;

    void push(const std::shared_ptr<GameState> &state);
    void replace(const std::shared_ptr<GameState> &state);
    void pop();
    void clear();

    void update(const Time &dt);
    void draw() const;

private:
    std::vector<std::shared_ptr<GameState>> m_states;
};

} // namespace ae

#endif // AE_GAME_STATE_STACK_H
