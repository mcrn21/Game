#include "font.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "../../../3rd/stb/stb_truetype.h"

#include <GL/glew.h>
#include <fstream>
#include <vector>

namespace ae {

Font::Font()
    : m_ascent{0.0f}
    , m_descent{0.0f}
    , m_line_gap{0.0f}
    , m_scale{0.0f}
{}
bool Font::loadFromFile(const std::filesystem::path &path, float requested_pixel_height)
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
        return false;

    std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(file)), {});

    return loadFromMemory(buffer.data(), requested_pixel_height);
}

bool Font::loadFromMemory(const uint8_t *data, float requested_pixel_height)
{
    stbtt_fontinfo font;

    if (!stbtt_InitFont(&font, data, 0))
        return false;

    const int32_t atlas_width = 2048;
    const int32_t atlas_height = 2048;

    std::vector<uint8_t> atlas;
    atlas.resize(atlas_width * atlas_height, 0);

    stbtt_pack_context ctx;
    stbtt_PackBegin(&ctx, atlas.data(), atlas_width, atlas_height, 0, 1, nullptr);

    stbtt_PackSetOversampling(&ctx, 3, 3);

    // Минимальный размер атласа, чтобы глифы были читаемы при маленьком размере
    float atlas_pixel_height = std::max(requested_pixel_height, 96.0f);

    const int32_t ascii_count = 127 - 32 + 1;
    const int32_t cyrillic_count = 0x44F - 0x410 + 1;
    const int32_t total_chars = ascii_count + cyrillic_count + 2;

    std::vector<stbtt_packedchar> packed;
    packed.resize(total_chars);

    stbtt_pack_range ranges[4] = {};

    // ASCII
    ranges[0].font_size = atlas_pixel_height;
    ranges[0].first_unicode_codepoint_in_range = 32;
    ranges[0].num_chars = ascii_count;
    ranges[0].chardata_for_range = packed.data();

    // Кириллица (А–я)
    ranges[1].font_size = atlas_pixel_height;
    ranges[1].first_unicode_codepoint_in_range = 0x410;
    ranges[1].num_chars = cyrillic_count;
    ranges[1].chardata_for_range = packed.data() + ascii_count;

    // Ё
    ranges[2].font_size = atlas_pixel_height;
    ranges[2].first_unicode_codepoint_in_range = 0x401;
    ranges[2].num_chars = 1;
    ranges[2].chardata_for_range = packed.data() + ascii_count + cyrillic_count;

    // ё
    ranges[3].font_size = atlas_pixel_height;
    ranges[3].first_unicode_codepoint_in_range = 0x451;
    ranges[3].num_chars = 1;
    ranges[3].chardata_for_range = packed.data() + ascii_count + cyrillic_count + 1;

    stbtt_PackFontRanges(&ctx, data, 0, ranges, 4);
    stbtt_PackEnd(&ctx);

    // Создаём OpenGL текстуру
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

    m_texture = std::make_shared<Texture>(texture_id,
                                          ivec2{atlas_width, atlas_height},
                                          TextureFormat::RED);

    // Коэффициент масштабирования глифов из атласа к нужному размеру
    float scale = requested_pixel_height / atlas_pixel_height;

    int32_t asc, desc, gap;
    stbtt_GetFontVMetrics(&font, &asc, &desc, &gap);
    m_scale = stbtt_ScaleForPixelHeight(&font, requested_pixel_height);
    m_ascent = asc * m_scale;
    m_descent = desc * m_scale;
    m_line_gap = gap * m_scale;

    float cx = 0.0f, cy = 0.0f;
    for (int32_t i = 0; i < total_chars; ++i) {
        cx = 0.0f, cy = 0.0f;

        stbtt_aligned_quad q;
        stbtt_GetPackedQuad(packed.data(), atlas_width, atlas_height, i, &cx, &cy, &q, 0);

        uint32_t codepoint;
        if (i < ascii_count)
            codepoint = 32 + i;
        else if (i < ascii_count + cyrillic_count)
            codepoint = 0x410 + (i - ascii_count);
        else if (i == ascii_count + cyrillic_count)
            codepoint = 0x401;
        else
            codepoint = 0x451;

        Glyph g;
        g.size = vec2(q.x1 - q.x0, q.y1 - q.y0) * scale;
        g.offset = vec2(q.x0, q.y0) * scale;
        g.advance = packed[i].xadvance * scale;
        g.uv0 = vec2(q.s0, q.t0);
        g.uv1 = vec2(q.s1, q.t1);

        m_glyphs[codepoint] = g;
    }

    m_pixel_height = requested_pixel_height;

    return true;
}

const std::shared_ptr<Texture> &Font::getTexture() const
{
    return m_texture;
}

bool Font::isValid() const
{
    return m_texture && m_texture->isValid();
}

float Font::getAscent() const
{
    return m_ascent;
}
float Font::getDescent() const
{
    return m_descent;
}
float Font::getLineGap() const
{
    return m_line_gap;
}

const Glyph *Font::getGlyph(uint32_t codepoint) const
{
    auto it = m_glyphs.find(codepoint);
    return (it != m_glyphs.end()) ? &it->second : nullptr;
}

const float Font::getPixelHeight() const
{
    return m_pixel_height;
}

vec2 Font::getTextSize(const String &string) const
{
    float max_line_width = 0.0f;
    float current_line_width = 0.0f;
    int line_count = 1;

    for (uint32_t ch : string) {
        if (ch == '\n') {
            max_line_width = std::max(max_line_width, current_line_width);
            current_line_width = 0.0f;
            line_count++;
            continue;
        }

        const Glyph *glyph = getGlyph(ch);
        if (glyph)
            current_line_width += glyph->advance;
    }

    max_line_width = std::max(max_line_width, current_line_width);
    float line_height = m_ascent - m_descent + m_line_gap;
    float total_height = line_count * line_height;

    return vec2{max_line_width, total_height};
}

} // namespace ae
