#include "load_level_state.h"
#include "gameplay_state.h"

#include <ae/engine_context.h>
#include <ae/game_state_stack.h>
#include <ae/graphics/core/default_shaders.h>
#include <ae/graphics/scene/model.h>
#include <ae/gui/gui.h>
#include <ae/scene/scene.h>
#include <ae/system/log.h>
#include <ae/task.h>
#include <ae/task_manager.h>

LoadLevelState::LoadLevelState(EngineContext &engine_context)
    : GameState{engine_context}
    , m_current_task{0}
    , m_total_tasks{0}
{
    m_render_quad.create();
}

void LoadLevelState::onEnter()
{
    l_debug("LoadingLevelState::onEnter");

    auto &ctx = getEngineContext();
    m_load_level_gui = createGui();
    ctx.getGui()->push(m_load_level_gui);
    ctx.runLater([&]() { loadTestScene(); });
}

void LoadLevelState::onExit()
{
    l_debug("LoadingLevelState::onExit");

    auto &ctx = getEngineContext();
    ctx.getGui()->pop();
}

void LoadLevelState::update(const Time &dt)
{
    auto &ctx = getEngineContext();
    // Обновляем интерфейс
    ctx.getGui()->update(dt);
}

void LoadLevelState::draw(const Time &dt) const
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

s_ptr<LoadLevelGui> LoadLevelState::createGui()
{
    auto root = gui::Control::create<LoadLevelGui>(getEngineContext());
    return root;
}

void LoadLevelState::updateOutput(const String &string)
{
    m_load_level_gui->addString(string);
}

void LoadLevelState::loadTestScene()
{
    auto &ctx = getEngineContext();

    // Load textures
    struct TextureInfo
    {
        std::string name;
        std::string path;
        TextureType type = TextureType::DEFAULT;
    };

    std::vector<TextureInfo> textures = {{"54", "54.png"},
                                         {"cube_1_tex", "_.jpeg"},
                                         {"target", "target.png"},
                                         {"frame", "frame.png"},
                                         {"skybox",
                                          "skyboxes/StandardCubeMap.png",
                                          TextureType::CUBE_MAP}};

    auto load_textures_task_chain = createShared<TaskChain>();

    for (const auto &texture : textures) {
        load_textures_task_chain->addTask(createShared<CallbackTask>([&, texture]() {
            l_debug("Load texture: {}, {}", texture.name, texture.path);
            updateOutput(fmt::format("Load texture: {}, {}", texture.name, texture.path));
            ctx.getAssets()->loadFromFile<Texture>(texture.name, texture.path, texture.type);
        }));
    }

    // Load models
    struct ModelInfo
    {
        std::string name;
        std::string path;
    };

    std::vector<ModelInfo> models = {{"player", "walking_girl_cat_walk/scene.gltf"},
                                     {"level_model", "model3/1/untitled.gltf"}};

    auto load_models_task_chain = createShared<TaskChain>();

    for (const auto &model : models) {
        load_models_task_chain->addTask(createShared<CallbackTask>([&, model]() {
            l_debug("Load model: {}, {}", model.name, model.path);
            updateOutput(fmt::format("Load model: {}, {}", model.name, model.path));
            ctx.getAssets()->loadFromFile<Model>(model.name, model.path);
        }));
    }

    // Create level
    auto create_level_task = createShared<CallbackTask>([&]() {
        l_debug("Create level");
        updateOutput("\n### Create level ###");

        // Create player
        mat4 player_trasform{1.0f};
        player_trasform = glm::translate(player_trasform, vec3{4.0f, 6.0f, -20.0f});

        mat4 player_model_trasform{1.0f};
        player_model_trasform = glm::scale(player_model_trasform, vec3{0.6f});
        player_model_trasform = glm::rotate(player_model_trasform,
                                            glm::radians(90.0f),
                                            vec3{1.0f, 0.0f, 0.0f});

        auto player_model = ctx.getAssets()->get<Model>("player");
        ctx.getScene()->createPlayer(player_model, player_trasform, player_model_trasform);

        // Create level
        mat4 level_trasform{1.0f};
        level_trasform = glm::scale(level_trasform, vec3{1.5f});
        auto level_model = ctx.getAssets()->get<Model>("level_model");
        ctx.getScene()->createMeshNodeEntities(level_model->getRootNode(), level_trasform);

        // Create skybox
        auto skybox = createShared<Skybox>();
        auto skybox_texture = ctx.getAssets()->get<Texture>("skybox");
        skybox->create(skybox_texture);
        auto skybox_entity = ctx.getScene()->createSkybox(skybox);
        ctx.getScene()->setActiveSkybox(skybox_entity);
    });

    auto final_task_chain = createShared<NotifyTaskChain>();
    final_task_chain->addTask(
        createShared<CallbackTask>([&]() { updateOutput("\n### Load textures ###"); }));
    final_task_chain->addTask(load_textures_task_chain);
    final_task_chain->addTask(
        createShared<CallbackTask>([&]() { updateOutput("\n### Load models ###"); }));
    final_task_chain->addTask(load_models_task_chain);
    final_task_chain->addTask(create_level_task);
    final_task_chain->addTask(createShared<DelayTask>(seconds(0.2f)));

    final_task_chain->addTask(createShared<CallbackTask>([&]() {
        ctx.runLater([&]() { ctx.getGameStateStack()->replace(createShared<GameplayState>(ctx)); });
    }));

    final_task_chain->taskFinished.connect([this](int32_t finished_task, int32_t total_task) {
        float progress = static_cast<float>(finished_task + 1) / total_task;
        m_load_level_gui->setProgress(progress);
    });

    ctx.getTaskManager()->run(final_task_chain);
}
