#include "thread_pool.hpp"

#include <iostream>
#include <chrono>

namespace RS
{

ThreadPool::ThreadPool(uint32_t poolsize) noexcept
    : m_ticker(std::chrono::milliseconds(100)),
    m_idleWorkers(poolsize),
    m_terminate(false)
{
    static auto threadJob = [this]
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
    };

    for (uint32_t i = 0; i < poolsize; ++i)
    {
        m_workers.emplace_back(threadJob);
    }

    m_ticker.setFunc([this](std::chrono::milliseconds)
    {
        update();
    });
    m_ticker.start();
}

ThreadPool::~ThreadPool()
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

    if (m_tasks.size() > 0)
    {
        std::cerr << "tasks("
            << m_tasks.size()
            << ") left while shutting down\n";
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

} // RS
