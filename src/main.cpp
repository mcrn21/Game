#include "game/gameplay_state.h"
#include "game/main_menu_state.h"

#include <ae/app.h>

#include <ae/graphics/core/default_shaders.h>
#include <ae/graphics/core/material.h>
#include <ae/graphics/core/shader.h>
#include <ae/graphics/core/texture.h>
#include <ae/graphics/core/vertex.h>
#include <ae/graphics/core/vertex_array.h>
#include <ae/graphics/scene/model.h>
#include <ae/graphics/scene/shape.h>
#include <ae/graphics/scene/skybox.h>
#include <ae/graphics/scene/sprite.h>
#include <ae/scene/components.h>
#include <ae/system/memory.h>

#include <spdlog/spdlog.h>

using namespace ae;

struct TEST_S : public EnableSharedFromThis<TEST_S>
{
    virtual ~TEST_S() { spdlog::debug("TEST_S destroy"); }
    int32_t a = 123;
};

struct TEST_S_2 : public TEST_S
{
    ~TEST_S_2() { spdlog::debug("TEST_S_2 destroy"); }
    int32_t b = 1005;
};

template<typename T>
struct CustomDeleter
{
    void operator()(T *ptr) const noexcept
    {
        spdlog::debug("PIZDA");
        ptr->~T();
    }
};

int32_t main()
{
    spdlog::set_level(spdlog::level::debug);

    auto maybe_config = Config::loadFromFile("config.toml");
    if (!maybe_config)
        return 0;

    auto &app = App::getInstance();
    app.create(maybe_config.value());

    WeakPtr<void> weak_test_1;
    {
        // spdlog::debug("Test: {}", EnableSharedFromThisBase<TEST_S>::value);

        SharedPtr<void> test_2 = SharedPtr<TEST_S_2>::create<CustomDeleter<TEST_S_2>>();
        SharedPtr<TEST_S> test_3 = staticPointerCast<TEST_S>(test_2);
        spdlog::debug("test_3: {}", test_3->a);

        weak_test_1 = test_2;
        auto test_4 = weak_test_1.lock();
        auto test_5 = staticPointerCast<TEST_S_2>(test_2);
        spdlog::debug("test_5: {}", test_5->a);
    }

    auto test_5 = weak_test_1.lock();
    spdlog::debug("test_5: {}", test_5.get() != nullptr);

    // return 1;

    //////

    auto main_menu_state = std::make_shared<MainMenuState>();
    // auto gameplay_state = std::make_shared<GameplayState>();

    // std::static_pointer_cast<gui::Control>(main_menu_state);

    app.getGameStateStack()->push(main_menu_state);
    // app.getGameStateStack()->replace(gameplay_state);

    ///

    auto texture1 = app.getAssets()->loadFromFile<Texture>("54", "54.png");
    auto cube_1_tex = app.getAssets()->loadFromFile<Texture>("cube_1_tex", "_.jpeg");
    app.getAssets()->loadFromFile<Texture>("target", "target.png");
    app.getAssets()->loadFromFile<Texture>("frame", "frame.png");

    auto skybox_texture = app.getAssets()->loadFromFile<Texture>("skybox",
                                                                 "skyboxes/StandardCubeMap.png",
                                                                 TextureType::CUBE_MAP);

    // auto player = app.getAssets()->loadFromFile<Model>("fiona_low_poly",
    //                                                    "fiona_low_poly/scene.gltf");
    auto player = app.getAssets()->loadFromFile<Model>("low_poly_girl_2",
                                                       "walking_girl_cat_walk/scene.gltf");

    // auto player = app.getAssets()->loadFromFile<Model>("low_poly_girl_2",
    //                                                    "low_poly_girl/scene.gltf");

    ///

    auto level_model = app.getAssets()->loadFromFile<Model>("level_model", "model3/1/untitled.gltf");
    // auto level_model = app.getAssets()->loadFromFile<Model>("level_model", "test_map/scene.glb");

    auto girl_model = app.getAssets()->loadFromFile<Model>("girl_model",
                                                           "walking_girl_cat_walk/scene.gltf");
    auto box_model = app.getAssets()->loadFromFile<Model>("box_model", "box/scene.gltf");

    auto &registry = app.getScene()->getRegistry();

    entt::entity entity_id;
    entt::entity girl;

    app.created.connect([&]() {
        mat4 player_trasform{1.0f};
        player_trasform = glm::translate(player_trasform, vec3{4.0f, 6.0f, -20.0f});
        // player_trasform = glm::translate(player_trasform, vec3{34.0f, 0.0f, 10.0f});

        mat4 player_model_trasform{1.0f};
        // player_model_trasform = glm::scale(player_model_trasform, vec3{0.1f});
        player_model_trasform = glm::scale(player_model_trasform, vec3{0.6f});
        player_model_trasform = glm::rotate(player_model_trasform,
                                            glm::radians(90.0f),
                                            vec3{1.0f, 0.0f, 0.0f});

        app.getScene()->createPlayer(player, player_trasform, player_model_trasform);

        mat4 level_trasform{1.0f};
        level_trasform = glm::scale(level_trasform, vec3{1.5f});
        app.getScene()->createMeshNodeEntities(level_model->getRootNode(), level_trasform);

        // auto girl = app.getScene()->createMeshNodeEntity(girl_model->getRootNode());
        // registry.patch<Transform_C>(girl, [](auto &transform_c) {
        //     transform_c.scale = {1.5f, 1.5f, 1.5f};
        //     transform_c.rotation.x = glm::radians(0.0f);
        // });

        // auto box = app.getScene()->createMeshNodeEntity(box_model->getRootNode());

        {
            auto cube_1 = std::make_shared<Shape>();
            cube_1->createTriangle(1.0f); //createCube(1.0f);
            cube_1->getMaterial()->diffuse_texture = cube_1_tex;

            mat4 cube1_trasform{1.0f};
            cube1_trasform = glm::scale(cube1_trasform, vec3{3.0f});
            cube1_trasform = glm::rotate(cube1_trasform,
                                         glm::radians(180.0f),
                                         vec3{0.0f, 1.0f, 0.0f});

            auto cube1 = app.getScene()->createDrawableEntity(cube_1, cube1_trasform);
            auto &collider_c = registry.emplace<Collider_C>(cube1);
            collider_c = std::make_shared<MeshCollider>(cube_1->getTriangles(), cube1_trasform);

            mat4 cube2_trasform{1.0f};
            cube2_trasform = glm::scale(cube2_trasform, vec3{3.0f});
            cube2_trasform = glm::rotate(cube2_trasform,
                                         glm::radians(180.0f),
                                         vec3{0.0f, 0.0f, 1.0f});
            cube2_trasform = glm::rotate(cube2_trasform,
                                         glm::radians(180.0f),
                                         vec3{0.0f, 1.0f, 0.0f});
            cube2_trasform = glm::rotate(cube2_trasform,
                                         glm::radians(90.0f),
                                         vec3{1.0f, 0.0f, 0.0f});
            cube2_trasform = glm::translate(cube2_trasform, vec3{0.0f, 0.5f, -0.5f});

            auto cube2 = app.getScene()->createDrawableEntity(cube_1, cube2_trasform);
            auto &collider_2_c = registry.emplace<Collider_C>(cube2);
            collider_2_c = std::make_shared<MeshCollider>(cube_1->getTriangles(), cube2_trasform);
        }

        // Skybox
        auto skybox = std::make_shared<Skybox>();
        skybox->create(skybox_texture);
        auto skybox_entity = app.getScene()->createSkybox(skybox);
        app.getScene()->setActiveSkybox(skybox_entity);
    });

    return app.exec();
}
