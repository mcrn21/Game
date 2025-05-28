#ifndef AE_TEXTURE_LOADER_H
#define AE_TEXTURE_LOADER_H

#include "../graphics/core/texture.h"
#include "asset_loader.h"
#include "assets.h"

namespace ae {

template<>
class AssetLoader<Texture>
{
public:
    static SharedPtr<Texture> loadFromFile(Assets *assets,
                                           const std::string &asset_name,
                                           const std::filesystem::path &path,
                                           TextureType type = TextureType::DEFAULT)
    {
        if (path.empty())
            return nullptr;

        std::string name = asset_name.empty() ? path.stem().string() : asset_name;
        if (name.empty())
            return nullptr;

        auto texture = SharedPtr<Texture>::create();
        if (!texture->loadFromFile(path, type))
            return nullptr;

        assets->add(name, texture);

        return texture;
    }

    static SharedPtr<Texture> loadFromMemory(Assets *assets,
                                             const std::string &asset_name,
                                             const uint8_t *data,
                                             int32_t size,
                                             TextureType type = TextureType::DEFAULT)
    {
        if (size == 0)
            return nullptr;

        auto texture = SharedPtr<Texture>::create();
        if (!texture->loadFromMemory(data, size, type))
            return nullptr;

        assets->add(asset_name, texture);

        return texture;
    }
};

} // namespace ae

#endif // AE_TEXTURE_LOADER_H
