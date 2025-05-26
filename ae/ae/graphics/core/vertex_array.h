#ifndef AE_VERTEX_ARRAY_H
#define AE_VERTEX_ARRAY_H

#include "buffer.h"
#include "vertex_attrib.h"

#include <vector>

namespace ae {

class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    int32_t getVertexCount() const;
    int32_t getIndicesCount() const;

    uint32_t getId() const;

    template<typename V>
    void create(const std::vector<V> &vertices, const std::vector<uint32_t> &indices = {})
    {
        create(sizeof(V), VertexAttrib::get<V>(), !indices.empty());

        if (m_vao == 0)
            return;

        setData(vertices, indices);
    }

    template<typename V>
    void setData(const std::vector<V> &vertices, const std::vector<uint32_t> &indices = {})
    {
        if (!isValid())
            return;

        m_vbo.setData(vertices.data(), vertices.size() * sizeof(V));
        m_vertex_count = vertices.size();

        if (!indices.empty()) {
            m_ebo.setData(indices.data(), indices.size() * sizeof(uint32_t));
            m_indices_count = indices.size();
        }
    }

    bool isValid() const;
    void destroy();

    void draw(PrimitiveType primitive_type = PrimitiveType::TRIANGLES, bool fill = true) const;

    static void bind(const VertexArray &vertex_array);
    static void unbind();

private:
    void create(int32_t vertex_type_size,
                const std::vector<VertexAttrib> &vertex_attribs,
                bool using_ebo = true);

private:
    Buffer m_vbo;
    Buffer m_ebo;
    uint32_t m_vao;
    int32_t m_vertex_count;
    int32_t m_indices_count;
};

} // namespace ae

#endif // AE_VERTEX_ARRAY_H
