#include "player_s.h"
#include "../app.h"
#include "../common/spdlog_utils.h"
#include "../graphics/scene/model_instance.h"
#include "components.h"
#include "scene.h"

#include <spdlog/spdlog.h>

namespace ae {

Player_S::Player_S(Scene *scene)
    : System{scene}
{
    auto &app = App::getInstance();
    app.getInput()->cursorMoved.connect(&Player_S::onMouseMoved, this);
    app.getInput()->scrolled.connect(&Player_S::onScrolled, this);
}

Player_S::~Player_S()
{
    auto &app = App::getInstance();
    app.getInput()->cursorMoved.disconnect(this);
    app.getInput()->scrolled.disconnect(this);
}

entt::entity Player_S::getPlayerEntity() const
{
    return m_player_entity;
}

void Player_S::setPlayerEntity(entt::entity player_entity)
{
    m_player_entity = player_entity;
}

void Player_S::update()
{
    if (!isValid(m_player_entity))
        return;

    auto *input = App::getInstance().getInput();

    const auto &camera_c = get<Camera_C>(getScene()->getActiveCamera());
    const auto &player_c = get<Player_C>(m_player_entity);
    auto &movement_c = get<Movement_C>(m_player_entity);

    // movement_c.velocity = vec3{0.0f};

    // if (input->isKeyPressed(KeyCode::W))
    //     movement_c.velocity += camera_c.front * player_c.camera_speed;
    // else if (input->isKeyReleased(KeyCode::W))
    //     movement_c.velocity -= camera_c.front * player_c.camera_speed;

    // if (input->isKeyHeld(KeyCode::S))
    //     movement_c.velocity -= camera_c.front * player_c.camera_speed;
    // else if (input->isKeyReleased(KeyCode::S))
    //     movement_c.velocity += camera_c.front * player_c.camera_speed;

    // if (input->isKeyHeld(KeyCode::A))
    //     movement_c.velocity -= camera_c.right * player_c.camera_speed;
    // else if (input->isKeyReleased(KeyCode::A))
    //     movement_c.velocity += camera_c.right * player_c.camera_speed;

    // if (input->isKeyHeld(KeyCode::D))
    //     movement_c.velocity += camera_c.right * player_c.camera_speed;
    // else if (input->isKeyReleased(KeyCode::D))
    //     movement_c.velocity -= camera_c.right * player_c.camera_speed;

    vec3 input_dir{0.0f};

    if (input->isKeyHeld(KeyCode::W))
        input_dir += camera_c.front;
    if (input->isKeyHeld(KeyCode::S))
        input_dir -= camera_c.front;
    if (input->isKeyHeld(KeyCode::A))
        input_dir -= camera_c.right;
    if (input->isKeyHeld(KeyCode::D))
        input_dir += camera_c.right;

    if (glm::length2(input_dir) > 0.0f)
        input_dir = glm::normalize(input_dir);

    movement_c.input_direction = input_dir;

    if (input->isKeyHeld(KeyCode::SPACE) && movement_c.on_ground)
        movement_c.wants_to_jump = true;

    patch<Movement_C>(m_player_entity);

    // Camera rotate
    float delta_x = input->getCursorDeltaX();
    float delta_y = input->getCursorDeltaY();

    auto *window = App::getInstance().getWindow();

    auto &camera_center_transform_c = get<Transform_C>(player_c.camera_center);

    float dir_y = camera_center_transform_c.rotation.x;
    float dir_x = camera_center_transform_c.rotation.y;

    dir_y -= (-static_cast<float>(delta_y) / window->getSize().y * 1.5f);
    dir_y = std::clamp(dir_y, glm::radians(-89.0f), glm::radians(89.0f)); // ограничение по вертикали

    dir_x += -static_cast<float>(delta_x) / window->getSize().x * 1.5f;

    camera_center_transform_c.rotation.x = dir_y;
    camera_center_transform_c.rotation.y = dir_x;

    patch<Transform_C>(player_c.camera_center);
}

void Player_S::updateCameraPosition(const Time &elapsed_time)
{
    if (!isValid(m_player_entity))
        return;

    float bobbingAmplitudeVertical = 0.05f;
    float bobbingAmplitudeHorizontal = 0.03f;
    float bobbingFrequency = 10.0f;

    const auto &player_c = get<Player_C>(m_player_entity);
    const auto &movement_c = get<Movement_C>(m_player_entity);
    auto &camera_center_transform_c = get<Transform_C>(player_c.camera_center);

    float speed = glm::length(
        glm::vec2(movement_c.input_direction.x,
                  movement_c.input_direction.z)); // Движение в горизонтальной плоскости
    bool isMoving = speed > 0.0f;

    if (isMoving) {
        bobbingTime += elapsed_time.asSeconds() * bobbingFrequency;

        // Смещения по осям
        float horizontalOffset = std::sin(bobbingTime) * bobbingAmplitudeHorizontal;
        float verticalOffset = std::sin(2.0f * bobbingTime) * bobbingAmplitudeVertical;

        // в два раза быстрее

        // Применяем bobbing к камере
        camera_center_transform_c.position.y = 2.0f + verticalOffset;
        camera_center_transform_c.position.x += horizontalOffset;
    } else {
        bobbingTime = 0.0f; // Сбросить, чтобы не было рывков
        camera_center_transform_c.position.y = 2.0f;
        camera_center_transform_c.position.x = 0;
    }

    patch<Transform_C>(player_c.camera_center);

    // patch<Camera_C>(getScene()->getActiveCamera(), [&](auto &camera_c) {
    //     camera_c.position = getScene()->getGlobalPosition(player_c.camera_position_entity);
    //     getScene()->cameraLookAt(getScene()->getActiveCamera(),
    //                              getScene()->getGlobalPosition(player_c.camera_look_at_entity));
    // });

    const auto &animator_c = get<Animator_C>(player_c.model_entity);
    animator_c.animator->update(0.016f);

    static mat4 test{1.0f};
    test = glm::rotate(test,
                       glm::radians(0.1f),
                       vec3{1.0f, 0.0f, 0.0f}); //glm::translate(test, vec3{0.005f, 0.0f, 0.0f});

    auto mi = staticPointerCast<ModelInstance>(get<Drawable_C>(player_c.model_entity));

    // const auto &camera_c = get<Camera_C>(getScene()->getActiveCamera());
    // light_c.direction = camera_c.front;
    // patch<Light_C>(player_c.light_entity);

    // mi->getPose().setBoneLocalTransform("Head_06_14", test);
    // mi->getPose().setBoneLocalTransform("Spine_02_10", test);
    // mi->getPose().rotateBone("Spine_02_10", vec3{0.1f, 0.0f, 0.0f});

    // auto &camera_transform_c = get<>
    // spdlog::debug("Camera position: {}", getGlobalPosition(player_c.camera));
}

void Player_S::onMouseMoved(int32_t x, int32_t y, int32_t delta_x, int32_t delta_y)
{
    // if (!isValid(m_player_entity))
    //     return;

    // auto *window = App::getInstance().getWindow();

    // const auto &player_c = get<Player_C>(m_player_entity);

    // auto &transform_c = get<Transform_C>(player_c.camera_center);

    // float dir_y = transform_c.rotation.x;
    // float dir_x = transform_c.rotation.y;

    // dir_y -= (-static_cast<float>(delta_y) / window->getSize().y * 1.5f);
    // dir_y = std::clamp(dir_y, glm::radians(-89.0f), glm::radians(89.0f)); // ограничение по вертикали

    // dir_x += -static_cast<float>(delta_x) / window->getSize().x * 1.5f;

    // transform_c.rotation.x = dir_y;
    // transform_c.rotation.y = dir_x;

    // patch<Transform_C>(player_c.camera_center);

    // auto mi = std::static_pointer_cast<ModelInstance>(
    //     registry.get<Drawable_C>(player_c.model_entity));

    // auto rot = mi->getPose().getBoneRotation("Spine_02_10");
    // rot.y = dir_x;
    // rot.x = dir_y;
    // mi->getPose().setBoneRotation("Spine_02_10", rot);

    // registry.patch<Transform_C>(player_c.model_entity,
    //                             [&](auto &transform_c) { transform_c.rotation.y = dir_x; });
}

void Player_S::onScrolled(float xoffset, float yoffset)
{
    auto &player_c = get<Player_C>(m_player_entity);

    if (yoffset < 0.0f && player_c.camera_offset > player_c.camera_max_offset)
        player_c.camera_offset -= 1.0f;

    if (yoffset > 0.0f && player_c.camera_offset < 0.0f)
        player_c.camera_offset += 1.0f;

    patch<Transform_C>(player_c.camera,
                       [&](auto &transform_c) { transform_c.position.z = player_c.camera_offset; });
}

} // namespace ae
