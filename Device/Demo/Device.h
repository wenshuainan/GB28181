#ifndef GB28181_DEVICE_H
#define GB28181_DEVICE_H

#include <string>

class Device
{
protected:
    std::string m_id;
    int32_t m_ch;
    Device *m_parent;

public:
    Device(const std::string& id, int32_t ch, Device* parent = nullptr);
    virtual ~Device();

public:
    virtual const std::string& getId() const;
    virtual int32_t getChannel() const;
};

#endif