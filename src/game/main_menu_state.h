#ifndef MAIN_MENU_STATE_H
#define MAIN_MENU_STATE_H

#include "gui/main_menu.h"

#include <ae/game_state.h>
#include <ae/graphics/core/quad.h>
#include <ae/gui/control.h>

using namespace ae;

class MainMenuState : public GameState
{
public:
    MainMenuState(bool in_gameplay = false);
    ~MainMenuState() = default;

    void onEnter();
    void onExit();

    void update(const Time &dt);
    void draw() const;

private:
    s_ptr<MainMenu> createGui();
    void setupKeyBindings();

private:
    s_ptr<MainMenu> m_main_menu;
    bool m_gameplay;
    Quad m_render_quad;
};

#endif // MAIN_MENU_STATE_H
