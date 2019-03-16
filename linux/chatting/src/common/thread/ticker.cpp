#include "ticker.hpp"

#include <thread>

namespace RS
{

Ticker::Ticker(const std::chrono::milliseconds& msec) noexcept
    : m_tickInterval(msec),
    m_running(false)
{
}

void
Ticker::set_func(
    const std::function<void(std::chrono::milliseconds)>& onTick
) noexcept
{
    m_onTick = onTick;
}

bool
Ticker::start() noexcept
{
    if (m_running || !m_onTick)
    {
        return false;
    }

    m_running = true;
    std::thread startTick(&Ticker::timer_loop, this);
    startTick.detach();
    return true;
}

void
Ticker::stop() noexcept
{
    m_running = false;
}

void
Ticker::timer_loop() noexcept
{
    using namespace std::chrono;

    while (m_running)
    {
        milliseconds now = duration_cast<milliseconds>(
            system_clock::now().time_since_epoch()
        );

        milliseconds delta = now - m_lastTick;
        m_lastTick = now;

        std::thread tick(m_onTick, delta);
        tick.detach();

        std::this_thread::sleep_for(m_tickInterval);
    }
}

} // RS
