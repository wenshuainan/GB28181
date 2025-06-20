#ifndef GLOBAL_TYPES_H
#define GLOBAL_TYPES_H

#include <string>
#include <vector>
#include "../../tinyxml2/tinyxml2.h"

using namespace tinyxml2;

template <typename T>
class Type
{
protected:
    T value;
    bool bValid; // 可选/必选

public:
    T& getValue()
    {
        return value;
    }

public:
    Type() : bValid(false) {}
    virtual ~Type() {}

public:
    bool isValid()
    {
        return bValid;
    }

    Type& operator=(const T& t)
    {
        value = t;
        bValid = true;

        return *this;
    }
};

/* A.2.1.2 设备编码类型 */
typedef Type<std::string> deviceIDType;

/* A.2.1.3 命令序列号类型 */
class SNType : public Type<int>
{
private:
    const int minInclusize = 1;

public:
    SNType& operator=(const std::string& str)
    {
        value = std::stoi(str);
        bValid = true;

        return *this;
    }

    SNType& operator=(const int& num)
    {
        value = num;
        bValid = true;

        return *this;
    }
};

/* A.2.1.4 状态类型 */
class statusType : public Type<std::string>
{
public:
    enum
    {
        ON,
        OFF
    };

public:
    statusType& operator=(const std::string& str)
    {
        value = str;
        bValid = true;

        return *this;
    }

    statusType& operator=(const int& num)
    {
        switch (num)
        {
        case ON:
            value = "ON";
            break;
        case OFF:
            value = "OFF";
            break;
        
        default:
            break;
        }
        bValid = true;

        return *this;
    }
};

/* A.2.1.5 结果类型 */
class resultType : public Type<std::string>
{
public:
    enum
    {
        OK,
        ERROR
    };

    resultType& operator=(const std::string& str)
    {
        value = str;
        bValid = true;

        return *this;
    }

    resultType& operator=(const int& num)
    {
        switch (num)
        {
        case OK:
            value = "OK";
            break;
        case ERROR:
            value = "ERROR";
            break;
        
        default:
            break;
        }
        bValid = true;

        return *this;
    }
};

/* A.2.1.6 控制码类型 */
class PTZCmdType : public Type<std::string>
{
public:
    const int length = 8;

    PTZCmdType& operator=(const std::string& str)
    {
        value = str;
        bValid = true;

        return *this;
    }
};

/* A.2.1.7 录像控制类型 */
class recordType : public Type<std::string>
{
public:
    enum
    {
        Record,
        StopRecord
    };

public:
    recordType& operator=(const std::string& str)
    {
        value = str;
        bValid = true;

        return *this;
    }

    recordType& operator=(const int& num)
    {
        switch (num)
        {
        case Record:
            value = "Record";
            break;
        case StopRecord:
            value = "StopRecord";
            break;
        
        default:
            break;
        }
        bValid = true;

        return *this;
    }
};

/* A.2.1.8 布放/撤防控制类型 */
class guardType : public Type<std::string>
{
public:
    enum
    {
        SetGuard,
        ResetGuard
    };

    guardType& operator=(const std::string& str)
    {
        value = str;
        bValid = true;

        return *this;
    }

    guardType& operator=(const int& num)
    {
        switch (num)
        {
        case SetGuard:
            value = "SetGuard";
            break;
        case ResetGuard:
            value = "ResetGuard";
            break;
        
        default:
            break;
        }
        bValid = true;

        return *this;
    }
};

/* A.2.1.9 目录项类型 */
class itemType
{
public:
    struct sequence
    {
        /* <! -- 目标设备/区域/系统/业务分组/虚拟组织编码（必选） --> */
        deviceIDType DeviceID;
        /* <! -- 设备/区域/系统/业务分组/虚拟组织名称（必选） --> */
        Type<std::string> Name;
        /* <! -- 当为设备时，设备厂商（必选） --> */
        Type<std::string> Manufacturer;
        /* <! -- 当为设备时，设备型号（必选） --> */
        Type<std::string> Model;
        /* <! -- 行政区域，可为2、4、6、8位（必选） --> */
        SNType CivilCode;
        /* <! -- 警区（可选） --> */
        Type<std::string> Block;
        /* <! -- 当为设备时，安装地址（必选） --> */
        Type<std::string> Address;
        /* <! -- 当为设备时，是否有子设备（必选）1-有，0-没有 --> */
        Type<int> Parental;
        /* <! -- 当为设备时，父节点ID（必选）：当无父设备时，为设备所属系统ID；当有父设备时，为父设备ID； --> */
        /* <! -- 当为业务分组时，父节点ID（必选）：所属系统ID； --> */
        /* <! -- 当为虚拟组织时，父节点ID（上级节点为虚拟组织时必选；上级节点为业务分组时，无此字段）：父节点虚拟组织ID； --> */
        /* <! -- 当为系统时，父节点ID（有父节点系统时必选）：父节点系统ID； --> */
        /* <! -- 当为区域时，无父节点ID； --> */
        /* <! -- 可多值，用英文半角"/"分隔 --> */
        Type<std::string> ParentID;
        /* <! -- 注册方式（必选）缺省为1；1-符合IETF RFC 3261 标准的认证注册模式 --> */
        /* <! -- 2-基于口令的双向认证注册模式 --> */
        /* <! -- 3-基于数字证书的双向认证注册模式（高安全级别要求） --> */
        /* <! -- 4-基于数字证书的单向认证注册模式（高安全级别要求） --> */
        Type<int> RegisterWay;
        /* <! -- 摄像机安全能力等级代码（可选）；A-GB 35114 前端设备安全能力 A 级 --> */
        /* <! -- B-GB 35114 前端设备安全能力 B 级 --> */
        /* <! -- C-GB 35114 前端设备安全能力 C 级 --> */
        Type<std::string> SecurityLevelCode;
        /* <! -- 保密属性（必选）缺省为0；0-不涉密；1-涉密 --> */
        Type<int> Secrecy;
    };

private:
    sequence value;

public:
    sequence& getValue()
    {
        return value;
    }
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