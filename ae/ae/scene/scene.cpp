#include "scene.h"
#include "../graphics/core/default_shaders.h"
#include "../graphics/scene/model_instance.h"
#include "../graphics/scene/shape.h"
#include "collisions_s.h"
#include "draw_s.h"
#include "lights_s.h"
#include "movement_s.h"
#include "player_s.h"

#include <glm/gtx/matrix_decompose.hpp>
#include <spdlog/spdlog.h>

namespace ae {

Scene::Scene()
    : SceneContext{&m_data}
{
    m_data.player_s = createUnique<Player_S>(this);
    m_data.lights_s = createUnique<Lights_S>(this);
    m_data.collisions_s = createUnique<Collisions_S>(this);
    m_data.movement_s = createUnique<Movement_S>(this);
    m_data.draw_s = createUnique<Draw_S>(this);

    // Camera
    m_data.registry.on_update<Camera_C>().connect<&Scene::onCameraUpdated>(this);
    setActiveCamera(createCamera());
    // Direct light
    setActiveDirectLight(createDirectLight());
    // Transform
    m_data.registry.on_update<Transform_C>().connect<&Scene::onTransformUpdated>(this);
    // Local AABB
    m_data.registry.on_update<LocalAABB_C>().connect<&Scene::onLocalAABBUpdated>(this);
    // Movement
    m_data.registry.on_construct<Movement_C>().connect<&Scene::onMovementConstructed>(this);
    m_data.registry.on_update<Movement_C>().connect<&Scene::onMovementUpdated>(this);

    for (int32_t i = 0; i < 10; ++i) {
        auto entity = createLight();

        auto &light_c = m_data.registry.get<Light_C>(entity);
        auto &transform_c = m_data.registry.get<Transform_C>(entity);

        // light_c.ambient = vec3{1.0f};
        light_c.diffuse = Color{0.0f, 1.0f, 0.0f};
        light_c.linear = 0.14f;
        light_c.quadratic = 0.07f;
        float angle = glm::radians(360.0f * (static_cast<float>(i) / 10));
        transform_c.position = vec3{std::cos(angle) * 10.0f - 2.0f,
                                    5.0f,
                                    std::sin(angle) * 10.0f - 30.f};

        auto sp = createShared<Shape>();
        sp->createSphere(0.2f);
        auto e = createDrawableEntity(sp);
        m_data.registry.get<Transform_C>(e).position = transform_c.position;
    }
}

Scene::~Scene()
{
    // m_data.render_target->sizeChanged.disconnect(this);
}

entt::registry &Scene::getRegistry()
{
    return m_data.registry;
}

void Scene::tickUpdate(const Time &elapsed_time)
{
    m_data.player_s->update();
    // m_data.collisions_s->update(elapsed_time);
    m_data.movement_s->update(elapsed_time);
    m_data.player_s->updateCameraPosition(elapsed_time);
    m_data.lights_s->update();
    m_data.draw_s->update();

    tickUpdated();

    m_data.camera_dirty = false;
    m_data.scene_dirty = false;
}

void Scene::draw() const
{
    m_data.render_texture.clear();
    drawSkybox();
    drawScene();
    m_data.render_texture.display();
}

void Scene::createPlayer(const s_ptr<Model> &model,
                         const mat4 &player_transform,
                         const mat4 &model_transform)
{
    auto player_entity = createTransformedEntity(player_transform);

    m_data.registry.emplace<Dynamic_C>(player_entity);

    auto &player_c = m_data.registry.emplace<Player_C>(player_entity);

    m_data.registry.emplace<Movement_C>(player_entity);

    player_c.camera_center = createTransformedEntity();
    get<Transform_C>(player_c.camera_center).position.y = 1.5f;
    addChild(player_entity, player_c.camera_center);

    player_c.camera = getActiveCamera();
    get<Transform_C>(player_c.camera).position.z = -10.0f;
    get<Transform_C>(player_c.camera).rotation.y = glm::radians(180.0f);
    // lookAt(player_c.camera, getGlobalPosition(player_c.camera_center));
    // patch<Transform_C>(player_c.camera);
    addChild(player_c.camera_center, player_c.camera);

    // // Camera entities
    // // Camera center
    // player_c.camera_center_entity = createTransformedEntity();
    // m_data.registry.get<Transform_C>(player_c.camera_center_entity).position.y = 1.0f;
    // addChild(player_entity, player_c.camera_center_entity);

    // // Camera look at point
    // player_c.camera_look_at_entity = createTransformedEntity();
    // m_data.registry.get<Transform_C>(player_c.camera_look_at_entity).position.z = 10.0f;
    // addChild(player_c.camera_center_entity, player_c.camera_look_at_entity);

    // // Camera offset
    // player_c.camera_position_entity = createTransformedEntity();
    // m_data.registry.get<Transform_C>(player_c.camera_position_entity).position.z
    //     = player_c.camera_offset;
    // addChild(player_c.camera_center_entity, player_c.camera_position_entity);

    // player_c.model_entity = createEntity(model->getRootNode(), entt::null, model_transform);

    player_c.model_entity = createModelEntity(model, model_transform);
    m_data.registry.emplace<Dynamic_C>(player_c.model_entity);
    addChild(player_entity, player_c.model_entity);

    auto &animator_c = m_data.registry.emplace<Animator_C>(player_c.model_entity);
    animator_c.animator = createShared<PoseAnimator>();
    animator_c.animator->setAnimation(model->getAnimation("Animation"));

    auto mi = staticPointerCast<ModelInstance>(
        m_data.registry.get<Drawable_C>(player_c.model_entity));

    animator_c.animator->setPose(mi->getPose());

    // animator_c.animator->play();

    auto &collider_c = m_data.registry.emplace<Collider_C>(player_entity);
    // collider_c = std::make_shared<SphereCollider>(vec3{0.0f}, 1.0f);
    collider_c = createShared<BoxCollider>(AABB{vec3{-1.0f}, vec3{1.0f}});

    m_data.player_s->setPlayerEntity(player_entity);

    player_c.light_entity = createLight(true, true);

    auto &light_c = m_data.registry.get<Light_C>(player_c.light_entity);
    light_c.linear = 0.022f;
    light_c.quadratic = 0.0019f;
    light_c.cut_off = std::cos(glm::radians(24.0f));
    light_c.outer_cut_off = std::cos(glm::radians(26.0f));

    get<Transform_C>(player_c.light_entity).rotation.y = glm::radians(180.0f);

    addChild(player_c.camera_center, player_c.light_entity);

    auto sp = createShared<Shape>();
    sp->createSphere(1.0f);
    sp->getMaterial()->color.setAlpha(0.2f);
    auto e = createDrawableEntity(sp);
    addChild(player_entity, e);

    spdlog::debug("Player: {}", static_cast<uint32_t>(player_c.model_entity));
}

void Scene::drawSkybox() const
{
    if (m_data.registry.valid(m_data.active_skybox)) {
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);
        glCullFace(GL_FRONT);

        Shader::use(*DefaultShaders::getSkybox());
        Shader *shader = DefaultShaders::getSkybox().get();

        shader->uniformMatrix("u_projMat",
                              const_cast<Scene *>(this)->getCameraProjTransform(getActiveCamera()));
        shader->uniformMatrix("u_viewMat",
                              mat4(mat3(const_cast<Scene *>(this)->getCameraViewTransform(
                                  getActiveCamera()))));

        const auto &skybox_c = m_data.registry.get<Skybox_C>(m_data.active_skybox);

        Texture::bind(*skybox_c->getTexture());

        skybox_c->draw();

        Texture::unbind();
        Shader::unuse();

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        glCullFace(GL_BACK);
    }
}

