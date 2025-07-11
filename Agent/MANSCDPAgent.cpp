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

bool MANSCDPAgent::agent(const SipMessageApp& message)
{
    /* 收到命令后返回 200 OK */
    sendResponse200(message);

    XMLDocument doc;
    if (doc.Parse(message.getMANSCDPContents()) != XML_SUCCESS)
    {
        return false;
    }

    std::string rootName = doc.RootElement()->Name();
    for (auto i : requests)
    {
        if (i->match(rootName))
        {
            return i->dispatch(doc.FirstChildElement());
        }
    }

    return false;
}

bool MANSCDPAgent::agent(const SipMessageApp& res, const SipMessageApp& req)
{
    XMLDocument doc;
    if (doc.Parse(req.getMANSCDPContents()) != XML_SUCCESS)
    {
        return false;
    }

    std::string rootName = doc.RootElement()->Name();
    for (auto i : requests)
    {
        if (i->match(rootName))
        {
            return i->dispatch(doc.FirstChildElement(), res.getCode());
        }
    }

    return false;
}

int32_t MANSCDPAgent::getKeepaliveTimeoutCount() const
{
    return status->getTimeoutCount();
}

bool MANSCDPAgent::sendResponse200(const SipMessageApp& req) const
{
    SipMessageApp message;
    const std::shared_ptr<SipUserAgent>& sip = m_ua->getSip();
    sip->makeResMessage(message, req, 200, "OK");

    return sip->send(message);
}

bool MANSCDPAgent::sendResponseCmd(const XMLDocument& xmldocRes) const
{
    SipMessageApp message;
    const std::shared_ptr<SipUserAgent>& sip = m_ua->getSip();
    sip->makeReqMessage(message, "MESSAGE");
    // message.setContentType("Application", "MANSCDP+xml");

    XMLPrinter printer;
    xmldocRes.Print(&printer);
    message.setMANSCDPContents(printer.CStr());

    return sip->send(message);
}

bool MANSCDPAgent::sendRequest(const XMLDocument& xmldocReq) const
{
    SipMessageApp message;
    const std::shared_ptr<SipUserAgent>& sip = m_ua->getSip();
    sip->makeReqMessage(message, "MESSAGE");
    // message.setContentType("Application", "MANSCDP+xml");

    XMLPrinter printer;
    xmldocReq.Print(&printer);
    printf(">>>>>> %s:%d\n%s\n", __FILE__, __LINE__, printer.CStr());

    message.setMANSCDPContents(printer.CStr());

    // printf(">>>>>> %s:%d\n", __FILE__, __LINE__);
    // message.print();
    // return false;

    return sip->send(message);
}

bool MANSCDPAgent::sendKeepalive(const KeepAliveNotify::Request *notify) const
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
    XMLPrinter printer;
    xmldocReq.Print(&printer);
    printf(">>>>>> %s:%d\n%s\n", __FILE__, __LINE__, printer.CStr());

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