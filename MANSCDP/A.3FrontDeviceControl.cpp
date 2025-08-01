#include "A.3FrontDeviceControl.h"
#include "Interface/9.3Control.h"

CommandFormat::CommandFormat(const uint8_t* cmd)
{
    memcpy(m_bytes, cmd, 8);
}

CommandFormat::~CommandFormat()
{}

std::shared_ptr<CommandFormat> CommandFormat::create(const uint8_t* cmd)
{
    if ((cmd[3] & 0xC0) == 0)
    {
        return std::make_shared<PTZCommand>(cmd);
    }
    else if ((cmd[3] & 0xF0) == 0x40)
    {
        return std::make_shared<FICommand>(cmd);
    }
    else if (cmd[3] == 0x81 || cmd[3] == 0x82 || cmd[3] == 0x83)
    {
        return std::make_shared<PresetCommand>(cmd);
    }
    else if (cmd[3] == 0x84 || cmd[3] == 0x85 || cmd[3] == 0x86
        || cmd[3] == 0x87 || cmd[3] == 0x88)
    {
        return std::make_shared<CruiseCommand>(cmd);
    }
    else if (cmd[3] == 0x89 || cmd[3] == 0x8A)
    {
        return std::make_shared<ScanCommand>(cmd);
    }
    else if (cmd[3] == 0x8C || cmd[3] == 0x8D)
    {
        return std::make_shared<AuxiliaryCommand>(cmd);
    }
    else
    {
        return nullptr;
    }
}

bool CommandFormat::readBit(int32_t byte, int32_t bit) const
{
    return m_bytes[byte-1] & (1 << bit);
}

uint8_t CommandFormat::readMSB4(int32_t byte) const
{
    return m_bytes[byte-1] >> 4;
}

uint8_t CommandFormat::readLSB4(int32_t byte) const
{
    return m_bytes[byte-1] & 0x0F;
}

uint8_t CommandFormat::readByte(int32_t byte) const
{
    return m_bytes[byte-1];
}

bool CommandFormat::parse()
{
    if (readByte(1) != 0xA5)
    {
        return false;
    }

    if (readMSB4(2) != 0)
    {
        return false;
    }

    uint8_t checkbit = (readMSB4(1) + readLSB4(1) + readMSB4(2)) % 16;
    if (readLSB4(2) != checkbit)
    {
        return false;
    }

    int i;
    int32_t checkcode = 0;
    for (i = 0; i < 7; i++)
    {
        checkcode += m_bytes[i];
    }
    checkcode %= 256;
    if (readByte(8) != checkcode)
    {
        // return false;
    }

    return true;
}

PTZCommand::PTZCommand(const uint8_t* cmd)
    : CommandFormat(cmd)
{}

PTZCommand::~PTZCommand()
{}

bool PTZCommand::parse()
{
    if (!CommandFormat::parse())
    {
        return false;
    }

    m_zoom.out = readBit(4, 5);
    m_zoom.in = readBit(4, 4);
    m_tilt.up = readBit(4, 3);
    m_tilt.down = readBit(4, 2);
    m_pan.left = readBit(4, 1);
    m_pan.right = readBit(4, 0);
    if (m_zoom.out && m_zoom.in)
    {
        return false;
    }
    if (m_tilt.up && m_tilt.down)
    {
        return false;
    }
    if (m_pan.left && m_pan.right)
    {
        return false;
    }

    m_panSpeed = readByte(5);
    m_tiltSpeed = readByte(6);
    m_zoomSpeed = readMSB4(7);

    return true;
}

bool PTZCommand::handle(int ch, Control* control)
{
    if (m_pan.left)
    {
        control->startPanLeft(ch, m_panSpeed);
    }
    else
    {
        control->stopPanLeft(ch);
    }
    if (m_pan.right)
    {
        control->startPanRight(ch, m_panSpeed);
    }
    else
    {
        control->stopPanRight(ch);
    }
    if (m_tilt.up)
    {
        control->startTiltUp(ch, m_tiltSpeed);
    }
    else
    {
        control->stopTiltUp(ch);
    }
    if (m_tilt.down)
    {
        control->startTiltDown(ch, m_tiltSpeed);
    }
    else
    {
        control->stopTiltDown(ch);
    }
    if (m_zoom.in)
    {
        control->startZoomIn(ch, m_zoomSpeed);
    }
    else
    {
        control->stopZoomIn(ch);
    }
    if (m_zoom.out)
    {
        control->startZoomOut(ch, m_zoomSpeed);
    }
    else
    {
        control->stopZoomOut(ch);
    }
    return true;
}

FICommand::FICommand(const uint8_t* cmd)
    : CommandFormat(cmd)
{}

FICommand::~FICommand()
{}

bool FICommand::parse()
{
    if (!CommandFormat::parse())
    {
        return false;
    }

    m_iris.small = readBit(4, 3);
    m_iris.big = readBit(4, 2);
    m_focus.near = readBit(4, 1);
    m_focus.far = readBit(4, 0);
    if (m_iris.small && m_iris.big)
    {
        return false;
    }
    if (m_focus.near && m_focus.far)
    {
        return false;
    }

    m_focusSpeed = readByte(5);
    m_irisSpeed = readByte(6);

    return true;
}

bool FICommand::handle(int ch, Control* control)
{
    if (m_focus.near)
    {
        control->startFocusNear(ch, m_focusSpeed);
    }
    else
    {
        control->stopFocusNear(ch);
    }
    if (m_focus.far)
    {
        control->startFocusFar(ch, m_focusSpeed);
    }
    else
    {
        control->stopFocusFar(ch);
    }
    if (m_iris.small)
    {
        control->startIrisSmall(ch, m_irisSpeed);
    }
    else
    {
        control->stopIrisSmall(ch);
    }
    if (m_iris.big)
    {
        control->startIrisBig(ch, m_irisSpeed);
    }
    else
    {
        control->stopIrisBig(ch);
    }
    return true;
}

PresetCommand::PresetCommand(const uint8_t* cmd)
    : CommandFormat(cmd)
{}

PresetCommand::~PresetCommand()
{}

bool PresetCommand::parse()
{
    if (!CommandFormat::parse())
    {
        return false;
    }

    m_presetNum = readByte(6);
    if (m_presetNum == 0)
    {
        return false;
    }

    return true;
}

bool PresetCommand::handle(int ch, Control* control)
{
    return false;
}

CruiseCommand::CruiseCommand(const uint8_t* cmd)
    : CommandFormat(cmd)
{}

CruiseCommand::~CruiseCommand()
{}

bool CruiseCommand::parse()
{
    return false;
}

bool CruiseCommand::handle(int ch, Control* control)
{
    return false;
}

ScanCommand::ScanCommand(const uint8_t* cmd)
    : CommandFormat(cmd)
{}

ScanCommand::~ScanCommand()
{}

bool ScanCommand::parse()
{
    return false;
}

bool ScanCommand::handle(int ch, Control* control)
{
    return false;
}

AuxiliaryCommand::AuxiliaryCommand(const uint8_t* cmd)
    : CommandFormat(cmd)
{}

AuxiliaryCommand::~AuxiliaryCommand()
{}

bool AuxiliaryCommand::parse()
{
    return false;
}

bool AuxiliaryCommand::handle(int ch, Control* control)
{
    return false;
}