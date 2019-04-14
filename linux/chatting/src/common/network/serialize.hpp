#ifndef __SERIALIZE_HPP__
#define __SERIALIZE_HPP__

#include <cstdint>

#include <string>
#include <sstream>

#define SERIALIZE_BUF_SIZE 4096

namespace RS
{

class Serializer
{
public:
    Serializer() noexcept;

public:
    void
    value(bool& val) noexcept;

    void
    value(int16_t& val) noexcept;

    void
    value(uint16_t& val) noexcept;

    void
    value(int32_t& val) noexcept;

    void
    value(uint32_t& val) noexcept;

    void
    value(float& val) noexcept;

    void
    value(double& val) noexcept;

    void
    value(long double& val) noexcept;

    void
    value(char* val, uint32_t length) noexcept;

private:
    virtual
    void
    value(void* val, uint32_t size) noexcept = 0;

protected:
    int8_t m_buf[SERIALIZE_BUF_SIZE];
    int32_t m_curpos;
    int32_t m_maxpos;
};

class ISerializer : public Serializer
{
public:
    ISerializer() noexcept;

public:
    bool
    set(const char* buffer, uint32_t size) noexcept;

private:
    void
    value(void* val, uint32_t size) noexcept override;
};

class OSerializer : public Serializer
{
public:
    OSerializer() noexcept;

public:
    int32_t
    get(char* buffer, uint32_t size) noexcept;

private:
    void
    value(void* val, uint32_t size) noexcept override;
};

class Serializable
{
public:
    virtual void serialize(Serializer* ser) noexcept = 0;
};

} // RS

#endif // __SERIALIZE_HPP__
