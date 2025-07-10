#ifndef CONTROL_PROCESS_H
#define CONTROL_PROCESS_H

#include "A.2.3Control.h"
#include "A.2.6Response.h"

class Control
{
public:
    Control() {}
    virtual ~Control() {}

public:
    /*
     * 源设备向目标设备发送摄像机云台控制、远程启动、强制关键帧、拉框放大、拉框缩小、PTZ精
     * 准控制、存储卡格式化、目标跟踪命令后,目标设备不发送应答命令,命令流程见9.3.2.1;
     * 
     * 源设备向目标设备发送录像控制、报警布防/撤防、报警复位、看守位控制、软件升级、设备配置
     * 命令后,目标设备应发送应答命令表示执行的结果,命令流程见9.3.2.2;
     */
    virtual bool process(const PTZCmdControl::Request& req) = 0;
    virtual bool process(const TeleBootControl::Request& req) = 0;
    virtual bool process(const RecordControl::Request& req, DeviceControlResponse::Response& res) = 0;
};

#endif