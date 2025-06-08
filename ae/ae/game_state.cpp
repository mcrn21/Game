#include "game_state.h"

namespace ae {

GameState::GameState(EngineContext &engine_context)
    : EngineContextObject{engine_context}
{}

void GameState::onEnter() {}

void GameState::onExit() {}

void GameState::onPause() {}

void GameState::onResume() {}

void GameState::update(const Time &dt) {}

void GameState::draw(const Time &dt) const {}

} // namespace ae
