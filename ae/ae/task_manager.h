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

    void run(const SharedPtr<Task> &task);
    void update(const Time &dt);

private:
    // Используется двойной буффер задач, который
    // переключается на кажлм апдейте
    int32_t m_current_buffer;
    std::vector<SharedPtr<Task>> m_tasks[2];
};

} // namespace ae

#endif // AE_TASK_MANAGER_H
