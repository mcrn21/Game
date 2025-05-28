#include "scene_context.h"
#include "../common/consts.h"
#include "../common/glm_utils.h"
#include "../graphics/scene/model_instance.h"
#include "components.h"

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace ae {

SceneContext::SceneContext(SceneData *data)
    : m_data{data}
{}

const RenderTexture &SceneContext::getRenderTexture() const
{
    return m_data->render_texture;
}

void SceneContext::setRenderTextureSize(const ivec2 &size)
{
    m_data->render_texture.create(size);

    // Update all cameras ratio
    m_data->registry.view<Camera_C>().each([&](auto entity, auto &camera_c) {
        camera_c.ratio = static_cast<float>(size.x) / size.y;
        camera_c.dirty = true;
    });
}

entt::entity SceneContext::createTransformedEntity(const mat4 &transform)
{
    auto entity = m_data->registry.create();

    auto &transform_c = m_data->registry.emplace<Transform_C>(entity);
    glm_utils::decomposeTransform(transform,
                                  transform_c.position,
                                  transform_c.rotation,
                                  transform_c.scale);

    m_data->registry.emplace<GlobalTransform_C>(entity);
    m_data->registry.emplace<TransformInheritance_C>(entity);

    return entity;
}

entt::entity SceneContext::createBoundedEntity(const mat4 &transform, const AABB &aabb)
{
    auto entity = createTransformedEntity(transform);

    auto &local_aabb_c = m_data->registry.emplace<LocalAABB_C>(entity);
    local_aabb_c.aabb = aabb;
    m_data->registry.emplace<GlobalAABB_C>(entity);

    return entity;
}

entt::entity SceneContext::createDrawableEntity(const SharedPtr<Drawable> &drawable,
                                                const mat4 &transform,
                                                entt::entity parent)
{
    if (!drawable)
        return entt::null;

    auto entity = createBoundedEntity(transform, drawable->getAABB());

    m_data->registry.emplace<Drawable_C>(entity, drawable);

    if (parent != entt::null)
        addChild(parent, entity);

    return entity;
}

entt::entity SceneContext::createMeshNodeEntity(const SharedPtr<MeshNode> &mesh_node,
                                                const mat4 &transform,
                                                entt::entity parent)
{
    if (!mesh_node)
        return entt::null;

    auto entity = createTransformedEntity(
        parent == entt::null ? transform * mesh_node->getTransform() : mesh_node->getTransform());

    if (parent != entt::null)
        addChild(parent, entity);

    for (const auto &mesh : mesh_node->getMeshes())
        auto child = createDrawableEntity(mesh, mat4{1.0f}, entity);

    for (const auto &child_mesh_node : mesh_node->getChildren())
        createMeshNodeEntity(child_mesh_node, mat4{1.0f}, entity);

    return entity;
}

entt::entity SceneContext::createModelEntity(const SharedPtr<Model> &model,
                                             const mat4 &transform,
                                             entt::entity parent)
{
    if (!model)
        return entt::null;

    return createDrawableEntity(SharedPtr<ModelInstance>::create(model), transform, parent);
}

void SceneContext::createMeshNodeEntities(const SharedPtr<MeshNode> &mesh_node,
                                          const mat4 &transform)
{
    if (!mesh_node)
        return;

    auto new_transform = transform * mesh_node->getTransform();

    for (const auto &mesh : mesh_node->getMeshes()) {
        auto entity = createDrawableEntity(mesh, new_transform);

        auto &collider_c = m_data->registry.emplace<Collider_C>(entity);
        collider_c = SharedPtr<MeshCollider>::create(mesh->getTriangles(), new_transform);
    }

    for (const auto &child_mesh_node : mesh_node->getChildren())
        createMeshNodeEntities(child_mesh_node, new_transform);
}

void SceneContext::destroyEntity(entt::entity entity)
{
    if (!isValid(entity))
        return;

    // Нельзя удалять активную камеру
    if (has<Camera_C>(entity) && entity == m_data->active_camera)
        return;

    // Нельзя удалять активны направленный свет
    if (has<DirectLight_C>(entity) && entity == m_data->active_direct_light)
        return;

    // Нельзя удалять активный скайбокс
    if (has<Skybox_C>(entity) && entity == m_data->active_skybox)
        return;

    if (has<Children_C>(entity)) {
        auto &children_c = get<Children_C>(entity);
        for (auto child : children_c.children)
            destroyEntity(child);
    }

    if (has<Parent_C>(entity)) {
        auto &parent_c = get<Parent_C>(entity);

        if (isValid(parent_c.parent)) {
            auto &parent_children_c = get<Children_C>(parent_c.parent);

            parent_children_c.children.erase(std::find(parent_children_c.children.begin(),
                                                       parent_children_c.children.end(),
                                                       entity),
                                             parent_children_c.children.end());
        }
    }

    m_data->registry.destroy(entity);
}

bool SceneContext::isValid(entt::entity entity) const
{
    return m_data->registry.valid(entity);
}

entt::entity SceneContext::getParent(entt::entity entity) const
{
    return has<Parent_C>(entity) ? get<Parent_C>(entity).parent : entt::null;
}

const std::vector<entt::entity> &SceneContext::getChildren(entt::entity entity) const
{
    if (!has<Children_C>(entity)) {
        static const std::vector<entt::entity> empty;
        return empty;
    }
    return get<Children_C>(entity).children;
}

void SceneContext::addChild(entt::entity entity, entt::entity child)
{
    if (!isValid(entity) || !isValid(child)
        || (has<Parent_C>(child) && get<Parent_C>(child).parent == entity))
        return;

    if (has<Parent_C>(child))
        removeChild(get<Parent_C>(child).parent, child);

    if (!has<Children_C>(entity))
        m_data->registry.emplace<Children_C>(entity);

    get<Children_C>(entity).children.push_back(child);
    m_data->registry.emplace_or_replace<Parent_C>(child, Parent_C{entity});

    if (has<Dynamic_C>(entity) && !has<Dynamic_C>(child))
        propagateDynamic(child);

    markGlobalTransformDirty(child);
}

void SceneContext::removeChild(entt::entity entity, entt::entity child)
{
    if (!isValid(entity) || !isValid(child) || !has<Children_C>(entity) || !has<Parent_C>(child)
        || (has<Parent_C>(child) && get<Parent_C>(child).parent != entity))
        return;

    auto &children_c = get<Children_C>(entity);
    children_c.children.erase(std::find(children_c.children.begin(),
                                        children_c.children.end(),
                                        child),
                              children_c.children.end());

    if (children_c.children.empty())
        m_data->registry.remove<Children_C>(entity);

    m_data->registry.remove<Parent_C>(child);
}

const mat4 &SceneContext::getLocalTransform(entt::entity entity) const
{
    if (!has<Transform_C>(entity))
        return INVALID_TRANSFORM;

    auto &transform_c = const_cast<SceneContext *>(this)->get<Transform_C>(entity);

    if (transform_c.dirty) {
        transform_c.transform = glm::translate(glm::mat4(1.0f), transform_c.position)
                                * glm::toMat4(glm::quat(transform_c.rotation))
                                * glm::scale(glm::mat4(1.0f), transform_c.scale);
        transform_c.dirty = false;
    }

    return transform_c.transform;
}

const mat4 &SceneContext::getGlobalTransform(entt::entity entity) const
{
    if (!has<GlobalTransform_C, Transform_C>(entity))
        return INVALID_TRANSFORM;

    if (!has<GlobalTransform_C>(entity))
        return get<Transform_C>(entity).transform;

    auto &global_transform_c = const_cast<SceneContext *>(this)->get<GlobalTransform_C>(entity);
    if (!global_transform_c.dirty)
        return global_transform_c.transform;

    mat4 parent_transform = mat4{1.0f};
    if (has<Parent_C>(entity)) {
        const mat4 &parent_global = getGlobalTransform(get<Parent_C>(entity).parent);

        int32_t flags = static_cast<int32_t>(TransformInheritance_C::ALL);

        if (has<TransformInheritance_C>(entity))
            flags = get<TransformInheritance_C>(entity).mode;

        parent_transform = buildInheritedTransform(parent_global, flags);
    }

    global_transform_c.transform = parent_transform * getLocalTransform(entity);

    glm_utils::decomposeTransform(global_transform_c.transform,
                                  global_transform_c.position,
                                  global_transform_c.rotation,
                                  global_transform_c.scale);

    global_transform_c.dirty = false;
    return global_transform_c.transform;
}

const vec3 &SceneContext::getGlobalPosition(entt::entity entity) const
{
    if (!has<GlobalTransform_C, Transform_C>(entity))
        return INVALID_VEC3;

    if (!has<GlobalTransform_C>(entity))
        return get<Transform_C>(entity).position;

    auto &global_transform_c = get<GlobalTransform_C>(entity);
    if (!global_transform_c.dirty)
        return global_transform_c.position;

    // Update global transform
    getGlobalTransform(entity);

    return global_transform_c.position;
}

const vec3 &SceneContext::getGlobalRotation(entt::entity entity) const
{
    if (!has<GlobalTransform_C, Transform_C>(entity))
        return INVALID_VEC3;

    if (!has<GlobalTransform_C>(entity))
        return get<Transform_C>(entity).position;

    auto &global_transform_c = get<GlobalTransform_C>(entity);
    if (!global_transform_c.dirty)
        return global_transform_c.rotation;

    // Update global transform
    getGlobalTransform(entity);

    return global_transform_c.rotation;
}

const vec3 &SceneContext::getGlobalScale(entt::entity entity) const
{
    if (!has<GlobalTransform_C, Transform_C>(entity))
        return INVALID_VEC3;

    if (!has<GlobalTransform_C>(entity))
        return get<Transform_C>(entity).position;

    auto &global_transform_c = get<GlobalTransform_C>(entity);
    if (!global_transform_c.dirty)
        return global_transform_c.scale;

    // Update global transform
    getGlobalTransform(entity);

    return global_transform_c.scale;
}

vec3 SceneContext::getGlobalDirection(entt::entity entity) const
{
    vec3 forward = -vec3{getGlobalTransform(entity)[2]}; // -Z
    return glm::normalize(forward);
}

void SceneContext::lookAt(entt::entity entity, const vec3 &target)
{
    if (!isValid(entity) || !has<Transform_C>(entity))
        return;

    patch<Transform_C>(entity, [&](auto &transform_c) {
        mat4 view = glm::lookAt(transform_c.position, target, vec3{0.f, 1.f, 0.f});
        transform_c.transform = glm::inverse(view);
        glm_utils::decomposeTransform(transform_c.transform,
                                      transform_c.position,
                                      transform_c.rotation,
                                      transform_c.scale);
        transform_c.dirty = false;
    });
}

const AABB &SceneContext::getLocalAABB(entt::entity entity) const
{
    if (has<LocalAABB_C>(entity))
        return get<LocalAABB_C>(entity).aabb;
    return AABB::invalid;
}

const AABB &SceneContext::getGlobalAABB(entt::entity entity) const
{
    if (!has<GlobalAABB_C>(entity))
        return AABB::invalid;

    auto &global_aabb_c = const_cast<SceneContext *>(this)->get<GlobalAABB_C>(entity);
    if (!global_aabb_c.dirty)
        return global_aabb_c.aabb;

    global_aabb_c.aabb = getLocalAABB(entity).transform(getGlobalTransform(entity));
    global_aabb_c.dirty = false;

    return global_aabb_c.aabb;
}

entt::entity SceneContext::createCamera()
{
    auto entity = createTransformedEntity();

    auto &transform_inheritance_c = get<TransformInheritance_C>(entity);
    transform_inheritance_c.mode = TransformInheritance_C::POSITION
                                   | TransformInheritance_C::ROTATION;

    auto &camera_c = m_data->registry.emplace<Camera_C>(entity);
    camera_c.ratio = static_cast<float>(getRenderTexture().getSize().x)
                     / getRenderTexture().getSize().y;

    return entity;
}

entt::entity SceneContext::createCamera(float fov, float near, float far, const mat4 &transform)
{
    auto entity = createTransformedEntity(transform);

    auto &transform_inheritance_c = get<TransformInheritance_C>(entity);
    transform_inheritance_c.mode = TransformInheritance_C::POSITION
                                   | TransformInheritance_C::ROTATION;

    auto &camera_c = m_data->registry.emplace<Camera_C>(entity);
    camera_c.fov = fov;
    camera_c.near = near;
    camera_c.far = far;
    camera_c.ratio = static_cast<float>(getRenderTexture().getSize().x)
                     / getRenderTexture().getSize().y;

    return entity;
}

entt::entity SceneContext::getActiveCamera() const
{
    return m_data->active_camera;
}

void SceneContext::setActiveCamera(entt::entity entity)
{
    if (isValid(entity) && has<Camera_C>(entity))
        m_data->active_camera = entity;
}

const mat4 &SceneContext::getCameraRotationTransform(entt::entity entity)
{
    if (!isValid(entity) || !has<Camera_C>(entity))
        return INVALID_TRANSFORM;

    auto &camera_c = get<Camera_C>(entity);
    if (camera_c.dirty) {
        camera_c.dirty = false;
        updateCameraTransforms(entity);
    }

    return camera_c.rotation_transform;
}

const mat4 &SceneContext::getCameraProjTransform(entt::entity entity)
{
    if (!isValid(entity) || !has<Camera_C>(entity))
        return INVALID_TRANSFORM;

    auto &camera_c = get<Camera_C>(entity);
    if (camera_c.dirty) {
        camera_c.dirty = false;
        updateCameraTransforms(entity);
    }

    return camera_c.proj_transform;
}

const mat4 &SceneContext::getCameraViewTransform(entt::entity entity)
{
    if (!isValid(entity) || !has<Camera_C>(entity))
        return INVALID_TRANSFORM;

    auto &camera_c = m_data->registry.get<Camera_C>(entity);
    if (camera_c.dirty) {
        camera_c.dirty = false;
        updateCameraTransforms(entity);
    }

    return camera_c.view_transform;
}

const Frustum &SceneContext::getCameraFrustum(entt::entity entity)
{
    if (!isValid(entity) || !has<Camera_C>(entity))
        return INVALID_FRUSTUM;

    auto &camera_c = get<Camera_C>(entity);
    if (camera_c.dirty) {
        camera_c.dirty = false;
        updateCameraTransforms(entity);
    }

    return camera_c.frustum;
}

entt::entity SceneContext::createDirectLight()
{
    auto entity = m_data->registry.create();

    auto &direct_light_c = m_data->registry.emplace<DirectLight_C>(entity);
    direct_light_c.direction = vec3{-1.0f};
    direct_light_c.ambient = Color{0.6, 0.6, 0.6, 1.0f};
    direct_light_c.diffuse = Color{0.4, 0.4, 0.4, 1.0f};

    return entity;
}

entt::entity SceneContext::createLight(bool spot, bool dynamic)
{
    auto entity = createTransformedEntity();

    auto &transform_inheritance_c = get<TransformInheritance_C>(entity);
    transform_inheritance_c.mode = TransformInheritance_C::POSITION
                                   | TransformInheritance_C::ROTATION;

    auto &light_c = m_data->registry.emplace<Light_C>(entity);

    if (spot)
        light_c.type = Light_C::SPOT;

    if (dynamic)
        m_data->registry.emplace<Dynamic_C>(entity);

    return entity;
}

entt::entity SceneContext::getActiveDirectLight() const
{
    return m_data->active_direct_light;
}

void SceneContext::setActiveDirectLight(entt::entity entity)
{
    if (isValid(entity) && has<DirectLight_C>(entity))
        m_data->active_direct_light = entity;
}

entt::entity SceneContext::createSkybox(const SharedPtr<Skybox> &skybox)
{
    if (!skybox)
        return entt::null;

    auto entity = m_data->registry.create();

    auto &skybox_c = m_data->registry.emplace<Skybox_C>(entity);
    skybox_c = skybox;

    return entity;
}

entt::entity SceneContext::getActiveSkybox() const
{
    return m_data->active_skybox;
}

void SceneContext::setActiveSkybox(entt::entity entity)
{
    if (!isValid(entity)) {
        m_data->active_skybox = entt::null;
        return;
    }

    if (has<Skybox_C>(entity))
        m_data->active_skybox = entity;
}

bool SceneContext::isCameraDirty() const
{
    return m_data->camera_dirty;
}

bool SceneContext::isSceneDirty() const
{
    return m_data->scene_dirty;
}

BVH<entt::entity, entt::null> &SceneContext::getStaticCollidersTree()
{
    return m_data->m_static_colliders_tree;
}

BVH<entt::entity, entt::null> &SceneContext::getDynamicCollidersTree()
{
    return m_data->m_dynamic_colliders_tree;
}

void SceneContext::updateCameraTransforms(entt::entity entity)
{
    auto &camera_c = get<Camera_C>(entity);

    glm::quat q = glm::quat(getGlobalRotation(entity));
    camera_c.rotation_transform = glm::toMat4(q);

    camera_c.front = glm::normalize(
        vec3{camera_c.rotation_transform * vec4{0.0f, 0.0f, -1.0f, 0.0f}});
    camera_c.up = glm::normalize(vec3{camera_c.rotation_transform * vec4{0.0f, 1.0f, 0.0f, 0.0f}});
    camera_c.right = glm::normalize(glm::cross(camera_c.front, camera_c.up));

    camera_c.proj_transform = glm::perspective(glm::radians(camera_c.fov),
                                               camera_c.ratio,
                                               camera_c.near,
                                               camera_c.far);
    camera_c.view_transform = glm::lookAt(getGlobalPosition(entity),
                                          getGlobalPosition(entity) + camera_c.front,
                                          camera_c.up);

    camera_c.frustum.update(camera_c.proj_transform * camera_c.view_transform);
}

void SceneContext::propagateDynamic(entt::entity entity)
{
    if (!isValid(entity) || has<Dynamic_C>(entity))
        return;

    m_data->registry.emplace<Dynamic_C>(entity);

    if (!has<Children_C>(entity))
        return;

    for (auto child : get<Children_C>(entity).children)
        propagateDynamic(child);
}

void SceneContext::markGlobalTransformDirty(entt::entity entity)
{
    if (has<GlobalTransform_C>(entity)) {
        auto &global_transform_c = get<GlobalTransform_C>(entity);
        if (!global_transform_c.dirty) {
            global_transform_c.dirty = true;

            // Make global AABB dirty
            if (has<GlobalAABB_C>(entity)) {
                auto &global_aabb_c = get<GlobalAABB_C>(entity);
                global_aabb_c.dirty = true;
            }

            // Make camera dirty
            if (has<Camera_C>(entity)) {
                auto &camera_c = get<Camera_C>(entity);
                camera_c.dirty = true;
                m_data->camera_dirty = true;
            }

            if (has<Children_C>(entity))
                for (auto child : get<Children_C>(entity).children)
                    markGlobalTransformDirty(child);

            patch<GlobalTransform_C>(entity);
        }
    }
}

mat4 SceneContext::buildInheritedTransform(const mat4 &transform, int32_t flags) const
{
    if (static_cast<TransformInheritance_C::Mode>(flags) == TransformInheritance_C::NONE)
        return mat4{1.0f};

    vec3 translation{0.0f};
    vec3 scale{1.0f};
    quat rotation{1, 0, 0, 0}; // identity
    vec3 skew;
    vec4 perspective;

    glm::decompose(transform, scale, rotation, translation, skew, perspective);

    mat4 result{1.0f};

    if (flags & TransformInheritance_C::POSITION)
        result = glm::translate(result, translation);

    if (flags & TransformInheritance_C::ROTATION)
        result *= glm::toMat4(rotation);

    if (flags & TransformInheritance_C::SCALE)
        result *= glm::scale(mat4{1.0f}, scale);

    return result;
}

} // namespace ae
