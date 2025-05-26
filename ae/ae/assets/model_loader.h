#ifndef AE_MODEL_LOADER_H
#define AE_MODEL_LOADER_H

#include "../graphics/scene/assimp_helper.h"
#include "../graphics/scene/model.h"
#include "asset_loader.h"
#include "assets.h"

namespace ae {

template<>
class AssetLoader<Model>
{
public:
    static std::shared_ptr<Model> loadFromFile(Assets *assets,
                                               const std::string &asset_name,
                                               const std::filesystem::path &path)
    {
        if (path.empty())
            return nullptr;

        std::string name = asset_name.empty() ? path.stem().string() : asset_name;
        if (name.empty())
            return nullptr;

        auto model = std::make_shared<Model>();

        AssimpHelper assimp_helper{path, assets};
        if (!assimp_helper.load(model.get()))
            return nullptr;

        assets->add(name, model);

        return model;
    }
};

} // namespace ae

#endif // AE_MODEL_LOADER_H
