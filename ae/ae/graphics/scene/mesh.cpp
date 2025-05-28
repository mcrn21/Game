#include "mesh.h"

namespace ae {

Mesh::Mesh() {}

Mesh::Mesh(const std::vector<Vertex> &vertices,
           const std::vector<uint32_t> &indices,
           const SharedPtr<Material> &material)
{
    create(vertices, indices, material);
}

const std::vector<Vertex> &Mesh::getVertices() const
{
    return m_vertices;
}

const std::vector<uint32_t> &Mesh::getIndices() const
{
    return m_indices;
}

const std::vector<Triangle> &Mesh::getTriangles() const
{
    return m_triangles;
}

SharedPtr<Material> Mesh::getMaterial() const
{
    return m_material;
}

void Mesh::setMaterial(const SharedPtr<Material> &material, const ivec4 &texture_rect)
{
    m_material = material;
}

void Mesh::create(const std::vector<Vertex> &vertices,
                  const std::vector<uint32_t> &indices,
                  const SharedPtr<Material> &material)
{
    destroy();
    m_vertex_array.create(vertices, indices);
    m_vertices = vertices;
    m_indices = indices;
    m_material = material;

    // Calculate triangles
    for (int32_t i = 0; i < indices.size(); i += 3) {
        Triangle triangle;
        triangle.v0 = vertices[indices[i]].position;
        triangle.v1 = vertices[indices[i + 1]].position;
        triangle.v2 = vertices[indices[i + 2]].position;
        m_triangles.push_back(triangle);
    }

    // Calculate aabb
    m_aabb.min = vec3{std::numeric_limits<float>::max()};
    m_aabb.max = vec3{std::numeric_limits<float>::lowest()};

    for (const auto &v : vertices) {
        m_aabb.min = glm::min(m_aabb.min, v.position);
        m_aabb.max = glm::max(m_aabb.max, v.position);
    }
}

bool Mesh::isValid() const
{
    return m_vertex_array.isValid();
}

void Mesh::destroy()
{
    m_vertex_array.destroy();
    m_material = nullptr;
    m_vertices.clear();
    m_indices.clear();
    m_triangles.clear();
}

const AABB &Mesh::getAABB() const
{
    return m_aabb;
}

bool Mesh::isTransparent() const
{
    return m_material && m_material->isTransparent();
}

void Mesh::draw(const RenderState &render_state) const
{
    if (m_material && m_material->diffuse_texture->isValid() && m_vertex_array.isValid()) {
        render_state.shader->uniformMatrix("u_model", render_state.transform);

        render_state.shader->uniformInt("u_material.diffuse_texture",
                                        Texture::getNextTextureNumber());
        render_state.shader->uniformInt("u_material.specular_texture",
                                        Texture::getNextTextureNumber() + 1);

        Texture::bind(*m_material->diffuse_texture);
        Texture::bind(*m_material->specular_texture);

        render_state.shader->uniformVec4("u_material.color", m_material->color.getColor());
        render_state.shader->uniformFloat("u_material.shininess", m_material->shininess);

        m_vertex_array.draw();
        // m_vertex_array.draw(PrimitiveType::TRIANGLES, false);

        Texture::unbind();
        Texture::unbind();
    }
}

} // namespace ae
