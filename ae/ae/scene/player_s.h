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
    void onMouseMoved(const ivec2 &, const ivec2 &);
    void onScrolled(const vec2 &scroll);

private:
    entt::entity m_player_entity;
    float bobbingTime = 0.0f;
};

} // namespace ae

#endif // AE_PLAYER_SYSTEM_H
