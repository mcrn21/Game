#ifndef AE_MEMORY_H
#define AE_MEMORY_H

#include <spdlog/spdlog.h>

#include <atomic>

namespace ae {

template<typename T>
class EnableSharedFromThis;

namespace detail {

template<typename T>
struct DefaultDeleter
{
    void operator()(T *ptr) const noexcept { delete ptr; }
};

template<typename T>
struct InplaceDeleter
{
    void operator()(T *ptr) const noexcept { ptr->~T(); }
};

struct ControlBlockBase
{
    ControlBlockBase() noexcept = default;
    virtual ~ControlBlockBase() noexcept = default;

    virtual void destroyValue() noexcept = 0;
    virtual void *getValue() const noexcept = 0;

    void addShared() noexcept { shared_count.fetch_add(1, std::memory_order_acq_rel); }
    void releaseShared() noexcept
    {
        if (shared_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            destroyValue();
            releaseWeak();
        }
    }
    void addWeak() noexcept { weak_count.fetch_add(1, std::memory_order_acq_rel); }
    bool releaseWeak() noexcept
    {
        // Если блок был улален возвращаем true
        if (weak_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            delete this;
            return true;
        }

        return false;
    }

    std::atomic<size_t> shared_count{0};
    std::atomic<size_t> weak_count{1};
};

template<typename T, typename Deleter = DefaultDeleter<T>>
struct ControlBlockRawPtr : public ControlBlockBase
{
    ControlBlockRawPtr(T *n_value) noexcept
        : value(n_value)
    {}
    ~ControlBlockRawPtr() = default;

    void destroyValue() noexcept
    {
        if (value)
            deleter(value);
    }

    void *getValue() const noexcept { return value; }

    T *value;
    Deleter deleter;
};

template<typename T, typename Deleter = InplaceDeleter<T>>
struct ControlBlockInplace : public ControlBlockBase
{
    template<typename... Args>
    ControlBlockInplace(Args &&...args) noexcept
        : constructed{false}
    {
        new (storage) T(std::forward<Args>(args)...);
        constructed = true;
    }

    void destroyValue() noexcept
    {
        if (constructed) {
            T *value = reinterpret_cast<T *>(storage);
            deleter(value);
            constructed = false;
        }
    }

    void *getValue() const noexcept { return const_cast<std::byte *>(storage); }

    alignas(T) std::byte storage[sizeof(T)];
    Deleter deleter;
    bool constructed;
};

template<typename T>
struct isAnyEnableSharedFromThisBase
{
private:
    template<typename U>
    static std::true_type test(const volatile EnableSharedFromThis<U> *);
    static std::false_type test(...);

public:
    static constexpr bool value = decltype(test(std::declval<T *>()))::value;
};

template<typename T>
inline constexpr bool isAnyEnableSharedFromThisBaseV = isAnyEnableSharedFromThisBase<T>::value;

} // namespace detail

struct EnableSharedFromThisBase
{
    virtual void internalAcceptOwnerBase(detail::ControlBlockBase *, void *) noexcept = 0;
};

template<typename T>
class SharedPtr
{
    friend class EnableSharedFromThis<T>;

    template<typename U>
    friend class SharedPtr;

    template<typename U>
    friend class WeakPtr;

    template<typename T2, typename U2>
    friend SharedPtr<T2> staticPointerCast(const SharedPtr<U2> &);

    template<typename T2, typename U2>
    friend SharedPtr<T2> dynamicPointerCast(const SharedPtr<U2> &);

    template<typename T2, typename U2>
    friend SharedPtr<T2> reinterpretPointerCast(const SharedPtr<U2> &);

public:
    SharedPtr()
        : m_control_block{nullptr}
        , m_raw{nullptr}
    {}

    SharedPtr(std::nullptr_t) noexcept
        : m_control_block{nullptr}
        , m_raw{nullptr}
    {}

    template<typename U,
             typename Deleter = detail::DefaultDeleter<U>,
             typename = std::enable_if_t<std::is_convertible_v<U *, T *>>>
    SharedPtr(U *value) noexcept
        : m_control_block{nullptr}
        , m_raw{nullptr}
    {
        if (value) {
            m_control_block = new detail::ControlBlockRawPtr<U, std::decay_t<Deleter>>(value);
            m_control_block->addShared();
            m_raw = value;

            // Найти базовый EnableSharedFromThis<B> и вызвать accept
            using RawT = std::remove_cv_t<std::remove_reference_t<U>>;
            if constexpr (detail::isAnyEnableSharedFromThisBaseV<RawT>) {
                auto *base = static_cast<EnableSharedFromThisBase *>(m_raw);
                if (base)
                    base->internalAcceptOwnerBase(m_control_block, m_raw);
            }
        }
    }

