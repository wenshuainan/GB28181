#ifndef GB28181_USER_AGENT_H
#define GB28181_USER_AGENT_H

#include <vector>
#include <memory>
#include <thread>
#include <map>
#include "SIP/Adapter/SipAdapter.h"
#include "MANSCDPDevice.h"
#include "Interface/9.1Registration.h"

class RegistrationAgent;
class MANSCDPAgent;

class UA : public MANSCDPDevice
         , public Registration
{
public:
    struct KeepaliveInfo
    {
        int32_t interval;       //保活间隔，以秒为单位
        int32_t timeoutCount;   //保活超时次数，超出认为离线
    };

    /* 前端外围设备信息
     * GB28181 附录E 11、12、13码位规定的前端外围设备，取值范围 131~199
     * 每个类型的前端设备对应一个vector，元素按照通道号从0开始依次排列
     */
    struct FrontDevice
    {
        std::vector<std::string> camera; // 摄像机通道国标编码，11、12、13位取值131/132
        std::vector<std::string> alarmIn; // 报警输入通道国标编码，11、12、13位取值134
        std::vector<std::string> alarmOut; // 报警输出通道国标编码，11、12、13位取值135
    };
    
private:
    bool m_bStarted;
    std::shared_ptr<SipUserAgent> m_sip; // sip用户代理
    std::unique_ptr<RegistrationAgent> m_regAgent; // 注册功能代理
    std::unique_ptr<MANSCDPAgent> m_cdpAgent; // MANSCDP协议代理
    std::unique_ptr<std::thread> m_thread;
    KeepaliveInfo m_keepaliveInfo; // 用户配置保活信息
    FrontDevice m_frontDevice;
    std::map<std::string, std::unique_ptr<Device>> m_devices; // 前端外围设备，key为设备国标编码

public:
    UA();
    virtual ~UA();

private:
    void threadProc();
    bool createDevices();

public:
    /// @brief 注册响应消息处理
    /// @param res 响应消息
    /// @return 
    bool dispatchRegistrationResponse(const SipUserMessage& res);
    /// @brief MANSCDP请求命令处理
    /// @param cmd 请求命令
    /// @return 
    bool dispatchMANSCDPRequest(const XMLDocument &cmd);
    /// @brief MANSCDP命令结果处理
    /// @param code SIP状态码
    /// @param cmd UA发送的命令
    /// @return 
    bool dispatchMANSCDPResult(int32_t code, const XMLDocument &cmd);
    /// @brief INVITE会话请求处理
    /// @param id session标识
    /// @param req INVITE请求消息
    /// @return 
    bool dispatchSessionRequest(const SessionIdentifier& id, const SipUserMessage& req);
    /// @brief MANSRTSP请求消息处理
    /// @param req 请求消息
    /// @return 
    bool dispatchMANSRTSPRequest(const SipUserMessage& req);

    std::shared_ptr<SipUserAgent> getSip() const;
    Device* getDevice(const std::string& id);

public:
    /* 主设备需要实现的MANSCDP协议接口 */
    virtual void onRegistrationStatus(int32_t code, const std::string& sipReasonPhrase);
    virtual bool queryDeviceStatus(DeviceStatusQueryResponse& res);
    virtual bool queryCatalog(std::vector<itemType>& items);
    virtual bool queryDeviceInfo(DeviceInfoQueryResponse& res);
    virtual bool getStatus(std::vector<std::string>& offDevices);
    virtual bool sendKeepalive();

public:
    bool start(const SipUserAgent::ClientInfo& client,
               const SipUserAgent::ServerInfo& server,
               const KeepaliveInfo& keepalive,
               const FrontDevice& frontDevice);
    bool stop();
};

#endif