#ifndef AE_PLAYER_SYSTEM_H
#define AE_PLAYER_SYSTEM_H

#include "../system/time.h"
#include "system.h"

#include <entt/entt.hpp>

namespace ae {

class Player_S : public System
{
public:
    Player_S(Scene *scene);
    ~Player_S();

    entt::entity getPlayerEntity() const;
    void setPlayerEntity(entt::entity player_entity);

    void update();
    void updateCameraPosition(const Time &elapsed_time);

private:
    void onMouseMoved(int32_t x, int32_t y, int32_t delta_x, int32_t delta_y);
    void onScrolled(float xoffset, float yoffset);

private:
    entt::entity m_player_entity;
    float bobbingTime = 0.0f;
};

} // namespace ae

#endif // AE_PLAYER_SYSTEM_H
