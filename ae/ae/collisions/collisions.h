#ifndef AE_COLLISIONS_H
#define AE_COLLISIONS_H

#include "colliders.h"
#include "collision_result.h"

#include <mutex>
#include <queue>

namespace ae {

struct Collisions
{
    // Test collider vs collider
    static void test(const Collider *collider1,
                     const vec3 &collider_1_position,
                     const Collider *collider2,
                     CollisionResult &result);

    // Test swept collider vs collider
    static void test(const Collider *collider1,
                     const vec3 &start_position,
                     const vec3 &velocity,
                     const Collider *collider2,
                     CollisionResult &result);

    // aabb vs triangles
    static void aabbVsTriangleNode(const AABB &aabb,
                                   const TrianglesNode &root,
                                   CollisionResult &result);

    static void aabbVsTriangle(const AABB &aabb, const Triangle &triangle, CollisionResult &result);

    // Swept aabb vs triangles
    static void sweptAABBVsTriangleNode(const AABB &aabb,
                                        const AABB &swept_aabb,
                                        const vec3 &velocity,
                                        const TrianglesNode &root,
                                        CollisionResult &result);

    static void sweptAABBVsTriangle(const AABB &aabb,
                                    const vec3 &velocity,
                                    const Triangle &triangle,
                                    CollisionResult &result);

    // raycast
    static void raycast(const Ray &ray, const Collider *collider, RaycastResult &result);

    // ray vs triangles
    static void rayVsTriangleNode(const Ray &ray, const TrianglesNode &root, RaycastResult &result);

    static void rayVsTriangle(const Ray &ray, const Triangle &triangle, RaycastResult &result);

    static bool rayVsAABB(const Ray &ray, const AABB &aabb);

private:
    static std::queue<const TrianglesNode *> m_queue;
    static std::mutex m_result_mutex;
};

} // namespace ae

#endif // AE_COLLISIONS_H
