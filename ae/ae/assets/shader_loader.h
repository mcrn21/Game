#ifndef AE_SHADER_LOADER_H
#define AE_SHADER_LOADER_H

#include "../graphics/core/shader.h"
#include "asset_loader.h"
#include "assets.h"

namespace ae {

template<>
class AssetLoader<Shader>
{
public:
    static std::shared_ptr<Shader> loadFromFile(Assets *assets,
                                                const std::string &asset_name,
                                                const std::filesystem::path &path,
                                                ShaderType type)
    {
        if (asset_name.empty())
            return nullptr;

        auto shader = std::make_shared<Shader>();
        if (shader->loadFromFile(path, type))
            assets->add<Shader>(asset_name, shader);

        return shader;
    }

    static std::shared_ptr<Shader> loadFromFile(Assets *assets,
                                                const std::string &asset_name,
                                                const std::filesystem::path &vertex_path,
                                                const std::filesystem::path &fragment_path)
    {
        if (asset_name.empty())
            return nullptr;

        auto shader = std::make_shared<Shader>();
        if (shader->loadFromFile(vertex_path, fragment_path))
            assets->add<Shader>(asset_name, shader);

        return shader;
    }

    static std::shared_ptr<Shader> loadFromFile(Assets *assets,
                                                const std::string &asset_name,
                                                const std::filesystem::path &vertex_path,
                                                const std::filesystem::path &geometry_path,
                                                const std::filesystem::path &fragment_path)
    {
        if (asset_name.empty())
            return nullptr;

        auto shader = std::make_shared<Shader>();
        if (shader->loadFromFile(vertex_path, geometry_path, fragment_path))
            assets->add<Shader>(asset_name, shader);

        return shader;
    }

    static std::shared_ptr<Shader> loadFromMemory(Assets *assets,
                                                  const std::string &asset_name,
                                                  const std::string &shader_code,
                                                  ShaderType type)
    {
        if (asset_name.empty())
            return nullptr;

        auto shader = std::make_shared<Shader>();
        if (shader->loadFromMemory(shader_code, type))
            assets->add<Shader>(asset_name, shader);

        return shader;
    }

    static std::shared_ptr<Shader> loadFromMemory(Assets *assets,
                                                  const std::string &asset_name,
                                                  const std::string &vertex_shader,
                                                  const std::string &fragment_shader)
    {
        if (asset_name.empty())
            return nullptr;

        auto shader = std::make_shared<Shader>();
        if (shader->loadFromMemory(vertex_shader, fragment_shader))
            assets->add<Shader>(asset_name, shader);

        return shader;
    }

    static std::shared_ptr<Shader> loadFromMemory(Assets *assets,
                                                  const std::string &asset_name,
                                                  const std::string &vertex_shader,
                                                  const std::string &geometry_shader,
                                                  const std::string &fragment_shader)
    {
        if (asset_name.empty())
            return nullptr;

        auto shader = std::make_shared<Shader>();
        if (shader->loadFromMemory(vertex_shader, geometry_shader, fragment_shader))
            assets->add<Shader>(asset_name, shader);

        return shader;
    }
};

} // namespace ae

#endif // AE_SHADER_LOADER_H
