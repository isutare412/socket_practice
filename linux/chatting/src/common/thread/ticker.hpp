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
    setFunc(
        const std::function<void(std::chrono::milliseconds)>& onTick
    ) noexcept;

    bool
    start() noexcept;
    void
    stop() noexcept;

private:
    void
    timerLoop() noexcept;

private:
    const std::chrono::milliseconds m_tickInterval;
    std::function<void(std::chrono::milliseconds)> m_onTick;
    std::chrono::milliseconds m_lastTick;
    bool m_running;
};

} // RS

#endif // __TICKER_HPP__