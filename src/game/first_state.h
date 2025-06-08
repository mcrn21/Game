#ifndef FIRST_STATE_H
#define FIRST_STATE_H

#include "gui/first_state_gui.h"

#include <ae/game_state.h>
#include <ae/graphics/core/quad.h>
#include <ae/graphics/core/texture.h>

using namespace ae;

// Начальное игровое состояние
// splash & загрузка ресурсов для главного меню
class FirstState : public GameState
{
public:
    FirstState(EngineContext &engine_context);
    ~FirstState() = default;

    void onEnter();
    void onExit();

    void update(const Time &dt);
    void draw() const;

private:
    s_ptr<FirstStateGui> createGui();

private:
    s_ptr<FirstStateGui> m_first_state_gui;
    s_ptr<Texture> m_splash_texture;

    Quad m_render_quad;
};

#endif // FIRST_STATE_H
