#ifndef MANSCDP_AGENT_H
#define MANSCDP_AGENT_H

#include <memory>
#include "UA.h"
#include "Agent.h"
#include "Interface/9.3Control.h"
#include "Interface/9.4Alarm.h"
#include "Interface/9.5Query.h"
#include "Interface/9.6Status.h"
#include "Interface/9.7RecordQuery.h"

class MANSCDPAgent : public Agent
{
    friend UA;
    
private:
    std::shared_ptr<Control> m_devControl;
    std::shared_ptr<Query> m_devQuery;
    std::shared_ptr<Status> m_devStatus;
    std::shared_ptr<RecordQuery> m_devRecordQuery;
    std::shared_ptr<Alarm> m_devAlarm;

private:
    /* 
     * MANSCDP规定的所有请求命令
     * Control、Query、Notify
     * 遍历所有请求命令，匹配成功则调用其dispach方法处理
     */
    std::vector<std::shared_ptr<CmdTypeRequest>> m_cmdRequests;

public:
    MANSCDPAgent(UA *ua);
    ~MANSCDPAgent();

public:
    bool match(const std::string& method, const std::string& contentType);
    bool agent(const SipUserMessage& message);
    bool agent(const XMLDocument &xmldocReq) const;
    int32_t getKeepaliveTimeoutCount() const;
    bool recvedKeepaliveResponse(int32_t code) const;
    const std::unordered_map<std::string, int32_t>& getChannels() const;
    const char* getMainDeviceId() const;
    const char* getDeviceId(int32_t ch) const;
    bool makeKeepaliveNotify();
    int32_t getChNum(const std::string& deviceId) const;

public:
    bool sendResponseCmd(const XMLDocument& xmldocRes) const; //有应答命令
    bool sendKeepaliveNotify(const XMLDocument& notify) const;
    bool sendMediaStatusNotify(const SessionIdentifier& id, const XMLDocument& notify) const;
    bool sendAlarmNotify(const XMLDocument& notify) const;
};

#endif