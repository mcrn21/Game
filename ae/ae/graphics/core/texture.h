#ifndef AE_TEXTURE_H
#define AE_TEXTURE_H

#include "../common/enums.h"

#include <glm/glm.hpp>

#include <filesystem>
#include <stack>

using namespace glm;

namespace ae {

class Texture
{
public:    
    Texture();
    Texture(uint32_t id,
            const ivec2 &size,
            TextureFormat format,
            TextureType type = TextureType::DEFAULT);
    Texture(const std::filesystem::path &path, TextureType type = TextureType::DEFAULT);
    Texture(const uint8_t *data, int32_t size, TextureType type = TextureType::DEFAULT);
    Texture(const ivec2 &size,
            TextureFormat format,
            const uint8_t *data,
            TextureType type = TextureType::DEFAULT);
    ~Texture();

    bool loadFromFile(const std::filesystem::path &path, TextureType type = TextureType::DEFAULT);
    bool loadFromMemory(const uint8_t *data, int32_t size, TextureType type = TextureType::DEFAULT);

    uint32_t getId() const;
    const ivec2 &getSize() const;
    TextureFormat getFormat() const;

    vec4 getUVRect(const ivec4 &rect) const;

    void create2D(const ivec2 &size, TextureFormat format, const uint8_t *data);
    void createCubemap(const ivec2 &size, TextureFormat format, const uint8_t *data);
    bool isValid() const;
    void destroy();

    static void bind(const Texture &texture);
    static void unbind(int32_t count = 1);
    static void unbindAll();
    static int32_t getNextTextureNumber();

    static std::shared_ptr<Texture> getDefaultDiffuseTexture();
    static std::shared_ptr<Texture> getDefaultSpecularTexture();

private:
    static std::stack<std::pair<TextureType, uint32_t>> m_binded_textures;

    uint32_t m_id;
    ivec2 m_size;
    TextureFormat m_format;
    TextureType m_type;
};

} // namespace ae

#endif // AE_TEXTURE_H
