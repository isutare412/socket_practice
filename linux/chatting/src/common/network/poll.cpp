#include "poll.hpp"
#include "tools.hpp"

namespace RS
{

PollManager::PollManager() noexcept
{
}

PollManager::~PollManager() noexcept
{
}

bool PollManager::register_socket(int socket, int16_t event_flags) noexcept
{
    /*******************************************************************************************
     * Flags for event_flags parameter
     *******************************************************************************************
     * POLLIN       Data other than high priority data may be read without blocking.
     *              This is equivalent to ( POLLRDNORM | POLLRDBAND ).

     * POLLOUT      Normal data may be written without blocking. This is equivalent to POLLWRNORM.

     * POLLPRI      High priority data may be read without blocking.

     * POLLRDBAND   Priority data may be read without blocking.

     * POLLRDNORM   Normal data may be read without blocking.

     * POLLWRBAND   Priority data may be written without blocking.

     * POLLWRNORM   Normal data may be written without blocking.
     *******************************************************************************************
     */

    auto it = std::find_if(
        m_pollfds.begin(),
        m_pollfds.end(),
        [socket](const pollfd& fd_data)
        {
            if (fd_data.fd == socket)
            {
                return true;
            }
            return false;
        }
    );

    if (it != m_pollfds.end())
    {
        return false;
    }

    if (m_pollfds.size() >= UINT32_MAX)
    {
        return false;
    }

    event_flags &= (
        POLLIN
        | POLLOUT
        | POLLPRI
        | POLLRDBAND
        | POLLRDNORM
        | POLLWRBAND
        | POLLWRNORM
    );

    pollfd new_fd;
    new_fd.fd = socket;
    new_fd.events = event_flags;

    m_pollfds.push_back(new_fd);
    return true;
}

bool PollManager::unregister_socket(int socket) noexcept
{
    auto it = std::find_if(
        m_pollfds.begin(),
        m_pollfds.end(),
        [socket](const pollfd& fd_data)
        {
            if (fd_data.fd == socket)
            {
                return true;
            }
            return false;
        }
    );

    if (it == m_pollfds.end())
    {
        return false;
    }

    m_pollfds.erase(it);

    return true;
}

const std::vector<pollfd>* PollManager::poll(int msec) noexcept
{
    uint32_t fds_size = m_pollfds.size();
    if (fds_size == 0)
    {
        return nullptr;
    }

    msec = msec >= 0 ? msec : -1;
    int retval = ::poll(&m_pollfds[0], fds_size, msec);
    if (retval == -1)
    {
        print_perror("poll error");
        return nullptr;
    }
    // if the time limit is expired
    else if (retval == 0)
    {
        return nullptr;
    }

    return &m_pollfds;
}

void PollManager::clear() noexcept
{
    m_pollfds.clear();
}

}
