#ifndef AE_FONT_H
#define AE_FONT_H

#include "../../system/string.h"
#include "glyph.h"
#include "texture.h"

#include <filesystem>
#include <unordered_map>

namespace ae {

class Font
{
public:
    Font();
    ~Font() = default;

    bool loadFromFile(const std::filesystem::path &path, float requested_pixel_height);
    bool loadFromMemory(const uint8_t *data, float requested_pixel_height);

    const std::shared_ptr<Texture> &getTexture() const;

    bool isValid() const;

    float getAscent() const;
    float getDescent() const;
    float getLineGap() const;

    const Glyph *getGlyph(uint32_t codepoint) const;

    const float getPixelHeight() const;
    vec2 getTextSize(const String &string) const;

private:
    std::shared_ptr<Texture> m_texture;

    float m_ascent;
    float m_descent;
    float m_line_gap;
    float m_scale;

    std::unordered_map<uint32_t, Glyph> m_glyphs;

    float m_pixel_height;
};

} // namespace ae

#endif // AE_FONT_H
