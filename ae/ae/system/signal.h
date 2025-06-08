#ifndef AE_SIGNAL_H
#define AE_SIGNAL_H

#include "memory.h"

#include <functional>
#include <mutex>

namespace ae {

template<typename... Args>
class Signal
{
public:
    using SlotFunc = std::function<void(Args...)>;

    struct Connection
    {
        SlotFunc func;
        void *object = nullptr;
        bool active = true;
    };

    class ConnectionHandle
    {
    public:
        ConnectionHandle()
            : m_owner{nullptr}
        {}
        explicit ConnectionHandle(const w_ptr<Connection> &conn, Signal *owner)
            : m_conn{std::move(conn)}
            , m_owner{owner}
        {}

        void disconnect()
        {
            if (auto conn = m_conn.lock())
                m_owner->disconnect(conn);
        }

        bool isValid() const { return !m_conn.expired(); }

    private:
        w_ptr<Connection> m_conn;
        Signal *m_owner;
    };

    Signal() = default;
    ~Signal() = default;

    // Лямбда или свободная функция
    ConnectionHandle connect(SlotFunc slot)
    {
        auto conn = createShared<Connection>(Connection{std::move(slot)});
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_slots.push_back(conn);
        }
        return ConnectionHandle{conn, this};
    }

    template<typename TMethod, typename TObject>
        requires std::is_member_function_pointer_v<TMethod>
    ConnectionHandle connect(TMethod method, TObject *obj)
    {
        SlotFunc wrapper = [obj, method](Args... args) {
            (obj->*method)(std::forward<Args>(args)...);
        };
        auto conn = createShared<Connection>(
            Connection{std::move(wrapper), static_cast<void *>(obj)});
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_slots.push_back(conn);
        }
        return ConnectionHandle{conn, this};
    }

    // Лямбда с объектом
    template<typename T>
        requires(!std::is_member_function_pointer_v<T>)
    ConnectionHandle connect(SlotFunc func, T *obj)
    {
        auto conn = createShared<Connection>(Connection{std::move(func), static_cast<void *>(obj)});
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_slots.push_back(conn);
        }
        return ConnectionHandle{conn, this};
    }

    void emit(Args... args)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto &conn : m_slots) {
            if (conn->active && conn->func) {
                conn->func(args...);
            }
        }

        // Очистка "мёртвых" после вызова
        m_slots.erase(std::remove_if(m_slots.begin(),
                                     m_slots.end(),
                                     [](const s_ptr<Connection> &conn) { return !conn->active; }),
                      m_slots.end());
    }

    // Удалить по объекту
    void disconnect(void *object)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto &conn : m_slots) {
            if (conn->object == object)
                conn->active = false;
        }
    }

    void disconnect(const s_ptr<Connection> &conn)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        conn->active = false;
    }

    void disconnect()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_slots.clear();
    }

private:
    std::vector<s_ptr<Connection>> m_slots;
    std::mutex m_mutex;
};

} // namespace ae

#endif // AE_SIGNAL_H
