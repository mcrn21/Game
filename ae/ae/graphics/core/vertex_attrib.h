#ifndef AE_VERTEX_ATTRIB_H
#define AE_VERTEX_ATTRIB_H

#include "../common/enums.h"

#include <cstdint>
#include <vector>

#include <GL/glew.h>

namespace ae {

struct VertexAttrib
{
    template<typename T>
    static const std::vector<VertexAttrib> &get()
    {
        static std::vector<VertexAttrib> attribs;
        return attribs;
    }

    uint32_t index;  // Номер атрибута (0, 1, 2...)
    int32_t size;    // Кол-во компонентов (например, 3 для vec3)
    DataType type;   // Тип данных (GL_FLOAT и т.д.)
    bool normalized; // Нормализовать ли
    size_t offset;   // Смещение в байтах от начала вершины
};

} // namespace ae

#endif // AE_VERTEX_ATTRIB_H
