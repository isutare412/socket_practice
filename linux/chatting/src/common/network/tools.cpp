#include <cstdlib>

#include "tools.hpp"

namespace RS
{

static const uint32_t ADDRESS_BUF_SIZE = 32;

std::string
ip_to_string(
    uint32_t ip
)
{
    const uint8_t* byte_ptr = reinterpret_cast<uint8_t*>(&ip);

    char addr_buf[ADDRESS_BUF_SIZE];
    snprintf(addr_buf, ADDRESS_BUF_SIZE, "%u.%u.%u.%u",
        byte_ptr[0], byte_ptr[1], byte_ptr[2], byte_ptr[3]);

    return std::string(addr_buf);
}

std::string
sockaddr_to_string(
    const sockaddr_in& addr
)
{
    std::ostringstream oss;
    oss << ip_to_string(addr.sin_addr.s_addr);
    oss << ':';
    oss << addr.sin_port;

    return oss.str();
}

void
handle_perror(
    const char* msg, ...
)
{
    char msg_buf[256];

    va_list argptr;
    va_start(argptr, msg);
    vsnprintf(msg_buf, sizeof(msg_buf), msg, argptr);
    va_end(argptr);

    perror(msg_buf);
    exit(EXIT_FAILURE);
}

}