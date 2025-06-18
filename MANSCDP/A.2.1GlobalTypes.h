#ifndef GLOBAL_TYPES_H
#define GLOBAL_TYPES_H

#include <string>

/* A.2.1.2 设备编码类型 */
class deviceIDType
{
private:
    std::string deviceID;

public:
    std::string& getValue()
    {
        return deviceID;
    }

    deviceIDType& operator=(const std::string& str)
    {
        deviceID = str;
        return *this;
    }
};

/* A.2.1.3 命令序列号类型 */
class SNType
{
private:
    std::string SN;
    const int minInclusize = 1;

public:
    std::string& getValue()
    {
        return SN;
    }

    SNType& operator=(const std::string& str)
    {
        SN = str;
        return *this;
    }

    SNType& operator=(const int& num)
    {
        SN = std::to_string(num);
        return *this;
    }
};

/* A.2.1.4 状态类型 */
class statusType
{
public:
    enum
    {
        ON,
        OFF
    };

private:
    std::string status;

public:
    std::string& getValue()
    {
        return status;
    }

    statusType& operator=(const std::string& str)
    {
        status = str;
        return *this;
    }

    statusType& operator=(const int& num)
    {
        switch (num)
        {
        case ON:
            status = "ON";
            break;
        case OFF:
            status = "OFF";
            break;
        
        default:
            break;
        }

        return *this;
    }
};

/* A.2.1.5 结果类型 */
class resultType
{
public:
    enum
    {
        OK,
        ERROR
    };

private:
    std::string result;

public:
    std::string& getValue()
    {
        return result;
    }

    resultType& operator=(const std::string& str)
    {
        result = str;
        return *this;
    }

    resultType& operator=(const int& num)
    {
        switch (num)
        {
        case OK:
            result = "OK";
            break;
        case ERROR:
            result = "ERROR";
            break;
        
        default:
            break;
        }

        return *this;
    }
};

/* A.2.1.6 控制码类型 */
class PTZCmdType
{
private:
    std::string PTZCmd;
    const int length = 8;

public:
    std::string& getValue()
    {
        return PTZCmd;
    }

    PTZCmdType& operator=(const std::string& str)
    {
        PTZCmd = str;
        return *this;
    }
};

/* A.2.1.7 录像控制类型 */
class recordType
{
public:
    enum
    {
        Record,
        StopRecord
    };

private:
    std::string record;

public:
    std::string& getValue()
    {
        return record;
    }

    recordType& operator=(const std::string& str)
    {
        record = str;
        return *this;
    }

    recordType& operator=(const int& num)
    {
        switch (num)
        {
        case Record:
            record = "Record";
            break;
        case StopRecord:
            record = "StopRecord";
            break;
        
        default:
            break;
        }

        return *this;
    }
};

/* A.2.1.8 布放/撤防控制类型 */
class guardType
{
public:
    enum
    {
        SetGuard,
        ResetGuard
    };

private:
    std::string guard;

public:
    std::string& getValue()
    {
        return guard;
    }

    guardType& operator=(const std::string& str)
    {
        guard = str;
        return *this;
    }

    guardType& operator=(const int& num)
    {
        switch (num)
        {
        case SetGuard:
            guard = "SetGuard";
            break;
        case ResetGuard:
            guard = "ResetGuard";
            break;
        
        default:
            break;
        }

        return *this;
    }
};

/* A.2.1.9 目录项类型 */
class itemType
{
public:
    /* <! -- 目标设备/区域/系统/业务分组/虚拟组织编码（必选） --> */
    deviceIDType DeviceID;
    /* <! -- 设备/区域/系统/业务分组/虚拟组织名称（必选） --> */
    std::string Name;
    /* <! -- 当为设备时，设备厂商（必选） --> */
    std::string Manufacturer;
    /* <! -- 当为设备时，设备型号（必选） --> */
    std::string Model;
    /* <! -- 行政区域，可为2、4、6、8位（必选） --> */
    SNType CivilCode;
    /* <! -- 警区（可选） --> */
    std::string Block;
    /* <! -- 当为设备时，安装地址（必选） --> */
    std::string Address;
    /* <! -- 当为设备时，是否有子设备（必选）1-有，0-没有 --> */
    int Parental;
    /* <! -- 当为设备时，父节点ID（必选）：当无父设备时，为设备所属系统ID；当有父设备时，为父设备ID； --> */
    /* <! -- 当为业务分组时，父节点ID（必选）：所属系统ID； --> */
    /* <! -- 当为虚拟组织时，父节点ID（上级节点为虚拟组织时必选；上级节点为业务分组时，无此字段）：父节点虚拟组织ID； --> */
    /* <! -- 当为系统时，父节点ID（有父节点系统时必选）：父节点系统ID； --> */
    /* <! -- 当为区域时，无父节点ID； --> */
    /* <! -- 可多值，用英文半角"/"分隔 --> */
    std::string ParentID;
    /* <! -- 注册方式（必选）缺省为1；1-符合IETF RFC 3261 标准的认证注册模式 --> */
    /* <! -- 2-基于口令的双向认证注册模式 --> */
    /* <! -- 3-基于数字证书的双向认证注册模式（高安全级别要求） --> */
    /* <! -- 4-基于数字证书的单向认证注册模式（高安全级别要求） --> */
    int RegisterWay;
    /* <! -- 摄像机安全能力等级代码（可选）；A-GB 35114 前端设备安全能力 A 级 --> */
    /* <! -- B-GB 35114 前端设备安全能力 B 级 --> */
    /* <! -- C-GB 35114 前端设备安全能力 C 级 --> */
    std::string SecurityLevelCode;
    /* <! -- 保密属性（必选）缺省为0；0-不涉密；1-涉密 --> */
    int Secrecy;
};

/* A.2.1.10 文件目录项类型 */
class itemFileType
{};

/* A.2.1.11 PTZ精准控制类型 */
class PTZPreciseCtrlType
{};

/* A.2.1.12 OSD配置类型 */
class OSDCfgType
{};

/* A.2.1.13 视频参数属性类型 */
class videoParamAttributeCfgType
{};

/* A.2.1.14 移动设备位置类型 */
class itemMobilePositionType
{};

/* A.2.1.15 录像计划配置类型 */
class videoRecordPlanCfgType
{};

/* A.2.1.16 报警录像配置类型 */
class videoAlarmRecordCfgType
{};

/* A.2.1.17 视频画面遮挡配置类型 */
class pictrueMaskCfgType
{};

/* A.2.1.18 报警上报开关配置类型 */
class alarmReportCfgType
{};

/* A.2.1.19 基本参数配置类型 */
class basicParamCfgType
{};

/* A.2.1.20 视频参数范围配置类型 */
class videoParamOptCfgType
{};

/* A.2.1.21 SVAC编码配置类型 */
class SVACEncodeCfgType
{};

/* A.2.1.22 SVAC解码配置类型 */
class SVACDecodeCfgType
{};

/* A.2.1.23 画面翻转配置类型 */
class frameMirrorCfgType
{};

/* A.2.1.24 图像抓拍配置类型 */
class snapShotCfgType
{};

#endif