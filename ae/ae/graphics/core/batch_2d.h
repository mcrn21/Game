#ifndef AE_BATCH_2D_H
#define AE_BATCH_2D_H

#include "../../system/string.h"
#include "color.h"
#include "font.h"
#include "texture.h"
#include "vertex_array.h"
#include "vertex_attrib.h"

#include <unordered_map>
#include <vector>

namespace ae {

struct Batch2DVertex2
{
    vec3 position{0.0f};
    vec4 color{0.0f};
    vec2 tex_coords{0.0f};
};

template<>
inline const std::vector<VertexAttrib> &VertexAttrib::get<Batch2DVertex2>()
{
    static std::vector<VertexAttrib> attribs
        = {{0, 3, DataType::FLOAT, false, offsetof(Batch2DVertex2, position)},
           {1, 4, DataType::FLOAT, false, offsetof(Batch2DVertex2, color)},
           {2, 2, DataType::FLOAT, false, offsetof(Batch2DVertex2, tex_coords)}};
    return attribs;
}

class Batch2D
{
public:
    Batch2D();
    ~Batch2D() = default;

    void begin();

    void drawLine(const vec2 &from, const vec2 &to, float thickness, const Color &fill_color);
    void drawPath(const std::vector<vec2> &points,
                  float thickness,
                  const Color &fill_color,
                  bool closed = false);

    void drawRect(const vec2 &pos, const vec2 &size, const Color &fill_color);
    void drawChamferedRect(const vec2 &pos,
                           const vec2 &size,
                           const vec4 &chamfer,
                           const Color &fill_color);
    void drawRectWithBorder(const vec2 &pos,
                            const vec2 &size,
                            const Color &fill_color,
                            const Color &border_color,
                            float border_thickness);

    void drawTextureRect(const vec2 &pos, const vec2 &size, const SharedPtr<Texture> &texture);
    void drawTextureFrameRect(const vec2 &pos,
                              const vec2 &size,
                              const vec4 &borders,
                              const SharedPtr<Texture> &texture);

    void drawText(const String &text,
                  const vec2 &pos,
                  const Color &fill_color,
                  const SharedPtr<Font> &font,
                  float pixel_height = 32.0f,
                  float line_spaceing = 0.0f);

    void end();

    void draw() const;

private:
    void addDrawCommand(const SharedPtr<Texture> &texture, int32_t offset, int32_t count);

    void drawQuad(const vec2 &left_bottom,
                  const vec2 &right_top,
                  const vec2 &uv0,
                  const vec2 &uv1,
                  const Color &color);

    void drawChamferedQuad(const vec2 &left_bottom,
                           const vec2 &right_top,
                           const vec4 &chamfer,
                           const vec2 &uv0,
                           const vec2 &uv1,
                           const Color &color);

private:
    struct DrawCommand
    {
        SharedPtr<Texture> texture;
        int32_t offset;
        int32_t count;
    };

    std::vector<DrawCommand> m_draw_commands;
    std::vector<Batch2DVertex2> m_vertices;

    mutable VertexArray m_vertex_array;
};

} // namespace ae

#endif // AE_BATCH_2D_H