    SharedPtr(const SharedPtr &other) noexcept
        : m_control_block{nullptr}
        , m_raw{nullptr}
    {
        if (other.m_control_block) {
            m_control_block = other.m_control_block;
            m_control_block->addShared();
            m_raw = other.m_raw;
        }
    }

    template<typename U, typename = std::enable_if_t<std::is_convertible_v<U *, T *>>>
    SharedPtr(const SharedPtr<U> &other) noexcept
        : m_control_block{nullptr}
        , m_raw{nullptr}
    {
        if (other.m_control_block) {
            m_control_block = other.m_control_block;
            m_control_block->addShared();
            m_raw = other.m_raw;
        }
    }

    SharedPtr(SharedPtr &&other) noexcept
        : m_control_block(other.m_control_block)
        , m_raw{other.m_raw}
    {
        other.m_control_block = nullptr;
        other.m_raw = nullptr;
    }

    ~SharedPtr() { release(); }

    SharedPtr &operator=(std::nullptr_t) noexcept
    {
        release();
        return *this;
    }

    SharedPtr &operator=(const SharedPtr &other) noexcept
    {
        release();
        if (other.m_control_block) {
            m_control_block = other.m_control_block;
            m_control_block->addShared();
            m_raw = other.m_raw;
        }
        return *this;
    }

    template<typename U, typename = std::enable_if_t<std::is_convertible_v<U *, T *>>>
    SharedPtr &operator=(const SharedPtr<U> &other) noexcept
    {
        release();
        if (other.m_control_block) {
            m_control_block = other.m_control_block;
            m_control_block->addShared();
            m_raw = other.m_raw;
        }
        return *this;
    }

    SharedPtr &operator=(SharedPtr &&other) noexcept
    {
        if (this != &other) {
            release();
            m_control_block = other.m_control_block;
            m_raw = other.m_raw;

            other.m_control_block = nullptr;
            other.m_raw = nullptr;
        }
        return *this;
    }

    void reset() noexcept { release(); }

    void swap(SharedPtr &other) noexcept
    {
        std::swap(m_control_block, other.m_control_block);
        std::swap(m_raw, other.m_raw);
    }

    T *get() const noexcept { return m_raw; }

    template<typename U = T>
    std::enable_if_t<!std::is_void_v<U>, U &> operator*() const noexcept
    {
        return *m_raw;
    }

    T *operator->() const noexcept { return m_raw; }

    operator bool() const noexcept { return m_control_block != nullptr; }

    int32_t useCount() const noexcept
    {
        if (!m_control_block)
            return 0;
        return m_control_block->shared_count.load(std::memory_order_acquire);
    }

    template<typename U>
    bool operator==(const SharedPtr<U> &other) const noexcept
    {
        return m_raw == other.m_raw;
    }
    template<typename U>
    bool operator!=(const SharedPtr<U> &other) const noexcept
    {
        return m_raw != other.m_raw;
    }

    bool operator==(std::nullptr_t) const noexcept { return m_raw == nullptr; }
    bool operator!=(std::nullptr_t) const noexcept { return m_raw != nullptr; }

    friend bool operator==(std::nullptr_t, const SharedPtr &ptr) noexcept { return ptr == nullptr; }
    friend bool operator!=(std::nullptr_t, const SharedPtr &ptr) noexcept { return ptr != nullptr; }

    template<typename Deleter = detail::InplaceDeleter<T>, typename... Args>
    static SharedPtr create(Args &&...args)
    {
        using Block = detail::ControlBlockInplace<T, std::decay_t<Deleter>>;
        auto *block = new Block{std::forward<Args>(args)...};
        SharedPtr<T> ptr{block, reinterpret_cast<T *>(block->getValue()), true};

        // Найти базовый EnableSharedFromThis<B> и вызвать accept
        using RawT = std::remove_cv_t<std::remove_reference_t<T>>;
        if constexpr (detail::isAnyEnableSharedFromThisBaseV<RawT>) {
            auto *base = static_cast<EnableSharedFromThisBase *>(ptr.m_raw);
            if (base)
                base->internalAcceptOwnerBase(ptr.m_control_block, ptr.m_raw);
        }

        return ptr;
    }

private:
    SharedPtr(detail::ControlBlockBase *control_block, T *raw, bool add_ref = true)
        : m_control_block{nullptr}
        , m_raw{nullptr}
    {
        if (control_block && raw) {
            m_control_block = control_block;
            if (add_ref)
                m_control_block->addShared();
            m_raw = raw;
        }
    }

