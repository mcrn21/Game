#ifndef AE_SCENE_CONTEXT_H
#define AE_SCENE_CONTEXT_H

#include "../engine_context_object.h"
#include "../geometry/frustum.h"
#include "../geometry/primitives.h"
#include "../graphics/scene/drawable.h"
#include "../graphics/scene/model.h"
#include "../graphics/scene/skybox.h"
#include "scene_data.h"

#include <glm/glm.hpp>

using namespace glm;

namespace ae {

class SceneContext : public EngineContextObject
{
public:
    SceneContext(EngineContext &engine_context, SceneData *data);
    virtual ~SceneContext() = default;

    // Render texture
    const RenderTexture &getRenderTexture() const;
    void setRenderTextureSize(const ivec2 &size);

    // Create
    entt::entity createTransformedEntity(const mat4 &transform = mat4{1.0f});
    entt::entity createBoundedEntity(const mat4 &transform = mat4{1.0f},
                                     const AABB &aabb = AABB{vec3{-1.0f}, vec3{1.0f}});

    entt::entity createDrawableEntity(const s_ptr<Drawable> &drawable,
                                      const mat4 &transform = mat4{1.0f},
                                      entt::entity parent = entt::null);

    entt::entity createMeshNodeEntity(const s_ptr<MeshNode> &mesh_node,
                                      const mat4 &transform = mat4{1.0f},
                                      entt::entity parent = entt::null);

    entt::entity createModelEntity(const s_ptr<Model> &model,
                                   const mat4 &transform = mat4{1.0f},
                                   entt::entity parent = entt::null);

    void createMeshNodeEntities(const s_ptr<MeshNode> &mesh_node,
                                const mat4 &transform = mat4{1.0f});

    // Entities management
    void destroyEntity(entt::entity entity);
    bool isValid(entt::entity entity) const;

    // Entity children
    entt::entity getParent(entt::entity entity) const;
    const std::vector<entt::entity> &getChildren(entt::entity entity) const;
    void addChild(entt::entity entity, entt::entity child);
    void removeChild(entt::entity entity, entt::entity child);

    // Entity transform
    const mat4 &getLocalTransform(entt::entity entity) const;
    const mat4 &getGlobalTransform(entt::entity entity) const;
    const vec3 &getGlobalPosition(entt::entity entity) const;
    const vec3 &getGlobalRotation(entt::entity entity) const;
    const vec3 &getGlobalScale(entt::entity entity) const;
    vec3 getGlobalDirection(entt::entity entity) const;

    void lookAt(entt::entity entity, const vec3 &target);

    // Entity AABB
    const AABB &getLocalAABB(entt::entity entity) const;
    const AABB &getGlobalAABB(entt::entity entity) const;

    // Camera
    entt::entity createCamera();
    entt::entity createCamera(float fov, float near, float far, const mat4 &transform = mat4{1.0f});

    entt::entity getActiveCamera() const;
    void setActiveCamera(entt::entity entity);

    const mat4 &getCameraRotationTransform(entt::entity entity);
    const mat4 &getCameraProjTransform(entt::entity entity);
    const mat4 &getCameraViewTransform(entt::entity entity);
    const Frustum &getCameraFrustum(entt::entity entity);

    // Lights
    entt::entity createDirectLight();
    entt::entity createLight(bool spot = false, bool dynamic = false);

    entt::entity getActiveDirectLight() const;
    void setActiveDirectLight(entt::entity entity);

    // Skybox
    entt::entity createSkybox(const s_ptr<Skybox> &skybox);

    entt::entity getActiveSkybox() const;
    void setActiveSkybox(entt::entity entity);

    // Scene
    bool isCameraDirty() const;
    bool isSceneDirty() const;

    void clear();

    // Entity components
    template<typename... Component>
    bool has(entt::entity entity) const
    {
        return (m_data->registry.any_of<Component>(entity) && ...);
    }

    template<typename Component>
    Component &get(entt::entity entity)
    {
        return m_data->registry.get<Component>(entity);
    }

    template<typename Component>
    const Component &get(entt::entity entity) const
    {
        return m_data->registry.get<Component>(entity);
    }

    template<typename Component, typename... Func>
    decltype(auto) patch(entt::entity entt, Func &&...func)
    {
        return m_data->registry.patch<Component>(entt, std::forward<Func>(func)...);
    }

protected:
    void updateCameraTransforms(entt::entity entity);
    void propagateDynamic(entt::entity entity);
    void markGlobalTransformDirty(entt::entity entity);
    mat4 buildInheritedTransform(const mat4 &transform, int32_t flags) const;

private:
    SceneData *m_data;
};

} // namespace ae

#endif // AE_SCENE_CONTEXT_H
