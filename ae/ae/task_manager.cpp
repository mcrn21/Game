#include "task_manager.h"

namespace ae {

TaskManager::TaskManager()
{
    // Зарезервируем входящий буффер и текущий для задач
    int32_t reserve_tasks_count = 20;
    m_pending_tasks.reserve(reserve_tasks_count);
    m_tasks.reserve(reserve_tasks_count);
}

void TaskManager::run(const s_ptr<Task> &task)
{
    m_pending_tasks.push_back(task);
}

void TaskManager::update(const Time &dt)
{
    // Добавим новые задачи к текущим
    if (!m_pending_tasks.empty()) {
        if (m_tasks.empty())
            std::swap(m_tasks, m_pending_tasks);
        else {
            m_tasks.insert(m_tasks.end(),
                           std::make_move_iterator(m_pending_tasks.begin()),
                           std::make_move_iterator(m_pending_tasks.end()));
            m_pending_tasks.clear();
        }
    }

    if (!m_tasks.empty()) {
        m_tasks.erase(std::remove_if(m_tasks.begin(),
                                     m_tasks.end(),
                                     [&](auto &task) { return task->update(dt); }),
                      m_tasks.end());
    }
}

} // namespace ae
