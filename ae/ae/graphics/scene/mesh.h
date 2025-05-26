#ifndef AE_MESH_H
#define AE_MESH_H

#include "../../geometry/primitives.h"
#include "../core/material.h"
#include "../core/vertex.h"
#include "../core/vertex_array.h"
#include "drawable.h"

namespace ae {

class Mesh : public Drawable
{
public:
    Mesh();
    Mesh(const std::vector<Vertex> &vertices,
         const std::vector<uint32_t> &indices,
         const std::shared_ptr<Material> &material);
    ~Mesh() = default;

    const std::vector<Vertex> &getVertices() const;
    const std::vector<uint32_t> &getIndices() const;
    const std::vector<Triangle> &getTriangles() const;

    std::shared_ptr<Material> getMaterial() const;
    void setMaterial(const std::shared_ptr<Material> &material,
                     const ivec4 &texture_rect = ivec4{0});

    void create(const std::vector<Vertex> &vertices,
                const std::vector<uint32_t> &indices,
                const std::shared_ptr<Material> &material);
    bool isValid() const;
    void destroy();

    const AABB &getAABB() const;
    bool isTransparent() const;
    void draw(const RenderState &render_state) const;

private:
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
    std::vector<Triangle> m_triangles;
    std::shared_ptr<Material> m_material;
    VertexArray m_vertex_array;
    AABB m_aabb;
};

} // namespace ae

#endif // AE_MESH_H
