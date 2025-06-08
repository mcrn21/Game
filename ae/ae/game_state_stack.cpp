#include "game_state_stack.h"

namespace ae {

GameStateStack::GameStateStack() {}

void GameStateStack::push(const s_ptr<GameState> &state)
{
    if (!state)
        return;

    if (!m_states.empty())
        m_states.back()->onPause();

    m_states.push_back(state);
    state->onEnter();
}

void GameStateStack::replace(const s_ptr<GameState> &state)
{
    if (!state)
        return;

    pop();

    m_states.push_back(state);
    state->onEnter();
}

void GameStateStack::pop()
{
    if (m_states.empty())
        return;

    m_states.back()->onExit();
    m_states.pop_back();

    if (!m_states.empty())
        m_states.back()->onResume();
}

void GameStateStack::clear()
{
    m_states.clear();
}

void GameStateStack::update(const Time &dt)
{
    for (auto it = m_states.rbegin(); it != m_states.rend(); ++it) {        
        if (!(*it)->isTranslucent())
            break;
        (*it)->update(dt);
    }
}

void GameStateStack::draw(const Time &dt) const
{
    for (auto it = m_states.begin(); it != m_states.end(); ++it) {
        (*it)->draw(dt);
        if (!(*it)->isTransparent())
            break; // нижние состояния не рисовать
    }
}

} // namespace ae
