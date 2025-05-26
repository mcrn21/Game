#ifndef AE_COLLIDERS_H
#define AE_COLLIDERS_H

#include "../common/glm_utils.h"
#include "../geometry/geometry_utils.h"
#include "../geometry/primitives.h"
#include "../geometry/triangles_node.h"

#include <glm/glm.hpp>

#include <memory>
#include <vector>

using namespace glm;

namespace ae {

struct Collider
{
    enum Type { SPHERE, MESH, BOX };

    virtual ~Collider() = default;
    virtual void applyTransform(const mat4 &){};

    Type type;
    AABB aabb;
};

struct SphereCollider : public Collider
{
    SphereCollider(const Sphere &sphere)
        : local_sphere{sphere}
        , global_sphere{sphere}
    {
        type = SPHERE;
        aabb = geometry_utils::aabbFromSphere(sphere);
    }

    void applyTransform(const mat4 &transform)
    {
        global_sphere.center = vec3(transform * glm::vec4(local_sphere.center, 1.0f));
        aabb = geometry_utils::aabbFromSphere(global_sphere);
    };

    Sphere local_sphere;
    Sphere global_sphere;
};

struct BoxCollider : public Collider
{
    BoxCollider(const AABB &local_aabb)
        : local_aabb{local_aabb}
    {
        type = BOX;
        aabb = local_aabb;
    }

    void applyTransform(const mat4 &transform) { aabb = local_aabb.move(transform[3]); };

    AABB local_aabb;
};

struct MeshCollider : public Collider
{
    MeshCollider(const std::vector<Triangle> &triangles, const mat4 &transform = mat4{1.0f})
    {
        std::vector<Triangle> transformed_triangles;
        for (const auto &triangle : triangles) {
            Triangle transformed_triangle;
            transformed_triangle.v0 = glm_utils::transformVec3(triangle.v0, transform);
            transformed_triangle.v1 = glm_utils::transformVec3(triangle.v1, transform);
            transformed_triangle.v2 = glm_utils::transformVec3(triangle.v2, transform);
            transformed_triangles.push_back(transformed_triangle);
        }

        root = geometry_utils::buildTrianglesTree(transformed_triangles);
        //collisions_utils::buildTrianglesTree(transformed_triangles);

        type = MESH;
        aabb = geometry_utils::aabbFromTriangles(transformed_triangles);
        //collisions_utils::aabbFromTriangles(transformed_triangles);
    }

    std::unique_ptr<TrianglesNode> root;
};

} // namespace ae

#endif // AE_COLLIDERS_H
