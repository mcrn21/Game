#ifndef AE_ASSETS_H
#define AE_ASSETS_H

#include "asset_loader.h"

#include <memory>
#include <typeindex>
#include <unordered_map>

namespace ae {

class Assets
{
    using assets_map = std::unordered_map<std::string, std::shared_ptr<void>>;

public:
    Assets() = default;
    ~Assets() = default;

    template<typename T>
    std::shared_ptr<T> get(const std::string &asset_name);

    template<typename T>
    void add(const std::string &asset_name, const std::shared_ptr<T> &asset);

    template<typename T>
    void remove(const std::string &asset_name);

    template<typename T>
    bool has(const std::string &asset_name);

    template<typename T, typename... Args>
    std::shared_ptr<T> loadFromFile(const std::string &asset_name, Args &&...args);

    template<typename T, typename... Args>
    std::shared_ptr<T> loadFromMemory(const std::string &asset_name, Args &&...args);

private:
    std::unordered_map<std::type_index, assets_map> m_assets;
};

template<typename T>
inline std::shared_ptr<T> Assets::get(const std::string &asset_name)
{
    auto assets_found = m_assets.find(typeid(T));
    if (assets_found == m_assets.end())
        return nullptr;

    auto asset_found = assets_found->second.find(asset_name);
    if (asset_found == assets_found->second.end())
        return nullptr;

    return std::static_pointer_cast<T>(asset_found->second);
}

template<typename T>
inline void Assets::add(const std::string &asset_name, const std::shared_ptr<T> &asset)
{
    auto assets_found = m_assets.find(typeid(T));
    if (assets_found == m_assets.end())
        m_assets.emplace(typeid(T), assets_map{});
    m_assets[typeid(T)].emplace(asset_name, asset);
}

template<typename T>
inline void Assets::remove(const std::string &asset_name)
{
    auto assets_found = m_assets.find(typeid(T));
    if (assets_found == m_assets.end())
        return;

    auto asset_found = assets_found->second.find(asset_name);
    if (asset_found != assets_found->second.end())
        assets_found->second.erase(asset_found);
}

template<typename T>
inline bool Assets::has(const std::string &asset_name)
{
    auto assets_found = m_assets.find(typeid(T));
    if (assets_found == m_assets.end())
        return false;

    auto asset_found = assets_found->second.find(asset_name);
    if (asset_found == assets_found->second.end())
        return false;

    return true;
}

template<typename T, typename... Args>
inline std::shared_ptr<T> Assets::loadFromFile(const std::string &asset_name, Args &&...args)
{
    return AssetLoader<T>::loadFromFile(this, asset_name, std::forward<Args>(args)...);
}

template<typename T, typename... Args>
inline std::shared_ptr<T> Assets::loadFromMemory(const std::string &asset_name, Args &&...args)
{
    return AssetLoader<T>::loadFromMemory(this, asset_name, std::forward<Args>(args)...);
}

} // namespace ae

#endif // AE_ASSETS_H
