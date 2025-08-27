#include "Device.h"

Device::Device(const std::string& id, int32_t ch, Device* parent)
    : m_id(id)
    , m_ch(ch)
    , m_parent(parent)
{
    printf("++++++ Device(%s, %d)\n", m_id.c_str(), m_ch);
}

Device::~Device()
{
    printf("------ Device(%s, %d)\n", m_id.c_str(), m_ch);
}

const std::string& Device::getId() const
{
    return m_id;
}

int32_t Device::getChannel() const
{
    return m_ch;
}