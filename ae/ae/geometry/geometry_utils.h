#ifndef AE_GEOMETRY_UTILS_H
#define AE_GEOMETRY_UTILS_H

#include "primitives.h"
#include "triangles_node.h"

#include <glm/glm.hpp>

#include <vector>

using namespace glm;

namespace ae::geometry_utils {

AABB aabbFromSphere(const Sphere &sphere);
AABB aabbFromTriangles(const std::vector<Triangle> &triangles);

std::unique_ptr<TrianglesNode> buildTrianglesTree(const std::vector<Triangle> &triangles,
                                                  int depth = 0);

int32_t largestAxis(const vec3 &v);

float magnitudeSq(const vec3 &v);

Interval projectAABBOnAxis(const AABB &aabb, const vec3 &axis);
Interval projectTriangleOnAxis(const Triangle &triangle, const vec3 &axis);

vec3 projectPointOnTrianglePlane(const Triangle &triangle, const vec3 &p);

Plane planeFromTriangle(const Triangle &triangle);
Plane planeFromPointNormal(const vec3 &point, const vec3 &normal);

bool pointInTriangle(const Triangle &triangle, const vec3 &p);

vec3 closestPointOnLine(const Line &line, const vec3 &p);
vec3 closestPointOnPlane(const Plane &plane, const vec3 &p);
vec3 closestPointOnTriangle(const Triangle &triangle, const vec3 &p);

} // namespace ae::geometry_utils

#endif // AE_GEOMETRY_UTILS_H
