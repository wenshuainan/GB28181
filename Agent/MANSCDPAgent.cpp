#include <iostream>
#include "MANSCDPAgent.h"
#include "tinyxml2.h"
#include "UA.h"
#include "MANSCDP/A.2.5Notify.h"
#include "MANSCDP/A.2.6Response.h"
#include "DevControl.h"
#include "DevQuery.h"
#include "DevStatus.h"

MANSCDPAgent::MANSCDPAgent(UA *ua) : Agent(ua)
{
    m_devControl = std::make_shared<DevControl>();
    m_devQuery = std::make_shared<DevQuery>();
    m_devStatus = std::make_shared<DevStatus>();

    m_cmdRequests.push_back(std::make_shared<ControlReuest>(this, m_devControl.get()));
    m_cmdRequests.push_back(std::make_shared<QueryRequest>(this, m_devQuery.get()));
    m_cmdRequests.push_back(std::make_shared<NotifyRequest>(this, m_devStatus.get()));
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

bool MANSCDPAgent::sendResponseCmd(const XMLDocument& xmldocRes) const
{
    const std::shared_ptr<SipUserAgent>& sip = m_ua->getSip();
    return sip->sendMANSCDPResponse(xmldocRes);
}

bool MANSCDPAgent::sendKeepaliveNotify(const KeepAliveNotify::Notify *notify) const
{
    KeepAliveNotify::Notify keepalive;

    if (notify == nullptr)
    {
        m_devStatus->getStatus(keepalive);
    }
    else
    {
        keepalive = *notify;
    }

    XMLDocument xmldocNotify;
    KeepAliveNotify::encode(keepalive, &xmldocNotify);

    const std::shared_ptr<SipUserAgent>& sip = m_ua->getSip();
    if (sip->sendKeepaliveNotify(xmldocNotify))
    {
        m_devStatus->addSentCount();
        return true;
    }
    else
    {
        return false;
    }
}

bool MANSCDPAgent::sendMediaStatusNotify(const SessionIdentifier& id, const MediaStatusNotify::Notify& notify) const
{
    XMLDocument xmldocNotify;
    MediaStatusNotify::encode(notify, &xmldocNotify);

    const std::shared_ptr<SipUserAgent>& sip = m_ua->getSip();
    return sip->sendSessionNotify(id, xmldocNotify);
}