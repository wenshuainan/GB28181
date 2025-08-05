#include <iostream>
#include "MANSCDPAgent.h"
#include "tinyxml2.h"
#include "UA.h"
#include "MANSCDP/A.2.5Notify.h"
#include "MANSCDP/A.2.6Response.h"
#include "DevControl.h"
#include "DevQuery.h"
#include "DevStatus.h"
#include "DevRecordQuery.h"
#include "DevAlarm.h"
#include "MANSCDP/A.2.5Notify.h"

MANSCDPAgent::MANSCDPAgent(UA *ua) : Agent(ua)
{
    m_devControl = std::make_shared<DevControl>(this);
    m_devQuery = std::make_shared<DevQuery>();
    m_devStatus = std::make_shared<DevStatus>();
    m_devRecordQuery = std::make_shared<DevRecordQuery>();
    m_devAlarm = std::make_shared<DevAlarm>(this);

    m_cmdRequests.push_back(std::make_shared<ControlReuest>(this, m_devControl.get()));
    m_cmdRequests.push_back(std::make_shared<QueryRequest>(this, m_devQuery.get(), m_devRecordQuery.get()));
}

MANSCDPAgent::~MANSCDPAgent()
{
    m_cmdRequests.clear();
}

bool MANSCDPAgent::match(const std::string& method, const std::string& contentType)
{
    return strCaseCmp(method, "MESSAGE")
            && strCaseCmp(contentType, "Application/MANSCDP+xml");
}

bool MANSCDPAgent::agent(const SipUserMessage& message)
{
    std::cout << "not agent:" << std::endl;
    message.print();
    return false;
}

bool MANSCDPAgent::agent(const XMLDocument &xmldocReq) const
{
    std::string rootName = xmldocReq.RootElement()->Name();
    for (auto i : m_cmdRequests)
    {
        if (i->match(rootName))
        {
            return i->dispatch(xmldocReq.FirstChildElement());
        }
    }

    return false;
}

int32_t MANSCDPAgent::getKeepaliveTimeoutCount() const
{
    return m_devStatus->getTimeoutCount();
}

bool MANSCDPAgent::recvedKeepaliveResponse(int32_t code) const
{
    if (code == 200)
    {
        m_devStatus->addRecvedCount();
        return true;
    }
    else
    {
        return false;
    }
}

void MANSCDPAgent::clearKeepaliveTimeoutCount() const
{
    m_devStatus->clearTimeoutCount();
}

const std::unordered_map<std::string, int32_t>& MANSCDPAgent::getChannels() const
{
    return m_ua->m_channels;
}

const char* MANSCDPAgent::getMainDeviceId() const
{
    return m_ua->m_sip->getSipUser();
}

const char* MANSCDPAgent::getDeviceId(int32_t ch) const
{
    for (auto& c : m_ua->m_channels)
    {
        if (ch == c.second)
        {
            return c.first.c_str();
        }
    }
    return "";
}

bool MANSCDPAgent::makeKeepaliveNotify()
{
    KeepaliveNotify keepalive(this, m_devStatus.get());
    return keepalive.notify();
}

int32_t MANSCDPAgent::getChNum(const std::string& deviceId) const
{
    return m_ua->getChNum(deviceId);
}

std::shared_ptr<Alarm> MANSCDPAgent::getDevAlarm() const
{
    return m_devAlarm;
}

bool MANSCDPAgent::sendResponseCmd(const XMLDocument& xmldocRes) const
{
    return m_ua->m_sip->sendMANSCDPResponse(xmldocRes);
}

bool MANSCDPAgent::sendKeepaliveNotify(const XMLDocument& notify) const
{
    return m_ua->m_sip->sendKeepaliveNotify(notify);
}

bool MANSCDPAgent::sendMediaStatusNotify(const SessionIdentifier& id, const XMLDocument& notify) const
{
    return m_ua->m_sip->sendSessionNotify(id, notify);
}

bool MANSCDPAgent::sendAlarmNotify(const XMLDocument& notify) const
{
    return m_ua->m_sip->sendAlarmNotify(notify);
}