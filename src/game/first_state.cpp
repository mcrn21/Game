#include "first_state.h"
#include "main_menu_state.h"

#include <ae/assets/asset_loaders.h>
#include <ae/engine.h>
#include <ae/game_state_stack.h>
#include <ae/graphics/core/default_shaders.h>
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

    // Загрузка ассетов для начальной графики
    loadFirstAssets();
    m_first_state_gui->createFakeTerminal();

    // Грузим и показываем все остальное
    auto task_chain = createShared<NotifyTaskChain>();
    addIntroOutputTasks(task_chain);
    addLoadOtherAssetsTasks(task_chain);

    // // Show main menu task
    task_chain->addTask(createShared<CallbackTask>([&]() {
        ctx.runLater([&]() { ctx.getGameStateStack()->replace(createShared<MainMenuState>(ctx)); });
    }));

    task_chain->taskFinished.connect([this](int32_t finished_task, int32_t total_task) {
        float progress = static_cast<float>(finished_task + 1) / total_task;
        m_first_state_gui->setProgress(progress);
    });

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

void FirstState::draw(const Time &dt) const
{
    auto &ctx = getEngineContext();

    // Сцена и интерфейс рендерятся в текстуры
    ctx.getGui()->draw();

    // Рендерем текстуры
    glDisable(GL_DEPTH_TEST);
    m_render_quad.draw(ctx.getGui()->getRenderTexture().getSampledTexture(),
                       dt.asSeconds(),
                       DefaultShaders::getOldTerminalScreenQuad().get());
    glEnable(GL_DEPTH_TEST);
}

void FirstState::loadFirstAssets()
{
    auto &ctx = getEngineContext();

    // Load splash
    auto splash_texture = ctx.getAssets()->loadFromFile<Texture>("splash", "data/gfx/splash.png");
    if (splash_texture)
        m_first_state_gui->setSplashTexture(splash_texture);

    // Load font
    auto font = createShared<Font>();
    if (font->loadFromFile("data/fonts/font1.ttf"))
        Gui::setDefaultFont(font);
}

void FirstState::addLoadOtherAssetsTasks(s_ptr<NotifyTaskChain> &task_chain)
{
    auto &ctx = getEngineContext();
    task_chain->addTask(createShared<CallbackTask>(
        [&]() { ctx.getAssets()->loadFromFile<SoundBuffer>("ft_btn", "data/sfx/ft_btn.ogg"); }));
}

void FirstState::addIntroOutputTasks(s_ptr<NotifyTaskChain> &task_chain)
{
    std::string intro_text =
        R"(:: NULLEX/OS :: CORE INITIALIZATION
-------------------------------------------

[BOOT] Kernel integrity............ OK  
[BOOT] Executing cold start protocol  
[BOOT] System clock............... UNSYNCED  
[BOOT] Network link............... UNREACHABLE  
[BOOT] Peripheral map............. INCOMPLETE  
[BOOT] Core modules............... PARTIALLY LOADED  
[BOOT] Access node................ LOCKED  
[BOOT] Audio/Visual layer......... SUPPRESSED

>> NOTICE: No trusted signature found
>> WARNING: Unauthorized shell activity detected  
>> WARNING: Memory state: UNKNOWN  
>> WARNING: BIOS checksum mismatch  
>> ALERT: Unexpected I/O on channel_3a  

[RECOVERY] Booting in failsafe mode  
[LOGIN] Guest access granted (lvl: observer)  
[SESSION] Failsafe shell active  
[STATE] Logging disabled  
[TRACE] External signal detected — source: UNRESOLVED

---------------------------------------------------
> Loading resources...)";

    for (auto &&part : std::views::split(intro_text, '\n')) {
        String string{std::string(part.begin(), part.end())};

        task_chain->addTask(createShared<CallbackTask>(
            [this, string]() { m_first_state_gui->appendStringToCmdOutput(string); }));
        // task_chain->addTask(createShared<DelayTask>(seconds(0.1)));
    }
}

s_ptr<FirstStateGui> FirstState::createGui()
{
    auto root = gui::Control::create<FirstStateGui>(getEngineContext());
    return root;
}
