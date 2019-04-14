#include <cstdlib>
#include <unistd.h>

#include "tools.hpp"

namespace RS
{

static const uint32_t ADDRESS_BUF_SIZE = 32;

std::string ip_to_string(uint32_t ip) noexcept
{
    const uint8_t* byte_ptr = reinterpret_cast<uint8_t*>(&ip);

    char addr_buf[ADDRESS_BUF_SIZE];
    snprintf(addr_buf, ADDRESS_BUF_SIZE, "%u.%u.%u.%u",
        byte_ptr[0], byte_ptr[1], byte_ptr[2], byte_ptr[3]);

    return std::string(addr_buf);
}

std::string sockaddr_to_string(const sockaddr_in& addr) noexcept
{
    std::ostringstream oss;
    oss << ip_to_string(addr.sin_addr.s_addr);
    oss << ':';
    oss << ntohs(addr.sin_port);

    return oss.str();
}

ErrorType read_nbytes(int socket, void* buf, int32_t bytes) noexcept
{
    int32_t* const target = reinterpret_cast<int32_t*>(buf);
    int32_t remain_buf_size = bytes;
    int32_t cur_buf_cursor = 0;

    int32_t readcnt;
    while (remain_buf_size > 0
        && (readcnt = read(socket, &target[cur_buf_cursor], remain_buf_size)) > 0)
    {
        remain_buf_size -= readcnt;
        cur_buf_cursor += readcnt;
    }

    if (readcnt == -1)
    {
        print_perror("failed to read_nbytes");
        return ErrorType::SYS_ERROR;
    }

    if (readcnt == 0)
    {
        return ErrorType::EOF_DETECTED;
    }

    return ErrorType::NO_ERROR;
}

void print_perror(const char* msg, ...) noexcept
{
    char msg_buf[512];

    va_list argptr;
    va_start(argptr, msg);
    vsnprintf(msg_buf, sizeof(msg_buf), msg, argptr);
    va_end(argptr);

    perror(msg_buf);
}

void handle_perror(const char* msg, ...) noexcept
{
    char msg_buf[512];

    va_list argptr;
    va_start(argptr, msg);
    vsnprintf(msg_buf, sizeof(msg_buf), msg, argptr);
    va_end(argptr);

    perror(msg_buf);
    exit(EXIT_FAILURE);
}

}