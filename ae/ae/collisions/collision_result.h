#ifndef AE_COLLISION_RESULT_H
#define AE_COLLISION_RESULT_H

#include <glm/glm.hpp>

using namespace glm;

namespace ae {

struct CollisionResult
{
    void reset()
    {
        hit = false;
        point = vec3{0.0f};
        normal = vec3{0.0f};
        depth = std::numeric_limits<float>::max();
        toi = 1.0f;
        v0 = vec3{0.0f};
        v1 = vec3{0.0f};
        v2 = vec3{0.0f};
    }

    bool hit = false;                                // Было ли столкновение
    vec3 point{0.0f};                                // Точка столкновения
    vec3 normal{0.0f};                               // Нормаль треугольника
    float depth = std::numeric_limits<float>::max(); // Глубина
    float toi = 1.0f;
    vec3 v0{0.0f};
    vec3 v1{0.0f};
    vec3 v2{0.0f};
};

} // namespace ae

#endif // AE_COLLISION_RESULT_H
