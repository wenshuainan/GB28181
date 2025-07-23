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
#include "MANSCDP/A.2.5Notify.h"

MANSCDPAgent::MANSCDPAgent(UA *ua) : Agent(ua)
{
    m_devControl = std::make_shared<DevControl>();
    m_devQuery = std::make_shared<DevQuery>();
    m_devStatus = std::make_shared<DevStatus>();
    m_devRecordQuery = std::make_shared<DevRecordQuery>();

    m_cmdRequests.push_back(std::make_shared<ControlReuest>(this, m_devControl.get()));
    m_cmdRequests.push_back(std::make_shared<QueryRequest>(this, m_devQuery.get(), m_devRecordQuery.get()));
    // m_cmdRequests.push_back(std::make_shared<NotifyRequest>(this, m_devStatus.get()));
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

const std::unordered_map<std::string, int32_t>& MANSCDPAgent::getChannels() const
{
    return m_ua->m_channels;
}

const char* MANSCDPAgent::getDeviceID() const
{
    return m_ua->m_sip->getUserId();
}

bool MANSCDPAgent::makeKeepaliveNotify()
{
    KeepaliveNotify notify(this, m_devStatus.get());
    return notify.handle();
}

bool MANSCDPAgent::sendResponseCmd(const XMLDocument& xmldocRes) const
{
    const std::shared_ptr<SipUserAgent>& sip = m_ua->getSip();
    return sip->sendMANSCDPResponse(xmldocRes);
}

bool MANSCDPAgent::sendKeepaliveNotify(const XMLDocument& notify) const
{
    const std::shared_ptr<SipUserAgent>& sip = m_ua->getSip();
    return sip->sendKeepaliveNotify(notify);
}

bool MANSCDPAgent::sendMediaStatusNotify(const SessionIdentifier& id, const XMLDocument& notify) const
{
    const std::shared_ptr<SipUserAgent>& sip = m_ua->getSip();
    return sip->sendSessionNotify(id, notify);
}