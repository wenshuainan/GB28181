#include "MANSCDPAgent.h"
#include "tinyxml2.h"
#include "UA.h"
#include "A.2.5Notify.h"
#include "A.2.6Response.h"
#include "DevControl.h"
#include "DevQuery.h"
#include "DevStatus.h"

MANSCDPAgent::MANSCDPAgent(UA *ua) : Agent(ua)
{
    control = std::make_shared<DevControl>();
    query = std::make_shared<DevQuery>();
    status = std::make_shared<DevStatus>();

    requests.push_back(std::make_shared<ControlReuest>(this, control.get()));
    requests.push_back(std::make_shared<QueryRequest>(this, query.get()));
    requests.push_back(std::make_shared<NotifyRequest>(this, status.get()));
}

MANSCDPAgent::~MANSCDPAgent()
{
    requests.clear();
}

bool MANSCDPAgent::match(const std::string& method, const std::string& contentType)
{
    return contentType == "Application/MANSCDP+xml";
}

bool MANSCDPAgent::agent(const SipUserMessage& message)
{
    return false;
}

bool MANSCDPAgent::agent(const XMLDocument &xmldocReq) const
{
    std::string rootName = xmldocReq.RootElement()->Name();
    for (auto i : requests)
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
    return status->getTimeoutCount();
}

bool MANSCDPAgent::recvedKeepaliveResponse(int32_t code) const
{
    status->addRecvedCount();
    return true;
}

bool MANSCDPAgent::sendResponseCmd(const XMLDocument& xmldocRes) const
{
    const std::shared_ptr<SipUserAgent>& sip = m_ua->getSip();
    return sip->sendMANSCDPResponse(xmldocRes);
}

bool MANSCDPAgent::sendKeepaliveRequest(const KeepAliveNotify::Request *notify) const
{
    KeepAliveNotify::Request req;

    if (notify == nullptr)
    {
        status->getStatus(req);
    }
    else
    {
        req = *notify;
    }

    XMLDocument xmldocReq;
    KeepAliveNotify::serialize(req, &xmldocReq);

    const std::shared_ptr<SipUserAgent>& sip = m_ua->getSip();
    return sip->sendKeepaliveRequest(xmldocReq);
}