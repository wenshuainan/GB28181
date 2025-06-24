#include "MANSCDPAgent.h"
#include "tinyxml2.h"
#include "A.2.6Response.h"
#include "UA.h"

MANSCDPAgent::MANSCDPAgent(UA *ua) : Agent(ua)
{
    control = nullptr;

    requests.push_back(new ControlReuest(this));
}

MANSCDPAgent::~MANSCDPAgent()
{
    for (auto i : requests)
    {
        delete i;
    }
}

bool MANSCDPAgent::start()
{
    return true;
}

bool MANSCDPAgent::stop()
{
    return true;
}

bool MANSCDPAgent::match(const std::string& method, const std::string& contentType)
{
    return contentType == "Application/MANSCDP+xml";
}

bool MANSCDPAgent::match(const std::string& callID)
{
    return m_callID == callID;
}

bool MANSCDPAgent::agent(const SipGenericMessage& message)
{
    auto type = message.getType();

    if (type == SipGenericMessage::Type::Request)
    {
        XMLDocument doc;
        cacheMessage = std::make_shared<const SipGenericMessage>(message);
    
        if (doc.Parse(message.getBody().c_str()) != XML_SUCCESS)
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

bool MANSCDPAgent::sendResponse(int code, const XMLDocument& doc)
{
    SipUserAgent *sipUA = m_ua->getSipUA();
    SipGenericMessage message;
    sipUA->genResMessage(message, *cacheMessage, code, "OK");
    message.addField("Content-Type", "Application/MANSCDP+xml");

    XMLPrinter printer;
    doc.Print(&printer);
    message.setBody(printer.CStr());

    return sipUA->send(message);
}

bool MANSCDPAgent::agentControl(const PTZCmdRequest::Request& req)
{
    if (control)
    {
        DeviceControlResponse::Response res;
        control->process(req, res);

        XMLDocument doc;
        DeviceControlResponse::serialize(res, &doc);

        return sendResponse(200, doc);
    }

    return false;
}

bool MANSCDPAgent::agentControl(const TeleBootRequest::Request& req)
{
    return false;
}