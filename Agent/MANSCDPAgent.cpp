#include "MANSCDPAgent.h"
#include "tinyxml2.h"
#include "UA.h"
#include "A.2.6Response.h"
#include "DevControl.h"
#include "DevQuery.h"

MANSCDPAgent::MANSCDPAgent(UA *ua) : Agent(ua)
{
    control = std::make_shared<DevControl>();
    query = std::make_shared<DevQuery>();

    requests.push_back(std::make_shared<ControlReuest>(this, control.get()));
    requests.push_back(std::make_shared<QueryRequest>(this, query.get()));
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
        XMLDocument doc;
        lastReqMessage = std::make_shared<const SipMessageApp>(message);
    
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

bool MANSCDPAgent::sendResponse(const XMLDocument& xmldocRes) const
{
    SipMessageApp message;
    const std::shared_ptr<SipUserAgent>& sip = m_ua->getSip();
    sip->genResMessage(message, *lastReqMessage, 200, "OK");
    message.addField("Content-Type", "Application/MANSCDP+xml");

    XMLPrinter printer;
    xmldocRes.Print(&printer);
    message.setBody(printer.CStr());

    return sip->send(message);
}
