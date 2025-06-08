#include "vertex_array.h"
#include "../common/utils.h"

#include <GL/glew.h>

namespace ae {

VertexArray::VertexArray()
    : m_vbo{BufferType::ARRAY_BUFFER, UsageType::DYNAMIC}
    , m_ebo{BufferType::ELEMENT_ARRAY_BUFFER, UsageType::DYNAMIC}
    , m_vao{0}
    , m_vertex_count{0}
    , m_indices_count{0}
{}

VertexArray::~VertexArray()
{
    destroy();
}

int32_t VertexArray::getVertexCount() const
{
    return m_vertex_count;
}

int32_t VertexArray::getIndicesCount() const
{
    return m_indices_count;
}

uint32_t VertexArray::getId() const
{
    return m_vao;
}

bool VertexArray::isValid() const
{
    return m_vao != 0;
}

void VertexArray::destroy()
{
    if (m_vao != 0) {
        m_vbo.destroy();
        m_ebo.destroy();

        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;

        m_vertex_count = 0;
        m_indices_count = 0;
    }
}

void VertexArray::draw(PrimitiveType primitive_type, bool fill) const
{
    if (m_vao != 0) {
        glBindVertexArray(m_vao);

        if (!fill)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        if (m_ebo.isValid()) {
            glDrawElements(graphics_utils::primitiveTypeToGl(primitive_type),
                           m_indices_count,
                           GL_UNSIGNED_INT,
                           0);
        } else {
            glDrawArrays(graphics_utils::primitiveTypeToGl(primitive_type), 0, m_vertex_count);
        }

        if (!fill)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glBindVertexArray(0);
    }
}

void VertexArray::bind(const VertexArray &vertex_array)
{
    if (vertex_array.m_vao != 0)
        glBindVertexArray(vertex_array.m_vao);
}

void VertexArray::unbind()
{
    glBindVertexArray(0);
}

void VertexArray::create(int32_t vertex_type_size,
                         const std::vector<VertexAttrib> &vertex_attribs,
                         bool using_ebo)
{
    destroy();

    if (vertex_type_size == 0 || vertex_attribs.empty())
        return;

    glGenVertexArrays(1, &m_vao);

    glBindVertexArray(m_vao);

    m_vbo.create();
    if (using_ebo)
        m_ebo.create();

    Buffer::bind(m_vbo);
    if (using_ebo)
        Buffer::bind(m_ebo);

    for (const auto &attrib : vertex_attribs) {
        if (attrib.type == DataType::FLOAT) {
            glVertexAttribPointer(attrib.index,
                                  attrib.size,
                                  graphics_utils::dataTypeToGl(attrib.type),
                                  graphics_utils::boolToGl(attrib.normalized),
                                  vertex_type_size,
                                  (const void *) attrib.offset);
        } else if (attrib.type == DataType::INT) {
            glVertexAttribIPointer(attrib.index,
                                   attrib.size,
                                   graphics_utils::dataTypeToGl(attrib.type),
                                   vertex_type_size,
                                   (const void *) attrib.offset);
        }
        glEnableVertexAttribArray(attrib.index);
    }

    glBindVertexArray(0);

    Buffer::unbind(m_vbo);
    if (using_ebo)
        Buffer::unbind(m_ebo);

    m_vertex_count = 0;
    m_indices_count = 0;
}

} // namespace ae
