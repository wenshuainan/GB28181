#include "MANSCDPAgent.h"
#include "tinyxml2.h"
#include "UA.h"
#include "A.2.6Response.h"
#include "DevControl.h"
#include "DevQuery.h"
#include "A.2.5Notify.h"

MANSCDPAgent::MANSCDPAgent(UA *ua) : Agent(ua)
{
    control = std::make_shared<DevControl>();
    query = std::make_shared<DevQuery>();

    requests.push_back(std::make_shared<ControlReuest>(this, control.get()));
    requests.push_back(std::make_shared<QueryRequest>(this, query.get()));
    requests.push_back(std::make_shared<NotifyRequest>(this));
}

MANSCDPAgent::~MANSCDPAgent()
{
    requests.clear();
}

bool MANSCDPAgent::match(const std::string& method, const std::string& contentType)
{
    return contentType == "Application/MANSCDP+xml";
}

bool MANSCDPAgent::match(const std::string& callID)
{
    return outCallID == callID;
}

bool MANSCDPAgent::agent(const SipMessageApp& message)
{
    auto type = message.getType();

    if (type == SipMessageApp::Type::Request)
    {
        /* 收到命令后返回 200 OK */
        sendResponse200(message);

        XMLDocument doc;
        if (doc.Parse(message.getBody()) != XML_SUCCESS)
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
    }
    else
    {
    }

    return false;
}

bool MANSCDPAgent::agent(const XMLDocument& xmldocReq)
{
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

bool MANSCDPAgent::sendResponse200(const SipMessageApp& req) const
{
    SipMessageApp message;
    const std::shared_ptr<SipUserAgent>& sip = m_ua->getSip();
    sip->genResMessage(message, req, 200, "OK");

    return sip->send(message);
}

bool MANSCDPAgent::sendResponseCmd(const XMLDocument& xmldocRes) const
{
    SipMessageApp message;
    const std::shared_ptr<SipUserAgent>& sip = m_ua->getSip();
    sip->genReqMessage(message, "MESSAGE");
    message.addField("Content-Type", "Application/MANSCDP+xml");

    XMLPrinter printer;
    xmldocRes.Print(&printer);
    message.setBody(printer.CStr());

    return sip->send(message);
}

bool MANSCDPAgent::sendRequest(const XMLDocument& xmldocReq) const
{
    SipMessageApp message;
    const std::shared_ptr<SipUserAgent>& sip = m_ua->getSip();
    sip->genReqMessage(message, "MESSAGE");
    message.addField("Content-Type", "Application/MANSCDP+xml");

    XMLPrinter printer;
    xmldocReq.Print(&printer);
    message.setBody(printer.CStr());

    return sip->send(message);
}