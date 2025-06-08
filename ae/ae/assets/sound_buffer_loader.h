#ifndef AE_SOUND_BUFFER_LOADER_H
#define AE_SOUND_BUFFER_LOADER_H

#include "../audio/sound_buffer.h"
#include "asset_loader.h"
#include "assets.h"

namespace ae {

template<>
class AssetLoader<SoundBuffer>
{
public:
    static s_ptr<SoundBuffer> loadFromFile(Assets *assets,
                                           const std::string &asset_name,
                                           const std::filesystem::path &path)
    {
        if (path.empty())
            return nullptr;

        std::string name = asset_name.empty() ? path.stem().string() : asset_name;
        if (name.empty())
            return nullptr;

        auto sound_buffer = createShared<SoundBuffer>();
        if (!sound_buffer->loadFromFile(path))
            return nullptr;

        assets->add(name, sound_buffer);

        return sound_buffer;
    }

    static s_ptr<SoundBuffer> loadFromMemory(Assets *assets,
                                             const std::string &asset_name,
                                             const uint8_t *data,
                                             int32_t size)
    {
        if (size == 0)
            return nullptr;

        auto sound_buffer = createShared<SoundBuffer>();
        if (!sound_buffer->loadFromMemory(data, size))
            return nullptr;

        assets->add(asset_name, sound_buffer);

        return sound_buffer;
    }
};

} // namespace ae

#endif // AE_SOUND_BUFFER_LOADER_H
