#ifndef AE_TASK_H
#define AE_TASK_H

#include "system/time.h"

#include <functional>
#include <memory>
#include <queue>

namespace ae {

class Task : public std::enable_shared_from_this<Task>
{
public:
    Task() = default;
    virtual ~Task() = default;

    void run();

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

    void addTask(const std::shared_ptr<Task> &task);
    bool update(const Time &dt);

private:
    std::queue<std::shared_ptr<Task>> m_tasks;
};

} // namespace ae

#endif // AE_TASK_H
