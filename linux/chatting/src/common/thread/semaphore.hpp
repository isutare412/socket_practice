#ifndef __SEMAPHORE_HPP__
#define __SEMAPHORE_HPP__

#include <mutex>
#include <condition_variable>

namespace RS
{

class Semaphore
{
public:
    Semaphore(int64_t initial_count) noexcept;

public:
    void notify() noexcept;
    void wait() noexcept;
    bool try_wait() noexcept;

private:
    std::mutex m_mutex;
    std::condition_variable m_condition;
    int64_t m_count;
};

} // RS

#endif // __SEMAPHORE_HPP__
