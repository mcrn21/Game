#include "task_manager.h"

#include <algorithm>

namespace ae {

TaskManager::TaskManager()
{
    m_current_buffer = 0;
}

void TaskManager::run(const SharedPtr<Task> &task)
{
    m_tasks[m_current_buffer].push_back(task);
}

void TaskManager::update(const Time &dt)
{
    int32_t current_buffer = m_current_buffer;

    ++m_current_buffer;
    if (m_current_buffer == 2)
        m_current_buffer = 0;

    m_tasks[current_buffer].erase(std::remove_if(m_tasks[current_buffer].begin(),
                                                 m_tasks[current_buffer].end(),
                                                 [&](auto &task) { return task->update(dt); }),
                                  m_tasks[current_buffer].end());
}

} // namespace ae
