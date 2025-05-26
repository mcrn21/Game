#include "buffer.h"
#include "../common/utils.h"

#include <GL/glew.h>

namespace ae {

Buffer::Buffer(BufferType type, UsageType usage_type)
    : m_type{type}
    , m_usage_type{usage_type}
    , m_id{0}
    , m_size{0}
{}

Buffer::~Buffer()
{
    destroy();
}

BufferType Buffer::getType() const
{
    return m_type;
}

UsageType Buffer::getUsageType() const
{
    return m_usage_type;
}

uint32_t Buffer::getId() const
{
    return m_id;
}

int32_t Buffer::getSize() const
{
    return m_size;
}

void Buffer::create(int32_t size)
{
    destroy();

    glGenBuffers(1, &m_id);
    glBindBuffer(graphics_utils::bufferTypeToGl(m_type), m_id);
    glBufferData(graphics_utils::bufferTypeToGl(m_type),
                 size,
                 nullptr,
                 graphics_utils::usageTypeToGl(m_usage_type));
    glBindBuffer(graphics_utils::bufferTypeToGl(m_type), 0);

    m_size = size;
}

bool Buffer::isValid() const
{
    return m_id != 0;
}

void Buffer::destroy()
{
    if (m_id != 0) {
        glDeleteBuffers(1, &m_id);
        m_id = 0;
        m_size = 0;
    }
}

void Buffer::resize(int32_t size)
{
    if (m_id == 0)
        return;

    glBindBuffer(graphics_utils::bufferTypeToGl(m_type), m_id);
    glBufferData(graphics_utils::bufferTypeToGl(m_type),
                 size,
                 nullptr,
                 graphics_utils::usageTypeToGl(m_usage_type));
    glBindBuffer(graphics_utils::bufferTypeToGl(m_type), 0);

    m_size = size;
}

void Buffer::setData(const void *value, int32_t size, int32_t offset)
{
    if (m_id == 0)
        return;

    glBindBuffer(graphics_utils::bufferTypeToGl(m_type), m_id);

    if ((size + offset) > m_size) {
        glBufferData(graphics_utils::bufferTypeToGl(m_type),
                     size + offset,
                     nullptr,
                     graphics_utils::usageTypeToGl(m_usage_type));
        m_size = size;
    }

    glBufferSubData(graphics_utils::bufferTypeToGl(m_type), offset, size, value);
    glBindBuffer(graphics_utils::bufferTypeToGl(m_type), 0);
}

void Buffer::bind(const Buffer &buffer)
{
    if (buffer.m_id != 0)
        glBindBuffer(graphics_utils::bufferTypeToGl(buffer.m_type), buffer.m_id);
}

void Buffer::bindBase(const Buffer &buffer, int32_t index)
{
    if (buffer.m_id == 0)
        return;
    glBindBufferBase(graphics_utils::bufferTypeToGl(buffer.m_type), index, buffer.m_id);
}

void Buffer::unbind(const Buffer &buffer)
{
    if (buffer.m_id != 0)
        glBindBuffer(graphics_utils::bufferTypeToGl(buffer.m_type), 0);
}

void Buffer::unbind(BufferType type)
{
    glBindBuffer(graphics_utils::bufferTypeToGl(type), 0);
}

} // namespace ae
