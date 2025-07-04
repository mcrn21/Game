#ifndef AE_TASK_H
#define AE_TASK_H

#include "engine_context_object.h"
#include "system/memory.h"
#include "system/signal.h"
#include "system/time.h"

#include <functional>
#include <queue>

namespace ae {

class Task : public EnableSharedFromThis<Task>
{
public:
    Task() = default;
    virtual ~Task() = default;

    virtual bool update(const Time &dt) = 0; // true = завершена
};

class DelayTask : public Task
{
public:
    DelayTask(const Time &delay);
    ~DelayTask() = default;

    bool update(const Time &dt);

private:
    Time m_remaining;
};

class CallbackTask : public Task
{
public:
    CallbackTask(const std::function<void()> &callback);

    bool update(const Time &);

private:
    std::function<void()> m_callback;
    bool m_done;
};

class TaskChain : public Task
{
public:
    TaskChain() = default;
    ~TaskChain() = default;

    void addTask(const s_ptr<Task> &task);
    bool update(const Time &dt);

private:
    std::queue<s_ptr<Task>> m_tasks;
};

class NotifyTaskChain : public Task
{
public:
    NotifyTaskChain();
    ~NotifyTaskChain() = default;

    void addTask(const s_ptr<Task> &task);
    bool update(const Time &dt);

public:
    Signal<int32_t, int32_t> taskFinished;

private:
    std::queue<s_ptr<Task>> m_tasks;
    int32_t m_current_task;
    int32_t m_total_task;
};

} // namespace ae

#endif // AE_TASK_H
