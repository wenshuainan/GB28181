#ifndef A_3_FRONT_DEVICE_CONTROL_H
#define A_3_FRONT_DEVICE_CONTROL_H

#include <stdint.h>
#include <memory>

class Control;

/* A.3 前端设备控制协议 */

/* A.3.1 指令格式 */
class CommandFormat
{
private:
    uint8_t m_bytes[8];

public:
    CommandFormat(const uint8_t* cmd);
    virtual ~CommandFormat();
    static std::shared_ptr<CommandFormat> create(const uint8_t* cmd);

protected:
    bool readBit(int32_t byte, int32_t bit) const;
    uint8_t readMSB4(int32_t byte) const;
    uint8_t readLSB4(int32_t byte) const;
    uint8_t readByte(int32_t byte) const;

public:
    virtual bool parse();
    virtual bool handle(int ch, Control* control) = 0;
};

/* A.3.2 PTZ指令 */
class PTZCommand : public CommandFormat
{
private:
    struct
    {
        bool out;
        bool in;
    } m_zoom;

    struct
    {
        bool up;
        bool down;
    } m_tilt;

    struct
    {
        bool left;
        bool right;
    } m_pan;

    uint8_t m_panSpeed;
    uint8_t m_tiltSpeed;
    uint8_t m_zoomSpeed;

public:
    PTZCommand(const uint8_t* cmd);
    virtual ~PTZCommand();

public:
    virtual bool parse();
    virtual bool handle(int ch, Control* control);
};

/* A.3.3 FI指令 */
class FICommand : public CommandFormat
{
private:
    struct
    {
        bool small;
        bool big;
    } m_iris;

    struct
    {
        bool near;
        bool far;
    } m_focus;

    uint8_t m_focusSpeed;
    uint8_t m_irisSpeed;
    
public:
    FICommand(const uint8_t* cmd);
    virtual ~FICommand();

public:
    virtual bool parse();
    virtual bool handle(int ch, Control* control);
};

/* A.3.4 预置位指令 */
class PresetCommand : public CommandFormat
{
private:
    uint8_t m_presetNum;

public:
    PresetCommand(const uint8_t* cmd);
    virtual ~PresetCommand();

public:
    virtual bool parse();
    virtual bool handle(int ch, Control* control);
};

/* A.3.5 巡航指令 */
class CruiseCommand : public CommandFormat
{
private:
    uint8_t m_cruiseNum;

public:
    CruiseCommand(const uint8_t* cmd);
    virtual ~CruiseCommand();

public:
    virtual bool parse();
    virtual bool handle(int ch, Control* control);
};

/* A.3.6 扫描指令 */
class ScanCommand : public CommandFormat
{
public:
    ScanCommand(const uint8_t* cmd);
    virtual ~ScanCommand();

public:
    virtual bool parse();
    virtual bool handle(int ch, Control* control);
};

/* A.3.7 辅助开关控制指令 */
class AuxiliaryCommand : public CommandFormat
{
public:
    AuxiliaryCommand(const uint8_t* cmd);
    virtual ~AuxiliaryCommand();

public:
    virtual bool parse();
    virtual bool handle(int ch, Control* control);
};

#endif