    void release() noexcept
    {
        if (m_control_block) {
            m_control_block->releaseShared();
            m_control_block = nullptr;
            m_raw = nullptr;
        }
    }

private:
    detail::ControlBlockBase *m_control_block;
    T *m_raw;
};

template<typename T>
class WeakPtr
{
    template<typename U>
    friend class SharedPtr;

public:
    WeakPtr() noexcept
        : m_control_block{nullptr}
    {}

    WeakPtr(std::nullptr_t) noexcept
        : m_control_block{nullptr}
    {}

    WeakPtr(const WeakPtr &other) noexcept
        : m_control_block{other.m_control_block}
    {
        if (m_control_block)
            m_control_block->addWeak();
    }

    template<typename U, typename = std::enable_if_t<std::is_convertible_v<U *, T *>>>
    WeakPtr(const SharedPtr<U> &other) noexcept
        : m_control_block{nullptr}
    {
        m_control_block = other.m_control_block;
        if (other.m_control_block)
            m_control_block->addWeak();
    }

    ~WeakPtr() noexcept { release(); }

    WeakPtr &operator=(std::nullptr_t) noexcept
    {
        release();
        return *this;
    }

    WeakPtr &operator=(const WeakPtr &ptr) noexcept
    {
        release();
        m_control_block = ptr.m_control_block;
        if (ptr.m_control_block)
            m_control_block->addWeak();
        return *this;
    }

    template<typename U, typename = std::enable_if_t<std::is_convertible_v<U *, T *>>>
    WeakPtr &operator=(const SharedPtr<U> &ptr) noexcept
    {
        release();
        m_control_block = ptr.m_control_block;
        if (ptr.m_control_block)
            m_control_block->addWeak();
        return *this;
    }

    operator bool() const noexcept { return m_control_block != nullptr; }

    SharedPtr<T> lock() const noexcept
    {
        if (!m_control_block)
            return SharedPtr<T>{};

        size_t count = m_control_block->shared_count.load(std::memory_order_acquire);
        while (count != 0) {
            if (m_control_block->shared_count.compare_exchange_weak(count,
                                                                    count + 1,
                                                                    std::memory_order_acq_rel,
                                                                    std::memory_order_relaxed)) {
                return SharedPtr<T>{m_control_block,
                                    reinterpret_cast<T *>(m_control_block->getValue()),
                                    false};
            }
        }

        return SharedPtr<T>{};
    }

    void reset() noexcept { release(); }

private:
    void release() noexcept
    {
        if (m_control_block && m_control_block->releaseWeak())
            m_control_block = nullptr;
    }

private:
    detail::ControlBlockBase *m_control_block;
};

template<typename T>
class EnableSharedFromThis : public EnableSharedFromThisBase
{
    friend class SharedPtr<T>;

public:
    SharedPtr<T> sharedFromThis() { return m_weak.lock(); }
    SharedPtr<const T> sharedFromThis() const { return m_weak.lock(); }

private:
    void internalAcceptOwnerBase(detail::ControlBlockBase *control_block, void *raw) noexcept
    {
        if (!m_weak.lock())
            m_weak = SharedPtr<T>{control_block, static_cast<T *>(raw), true};
    }

private:
    WeakPtr<T> m_weak;
};

template<typename T, typename U>
inline SharedPtr<T> staticPointerCast(const SharedPtr<U> &ptr)
{
    return SharedPtr<T>{ptr.m_control_block, static_cast<T *>(ptr.get()), true};
}

template<typename T, typename U>
inline SharedPtr<T> dynamicPointerCast(const SharedPtr<U> &ptr)
{
    return SharedPtr<T>{ptr.m_control_block, dynamic_cast<T *>(ptr.get()), true};
}

template<typename T, typename U>
inline SharedPtr<T> reinterpretPointerCast(const SharedPtr<U> &ptr)
{
    return SharedPtr<T>{ptr.m_control_block, reinterpret_cast<T *>(ptr.get()), true};
}

} // namespace ae

#endif // AE_MEMORY_H
