#ifndef AE_TASK_MANAGER_H
#define AE_TASK_MANAGER_H

#include "system/time.h"
#include "task.h"

#include <vector>

namespace ae {

class TaskManager
{
public:
    TaskManager();
    ~TaskManager() = default;

    void run(const s_ptr<Task> &task);
    void update(const Time &dt);

private:
    std::vector<s_ptr<Task>> m_pending_tasks;
    std::vector<s_ptr<Task>> m_tasks;
};

} // namespace ae

#endif // AE_TASK_MANAGER_H
