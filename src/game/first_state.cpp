#include "first_state.h"
#include "main_menu_state.h"

#include <ae/engine.h>
#include <ae/game_state_stack.h>
#include <ae/gui/gui.h>
#include <ae/task.h>
#include <ae/task_manager.h>

FirstState::FirstState(EngineContext &engine_context)
    : GameState{engine_context}
{
    m_render_quad.create();
}

void FirstState::onEnter()
{
    auto &ctx = getEngineContext();

    m_first_state_gui = createGui();
    ctx.getGui()->push(m_first_state_gui);

    // Load splash image and
    auto load_splash_texture_task = createShared<CallbackTask>([&]() {
        auto splash_texture = createShared<Texture>();
        if (splash_texture->loadFromFile("data/textures/splash.png"))
            m_first_state_gui->setSplashTexture(splash_texture);
    });

    // Load default font task
    auto load_default_font_task = createShared<CallbackTask>([&]() {
        auto font = createShared<Font>();
        if (font->loadFromFile("data/fonts/font1.ttf"))
            Gui::setDefaultFont(font);
    });

    //
    auto load_main_menu_sounds_task = createShared<CallbackTask>(
        [&]() { ctx.getAssets()->loadFromFile<SoundBuffer>("ft_btn", "data/sfx/ft_btn.ogg"); });

    // Show main menu task
    auto show_main_menu_task = createShared<CallbackTask>(
        [&]() { ctx.getGameStateStack()->replace(createShared<MainMenuState>(ctx)); });

    auto task_chain = createShared<TaskChain>();
    task_chain->addTask(load_splash_texture_task);
    task_chain->addTask(load_default_font_task);
    task_chain->addTask(load_main_menu_sounds_task);
    task_chain->addTask(show_main_menu_task);

    ctx.getTaskManager()->run(task_chain);
}

void FirstState::onExit()
{
    auto &ctx = getEngineContext();
    ctx.getGui()->pop();
}

void FirstState::update(const Time &dt)
{
    auto &ctx = getEngineContext();
    // Обновляем интерфейс
    ctx.getGui()->update(dt);
}

void FirstState::draw() const
{
    auto &ctx = getEngineContext();

    // Сцена и интерфейс рендерятся в текстуры
    ctx.getGui()->draw();

    // Рендерем текстуры
    glDisable(GL_DEPTH_TEST);
    m_render_quad.draw(ctx.getGui()->getRenderTexture().getSampledTexture());
    glEnable(GL_DEPTH_TEST);
}

s_ptr<FirstStateGui> FirstState::createGui()
{
    auto root = gui::Control::create<FirstStateGui>(getEngineContext());
    return root;
}
