#include <cstdio>

#include <iostream>
#include <chrono>

#include "thread_pool.hpp"

namespace RS
{

ThreadPool::ThreadPool(uint32_t poolsize) noexcept
    : m_semaphore(0),
    m_pool_size(poolsize),
    m_block_enqueue(false),
    m_terminate(false)
{
}

ThreadPool::~ThreadPool()
{
    stop_running(false);

    if (m_allocatedJob.size() > 0)
    {
        printf("tasks(%lu) left while shutting down ThreadPool\n", m_allocatedJob.size());
    }
}

bool ThreadPool::run(bool start) noexcept
{
    if (start)
    {
        return start_running();
    }
    else
    {
        return stop_running(true);
    }
}

void ThreadPool::handle_task() noexcept
{
    while (true)
    {
        std::unique_ptr<ThreadTask> task;
        {
            m_semaphore.wait();
            std::lock_guard<decltype(m_mutex)> lock(m_mutex);

            if (m_terminate)
            {
                std::cout << "thread terminated; id(" << std::this_thread::get_id() << ")\n";
                return;
            }

            if (m_allocatedJob.empty())
            {
                fprintf(stderr, "no allocated job... wrong wakeup\n");
                continue;
            }

            task = std::move(m_allocatedJob.front());
            m_allocatedJob.pop();
        }

        task->execute();
    }
}

bool ThreadPool::start_running() noexcept
{
    m_block_enqueue = false;
    m_terminate = false;

    if (m_workers.size() != 0)
    {
        return false;
    }

    for (uint32_t i = 0; i < m_pool_size; ++i)
    {
        m_workers.emplace_back(
            [this]
            {
                handle_task();
            }
        );
    }

    return true;
}

bool ThreadPool::stop_running(bool wait_task) noexcept
{
    m_block_enqueue = true;

    while (true)
    {
        std::lock_guard<decltype(m_mutex)> lock(m_mutex);

        if (wait_task && m_allocatedJob.size() > 0)
        {
            continue;
        }

        m_terminate = true;
        break;
    }

    for (uint32_t i = 0; i < m_workers.size(); ++i)
    {
        m_semaphore.notify();
    }

    for (std::thread& worker : m_workers)
    {
        worker.join();
    }
    m_workers.clear();

    return true;
}

} // RS
