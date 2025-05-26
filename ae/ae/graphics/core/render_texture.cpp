#include "render_texture.h"
#include "../common/utils.h"

#include "../../../3rd/stb/stb_image_write.h"

#include <GL/glew.h>

#include <algorithm>

namespace ae {

RenderTexture::RenderTexture()
    : m_fbo{0}
    , m_depth_rbo{0}
    , m_sample_fbo{0}
    , m_sample_texture_dirty{false}
{}

RenderTexture::RenderTexture(const ivec2 &size)
    : m_fbo{0}
    , m_depth_rbo{0}
    , m_sample_fbo{0}
    , m_sample_texture_dirty{false}
{
    create(size);
}

RenderTexture::~RenderTexture()
{
    destroy();
}

const Texture &RenderTexture::getTexture() const
{
    return m_texture;
}

const Texture &RenderTexture::getSampledTexture() const
{
    if (m_sample_texture_dirty) {
        m_sample_texture_dirty = false;

        // Выполняем GPU-копирование через FBO + blit
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_sample_fbo);

        glBlitFramebuffer(0,
                          0,
                          m_texture.getSize().x,
                          m_texture.getSize().y,
                          0,
                          0,
                          m_sample_texture.getSize().x,
                          m_sample_texture.getSize().y,
                          GL_COLOR_BUFFER_BIT,
                          GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    return m_sample_texture;
}

uint32_t RenderTexture::getId() const
{
    return m_fbo;
}

void RenderTexture::create(const ivec2 &size, bool alpha)
{
    destroy();

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    uint32_t texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    GLuint gl_format = alpha ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, gl_format, size.x, size.y, 0, gl_format, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    glGenRenderbuffers(1, &m_depth_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depth_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                              GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER,
                              m_depth_rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Set texture
    m_texture = Texture{texture_id, size, alpha ? TextureFormat::RGBA : TextureFormat::RGB};

    // Sample texture
    m_sample_texture.create2D(size, alpha ? TextureFormat::RGBA : TextureFormat::RGB, nullptr);

    // Set sample fbo
    glGenFramebuffers(1, &m_sample_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_sample_fbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           m_sample_texture.getId(),
                           0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    RenderTarget::setSize(size);
    RenderTarget::setViewport(ivec4{0, 0, size.x, size.y});
}

bool RenderTexture::isValid() const
{
    return m_texture.isValid() && m_fbo != 0 && m_sample_fbo != 0;
}

void RenderTexture::destroy()
{
    m_texture.destroy();

    if (m_fbo != 0) {
        glDeleteFramebuffers(1, &m_fbo);
        m_fbo = 0;
    }

    if (m_depth_rbo != 0) {
        glDeleteRenderbuffers(1, &m_depth_rbo);
        m_depth_rbo = 0;
    }

    m_sample_texture.destroy();

    if (m_sample_fbo != 0) {
        glDeleteFramebuffers(1, &m_sample_fbo);
        m_sample_fbo = 0;
    }
}

void RenderTexture::clear() const
{
    if (isValid())
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    RenderTarget::clear();
}

void RenderTexture::display() const
{
    if (isValid()) {
        m_sample_texture_dirty = true;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void RenderTexture::saveToFile(const std::filesystem::path &path) const
{
    if (!isValid())
        return;

    int width = getSize().x;
    int height = getSize().y;
    int pixel_count = width * height;

    GLenum format = graphics_utils::textureFormatToGl(m_texture.getFormat());
    int components = 0;
    switch (format) {
    case GL_RED:
        components = 1;
        break;
    case GL_RG:
        components = 2;
        break;
    case GL_RGB:
        components = 3;
        break;
    case GL_RGBA:
        components = 4;
        break;
    default:
        components = 1;
        break;
    }

    std::vector<float> float_data(pixel_count * components);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glReadPixels(0, 0, width, height, format, GL_FLOAT, float_data.data());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Конвертируем в RGB (даже если текстура — RED)
    std::vector<uint8_t> out_data;
    out_data.reserve(pixel_count * 3); // всегда сохраняем как RGB PNG

    for (int i = 0; i < pixel_count; ++i) {
        float r = float_data[i * components + 0];
        float g = components > 1 ? float_data[i * components + 1] : r;
        float b = components > 2 ? float_data[i * components + 2] : r;

        out_data.push_back(static_cast<uint8_t>(std::clamp(r, 0.0f, 1.0f) * 255.0f));
        out_data.push_back(static_cast<uint8_t>(std::clamp(g, 0.0f, 1.0f) * 255.0f));
        out_data.push_back(static_cast<uint8_t>(std::clamp(b, 0.0f, 1.0f) * 255.0f));
    }

    stbi_write_png(path.c_str(), width, height, 3, out_data.data(), width * 3);
}

} // namespace ae
