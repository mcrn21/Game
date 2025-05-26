#ifndef AE_COMPONENTS_H
#define AE_COMPONENTS_H

#include "../collisions/colliders.h"
#include "../geometry/frustum.h"
#include "../geometry/primitives.h"
#include "../graphics/core/color.h"
#include "../graphics/scene/pose_animator.h"
#include "../graphics/scene/drawable.h"
#include "../graphics/scene/skybox.h"

#include <entt/entt.hpp>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <memory>
#include <vector>

using namespace glm;

namespace ae {

struct Camera_C
{
    float ratio{1.0f};
    float fov{70.0f};

    float near{0.1f};
    float far{100.f};

    bool dirty{true};

    vec3 front{0.0f, 0.0f, -1.0f};
    vec3 up{0.0f, 1.0f, 0.0f};
    vec3 right{1.0f, 0.0f, 0.0f};

    mat4 rotation_transform{1.0f};
    mat4 proj_transform{1.0f};
    mat4 view_transform{1.0f};

    Frustum frustum;
};

struct DirectLight_C
{
    vec3 direction{0.0f};

    Color ambient = Color::white;
    Color diffuse = Color::white;
    Color specular = Color::white;
};

struct Light_C
{
    enum Type { POINT, SPOT };

    Color ambient = Color::white;
    Color diffuse = Color::white;
    Color specular = Color::white;

    float constant = 1.0f;
    float linear = 0.14f;
    float quadratic = 0.07f;
    float cut_off = 0.0f;

    float outer_cut_off = 0.0f;
    float radius = 0.0f;

    bool enable = true;
    Type type = POINT;
};

using Drawable_C = std::shared_ptr<Drawable>;

struct Animator_C
{
    std::shared_ptr<PoseAnimator> animator;
};

using Skybox_C = std::shared_ptr<Skybox>;

struct Transform_C
{
    vec3 position{0.f};
    vec3 rotation{0.f}; // Euler
    vec3 scale{1.f};

    mat4 transform{1.f};
    bool dirty = true;
};

struct GlobalTransform_C
{
    vec3 position{0.f};
    vec3 rotation{0.f}; // Euler
    vec3 scale{1.f};

    mat4 transform{1.f};
    bool dirty = true;
};

struct TransformInheritance_C
{
    enum Mode {
        NONE = 0,
        POSITION = 0x01,
        ROTATION = 0x02,
        SCALE = 0x04,
        ALL = POSITION | ROTATION | SCALE
    };

    int32_t mode = ALL;
};

struct LocalAABB_C
{
    AABB aabb;
};

struct GlobalAABB_C
{
    AABB aabb;
    bool dirty = true;
};

struct Parent_C
{
    entt::entity parent = entt::null;
};

struct Children_C
{
    std::vector<entt::entity> children;
};

struct Player_C
{
    float camera_speed{20.0f};

    float camera_max_offset = -10.0f;
    float camera_offset = -10.0f;

    entt::entity camera = entt::null;
    entt::entity camera_center = entt::null;

    entt::entity model_entity = entt::null;
    entt::entity light_entity = entt::null;
};

struct Movement_C
{
    vec3 velocity{0.0f};
    float step_up_velocity = 0.0f;

    vec3 input_direction{0.0f};
    float move_speed = 20.0f;

    bool wants_to_jump = false;
    float jump_velocity = 50.0f;

    float gravity = 4.0f; //9.81f;
    bool on_ground = false;
    vec3 ground_normal{0.0f};
    float max_slope_cos = 0.707f;

    bool is_stepping_up = false;
    float step_lerp_timer = 0.0f;
    float step_lerp_duration = 0.1f;
    vec3 step_start_pos = {};
    vec3 step_end_pos = {};
};

struct InMotion_C
{};

struct Dynamic_C
{};

using Collider_C = std::shared_ptr<Collider>;

struct Collisions_C
{
    struct Collision
    {
        float toi = 0.0f;
        vec3 normal{0.0f};
        vec3 point{0.0f};
        float depth = 0.0;
        vec3 v0{0.0f};
        vec3 v1{0.0f};
        vec3 v2{0.0f};
        AABB aabb;
        vec3 vel{0.0f};
    };

    std::vector<Collision> collisions;
};

} // namespace ae

#endif // AE_COMPONENTS_H
