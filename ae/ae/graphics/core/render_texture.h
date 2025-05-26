#ifndef AE_RENDER_TEXTURE_H
#define AE_RENDER_TEXTURE_H

#include "render_target.h"
#include "texture.h"

#include <filesystem>

namespace ae {

class RenderTexture : public RenderTarget
{
public:
    RenderTexture();
    RenderTexture(const ivec2 &size);
    ~RenderTexture();

    const Texture &getTexture() const;
    const Texture &getSampledTexture() const;
    uint32_t getId() const;

    void create(const ivec2 &size, bool alpha = false);
    bool isValid() const;
    void destroy();

    void clear() const;
    void display() const;

    void saveToFile(const std::filesystem::path &path) const;

private:
    Texture m_texture;
    uint32_t m_fbo;
    uint32_t m_depth_rbo;

    Texture m_sample_texture;
    uint32_t m_sample_fbo;
    mutable bool m_sample_texture_dirty;
};

} // namespace ae

#endif // AE_RENDER_TEXTURE_H
