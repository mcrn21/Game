#include "task_manager.h"

#include <algorithm>

namespace ae {

TaskManager::TaskManager() {}

void TaskManager::run(const std::shared_ptr<Task> &task)
{
    m_tasks.push_back(task);
}

void TaskManager::update(const Time &dt)
{
    m_tasks.erase(std::remove_if(m_tasks.begin(),
                                 m_tasks.end(),
                                 [&](auto &task) { return task->update(dt); }),
                  m_tasks.end());
}

} // namespace ae
