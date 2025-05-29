#ifndef AE_TRIANGLES_NODE_H
#define AE_TRIANGLES_NODE_H

#include "../system/memory.h"
#include "primitives.h"

#include <vector>

namespace ae {

struct TrianglesNode
{
    bool isLeaf() const { return triangles.size() > 0; }

    AABB aabb;
    std::vector<Triangle> triangles;
    u_ptr<TrianglesNode> left;
    u_ptr<TrianglesNode> right;
};

} // namespace ae

#endif // AE_TRIANGLES_NODE_H