void Scene::drawShadow() const {}

void Scene::drawScene() const
{
    if (m_data.registry.valid(m_data.active_camera)) {
        Shader::use(*DefaultShaders::getMain());

        RenderState render_state;
        render_state.shader = DefaultShaders::getMain().get();

        render_state.shader->uniformMatrix("u_projMat",
                                           const_cast<Scene *>(this)->getCameraProjTransform(
                                               getActiveCamera()));
        render_state.shader->uniformMatrix("u_viewMat",
                                           const_cast<Scene *>(this)->getCameraViewTransform(
                                               getActiveCamera()));
        render_state.shader->uniformVec3("u_viewPos",
                                         const_cast<Scene *>(this)->getGlobalPosition(
                                             getActiveCamera()));

        m_data.lights_s->draw(render_state);
        m_data.draw_s->drawEntities(render_state);

        Shader::unuse();
    }
}

void Scene::onCameraUpdated(entt::registry &registry, entt::entity entity)
{
    auto &camera_c = registry.get<Camera_C>(entity);
    camera_c.dirty = true;
    m_data.camera_dirty = true;
}

void Scene::onTransformUpdated(entt::registry &registry, entt::entity entity)
{
    auto &transform_c = registry.get<Transform_C>(entity);
    transform_c.dirty = true;
    markGlobalTransformDirty(entity);
    m_data.scene_dirty = true;
}

void Scene::onLocalAABBUpdated(entt::registry &registry, entt::entity entity)
{
    auto &global_aabb_c = registry.get<GlobalAABB_C>(entity);
    global_aabb_c.dirty = true;
}

void Scene::onMovementConstructed(entt::registry &registry, entt::entity entity)
{
    const auto &movement_c = registry.get<Movement_C>(entity);
    if (glm::length(movement_c.velocity) != 0)
        registry.emplace<InMotion_C>(entity);
}

void Scene::onMovementUpdated(entt::registry &registry, entt::entity entity)
{
    const auto &movement_c = registry.get<Movement_C>(entity);
    if (glm::length(movement_c.velocity) != 0) {
        if (!registry.any_of<InMotion_C>(entity))
            registry.emplace<InMotion_C>(entity);
    } else
        registry.remove<InMotion_C>(entity);
}

} // namespace ae
