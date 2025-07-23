#include <iostream>
#include "DevStatus.h"

DevStatus::DevStatus()
{}

DevStatus::~DevStatus()
{}

bool DevStatus::getStatus(int32_t ch)
{
    return true; // 正常
}

void DevStatus::onKeepaliveSuccess()
{}

void DevStatus::onKeepaliveTimeout(int32_t count)
{
    std::cout << "Keepalive timeout: " << count << std::endl;
}