#ifndef AE_ASSET_LOADER_H
#define AE_ASSET_LOADER_H

#include "../system/memory.h"

namespace ae {

class Assets;

template<typename T>
class AssetLoader
{
public:
    template<typename... Args>
    static s_ptr<T> loadFromFile(Assets *, const std::string &, Args &&...args)
    {
        return nullptr;
    }

    template<typename... Args>
    static s_ptr<T> loadFromMemory(Assets *, const std::string &, Args &&...args)
    {
        return nullptr;
    }
};

} // namespace ae

#endif // AE_ASSET_LOADER_H
