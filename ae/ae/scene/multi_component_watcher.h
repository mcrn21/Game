#ifndef AE_MULTI_COMPONENT_WATCHER_H
#define AE_MULTI_COMPONENT_WATCHER_H

#include "../system/memory.h"

#include <entt/entt.hpp>

namespace ae {

class MultiComponentWatcherBase
{
public:
    MultiComponentWatcherBase()
        : m_registry{nullptr}
    {}
    virtual ~MultiComponentWatcherBase() = default;

    entt::registry *getRegistry() const { return m_registry; }

    void bind(entt::registry &registry)
    {
        if (!m_registry)
            m_registry = &registry;
    }

    virtual void freeze() = 0;
    virtual void process() = 0;

private:
    entt::registry *m_registry;
};

template<typename... Components>
class MultiComponentWatcher : public MultiComponentWatcherBase
{
public:
    using Callback = std::function<void(entt::registry &, entt::entity)>;

private:
    using reactive_storage = entt::reactive_mixin<entt::storage<void>>;

    struct Storage
    {
        std::unordered_map<entt::id_type, u_ptr<reactive_storage>> storages;
        std::unordered_set<entt::entity> entities;
        Callback callback;
    };

public:
    MultiComponentWatcher() = default;
    ~MultiComponentWatcher() = default;

    MultiComponentWatcher &onCreated(Callback callback)
    {
        if (!getRegistry())
            throw std::runtime_error("MultiComponentWatcher must be bound before use.");

        if (m_created_storage) {
            m_created_storage->callback = callback;
            return *this;
        }

        m_created_storage = createUnique<Storage>();
        m_created_storage->callback = callback;

        (m_created_storage->storages.emplace(entt::type_hash<Components>::value(),
                                             [&] {
                                                 auto rs = createUnique<reactive_storage>();
                                                 rs->bind(*getRegistry());
                                                 rs->template on_construct<Components>();
                                                 return rs;
                                             }()),
         ...);

        return *this;
    }

    MultiComponentWatcher &onUpdated(Callback callback)
    {
        if (!getRegistry())
            throw std::runtime_error("MultiComponentWatcher must be bound before use.");

        if (m_updated_storage) {
            m_updated_storage->callback = std::move(callback);
            return *this;
        }

        m_updated_storage = createUnique<Storage>();
        m_updated_storage->callback = std::move(callback);

        (m_updated_storage->storages.emplace(entt::type_hash<Components>::value(),
                                             [&] {
                                                 auto rs = createUnique<reactive_storage>();
                                                 rs->bind(*getRegistry());
                                                 rs->template on_update<Components>();
                                                 return rs;
                                             }()),
         ...);

        return *this;
    }

    MultiComponentWatcher &onDestroyed(Callback callback)
    {
        if (!getRegistry())
            throw std::runtime_error("MultiComponentWatcher must be bound before use.");

        if (m_destroyed_storage) {
            m_destroyed_storage->callback = std::move(callback);
            return *this;
        }

        m_destroyed_storage = createUnique<Storage>();
        m_destroyed_storage->callback = std::move(callback);

        (m_destroyed_storage->storages.emplace(entt::type_hash<Components>::value(),
                                               [&] {
                                                   auto rs = createUnique<reactive_storage>();
                                                   rs->bind(*getRegistry());
                                                   rs->template on_destroy<Components>();
                                                   return rs;
                                               }()),
         ...);

        return *this;
    }

    void freeze()
    {
        if (!getRegistry())
            return;

        auto freeze_storage = [&](u_ptr<Storage> &storage) {
            if (!storage)
                return;
            for (auto &[id, rs] : storage->storages) {
                for (auto [entity] : rs->each()) {
                    if ((getRegistry()->template all_of<Components>(entity) && ...))
                        storage->entities.insert(entity);
                }
                rs->clear();
            }
        };

        freeze_storage(m_created_storage);
        freeze_storage(m_updated_storage);
        freeze_storage(m_destroyed_storage);
    }
    void process()
    {
        if (!getRegistry())
            return;

        auto process_storage = [&](u_ptr<Storage> &storage) {
            if (!storage || !storage->callback)
                return;
            for (auto entity : storage->entities)
                storage->callback(*getRegistry(), entity);
            storage->entities.clear();
        };

        process_storage(m_created_storage);
        process_storage(m_updated_storage);
        process_storage(m_destroyed_storage);
    }

private:
    u_ptr<Storage> m_created_storage;
    u_ptr<Storage> m_updated_storage;
    u_ptr<Storage> m_destroyed_storage;
};

class ComponentWatcher
{
public:
    ComponentWatcher()
        : m_registry{nullptr}
    {}
    ~ComponentWatcher() = default;

    void bind(entt::registry &registry)
    {
        if (!m_registry)
            m_registry = &registry;
    }

    template<typename... Components>
    MultiComponentWatcher<Components...> &watch()
    {
        if (!m_registry)
            throw std::runtime_error("ComponentWatcher must be bound before use.");

        using Watcher = MultiComponentWatcher<Components...>;

        auto hash = getHash<Components...>();
        if (m_watchers.contains(hash))
            return *static_cast<Watcher *>(m_watchers[hash].get());

        m_watchers.insert(std::pair{hash, std::move(createUnique<Watcher>())});
        auto watcher = static_cast<Watcher *>(m_watchers[hash].get());
        watcher->bind(*m_registry);
        return *watcher;
    }

    void freeze()
    {
        for (auto &[id, watcher] : m_watchers)
            watcher->freeze();
    }

    void process()
    {
        for (auto &[id, watcher] : m_watchers)
            watcher->process();
    }

private:
    template<typename... Components>
    uint64_t getHash()
    {
        uint64_t seed = 0;
        (void(seed ^= entt::type_hash<Components>::value() + 0x9e3779b9 + (seed << 6) + (seed >> 2)),
         ...);
        return seed;
    }

private:
    entt::registry *m_registry;
    std::unordered_map<uint64_t, u_ptr<MultiComponentWatcherBase>> m_watchers;
};

} // namespace ae

#endif // AE_MULTI_COMPONENT_WATCHER_H
