#ifndef AE_FRUSTUM_H
#define AE_FRUSTUM_H

#include "primitives.h"

#include <glm/glm.hpp>

#include <array>

using namespace glm;

namespace ae {

class Frustum
{
public:
    Frustum();
    ~Frustum() = default;

    const std::array<vec3, 8> &getPoints() const;
    const AABB &getAABB() const;

    bool intersectWithAABB(const AABB &other_aabb) const;

    void update(const glm::mat4 &proj_view_mat);

private:
    std::array<vec3, 8> m_points;
    AABB m_aabb;
    std::array<Plane, 6> m_planes;
};

} // namespace ae

#endif // AE_FRUSTUM_H
