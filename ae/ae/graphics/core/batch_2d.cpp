#include "batch_2d.h"
#include "../common/utils.h"

namespace ae {

Batch2D::Batch2D()
{
    m_vertex_array.create(std::vector<Batch2DVertex2>());
}

void Batch2D::begin()
{
    m_draw_commands.clear();
    m_vertices.clear();
}

void Batch2D::drawLine(const vec2 &from, const vec2 &to, float thickness, const Color &fill_color)
{
    int32_t start = m_vertices.size();

    vec2 dir = to - from;
    vec2 normal = normalize(vec2(-dir.y, dir.x));
    vec2 offset = normal * (thickness * 0.5f);

    vec3 p1 = vec3(from - offset, 0.0f);
    vec3 p2 = vec3(from + offset, 0.0f);
    vec3 p3 = vec3(to + offset, 0.0f);
    vec3 p4 = vec3(to - offset, 0.0f);

    m_vertices.push_back({p1, fill_color.getColor(), {0.0f, 0.0f}});
    m_vertices.push_back({p2, fill_color.getColor(), {1.0f, 0.0f}});
    m_vertices.push_back({p4, fill_color.getColor(), {0.0f, 1.0f}});

    m_vertices.push_back({p2, fill_color.getColor(), {1.0f, 0.0f}});
    m_vertices.push_back({p3, fill_color.getColor(), {1.0f, 1.0f}});
    m_vertices.push_back({p4, fill_color.getColor(), {0.0f, 1.0f}});

    addDrawCommand(nullptr, start, 6);
}

void Batch2D::drawPath(const std::vector<vec2> &points,
                       float thickness,
                       const Color &fill_color,
                       bool closed)
{
    if (points.size() < 2)
        return;

    int32_t start = m_vertices.size();
    float half_thickness = thickness * 0.5f;

    size_t count = points.size();
    size_t limit = closed ? count + 1 : count;

    vec2 prev_out, prev_in;
    bool has_prev = false;

    for (size_t i = 0; i < limit; ++i) {
        size_t i_curr = i % count;
        size_t i_prev = (i_curr + count - 1) % count;
        size_t i_next = (i_curr + 1) % count;

        const vec2 &p_curr = points[i_curr];

        // Compute directions and normals
        vec2 dir_prev, dir_next;
        vec2 normal_prev, normal_next;

        if (!closed && i == 0) {
            dir_next = normalize(points[i_next] - points[i_curr]);
            normal_prev = vec2(-dir_next.y, dir_next.x);
            normal_next = normal_prev;
        } else if (!closed && i == count - 1) {
            dir_prev = normalize(points[i_curr] - points[i_prev]);
            normal_prev = vec2(-dir_prev.y, dir_prev.x);
            normal_next = normal_prev;
        } else {
            dir_prev = normalize(points[i_curr] - points[i_prev]);
            dir_next = normalize(points[i_next] - points[i_curr]);
            normal_prev = vec2(-dir_prev.y, dir_prev.x);
            normal_next = vec2(-dir_next.y, dir_next.x);
        }

        // Miter calculation
        vec2 miter = normalize(normal_prev + normal_next);
        float miter_len = half_thickness / std::max(0.0001f, dot(miter, normal_next));
        vec2 offset = miter * miter_len;

        vec2 v_out = p_curr + offset;
        vec2 v_in = p_curr - offset;

        vec3 v_out_3d = vec3{v_out, 0.0f};
        vec3 v_in_3d = vec3{v_in, 0.0f};

        if (has_prev) {
            // Draw quad between prev and current mitered points
            m_vertices.push_back({vec3{prev_out, 0.0f}, fill_color.getColor(), vec2{0.0f, 0.0f}});
            m_vertices.push_back({vec3{prev_in, 0.0f}, fill_color.getColor(), vec2{0.0f, 0.0f}});
            m_vertices.push_back({v_out_3d, fill_color.getColor(), vec2{0.0f, 0.0f}});

            m_vertices.push_back({v_out_3d, fill_color.getColor(), vec2{0.0f, 0.0f}});
            m_vertices.push_back({vec3{prev_in, 0.0f}, fill_color.getColor(), vec2{0.0f, 0.0f}});
            m_vertices.push_back({v_in_3d, fill_color.getColor(), vec2{0.0f, 0.0f}});
        }

        prev_out = v_out;
        prev_in = v_in;
        has_prev = true;

        // В режиме `closed` ограничиваем последнюю итерацию, чтобы не зациклить лишнее
        if (!closed && i == count - 1)
            break;
    }

    addDrawCommand(nullptr, start, m_vertices.size() - start);
}

void Batch2D::drawRect(const vec2 &pos, const vec2 &size, const Color &fill_color)
{
    int32_t start = m_vertices.size();
    drawQuad(pos, pos + size, vec2{0.0f}, vec2{1.0f}, fill_color);
    addDrawCommand(nullptr, start, m_vertices.size() - start);
}

void Batch2D::drawChamferedRect(const vec2 &pos,
                                const vec2 &size,
                                const vec4 &chamfer,
                                const Color &fill_color)
{
    int32_t start = m_vertices.size();
    drawChamferedQuad(pos, pos + size, chamfer, vec2{0.0f}, vec2{1.0f}, fill_color);
    addDrawCommand(nullptr, start, m_vertices.size() - start);
}

void Batch2D::drawRectWithBorder(const vec2 &pos,
                                 const vec2 &size,
                                 const Color &fill_color,
                                 const Color &border_color,
                                 float border_thickness)
{
    int32_t start = m_vertices.size();

    vec2 inner_pos = pos + vec2{border_thickness, border_thickness};
    vec2 inner_size = size - vec2{border_thickness * 2.0f, border_thickness * 2.0f};

    if (inner_size.x > 0.0f && inner_size.y > 0.0f)
        drawQuad(inner_pos, inner_pos + inner_size, vec2{0.0f}, vec2{1.0f}, fill_color);

    // Top
    drawQuad({pos.x, pos.y + size.y - border_thickness},
             {pos.x + size.x, pos.y + size.y},
             vec2{0.0f},
             vec2{1.0f},
             border_color);

    // Bottom
    drawQuad({pos.x, pos.y},
             {pos.x + size.x, pos.y + border_thickness},
             vec2{0.0f},
             vec2{1.0f},
             border_color);

    // Left
    drawQuad({pos.x, pos.y + border_thickness},
             {pos.x + border_thickness, pos.y + size.y - border_thickness},
             vec2{0.0f},
             vec2{1.0f},
             border_color);

    // Right
    drawQuad({pos.x + size.x - border_thickness, pos.y + border_thickness},
             {pos.x + size.x, pos.y + size.y - border_thickness},
             vec2{0.0f},
             vec2{1.0f},
             border_color);

    addDrawCommand(nullptr, start, m_vertices.size() - start);
}

void Batch2D::drawTextureRect(const vec2 &pos,
                              const vec2 &size,
                              const s_ptr<Texture> &texture)
{
    int32_t start = m_vertices.size();
    drawQuad(pos, pos + size, vec2{0.0f}, vec2{1.0f}, Color::white);
    addDrawCommand(texture, start, m_vertices.size() - start);
}

void Batch2D::drawTextureFrameRect(const vec2 &pos,
                                   const vec2 &size,
                                   const vec4 &borders,
                                   const s_ptr<Texture> &texture)
{
    int32_t start = m_vertices.size();

    float tex_w = texture->getSize().x;
    float tex_h = texture->getSize().y;

    // Нормализованные UV-границы
    float u_l = borders.x / tex_w;
    float u_r = 1.0f - (borders.z / tex_w);
    float v_t = borders.y / tex_h;
    float v_b = 1.0f - (borders.w / tex_h);

    // Позиции по X
    float x0 = pos.x;
    float x1 = pos.x + borders.x;
    float x2 = pos.x + size.x - borders.z;
    float x3 = pos.x + size.x;

    // Позиции по Y
    float y0 = pos.y;
    float y1 = pos.y + borders.w;
    float y2 = pos.y + size.y - borders.y;
    float y3 = pos.y + size.y;

    // 1. Top-left
    drawQuad({x0, y0}, {x1, y1}, {0.0f, 0.0f}, {u_l, v_t}, Color::white);
    // 2. Top
    drawQuad({x1, y0}, {x2, y1}, {u_l, 0.0f}, {u_r, v_t}, Color::white);
    // 3. Top-right
    drawQuad({x2, y0}, {x3, y1}, {u_r, 0.0f}, {1.0f, v_t}, Color::white);

    // 4. Left
    drawQuad({x0, y1}, {x1, y2}, {0.0f, v_t}, {u_l, v_b}, Color::white);
    // 5. Center
    drawQuad({x1, y1}, {x2, y2}, {u_l, v_t}, {u_r, v_b}, Color::white);
    // 6. Right
    drawQuad({x2, y1}, {x3, y2}, {u_r, v_t}, {1.0f, v_b}, Color::white);

    // 7. Bottom-left
    drawQuad({x0, y2}, {x1, y3}, {0.0f, v_b}, {u_l, 1.0f}, Color::white);
    // 8. Bottom
    drawQuad({x1, y2}, {x2, y3}, {u_l, v_b}, {u_r, 1.0f}, Color::white);
    // 9. Bottom-right
    drawQuad({x2, y2}, {x3, y3}, {u_r, v_b}, {1.0f, 1.0f}, Color::white);

    addDrawCommand(texture, start, m_vertices.size() - start);
}

void Batch2D::drawText(const String &text,
                       const vec2 &pos,
                       const Color &fill_color,
                       const s_ptr<Font> &font,
                       float pixel_height,
                       float line_spaceing)
{
    if (!font)
        return;

    auto *font_page = font->getFontPage(pixel_height);

    int32_t start = m_vertices.size();

    vec2 cursor = pos;
    cursor.y += font_page->getAscent();

    for (uint32_t cp : text) {
        if (cp == '\n') {
            cursor.x = pos.x;
            cursor.y += font_page->getAscent() - font_page->getDescent() + line_spaceing;
            continue;
        }

        const Glyph *g = font_page->getGlyph(cp);
        if (!g)
            continue;

        vec2 glyphPos = cursor + g->offset;
        vec3 p = {glyphPos.x, glyphPos.y, 0.0f};

        vec3 p1 = {p.x, p.y, 0.0f};
        vec3 p2 = {p.x + g->size.x, p.y, 0.0f};
        vec3 p3 = {p.x + g->size.x, p.y + g->size.y, 0.0f};
        vec3 p4 = {p.x, p.y + g->size.y, 0.0f};

        m_vertices.push_back({p1, fill_color.getColor(), g->uv0});
        m_vertices.push_back({p2, fill_color.getColor(), {g->uv1.x, g->uv0.y}});
        m_vertices.push_back({p3, fill_color.getColor(), g->uv1});

        m_vertices.push_back({p3, fill_color.getColor(), g->uv1});
        m_vertices.push_back({p4, fill_color.getColor(), {g->uv0.x, g->uv1.y}});
        m_vertices.push_back({p1, fill_color.getColor(), g->uv0});

        cursor.x += g->advance;
    }

    addDrawCommand(font_page->getTexture(), start, m_vertices.size() - start);
}

void Batch2D::end()
{
    m_vertex_array.setData(m_vertices);
}

void Batch2D::draw() const
{
    if (m_draw_commands.empty() || m_vertices.empty())
        return;

    VertexArray::bind(m_vertex_array);

    for (const auto &draw_command : m_draw_commands) {
        if (draw_command.texture)
            Texture::bind(*draw_command.texture);
        else
            Texture::bind(*Texture::getDefaultDiffuseTexture());

        glDrawArrays(graphics_utils::primitiveTypeToGl(PrimitiveType::TRIANGLES),
                     draw_command.offset,
                     draw_command.count);

        Texture::unbind();
    }

    VertexArray::unbind();
}

void Batch2D::addDrawCommand(const s_ptr<Texture> &texture, int32_t offset, int32_t count)
{
    if (!m_draw_commands.empty() && m_draw_commands.back().texture == texture) {
        m_draw_commands.back().count += count;
        return;
    }

    DrawCommand draw_command;
    draw_command.texture = texture;
    draw_command.offset = offset;
    draw_command.count = count;
    m_draw_commands.push_back(draw_command);
}

void Batch2D::drawQuad(const vec2 &left_bottom,
                       const vec2 &right_top,
                       const vec2 &uv0,
                       const vec2 &uv1,
                       const Color &color)
{
    vec3 p1 = {left_bottom.x, left_bottom.y, 0.0f};
    vec3 p2 = {right_top.x, left_bottom.y, 0.0f};
    vec3 p3 = {right_top.x, right_top.y, 0.0f};
    vec3 p4 = {left_bottom.x, right_top.y, 0.0f};

    m_vertices.push_back({p1, color.getColor(), {uv0.x, uv0.y}});
    m_vertices.push_back({p2, color.getColor(), {uv1.x, uv0.y}});
    m_vertices.push_back({p4, color.getColor(), {uv0.x, uv1.y}});

    m_vertices.push_back({p2, color.getColor(), {uv1.x, uv0.y}});
    m_vertices.push_back({p3, color.getColor(), {uv1.x, uv1.y}});
    m_vertices.push_back({p4, color.getColor(), {uv0.x, uv1.y}});
}

void Batch2D::drawChamferedQuad(const vec2 &left_bottom,
                                const vec2 &right_top,
                                const vec4 &chamfer,
                                const vec2 &uv0,
                                const vec2 &uv1,
                                const Color &color)
{
    float l = left_bottom.x;
    float b = left_bottom.y;
    float r = right_top.x;
    float t = right_top.y;

    // Максимально допустимый срез для каждого угла
    float max_chamfer_x = 0.5f * (r - l);
    float max_chamfer_y = 0.5f * (t - b);

    // Ограничиваем каждый срез по оси X и Y
    auto clamp_chamfer = [&](float val) {
        return std::min(val, std::min(max_chamfer_x, max_chamfer_y));
    };

    float c0 = clamp_chamfer(chamfer.x); // левый нижний
    float c1 = clamp_chamfer(chamfer.y); // правый нижний
    float c2 = clamp_chamfer(chamfer.z); // правый верхний
    float c3 = clamp_chamfer(chamfer.w); // левый верхний

    // Вершины по часовой стрелке, с учётом каждого среза
    vec3 p0 = {l + c0, b, 0.0f}; // нижняя сторона между левым и правым низом
    vec3 p1 = {r - c1, b, 0.0f};
    vec3 p2 = {r, b + c1, 0.0f}; // правая сторона между низом и верхом
    vec3 p3 = {r, t - c2, 0.0f};
    vec3 p4 = {r - c2, t, 0.0f}; // верхняя сторона между правым и левым верхом
    vec3 p5 = {l + c3, t, 0.0f};
    vec3 p6 = {l, t - c3, 0.0f}; // левая сторона между верхом и низом
    vec3 p7 = {l, b + c0, 0.0f};

    auto col = color.getColor();

    vec2 size = right_top - left_bottom;
    auto uv = [&](const vec3 &p) {
        return vec2{uv0.x + (uv1.x - uv0.x) * (p.x - l) / size.x,
                    uv0.y + (uv1.y - uv0.y) * (p.y - b) / size.y};
    };

    int32_t start = m_vertices.size();

    auto triangle = [&](const vec3 &a, const vec3 &b, const vec3 &c) {
        m_vertices.push_back({a, col, uv(a)});
        m_vertices.push_back({b, col, uv(b)});
        m_vertices.push_back({c, col, uv(c)});
    };

    // Основной прямоугольник (восьмиугольник) из 6 треугольников
    triangle(p0, p1, p5);
    triangle(p1, p4, p5);
    triangle(p1, p2, p3);
    triangle(p1, p3, p4);
    triangle(p5, p4, p6);
    triangle(p4, p3, p6);
    triangle(p5, p6, p0);
    triangle(p6, p7, p0);
    triangle(p0, p7, p1);
    triangle(p7, p2, p1);
}

} // namespace ae
