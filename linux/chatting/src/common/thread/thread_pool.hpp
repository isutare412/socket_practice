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

#include "ticker.hpp"
#include "thread_task.hpp"

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
    update() noexcept;

    void
    handle_task() noexcept;

    bool
    start_running() noexcept;

    bool
    stop_running() noexcept;

private:
    std::mutex m_mutex;
    std::condition_variable m_condition;

    Ticker m_ticker;

    std::vector<std::thread> m_workers;
    const uint32_t m_pool_size;
    uint32_t m_idleWorkers;

    std::queue<std::unique_ptr<ThreadTask>> m_allocatedJob;
    std::queue<std::unique_ptr<ThreadTask>> m_tasks;
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

    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_terminate)
    {
        std::cerr << "cannot enqueue while terminating\n";
        return;
    }

    m_tasks.emplace(std::make_unique<ThreadTask>(std::move(boundTask)));
}

} // RS

#endif // __THREAD_POOL_HPP__
