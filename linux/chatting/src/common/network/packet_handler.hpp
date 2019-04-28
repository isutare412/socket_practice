#ifndef __PACKET_HANDLER_HPP__
#define __PACKET_HANDLER_HPP__

#include <cstdio>
#include <cstring>

#include "serialize.hpp"

namespace RS
{

template <class PB>
class PacketFunctorBase
{
public:
    virtual ~PacketFunctorBase() noexcept {};

public:
    virtual bool handle(const char* buffer, uint32_t size) = 0;
};


template <class PB, class PT>
class PacketFunctor : public PacketFunctorBase<PB>
{
public:
    using Handler = bool (*)(const PT&);

public:
    PacketFunctor(Handler func_ptr) noexcept;

    bool handle(const char* buffer, uint32_t size) noexcept override;

private:
    Handler m_handler;
};


template <class PB, int32_t PACKET_MAX>
class PacketHandler
{
public:
    PacketHandler() noexcept;
    virtual ~PacketHandler() noexcept;

public:
    template <class PT>
    void register_handler(bool (*func)(const PT&)) noexcept;

    bool handle(int32_t packet_type, const char* buffer, uint32_t bufsize) noexcept;

private:
    PacketFunctorBase<PB>* m_packetFunctors[PACKET_MAX];
};


////////////////////////////////////////////////////////////////////////
// PacketFunctor definitions
////////////////////////////////////////////////////////////////////////

template <class PB, class PT>
PacketFunctor<PB, PT>::PacketFunctor(Handler func_ptr) noexcept
    : m_handler(func_ptr)
{
}

template <class PB, class PT>
bool PacketFunctor<PB, PT>::handle(const char* buffer, uint32_t size) noexcept
{
    ISerializer ser;
    ser.set(buffer, size);

    PT packet;
    packet.serialize(&ser);

    return (*m_handler)(packet);
}

////////////////////////////////////////////////////////////////////////
// PacketHandler definitions
////////////////////////////////////////////////////////////////////////

template <class PB, int32_t PACKET_MAX>
PacketHandler<PB, PACKET_MAX>::PacketHandler() noexcept
{
    memset(m_packetFunctors, 0, sizeof(m_packetFunctors));
}

template <class PB, int32_t PACKET_MAX>
PacketHandler<PB, PACKET_MAX>::~PacketHandler() noexcept
{
    for (int32_t i = 0; i < PACKET_MAX; ++i)
    {
        delete m_packetFunctors[i];
        m_packetFunctors[i] = nullptr;
    }
}

template <class PB, int32_t PACKET_MAX>
template <class PT>
void PacketHandler<PB, PACKET_MAX>::register_handler(bool (*func)(const PT&)) noexcept
{
    PacketFunctorBase<PB>* functor = new PacketFunctor<PB, PT>(func);
    PT targetPacket;
    int typeIdx = static_cast<int>(targetPacket.type);
    m_packetFunctors[typeIdx] = functor;

    printf("registered handler; PacketType(%d)\n", typeIdx);
}

template <class PB, int32_t PACKET_MAX>
bool PacketHandler<PB, PACKET_MAX>::handle(
    int32_t packet_type, const char* buffer, uint32_t bufsize) noexcept
{
    if (packet_type >= PACKET_MAX)
    {
        fprintf(stderr, "packet_type is weird; type(%d), max(%d)\n", packet_type, PACKET_MAX);
        return false;
    }

    return m_packetFunctors[packet_type]->handle(buffer, bufsize);
}

}

#endif // __PACKET_HANDLER_HPP__
