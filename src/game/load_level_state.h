#ifndef LOAD_LEVEL_STATE_H
#define LOAD_LEVEL_STATE_H

#include "gui/load_level_gui.h"

#include <ae/game_state.h>
#include <ae/graphics/core/quad.h>

using namespace ae;

class LoadLevelState : public GameState
{
public:
    LoadLevelState(EngineContext &engine_context);
    ~LoadLevelState() = default;

    void onEnter();
    void onExit();

    void update(const Time &dt);
    void draw(const Time &dt) const;

private:
    s_ptr<LoadLevelGui> createGui();
    void updateOutput(const String &string);
    void loadTestScene();

private:
    s_ptr<LoadLevelGui> m_load_level_gui;

    int32_t m_current_task;
    int32_t m_total_tasks;

    Quad m_render_quad;
};

#endif // LOAD_LEVEL_STATE_H
