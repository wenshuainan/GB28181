#include <iostream>
#include "DevControl.h"
#include "Agent/MANSCDPAgent.h"
#include "MANSCDP/A.3FrontDeviceControl.h"

DevControl::DevControl(MANSCDPAgent *agent)
    : Control(agent)
{}

DevControl::~DevControl()
{}

bool DevControl::controlPTZ(int32_t ch, const PTZCommand *cmd)
{
    std::cout << "controlPTZ" << " channel=" << ch << std::endl;
    std::cout << "pan: (" << cmd->m_pan.left << ", " << cmd->m_pan.right << ") " << cmd->m_panSpeed << std::endl;
    std::cout << "tilt: (" << cmd->m_tilt.up << ", " << cmd->m_tilt.down << ") " << cmd->m_tiltSpeed << std::endl;
    std::cout << "zoom: (" << cmd->m_zoom.out << ", " << cmd->m_zoom.in << ") " << cmd->m_zoomSpeed << std::endl;
    return true;
}

bool DevControl::controlFI(int32_t ch, const FICommand *cmd)
{
    std::cout << "controlFI" << " channel=" << ch << std::endl;
    std::cout << "focus: (" << cmd->m_focus.near << ", " << cmd->m_focus.far << ") " << cmd->m_focusSpeed << std::endl;
    std::cout << "iris: (" << cmd->m_iris.small << ", " << cmd->m_iris.big << ") " << cmd->m_irisSpeed << std::endl;
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