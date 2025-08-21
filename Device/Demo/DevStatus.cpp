#include <iostream>
#include "DevStatus.h"

DevStatus::DevStatus(MANSCDPAgent *agent)
    : Status(agent)
{}

DevStatus::~DevStatus()
{}

bool DevStatus::getStatus(int32_t ch)
{
    return true; // 正常
}

void DevStatus::onKeepaliveSuccess()
{
    printf("Keepalive success\n");
}

void DevStatus::onKeepaliveTimeout(int32_t count)
{
    printf("Keepalive timeout: %d\n", count);
}