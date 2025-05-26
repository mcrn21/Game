#ifndef AE_TASK_MANAGER_H
#define AE_TASK_MANAGER_H

#include "system/time.h"
#include "task.h"

#include <memory>
#include <vector>

namespace ae {

class TaskManager
{
public:
    TaskManager();
    ~TaskManager() = default;

    void run(const std::shared_ptr<Task> &task);
    void update(const Time &dt);

private:
    std::vector<std::shared_ptr<Task>> m_tasks;
};

} // namespace ae

#endif // AE_TASK_MANAGER_H
