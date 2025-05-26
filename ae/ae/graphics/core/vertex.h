#ifndef AE_VERTEX_H
#define AE_VERTEX_H

#define MAX_BONE_INFLUENCE 4

#include "../common/enums.h"
#include "vertex_attrib.h"

#include <glm/glm.hpp>

using namespace glm;

namespace ae {

struct Vertex
{
    vec3 position{0.0f};
    vec3 normal{0.0f};
    vec4 color{0.0f};
    vec2 tex_coords{0.0f};

    int32_t bone_ids[MAX_BONE_INFLUENCE] = {-1, -1, -1, -1};      // индексы костей
    float weights[MAX_BONE_INFLUENCE] = {0.0f, 0.0f, 0.0f, 0.0f}; // веса влияния костей
};

template<>
inline const std::vector<VertexAttrib> &VertexAttrib::get<Vertex>()
{
    static std::vector<VertexAttrib> attribs
        = {{0, 3, DataType::FLOAT, false, offsetof(Vertex, position)},
           {1, 3, DataType::FLOAT, false, offsetof(Vertex, normal)},
           {2, 4, DataType::FLOAT, false, offsetof(Vertex, color)},
           {3, 2, DataType::FLOAT, false, offsetof(Vertex, tex_coords)},
           {4, 4, DataType::INT, false, offsetof(Vertex, bone_ids)},
           {5, 4, DataType::FLOAT, false, offsetof(Vertex, weights)}};
    return attribs;
}

struct Vertex2D
{
    vec2 position;
    vec4 color;
    vec2 tex_coords;
};

template<>
inline const std::vector<VertexAttrib> &VertexAttrib::get<Vertex2D>()
{
    static std::vector<VertexAttrib> attribs
        = {{0, 2, DataType::FLOAT, false, offsetof(Vertex2D, position)},
           {1, 4, DataType::FLOAT, false, offsetof(Vertex2D, color)},
           {2, 2, DataType::FLOAT, false, offsetof(Vertex2D, tex_coords)}};
    return attribs;
}

} // namespace ae

#endif // AE_VERTEX_H
