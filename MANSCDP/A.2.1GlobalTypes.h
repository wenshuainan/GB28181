#ifndef GLOBAL_TYPES_H
#define GLOBAL_TYPES_H

#include <time.h>
#include <string>
#include <vector>
#include "tinyxml2.h"

using namespace tinyxml2;

class integerType
{
protected:
    int32_t value;
    std::string strValue;
    bool bValid; // 可选/必选

public:
    integerType() : bValid(false) {}
    virtual ~integerType() {}

public:
    bool isValid() const
    {
        return bValid;
    }

    int32_t getValue() const
    {
        return value;
    }

    int32_t getInt() const
    {
        return value;
    }

    const std::string& getStr() const
    {
        return strValue;
    }

    integerType& operator=(const integerType& other)
    {
        value = other.value;
        strValue = other.strValue;
        bValid = other.bValid;
        return *this;
    }

    integerType& operator=(const int32_t& num)
    {
        value = num;
        strValue = std::to_string(num);
        bValid = true;
        return *this;
    }

    integerType& operator=(const std::string& str)
    {
        value = std::stoi(str); // WARN: will crash if str is not a number
        strValue = str;
        bValid = true;
        return *this;
    }
};

class stringType
{
protected:
    std::string value;
    bool bValid; // 可选/必选

public:
    stringType() : bValid(false) {}
    virtual ~stringType() {}

public:
    bool isValid() const
    {
        return bValid;
    }

    const std::string& getValue() const
    {
        return value;
    }

    const std::string& getStr() const
    {
        return value;
    }

    stringType& operator=(const stringType& other)
    {
        value = other.value;
        bValid = other.bValid;
        return *this;
    }

    stringType& operator=(const std::string& str)
    {
        value = str;
        bValid = true;
        return *this;
    }

    stringType& operator=(const int32_t& num)
    {
        value = std::to_string(num);
        bValid = true;
        return *this;
    }
};

class doubleType
{
protected:
    double value;
    std::string strValue;
    bool bValid; // 可选/必选

public:
    doubleType() : bValid(false) {}
    virtual ~doubleType() {}

public:
    bool isValid() const
    {
        return bValid;
    }

    double getValue() const
    {
        return value;
    }

    double getDouble() const
    {
        return value;
    }

    const std::string& getStr() const
    {
        return strValue;
    }

    doubleType& operator=(const doubleType& other)
    {
        value = other.value;
        strValue = other.strValue;
        bValid = other.bValid;
        return *this;
    }

    doubleType& operator=(const int32_t& num)
    {
        value = num;
        strValue = std::to_string(num);
        bValid = true;
        return *this;
    }

    doubleType& operator=(const std::string& str)
    {
        value = std::stod(str); // WARN: will crash if str is not a number
        strValue = str;
        bValid = true;
        return *this;
    }
};

/* A.2.1.2 设备编码类型 */
typedef stringType deviceIDType;

/* A.2.1.3 命令序列号类型 */
class SNType : public integerType
{
private:
    const int32_t minInclusize = 1;

public:
    SNType& operator=(const SNType& other)
    {
        value = other.value;
        bValid = other.bValid;
        return *this;
    }

    SNType& operator=(const std::string& str)
    {
        value = std::stoi(str);
        bValid = true;
        return *this;
    }

    SNType& operator=(const int32_t& num)
    {
        value = num;
        bValid = true;
        return *this;
    }
};

/* A.2.1.4 状态类型 */
class statusType : public stringType
{
public:
    enum
    {
        ON,
        OFF
    };

public:
    statusType& operator=(const statusType& other)
    {
        value = other.value;
        bValid = other.bValid;
        return *this;
    }

    statusType& operator=(const std::string& str)
    {
        value = str;
        bValid = true;
        return *this;
    }

    statusType& operator=(const int32_t& num)
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
class resultType : public stringType
{
public:
    enum
    {
        OK,
        ERROR
    };

    resultType& operator=(const resultType& other)
    {
        value = other.value;
        bValid = other.bValid;
        return *this;
    }

    resultType& operator=(const std::string& str)
    {
        value = str;
        bValid = true;
        return *this;
    }

    resultType& operator=(const int32_t& num)
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

class dateTimeType
{
private:
    bool bValid;
    std::string strValue;
    int32_t year;
    int32_t month;
    int32_t day;
    int32_t hour;
    int32_t minute;
    int32_t second;

public:
    dateTimeType() : bValid(false) {}

public:
    bool isValid() const { return bValid; }

