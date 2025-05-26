#ifndef FIRST_STATE_H
#define FIRST_STATE_H

#include <ae/game_state.h>
#include <ae/graphics/core/quad.h>

using namespace ae;

// Начальное игровое состояние
// splash & загрузка ресурсов для главного меню
class FirstState : public GameState
{
public:
    FirstState();
    ~FirstState() = default;

    void onEnter();
    void onExit();

    void update(const Time &dt);
    void draw() const;

private:
    Quad m_render_quad;
};

#endif // FIRST_STATE_H
