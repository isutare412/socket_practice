#ifndef __TICKER_HPP__
#define __TICKER_HPP__

#include <cstdint>

#include <chrono>
#include <functional>

namespace RS
{

class Ticker
{
public:
    Ticker(const std::chrono::milliseconds& msec) noexcept;

public:
    void
    set_func(
        const std::function<void(std::chrono::milliseconds)>& onTick
    ) noexcept;

    void
    set_func(
        std::function<void(std::chrono::milliseconds)>&& onTick
    ) noexcept;

    bool
    start() noexcept;
    void
    stop() noexcept;

private:
    void
    timer_loop() noexcept;

private:
    const std::chrono::milliseconds m_tickInterval;
    std::function<void(std::chrono::milliseconds)> m_onTick;
    std::chrono::milliseconds m_lastTick;
    bool m_running;
};

} // RS

#endif // __TICKER_HPP__