#include <cstring>

#include <type_traits>

#include "serialize.hpp"

namespace RS
{

Serializer::Serializer() noexcept
    : m_curpos(0),
    m_maxpos(0)
{
    ::memset(m_buf, '\0', SERIALIZE_BUF_SIZE);
}

void
Serializer::value(bool& val) noexcept
{
    value((void*)&val, sizeof(std::remove_reference_t<decltype(val)>));
}

void
Serializer::value(int16_t& val) noexcept
{
    value((void*)&val, sizeof(std::remove_reference_t<decltype(val)>));
}

void
Serializer::value(uint16_t& val) noexcept
{
    value((void*)&val, sizeof(std::remove_reference_t<decltype(val)>));
}

void
Serializer::value(int32_t& val) noexcept
{
    value((void*)&val, sizeof(std::remove_reference_t<decltype(val)>));
}

void
Serializer::value(uint32_t& val) noexcept
{
    value((void*)&val, sizeof(std::remove_reference_t<decltype(val)>));
}

void
Serializer::value(float& val) noexcept
{
    value((void*)&val, sizeof(std::remove_reference_t<decltype(val)>));
}

void
Serializer::value(double& val) noexcept
{
    value((void*)&val, sizeof(std::remove_reference_t<decltype(val)>));
}

void
Serializer::value(long double& val) noexcept
{
    value((void*)&val, sizeof(std::remove_reference_t<decltype(val)>));
}

void
Serializer::value(char* val, uint32_t length) noexcept
{
    value((void*)val, length);
}

ISerializer::ISerializer() noexcept
    : Serializer()
{
}

bool
ISerializer::set(const char* buffer, uint32_t size) noexcept
{
    if (size > SERIALIZE_BUF_SIZE)
    {
        return false;
    }

    ::memcpy((void*)m_buf, (void*)buffer, size);
    m_curpos = 0;
    m_maxpos = size;
    return true;
}

void
ISerializer::value(void* val, uint32_t size) noexcept
{
    ::memcpy(val, (void*)&m_buf[m_curpos], size);
    m_curpos += size;
}

OSerializer::OSerializer() noexcept
    : Serializer()
{
}

bool
OSerializer::get(char* buffer, uint32_t size) noexcept
{
    if (m_maxpos > size)
    {
        return false;
    }

    ::memcpy((void*)buffer, (const void*)m_buf, m_maxpos);
    return true;
}

void
OSerializer::value(void* val, uint32_t size) noexcept
{
    if (m_maxpos + size > SERIALIZE_BUF_SIZE)
    {
        return;
    }

    ::memcpy((void*)&m_buf[m_maxpos], val, size);
    m_maxpos += size;
}

} // RS
