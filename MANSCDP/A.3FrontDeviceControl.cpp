#include "A.3FrontDeviceControl.h"
#include "Interface/9.3Control.h"

CommandFormat::CommandFormat(const uint8_t* cmd)
{
    printf("++++++ CommandFormat\n");
    memcpy(m_bytes, cmd, 8);
}

CommandFormat::~CommandFormat()
{
    printf("------ CommandFormat\n");
}

std::unique_ptr<CommandFormat> CommandFormat::create(const uint8_t* cmd)
{
    printf("FrontDeviceControl create Command\n");
    CommandFormat *command = nullptr;
    if ((cmd[3] & 0xC0) == 0)
    {
        command = new PTZCommand(cmd);
    }
    else if ((cmd[3] & 0xF0) == 0x40)
    {
        command = new FICommand(cmd);
    }
    else if (cmd[3] == 0x81 || cmd[3] == 0x82 || cmd[3] == 0x83)
    {
        command = new PresetCommand(cmd);
    }
    else if (cmd[3] == 0x84 || cmd[3] == 0x85 || cmd[3] == 0x86
        || cmd[3] == 0x87 || cmd[3] == 0x88)
    {
        command = new CruiseCommand(cmd);
    }
    else if (cmd[3] == 0x89 || cmd[3] == 0x8A)
    {
        command = new ScanCommand(cmd);
    }
    else if (cmd[3] == 0x8C || cmd[3] == 0x8D)
    {
        command = new AuxiliaryCommand(cmd);
    }
    else
    {
        printf("unknown front device control command %02X\n", cmd[3]);
        return nullptr;
    }
    return std::unique_ptr<CommandFormat>(command);
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
        printf("!=0xA5\n");
        return false;
    }

    if (readMSB4(2) != 0)
    {
        printf("!=0\n");
        return false;
    }

    uint8_t checkbit = (readMSB4(1) + readLSB4(1) + readMSB4(2)) % 16;
    if (readLSB4(2) != checkbit)
    {
        printf("!=%d\n", checkbit);
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
{
    printf("++++++ PTZCommand\n");
    m_pan.left = false;
    m_pan.right = false;
    m_tilt.up = false;
    m_tilt.down = false;
    m_zoom.in = false;
    m_zoom.out = false;
    m_panSpeed = 0;
    m_tiltSpeed = 0;
    m_zoomSpeed = 0;
}

PTZCommand::~PTZCommand()
{
    printf("------ PTZCommand\n");
}

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
        printf("m_zoom.out == m_zoom.in == true\n");
        return false;
    }
    if (m_tilt.up && m_tilt.down)
    {
        printf("m_tilt.up == m_tilt.down == true\n");
        return false;
    }
    if (m_pan.left && m_pan.right)
    {
        printf("m_pan.left == m_pan.right == true\n");
        return false;
    }

    m_panSpeed = readByte(5);
    m_tiltSpeed = readByte(6);
    m_zoomSpeed = readMSB4(7);

    return true;
}

FICommand::FICommand(const uint8_t* cmd)
    : CommandFormat(cmd)
{
    printf("++++++ FICommand\n");
    m_iris.small = false;
    m_iris.big = false;
    m_focus.near = false;
    m_focus.far = false;
    m_focusSpeed = 0;
    m_irisSpeed = 0;
}

FICommand::~FICommand()
{
    printf("------ FICommand\n");
}

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
        printf("m_iris.small == m_iris.big == true\n");
        return false;
    }
    if (m_focus.near && m_focus.far)
    {
        printf("m_focus.near == m_focus.far == true\n");
        return false;
    }

    m_focusSpeed = readByte(5);
    m_irisSpeed = readByte(6);

    return true;
}

PresetCommand::PresetCommand(const uint8_t* cmd)
    : CommandFormat(cmd)
{
    printf("++++++ PresetCommand\n");
}

PresetCommand::~PresetCommand()
{
    printf("------ PresetCommand\n");
}

bool PresetCommand::parse()
{
    if (!CommandFormat::parse())
    {
        return false;
    }

    m_presetNum = readByte(6);
    if (m_presetNum == 0)
    {
        printf("m_presetNum is 0\n");
        return false;
    }

    return true;
}

CruiseCommand::CruiseCommand(const uint8_t* cmd)
    : CommandFormat(cmd)
{
    printf("++++++ CruiseCommand\n");
}

CruiseCommand::~CruiseCommand()
{
    printf("------ CruiseCommand\n");
}

bool CruiseCommand::parse()
{
    return false;
}

ScanCommand::ScanCommand(const uint8_t* cmd)
    : CommandFormat(cmd)
{
    printf("++++++ ScanCommand\n");
}

ScanCommand::~ScanCommand()
{
    printf("------ ScanCommand\n");
}

bool ScanCommand::parse()
{
    return false;
}

AuxiliaryCommand::AuxiliaryCommand(const uint8_t* cmd)
    : CommandFormat(cmd)
{
    printf("++++++ AuxiliaryCommand\n");
}

AuxiliaryCommand::~AuxiliaryCommand()
{
    printf("------ AuxiliaryCommand\n");
}

bool AuxiliaryCommand::parse()
{
    return false;
}
