#include "thread_pool.hpp"

#include <iostream>
#include <chrono>

namespace RS
{

ThreadPool::ThreadPool(uint32_t poolsize) noexcept
    : m_ticker(std::chrono::milliseconds(100)),
    m_pool_size(poolsize),
    m_idleWorkers(poolsize),
    m_terminate(false)
{
}

ThreadPool::~ThreadPool()
{
    stop_running();

    if (m_tasks.size() > 0)
    {
        fprintf(stdout, "tasks(%lu) left while shutting down ThreadPool\n", m_tasks.size());
    }
}

bool
ThreadPool::run(
    bool start
) noexcept
{
    if (start)
    {
        return start_running();
    }
    else
    {
        return stop_running();
    }
}

void
ThreadPool::update() noexcept
{
    std::unique_lock<std::mutex> lock(m_mutex);

    if (m_tasks.empty() || m_terminate)
    {
        return;
    }

    while (!m_tasks.empty() && m_idleWorkers > 0)
    {
        m_allocatedJob.push(std::move(m_tasks.front()));
        m_tasks.pop();

        lock.unlock();
        m_condition.notify_one();
        lock.lock();
    }
}

void
ThreadPool::handle_task() noexcept
{
    while (true)
    {
        std::unique_ptr<ThreadTask> task;
        {
            std::unique_lock<std::mutex> lock(m_mutex);

            m_idleWorkers += 1;
            if (m_terminate)
            {
                return;
            }

            m_condition.wait(lock, [this]
            {
                return m_terminate || !m_allocatedJob.empty();
            });

            if (m_terminate)
            {
                return;
            }

            m_idleWorkers -= 1;
            task = std::move(m_allocatedJob.front());
            m_allocatedJob.pop();
        }

        task->execute();
    }
}

bool
ThreadPool::start_running() noexcept
{
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

    m_ticker.setFunc([this](std::chrono::milliseconds)
    {
        update();
    });

    return m_ticker.start();
}

bool
ThreadPool::stop_running() noexcept
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_terminate = true;
    }
    m_ticker.stop();
    m_condition.notify_all();

    for (std::thread& worker : m_workers)
    {
        worker.join();
    }
    m_workers.clear();

    return true;
}

} // RS
