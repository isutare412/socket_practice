#ifndef __TOOLS_HPP__
#define __TOOLS_HPP__

#include <arpa/inet.h>
#include <sys/socket.h>

#include <string>
#include <sstream>

namespace RS
{

enum class ErrorType;

std::string
ip_to_string(
    uint32_t ip
) noexcept;

std::string
sockaddr_to_string(
    const sockaddr_in& addr
) noexcept;

ErrorType
read_nbytes(
    int socket,
    void* buf,
    int32_t bytes
) noexcept;

void
print_perror(
    const char* msg, ...
) noexcept;

void
handle_perror(
    const char* msg, ...
) noexcept;

enum class ErrorType
{
    NO_ERROR,
    SYS_ERROR,
    EOF_DETECTED
};

}

#endif // __TOOLS_HPP__
