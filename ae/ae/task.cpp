#include "task.h"
#include "app.h"

namespace ae {

void Task::run()
{
    App::getInstance().getTaskManager()->run(shared_from_this());
}

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

void TaskChain::addTask(const std::shared_ptr<Task> &task)
{
    m_tasks.push(task);
}

bool TaskChain::update(const Time &dt)
{
    while (!m_tasks.empty()) {
        if (m_tasks.front()->update(dt)) {
            m_tasks.pop();
        } else
            return false;
    }
    return true;
}

} // namespace ae
