#include "thread_task.hpp"

namespace RS
{

ThreadTask::ThreadTask() noexcept
{
}

ThreadTask::ThreadTask(std::function<void()>&& task) noexcept
    : m_task(std::move(task))
{
}

ThreadTask::~ThreadTask() noexcept
{
}

ThreadTask::ThreadTask(ThreadTask&& rhs) noexcept
    : m_task(std::move(rhs.m_task))
{
}

ThreadTask& ThreadTask::operator=(ThreadTask&& rhs) noexcept
{
    m_task = std::move(rhs.m_task);
    return *this;
}

void
ThreadTask::execute() const noexcept
{
    m_task();
}

} // RS
