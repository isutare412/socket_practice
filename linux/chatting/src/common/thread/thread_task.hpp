#ifndef __THREAD_TASK_HPP__
#define __THREAD_TASK_HPP__

#include <functional>

namespace RS
{

class ThreadTask
{
public:
    ThreadTask() noexcept;
    ThreadTask(std::function<void()>&& task) noexcept;
    virtual ~ThreadTask() noexcept;

    ThreadTask(const ThreadTask& rhs) = delete;
    ThreadTask& operator=(const ThreadTask& rhs) = delete;

    ThreadTask(ThreadTask&& rhs) noexcept;
    ThreadTask& operator=(ThreadTask&& rhs) noexcept;

public:
    void execute() const noexcept;

private:
    std::function<void()> m_task;
};

} // RS

#endif // __THREAD_TASK_HPP__