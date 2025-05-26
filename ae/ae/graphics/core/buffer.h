#ifndef AE_BUFFER_H
#define AE_BUFFER_H

#include "../common/enums.h"

#include <cstdint>

namespace ae {

class Buffer
{
public:
    Buffer(BufferType type, UsageType usage_type = UsageType::DYNAMIC);
    ~Buffer();

    BufferType getType() const;
    UsageType getUsageType() const;

    uint32_t getId() const;
    int32_t getSize() const;

    void create(int32_t size = 0);
    bool isValid() const;
    void destroy();

    void resize(int32_t size);

    template<typename T>
    void setData(const T *value, int32_t size = sizeof(T), int32_t offset = 0)
    {
        setData(static_cast<const void *>(value), size, offset);
    }
    void setData(const void *value, int32_t size, int32_t offset = 0);

    static void bind(const Buffer &buffer);
    static void bindBase(const Buffer &buffer, int32_t index);
    static void unbind(const Buffer &buffer);
    static void unbind(BufferType type);

private:
    BufferType m_type;
    UsageType m_usage_type;
    uint32_t m_id;
    int32_t m_size;
};

} // namespace ae

#endif // AE_BUFFER_H
