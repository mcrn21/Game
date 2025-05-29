#include "texture.h"
#include "../common/utils.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "../../../3rd/stb/stb_image.h"

#include <GL/glew.h>

#include <vector>

namespace ae {

const uint8_t DEFAULT_DIFFUSE_TEXTURE_DATA[] = {255, 255, 255, 255};
const uint8_t DEFAULT_SPECULAR_TEXTURE_DATA[] = {0, 0, 0, 255};

std::stack<std::pair<TextureType, uint32_t>> Texture::m_binded_textures;

Texture::Texture()
    : m_id{0}
    , m_size{0}
    , m_type{TextureType::DEFAULT}
{}

Texture::Texture(uint32_t id, const i32vec2 &size, TextureFormat format, TextureType type)
    : m_id{id}
    , m_size{size}
    , m_format{format}
    , m_type{type}
{}

Texture::Texture(const std::filesystem::path &path, TextureType type)
    : m_id{0}
    , m_size{0}
{
    loadFromFile(path, type);
}

Texture::Texture(const uint8_t *data, int32_t size, TextureType type)
    : m_id{0}
    , m_size{0}
{
    loadFromMemory(data, size, type);
}

Texture::Texture(const ivec2 &size, TextureFormat format, const uint8_t *data, TextureType type)
    : m_id{0}
    , m_size{0}
{
    switch (type) {
    case TextureType::DEFAULT:
        create2D(size, format, data);
        break;
    case TextureType::CUBE_MAP:
        createCubemap(size, format, data);
        break;
    }
}

Texture::~Texture()
{
    destroy();
}

bool Texture::loadFromFile(const std::filesystem::path &path, TextureType type)
{
    if (path.empty())
        return false;

    int32_t width = 0;
    int32_t height = 0;
    int32_t nr_components = 0;

    uint8_t *image_data = stbi_load(path.c_str(), &width, &height, &nr_components, 0);

    if (!image_data)
        return false;

    TextureFormat format;
    if (nr_components == 1) {
        stbi_image_free(image_data);
        return false;
    } else if (nr_components == 2) {
        stbi_image_free(image_data);
        return false;
    } else if (nr_components == 3)
        format = TextureFormat::RGB;
    else if (nr_components == 4)
        format = TextureFormat::RGBA;

    switch (type) {
    case TextureType::DEFAULT:
        create2D({width, height}, format, image_data);
        break;
    case TextureType::CUBE_MAP:
        createCubemap({width, height}, format, image_data);
        break;
    }

    stbi_image_free(image_data);

    return true;
}

bool Texture::loadFromMemory(const uint8_t *data, int32_t size, TextureType type)
{
    if (data == nullptr || size <= 0)
        return false;

    uint8_t *image_data = nullptr;
    int32_t width = 0;
    int32_t height = 0;
    int32_t nr_components = 0;

    image_data = stbi_load_from_memory(data, size, &width, &height, &nr_components, 0);

    if (!image_data)
        return false;

    TextureFormat format;
    if (nr_components == 1) {
        stbi_image_free(image_data);
        return false;
    } else if (nr_components == 2) {
        stbi_image_free(image_data);
        return false;
    } else if (nr_components == 3)
        format = TextureFormat::RGB;
    else if (nr_components == 4)
        format = TextureFormat::RGBA;

    switch (type) {
    case TextureType::DEFAULT:
        create2D({width, height}, format, image_data);
        break;
    case TextureType::CUBE_MAP:
        createCubemap({width, height}, format, image_data);
        break;
    }

    stbi_image_free(image_data);

    return true;
}

uint32_t Texture::getId() const
{
    return m_id;
}

const ivec2 &Texture::getSize() const
{
    return m_size;
}

TextureFormat Texture::getFormat() const
{
    return m_format;
}

vec4 Texture::getUVRect(const ivec4 &rect) const
{
    if (m_id == 0 || m_size.x == 0 || m_size.y == 0 || rect.z == 0 || rect.w == 0)
        return vec4{0.0f};

    vec4 uv{0.0f};

    uv.x = static_cast<float>(rect.x) / m_size.x;
    uv.y = static_cast<float>(rect.y) / m_size.y;
    uv.z = static_cast<float>(rect.x + rect.z) / m_size.x;
    uv.w = static_cast<float>(rect.y + rect.w) / m_size.y;

    return uv;
}

void Texture::create2D(const ivec2 &size, TextureFormat format, const uint8_t *data)
{
    destroy();

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 graphics_utils::textureFormatToGl(format),
                 size.x,
                 size.y,
                 0,
                 graphics_utils::textureFormatToGl(format),
                 GL_UNSIGNED_BYTE,
                 data);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    m_size = size;
    m_format = format;
    m_type = TextureType::DEFAULT;
}

