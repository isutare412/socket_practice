#ifndef __PACKET_HANDLER_HPP__
#define __PACKET_HANDLER_HPP__

#include <cstdio>
#include <cstring>

#include "serialize.hpp"
#include "common/session.hpp"

namespace RS
{

template <class ST, class PB>
class PacketFunctorBase
{
public:
    virtual ~PacketFunctorBase() noexcept {};

public:
    virtual bool handle(const char* buffer, uint32_t size, ST* session) = 0;
};


template <class ST, class PB, class PT>
class PacketFunctor : public PacketFunctorBase<ST, PB>
{
public:
    using Handler = bool (*)(ST*, const PT&);

public:
    PacketFunctor(Handler func_ptr) noexcept;

    bool handle(const char* buffer, uint32_t size, ST* session) noexcept override;

private:
    Handler m_handler;
};


template <class ST, class PB, int32_t PACKET_MAX>
class PacketHandler
{
public:
    PacketHandler() noexcept;
    virtual ~PacketHandler() noexcept;

public:
    template <class PT>
    void register_handler(bool (*func)(ST*, const PT&)) noexcept;

    bool handle(int32_t packet_type, const char* buffer, uint32_t bufsize, ST* session) noexcept;

private:
    PacketFunctorBase<ST, PB>* m_packetFunctors[PACKET_MAX];
};


////////////////////////////////////////////////////////////////////////
// PacketFunctor definitions
////////////////////////////////////////////////////////////////////////

template <class ST, class PB, class PT>
PacketFunctor<ST, PB, PT>::PacketFunctor(Handler func_ptr) noexcept
    : m_handler(func_ptr)
{
}

template <class ST, class PB, class PT>
bool PacketFunctor<ST, PB, PT>::handle(const char* buffer, uint32_t size, ST* session) noexcept
{
    ISerializer ser;
    ser.set(buffer, size);

    PT packet;
    packet.serialize(&ser);

    return (*m_handler)(session, packet);
}

////////////////////////////////////////////////////////////////////////
// PacketHandler definitions
////////////////////////////////////////////////////////////////////////

template <class ST, class PB, int32_t PACKET_MAX>
PacketHandler<ST, PB, PACKET_MAX>::PacketHandler() noexcept
{
    memset(m_packetFunctors, 0, sizeof(m_packetFunctors));
}

template <class ST, class PB, int32_t PACKET_MAX>
PacketHandler<ST, PB, PACKET_MAX>::~PacketHandler() noexcept
{
    for (int32_t i = 0; i < PACKET_MAX; ++i)
    {
        delete m_packetFunctors[i];
        m_packetFunctors[i] = nullptr;
    }
}

template <class ST, class PB, int32_t PACKET_MAX>
template <class PT>
void PacketHandler<ST, PB, PACKET_MAX>::register_handler(
    bool (*func)(ST*, const PT&)) noexcept
{
    PacketFunctorBase<ST, PB>* functor = new PacketFunctor<ST, PB, PT>(func);
    PT targetPacket;
    int typeIdx = static_cast<int>(targetPacket.type);
    m_packetFunctors[typeIdx] = functor;

    printf("registered handler; PacketType(%d)\n", typeIdx);
}

template <class ST, class PB, int32_t PACKET_MAX>
bool PacketHandler<ST, PB, PACKET_MAX>::handle(
    int32_t packet_type, const char* buffer, uint32_t bufsize, ST* session) noexcept
{
    if (packet_type >= PACKET_MAX)
    {
        fprintf(stderr, "packet_type is weird; type(%d), max(%d)\n", packet_type, PACKET_MAX);
        return false;
    }

    return m_packetFunctors[packet_type]->handle(buffer, bufsize, session);
}

}

#endif // __PACKET_HANDLER_HPP__
