#ifndef AE_FONT_LOADER_H
#define AE_FONT_LOADER_H

#include "../graphics/core/font.h"
#include "asset_loader.h"
#include "assets.h"

namespace ae {

template<>
class AssetLoader<Font>
{
public:
    static s_ptr<Font> loadFromFile(Assets *assets,
                                        const std::string &asset_name,
                                        const std::filesystem::path &path)
    {
        if (path.empty())
            return nullptr;

        std::string name = asset_name.empty() ? path.stem().string() : asset_name;
        if (name.empty())
            return nullptr;

        auto font = createShared<Font>();
        if (!font->loadFromFile(path))
            return nullptr;

        assets->add(name, font);

        return font;
    }

    static s_ptr<Font> loadFromMemory(Assets *assets,
                                          const std::string &asset_name,
                                          const uint8_t *data,
                                          int32_t size)
    {
        if (!data)
            return nullptr;

        auto font = createShared<Font>();
        if (!font->loadFromMemory(data, size))
            return nullptr;

        assets->add(asset_name, font);

        return font;
    }
};

} // namespace ae

#endif // AE_FONT_LOADER_H
