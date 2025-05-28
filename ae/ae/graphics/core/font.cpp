#include "font.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <GL/glew.h>
#include <algorithm>
#include <cstring>
#include <fstream>

namespace ae {

FontPage::FontPage()
    : m_pixel_height{0.0f}
    , m_ascent{0.0f}
    , m_descent{0.0f}
    , m_line_gap{0.0f}
{}

bool FontPage::load(const uint8_t *data, int32_t size, float pixel_height)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
        return false;

    FT_Face face;
    if (FT_New_Memory_Face(ft, data, static_cast<FT_Long>(size), 0, &face))
        return false;

    FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(pixel_height));

    const int32_t atlas_width = 2048;
    const int32_t atlas_height = 2048;

    std::vector<uint8_t> atlas(atlas_width * atlas_height, 0);

    int32_t pen_x = 1, pen_y = 1; // Текущая позиция в атласе
    int32_t row_height = 0;

    auto insertGlyph = [&](uint32_t codepoint) {
        if (FT_Load_Char(face, codepoint, FT_LOAD_RENDER))
            return;

        FT_GlyphSlot g = face->glyph;

        if (pen_x + g->bitmap.width + 1 >= atlas_width) {
            pen_x = 1;
            pen_y += row_height + 1;
            row_height = 0;
        }

        if (pen_y + g->bitmap.rows >= atlas_height)
            return;

        // Копируем bitmap в атлас
        for (int32_t y = 0; y < g->bitmap.rows; ++y) {
            for (int32_t x = 0; x < g->bitmap.width; ++x) {
                int32_t dst_x = pen_x + x;
                int32_t dst_y = pen_y + y;
                atlas[dst_y * atlas_width + dst_x] = g->bitmap.buffer[y * g->bitmap.pitch + x];
            }
        }

        Glyph glyph;
        glyph.size = vec2{g->bitmap.width, g->bitmap.rows};
        glyph.offset = vec2{g->bitmap_left, -g->bitmap_top};
        glyph.advance = g->advance.x / 64.0f; // 26.6 fixed point to float
        glyph.uv0 = vec2{static_cast<float>(pen_x) / atlas_width,
                         static_cast<float>(pen_y) / atlas_height};
        glyph.uv1 = vec2{static_cast<float>(pen_x + g->bitmap.width) / atlas_width,
                         static_cast<float>(pen_y + g->bitmap.rows) / atlas_height};

        m_glyphs[codepoint] = glyph;

        pen_x += g->bitmap.width + 1;
        row_height = std::max(static_cast<uint32_t>(row_height), g->bitmap.rows);
    };

    // ASCII + Кириллица + Ё/ё
    for (uint32_t c = 32; c <= 126; ++c)
        insertGlyph(c);
    for (uint32_t c = 0x410; c <= 0x44F; ++c)
        insertGlyph(c);
    insertGlyph(0x401); // Ё
    insertGlyph(0x451); // ё

    // Создаём текстуру
    uint32_t texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RED,
                 atlas_width,
                 atlas_height,
                 0,
                 GL_RED,
                 GL_UNSIGNED_BYTE,
                 atlas.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_ONE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_ONE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_ONE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);

    m_texture = SharedPtr<Texture>::create(texture_id,
                                           ivec2{atlas_width, atlas_height},
                                           TextureFormat::RED);

    m_pixel_height = pixel_height;
    m_ascent = face->size->metrics.ascender / 64.0f;
    m_descent = face->size->metrics.descender / 64.0f;
    m_line_gap = (face->size->metrics.height - face->size->metrics.ascender
                  + abs(face->size->metrics.descender))
                 / 64.0f;

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return true;
}

bool FontPage::isValid() const
{
    return m_texture && m_texture->isValid();
}

float FontPage::getPixelHeight() const
{
    return m_pixel_height;
}

float FontPage::getAscent() const
{
    return m_ascent;
}

float FontPage::getDescent() const
{
    return m_descent;
}

float FontPage::getLineGap() const
{
    return m_line_gap;
}

const SharedPtr<Texture> &FontPage::getTexture() const
{
    return m_texture;
}

const Glyph *FontPage::getGlyph(uint32_t codepoint) const
{
    auto it = m_glyphs.find(codepoint);
    return (it != m_glyphs.end()) ? &it->second : nullptr;
}

vec2 FontPage::getTextSize(const String &string, float line_spaceing) const
{
    float max_line_width = 0.0f;
    float current_line_width = 0.0f;
    int32_t line_count = 1;

    for (uint32_t codepoint : string) {
        if (codepoint == '\n') {
            max_line_width = std::max(max_line_width, current_line_width);
            current_line_width = 0.0f;
            line_count++;
            continue;
        }

        const Glyph *glyph = getGlyph(codepoint);
        if (glyph)
            current_line_width += glyph->advance;
    }

    max_line_width = std::max(max_line_width, current_line_width);
    float line_height = m_ascent - m_descent + line_spaceing;
    float total_height = line_count * line_height;

    return vec2{max_line_width, total_height};
}

Font::Font() {}

bool Font::loadFromFile(const std::filesystem::path &path)
{
    m_font_data.clear();
    m_pages.clear();

    std::ifstream file(path, std::ios::binary);
    if (!file)
        return false;

    m_font_data = std::vector<uint8_t>((std::istreambuf_iterator<char>(file)), {});
    return true;
}

bool Font::loadFromMemory(const uint8_t *data, int32_t size)
{
    if (!data || size == 0)
        return false;

    m_font_data.clear();
    m_pages.clear();

    m_font_data.resize(size);
    std::memcpy(m_font_data.data(), data, size);
    return true;
}

const FontPage *Font::getFontPage(float pixel_height) const
{
    if (m_font_data.empty())
        return nullptr;

    auto found = m_pages.find(pixel_height);
    if (found != m_pages.end())
        return found->second.get();

    auto page = std::make_unique<FontPage>();
    if (!page->load(m_font_data.data(), m_font_data.size(), pixel_height))
        return nullptr;
    auto [it, inserted] = m_pages.emplace(pixel_height, std::move(page));
    return it->second.get();
}

} // namespace ae
