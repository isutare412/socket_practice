#include "semaphore.hpp"

namespace RS
{

Semaphore::Semaphore(int64_t initial_count) noexcept
    : m_count(initial_count)
{
}

void Semaphore::notify() noexcept
{
    std::lock_guard<decltype(m_mutex)> lock(m_mutex);

    ++m_count;
    m_condition.notify_one();
}

void Semaphore::wait() noexcept
{
    std::unique_lock<decltype(m_mutex)> lock(m_mutex);

    while (m_count <= 0)
    {
        m_condition.wait(lock);
    }
    --m_count;
}

bool Semaphore::try_wait() noexcept
{
    std::unique_lock<decltype(m_mutex)> lock(m_mutex);

    if (m_count > 0)
    {
        --m_count;
        return true;
    }
    return false;
}

} // RS
