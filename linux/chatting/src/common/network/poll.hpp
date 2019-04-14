#ifndef __POLL_MANAGER_HPP__
#define __POLL_MANAGER_HPP__

#include <poll.h>

#include <vector>

namespace RS
{

class PollManager
{
public:
    PollManager() noexcept;
    virtual ~PollManager() noexcept;

public:
    bool register_socket(int socket, int16_t event_flags) noexcept;
    bool unregister_socket(int socket) noexcept;
    const std::vector<pollfd>* poll(int msec) noexcept;
    void clear() noexcept;

private:
    std::vector<pollfd> m_pollfds;
};

}

#endif //__POLL_MANAGER_HPP__
