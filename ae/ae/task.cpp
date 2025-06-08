#include "task.h"
#include "task_manager.h"

namespace ae {

DelayTask::DelayTask(const Time &delay)
    : m_remaining{delay}
{}

bool DelayTask::update(const Time &dt)
{
    if (m_remaining <= dt)
        return true;
    m_remaining -= dt;
    return false;
}

CallbackTask::CallbackTask(const std::function<void()> &callback)
    : m_callback{callback}
    , m_done{false}
{}

bool CallbackTask::update(const Time &)
{
    if (!m_done) {
        m_callback();
        m_done = true;
    }
    return true;
}

void TaskChain::addTask(const s_ptr<Task> &task)
{
    m_tasks.push(task);
}

bool TaskChain::update(const Time &dt)
{
    if (!m_tasks.empty()) {
        if (m_tasks.front()->update(dt))
            m_tasks.pop();
    }
    return m_tasks.empty() ? true : false;
}

} // namespace ae
