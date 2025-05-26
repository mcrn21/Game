#ifndef AE_TRIANGLES_NODE_H
#define AE_TRIANGLES_NODE_H

#include "primitives.h"

#include <memory>
#include <vector>

namespace ae {

struct TrianglesNode
{
    bool isLeaf() const { return triangles.size() > 0; }

    AABB aabb;
    std::vector<Triangle> triangles;
    std::unique_ptr<TrianglesNode> left;
    std::unique_ptr<TrianglesNode> right;
};

} // namespace ae

#endif // AE_TRIANGLES_NODE_H
