#ifndef __TOOLS_HPP__
#define __TOOLS_HPP__

#include <arpa/inet.h>
#include <sys/socket.h>

#include <string>
#include <sstream>

namespace RS
{

std::string
ip_to_string(
    uint32_t ip
);

std::string
sockaddr_to_string(
    const sockaddr_in& addr
);

void
handle_perror(
    const char* msg, ...
);

}

#endif // __TOOLS_HPP__