void Texture::createCubemap(const ivec2 &size, TextureFormat format, const uint8_t *data)
{
    destroy();

    int32_t face_size = size.x / 4;
    if (size.y != face_size * 3)
        return;

    const std::vector<std::pair<int32_t, int32_t>> face_coords = {
        {2, 1}, // POSITIVE_X (right)
        {0, 1}, // NEGATIVE_X (left)
        {1, 0}, // POSITIVE_Y (top)
        {1, 2}, // NEGATIVE_Y (bottom)
        {1, 1}, // POSITIVE_Z (front)
        {3, 1}  // NEGATIVE_Z (back)
    };

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, size.x);

    for (int32_t i = 0; i < 6; ++i) {
        auto [fx, fy] = face_coords[i];

        const uint8_t *face_data
            = &data[(fy * face_size * size.x + fx * face_size) * static_cast<int32_t>(format)];

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0,
                     graphics_utils::textureFormatToGl(format),
                     face_size,
                     face_size,
                     0,
                     graphics_utils::textureFormatToGl(format),
                     GL_UNSIGNED_BYTE,
                     face_data);
    }

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    m_size = size;
    m_format = format;
    m_type = TextureType::CUBE_MAP;
}

bool Texture::isValid() const
{
    return m_id != 0;
}

void Texture::destroy()
{
    if (m_id != 0)
        glDeleteTextures(1, &m_id);
}

void Texture::bind(const Texture &texture)
{
    if (texture.m_id == 0)
        return;

    int32_t texture_num = static_cast<int32_t>(m_binded_textures.size());

    glActiveTexture(GL_TEXTURE0 + texture_num);
    glBindTexture(graphics_utils::textureTypeToGl(texture.m_type), texture.m_id);

    m_binded_textures.push({texture.m_type, texture.m_id});
}

void Texture::unbind(int32_t count)
{
    count = std::min(count, static_cast<int32_t>(m_binded_textures.size()));
    while (count > 0) {
        glActiveTexture(GL_TEXTURE0 + static_cast<int32_t>(m_binded_textures.size() - 1));
        glBindTexture(graphics_utils::textureTypeToGl(m_binded_textures.top().first), 0);
        m_binded_textures.pop();
        --count;
    }
}

void Texture::unbindAll()
{
    while (!m_binded_textures.empty())
        unbind();
}

int32_t Texture::getNextTextureNumber()
{
    return m_binded_textures.size();
}

s_ptr<Texture> Texture::getDefaultDiffuseTexture()
{
    static s_ptr<Texture> texture = createShared<Texture>(ivec2{1, 1},
                                                              TextureFormat::RGB,
                                                              &DEFAULT_DIFFUSE_TEXTURE_DATA[0]);
    return texture;
}

s_ptr<Texture> Texture::getDefaultSpecularTexture()
{
    static s_ptr<Texture> texture = createShared<Texture>(ivec2{1, 1},
                                                              TextureFormat::RGB,
                                                              &DEFAULT_SPECULAR_TEXTURE_DATA[0]);
    return texture;
}

} // namespace ae
