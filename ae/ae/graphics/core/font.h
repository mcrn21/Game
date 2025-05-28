#ifndef AE_FONT_H
#define AE_FONT_H

#include "../../system/string.h"
#include "glyph.h"
#include "texture.h"

#include <filesystem>
#include <unordered_map>
#include <vector>

namespace ae {

class FontPage
{
public:
    FontPage();
    ~FontPage() = default;

    bool load(const uint8_t *data, int32_t size, float pixel_height);

    bool isValid() const;

    float getPixelHeight() const;
    float getAscent() const;
    float getDescent() const;
    float getLineGap() const;

    const std::shared_ptr<Texture> &getTexture() const;
    const Glyph *getGlyph(uint32_t codepoint) const;

    vec2 getTextSize(const String &string, float line_spaceing = 0.0f) const;

private:
    float m_pixel_height;
    float m_ascent;
    float m_descent;
    float m_line_gap;

    std::shared_ptr<Texture> m_texture;
    std::unordered_map<uint32_t, Glyph> m_glyphs;
};

class Font
{
public:
    Font();
    ~Font() = default;

    bool loadFromFile(const std::filesystem::path &path);
    bool loadFromMemory(const uint8_t *data, int32_t size);

    const FontPage *getFontPage(float pixel_height) const;

private:
    std::vector<uint8_t> m_font_data;
    mutable std::unordered_map<float, std::unique_ptr<FontPage>> m_pages;
};

} // namespace ae

#endif // AE_FONT_H
