#ifndef __THREAD_POOL_HPP__
#define __THREAD_POOL_HPP__

#include <cstdint>

#include <iostream>
#include <vector>
#include <queue>
#include <stdexcept>
#include <functional>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "thread_task.hpp"
#include "semaphore.hpp"

namespace RS
{

class ThreadPool
{
public:
    ThreadPool(uint32_t poolsize) noexcept;
    virtual ~ThreadPool();

public:
    bool
    run(
        bool start
    ) noexcept;

    template <typename Func, typename... Args>
    void
    enqueue(
        Func&& func,
        Args&&... args
    ) noexcept;

private:
    void
    handle_task() noexcept;

    bool
    start_running() noexcept;

    bool
    stop_running(
        bool wait_task
    ) noexcept;

private:
    std::mutex m_mutex;
    Semaphore m_semaphore;

    std::vector<std::thread> m_workers;
    const uint32_t m_pool_size;

    std::queue<std::unique_ptr<ThreadTask>> m_allocatedJob;
    bool m_block_enqueue;
    bool m_terminate;
};

template <typename Func, typename... Args>
void
ThreadPool::enqueue(
    Func&& func,
    Args&&... args
) noexcept
{
    auto boundTask = std::bind(
        std::forward<Func>(func),
        std::forward<Args>(args)...
    );

    {
        std::lock_guard<decltype(m_mutex)> lock(m_mutex);

        if (m_block_enqueue)
        {
            std::cerr << "cannot enqueue while terminating\n";
            return;
        }

        m_allocatedJob.emplace(std::make_unique<ThreadTask>(std::move(boundTask)));
    }

    m_semaphore.notify();
}

} // RS

#endif // __THREAD_POOL_HPP__