    const std::string& getStr()
    {
        char str[128] = {0};
        snprintf(str, sizeof(str), "%04d-%02d-%02dT%02d:%02d:%02d",
            year, month, day, hour, minute, second);
        return strValue = str;
    }

    dateTimeType& operator=(const dateTimeType& other)
    {
        year = other.year;
        month = other.month;
        day = other.day;
        hour = other.hour;
        minute = other.minute;
        second = other.second;
        bValid = other.bValid;
        return *this;
    }

    dateTimeType& operator=(const std::string& str)
    {
        int ret = sscanf(str.c_str(), "%d-%d-%dT%d:%d:%d",
                    &year, &month, &day, &hour, &minute, &second);
        if (ret == 6)
        {
            bValid = true;
        }
        else
        {
            bValid = false;
        }
        return *this;
    }

    dateTimeType& operator=(const time_t& unixTime)
    {
        if (unixTime > 0)
        {
            struct tm* p = localtime(&unixTime);
            year = p->tm_year + 1900;
            month = p->tm_mon + 1;
            day = p->tm_mday;
            hour = p->tm_hour;
            minute = p->tm_min;
            second = p->tm_sec;
            bValid = true;
        }
        else
        {
            bValid = false;
        }
        return *this;
    }
};

/* A.2.1.6 控制码类型 */
class PTZCmdType : public stringType
{
public:
    const int32_t length = 8;

    PTZCmdType& operator=(const PTZCmdType& other)
    {
        value = other.value;
        bValid = other.bValid;

        return *this;
    }

    PTZCmdType& operator=(const std::string& str)
    {
        value = str;
        bValid = true;

        return *this;
    }
};

/* A.2.1.7 录像控制类型 */
class recordType : public stringType
{
public:
    enum
    {
        Record,
        StopRecord
    };

public:
    recordType& operator=(const recordType& other)
    {
        value = other.value;
        bValid = other.bValid;

        return *this;
    }

    recordType& operator=(const std::string& str)
    {
        value = str;
        bValid = true;

        return *this;
    }

    recordType& operator=(const int32_t& num)
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
class guardType : public stringType
{
public:
    enum
    {
        SetGuard,
        ResetGuard
    };

    guardType& operator=(const guardType& other)
    {
        value = other.value;
        bValid = other.bValid;

        return *this;
    }

    guardType& operator=(const std::string& str)
    {
        value = str;
        bValid = true;

        return *this;
    }

