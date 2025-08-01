#include <iostream>
#include "DevControl.h"

DevControl::DevControl()
{}

DevControl::~DevControl()
{}

bool DevControl::startPanLeft(int32_t ch, int32_t speed)
{
    std::cout << "startPanLeft" << " channel=" << ch << " speed=" << speed << std::endl;
    return true;
}

bool DevControl::startPanRight(int32_t ch, int32_t speed)
{
    std::cout << "startPanRight" << " channel=" << ch << " speed=" << speed << std::endl;
    return true;
}

bool DevControl::startTiltUp(int32_t ch, int32_t speed)
{
    std::cout << "startTiltUp" << " channel=" << ch << " speed=" << speed << std::endl;
    return true;
}

bool DevControl::startTiltDown(int32_t ch, int32_t speed)
{
    std::cout << "startTiltDown" << " channel=" << ch << " speed=" << speed << std::endl;
    return true;
}

bool DevControl::startZoomIn(int32_t ch, int32_t speed)
{
    std::cout << "startZoomIn" << " channel=" << ch << " speed=" << speed << std::endl;
    return true;
}

bool DevControl::startZoomOut(int32_t ch, int32_t speed)
{
    std::cout << "startZoomOut" << " channel=" << ch << " speed=" << speed << std::endl;
    return true;
}

bool DevControl::stopPanLeft(int32_t ch)
{
    std::cout << "stopPanLeft" << " channel=" << ch << std::endl;
    return true;
}

bool DevControl::stopPanRight(int32_t ch)
{
    std::cout << "stopPanRight" << " channel=" << ch << std::endl;
    return true;
}

bool DevControl::stopTiltUp(int32_t ch)
{
    std::cout << "stopTiltUp" << " channel=" << ch << std::endl;
    return true;
}

bool DevControl::stopTiltDown(int32_t ch)
{
    std::cout << "stopTiltDown" << " channel=" << ch << std::endl;
    return true;
}

bool DevControl::stopZoomIn(int32_t ch)
{
    std::cout << "stopZoomIn" << " channel=" << ch << std::endl;
    return true;
}

bool DevControl::stopZoomOut(int32_t ch)
{
    std::cout << "stopZoomOut" << " channel=" << ch << std::endl;
    return true;
}

bool DevControl::startFocusNear(int32_t ch, int32_t speed)
{
    std::cout << "startFocusNear" << " channel=" << ch << " speed=" << speed << std::endl;
    return true;
}

bool DevControl::startFocusFar(int32_t ch, int32_t speed)
{
    std::cout << "startFocusFar" << " channel=" << ch << " speed=" << speed << std::endl;
    return true;
}

bool DevControl::startIrisSmall(int32_t ch, int32_t speed)
{
    std::cout << "startIrisSmall" << " channel=" << ch << " speed=" << speed << std::endl;
    return true;
}

bool DevControl::startIrisBig(int32_t ch, int32_t speed)
{
    std::cout << "startIrisBig" << " channel=" << ch << " speed=" << speed << std::endl;
    return true;
}

bool DevControl::stopFocusNear(int32_t ch)
{
    std::cout << "stopFocusNear" << " channel=" << ch << std::endl;
    return true;
}

bool DevControl::stopFocusFar(int32_t ch)
{
    std::cout << "stopFocusFar" << " channel=" << ch << std::endl;
    return true;
}

bool DevControl::stopIrisSmall(int32_t ch)
{
    std::cout << "stopIrisSmall" << " channel=" << ch << std::endl;
    return true;
}

bool DevControl::stopIrisBig(int32_t ch)
{
    std::cout << "stopIrisBig" << " channel=" << ch << std::endl;
    return true;
}

bool DevControl::reboot()
{
    std::cout << "reboot" << std::endl;
    return true;
}

bool DevControl::startRecord(int32_t ch, int32_t stream)
{
    std::cout << "startRecord" << " channel=" << ch << " stream=" << stream << std::endl;
    return true;
}

bool DevControl::stopRecord(int32_t ch, int32_t stream)
{
    std::cout << "stopRecord" << " channel=" << ch << " stream=" << stream << std::endl;
    return true;
}

bool DevControl::setGuard(int32_t ch)
{
    std::cout << "setGuard" << " channel=" << ch << std::endl;
    return true;
}

bool DevControl::resetGuard(int32_t ch)
{
    std::cout << "resetGuard" << " channel=" << ch << std::endl;
    return true;
}

bool DevControl::resetAlarm(int32_t ch, int32_t method, int32_t type)
{
    std::cout << "resetAlarm" << " channel=" << ch << " method=" << method << " type=" << type << std::endl;
    return true;
}