    guardType& operator=(const int32_t& num)
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

class complexType
{
public:
    virtual bool encode(XMLElement *parent) = 0;
};

/* A.2.1.9 目录项类型 */
class itemType : public complexType
{
public:
    /* <! -- 目标设备/区域/系统/业务分组/虚拟组织编码（必选） --> */
    deviceIDType DeviceID;
    /* <! -- 设备/区域/系统/业务分组/虚拟组织名称（必选） --> */
    stringType Name;
    /* <! -- 当为设备时，设备厂商（必选） --> */
    stringType Manufacturer;
    /* <! -- 当为设备时，设备型号（必选） --> */
    stringType Model;
    /* <! -- 行政区域，可为2、4、6、8位（必选） --> */
    stringType CivilCode;
    /* <! -- 警区（可选） --> */
    stringType Block;
    /* <! -- 当为设备时，安装地址（必选） --> */
    stringType Address;
    /* <! -- 当为设备时，是否有子设备（必选）1-有，0-没有 --> */
    integerType Parental;
    /* <! -- 当为设备时，父节点ID（必选）：当无父设备时，为设备所属系统ID；当有父设备时，为父设备ID； --> */
    /* <! -- 当为业务分组时，父节点ID（必选）：所属系统ID； --> */
    /* <! -- 当为虚拟组织时，父节点ID（上级节点为虚拟组织时必选；上级节点为业务分组时，无此字段）：父节点虚拟组织ID； --> */
    /* <! -- 当为系统时，父节点ID（有父节点系统时必选）：父节点系统ID； --> */
    /* <! -- 当为区域时，无父节点ID； --> */
    /* <! -- 可多值，用英文半角"/"分隔 --> */
    stringType ParentID;
    /* <! -- 注册方式（必选）缺省为1；1-符合IETF RFC 3261 标准的认证注册模式 --> */
    /* <! -- 2-基于口令的双向认证注册模式 --> */
    /* <! -- 3-基于数字证书的双向认证注册模式（高安全级别要求） --> */
    /* <! -- 4-基于数字证书的单向认证注册模式（高安全级别要求） --> */
    integerType RegisterWay;
    /* <! -- 摄像机安全能力等级代码（可选）；A-GB 35114 前端设备安全能力 A 级 --> */
    /* <! -- B-GB 35114 前端设备安全能力 B 级 --> */
    /* <! -- C-GB 35114 前端设备安全能力 C 级 --> */
    stringType SecurityLevelCode;
    /* <! -- 保密属性（必选）缺省为0；0-不涉密；1-涉密 --> */
    integerType Secrecy;
    /* 〈! -- 设备/系统IPv4/IPv6地址(可选)--〉 */
    stringType IPAddress;
    /* 〈! -- 设备/系统端口(可选)--〉 */
    integerType Port;
    /* 〈! -- 设备口令(可选)--〉 */
    stringType Password;
    /* 〈! -- 设备状态(必选)--〉 */
    statusType Status;
    /* 〈! -- 当为设备时,经度(一类、二类视频监控点必选)WGS-84坐标系--〉 */
    doubleType Longitude;
    /* 〈! -- 当为设备时,纬度(一类、二类视频监控点必选)WGS-84坐标系--〉 */
    doubleType Latitude;

public:
    virtual bool encode(XMLElement *parent)
    {
        XMLDocument *doc = parent->GetDocument();
        if (doc == nullptr)
        {
            return false;
        }

        XMLElement *xmlDeviceID = doc->NewElement("DeviceID");
        xmlDeviceID->SetText(DeviceID.getStr().c_str());
        parent->InsertEndChild(xmlDeviceID);

        XMLElement *xmlName = doc->NewElement("Name");
        xmlName->SetText(Name.getStr().c_str());
        parent->InsertEndChild(xmlName);

        XMLElement *xmlManufacturer = doc->NewElement("Manufacturer");
        xmlManufacturer->SetText(Manufacturer.getStr().c_str());
        parent->InsertEndChild(xmlManufacturer);

        XMLElement *xmlModel = doc->NewElement("Model");
        xmlModel->SetText(Model.getStr().c_str());
        parent->InsertEndChild(xmlModel);

        XMLElement *xmlCivilCode = doc->NewElement("CivilCode");
        xmlCivilCode->SetText(CivilCode.getStr().c_str());
        parent->InsertEndChild(xmlCivilCode);

        if (Block.isValid())
        {
            XMLElement *xmlBlock = doc->NewElement("Block");
            xmlBlock->SetText(Block.getStr().c_str());
            parent->InsertEndChild(xmlBlock);
        }

        XMLElement *xmlAddress = doc->NewElement("Address");
        xmlAddress->SetText(Address.getStr().c_str());
        parent->InsertEndChild(xmlAddress);

        XMLElement *xmlParental = doc->NewElement("Parental");
        xmlParental->SetText(Parental.getValue());
        parent->InsertEndChild(xmlParental);

        XMLElement *xmlParentID = doc->NewElement("ParentID");
        xmlParentID->SetText(ParentID.getStr().c_str());
        parent->InsertEndChild(xmlParentID);

        XMLElement *xmlRegisterWay = doc->NewElement("RegisterWay");
        xmlRegisterWay->SetText(RegisterWay.getValue());
        parent->InsertEndChild(xmlRegisterWay);

        if (SecurityLevelCode.isValid())
        {
            XMLElement *xmlSecurityLevelCode = doc->NewElement("SecurityLevelCode");
            xmlSecurityLevelCode->SetText(SecurityLevelCode.getStr().c_str());
            parent->InsertEndChild(xmlSecurityLevelCode);
        }

        XMLElement *xmlSecrecy = doc->NewElement("Secrecy");
        xmlSecrecy->SetText(Secrecy.getValue());
        parent->InsertEndChild(xmlSecrecy);

        if (IPAddress.isValid())
        {
            XMLElement *xmlIPAddress = doc->NewElement("IPAddress");
            xmlIPAddress->SetText(IPAddress.getStr().c_str());
            parent->InsertEndChild(xmlIPAddress);
        }

        XMLElement *xmlStatus = doc->NewElement("Status");
        xmlStatus->SetText(Status.getStr().c_str());
        parent->InsertEndChild(xmlStatus);

        return true;
    }
};

/* A.2.1.10 文件目录项类型 */
class itemFileType : public complexType
{
public:
    /* 〈! -- 目标设备编码(必选)--〉 */
    deviceIDType DeviceID;
    /* 〈!-- 目标设备名称(必选)--〉 */
    stringType Name;
    /* 〈! -- 文件路径名(可选)--〉 */
    stringType FilePath;
    /* 〈! -- 录像地址(可选)--〉 */
    stringType Address;
    /* 〈! -- 录像开始时间(可选)--〉 */
    dateTimeType StartTime;
    /* 〈! -- 录像结束时间(可选)--〉 */
    dateTimeType EndTime;
    /* 〈! -- 保密属性(必选)缺省为0;0-不涉密,1-涉密--〉 */
    integerType Secrecy;
    /* 〈! -- 录像产生类型(可选)time或alarm 或 manual--〉 */
    stringType Type;
    /* 〈! -- 录像触发者ID(可选)--〉 */
    stringType RecorderID;
    /* 〈! -- 录像文件大小,单位:Byte(可选)--〉 */
    stringType FileSize;
    /* 〈! -- 存储录像文件的设备/系统编码,(模糊查询时必选)--〉 */
    deviceIDType RecordLocation;
    /* 〈! --码流类型:0-主码流;1-子码流1;2-子码流2;以此类推(可选)--〉 */
    integerType StreamNumber;

public:
    virtual bool encode(XMLElement *parent)
    {
        XMLDocument *doc = parent->GetDocument();
        if (doc == nullptr)
        {
            return false;
        }

        XMLElement *xmlDeviceID = doc->NewElement("DeviceID");
        xmlDeviceID->SetText(DeviceID.getStr().c_str());
        parent->InsertEndChild(xmlDeviceID);

        XMLElement *xmlName = doc->NewElement("Name");
        xmlName->SetText(Name.getStr().c_str());
        parent->InsertEndChild(xmlName);

        if (FilePath.isValid())
        {
            XMLElement *xmlFilePath = doc->NewElement("FilePath");
            xmlFilePath->SetText(FilePath.getStr().c_str());
            parent->InsertEndChild(xmlFilePath);
        }

        if (Address.isValid())
        {
            XMLElement *xmlAddress = doc->NewElement("Address");
            xmlAddress->SetText(Address.getStr().c_str());
            parent->InsertEndChild(xmlAddress);
        }

        if (StartTime.isValid())
        {
            XMLElement *xmlStartTime = doc->NewElement("StartTime");
            xmlStartTime->SetText(StartTime.getStr().c_str());
            parent->InsertEndChild(xmlStartTime);
        }

        if (EndTime.isValid())
        {
            XMLElement *xmlEndTime = doc->NewElement("EndTime");
            xmlEndTime->SetText(EndTime.getStr().c_str());
            parent->InsertEndChild(xmlEndTime);
        }

        XMLElement *xmlSecrecy = doc->NewElement("Secrecy");
        xmlSecrecy->SetText(Secrecy.getValue());
        parent->InsertEndChild(xmlSecrecy);

        if (Type.isValid())
        {
            XMLElement *xmlType = doc->NewElement("Type");
            xmlType->SetText(Type.getStr().c_str());
            parent->InsertEndChild(xmlType);
        }

        if (RecorderID.isValid())
        {
            XMLElement *xmlRecorderID = doc->NewElement("RecorderID");
            xmlRecorderID->SetText(RecorderID.getStr().c_str());
            parent->InsertEndChild(xmlRecorderID);
        }

        if (FileSize.isValid())
        {
            XMLElement *xmlFileSize = doc->NewElement("FileSize");
            xmlFileSize->SetText(FileSize.getStr().c_str());
            parent->InsertEndChild(xmlFileSize);
        }

        if (RecordLocation.isValid())
        {
            XMLElement *xmlRecordLocation = doc->NewElement("RecordLocation");
            xmlRecordLocation->SetText(RecordLocation.getStr().c_str());
            parent->InsertEndChild(xmlRecordLocation);
        }

        if (StreamNumber.isValid())
        {
            XMLElement *xmlStreamNumber = doc->NewElement("StreamNumber");
            xmlStreamNumber->SetText(StreamNumber.getValue());
            parent->InsertEndChild(xmlStreamNumber);
        }

        return true;
    }
};

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