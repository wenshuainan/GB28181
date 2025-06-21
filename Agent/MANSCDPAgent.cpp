#include "MANSCDPAgent.h"
#include "tinyxml2.h"
#include "A.2.6Response.h"
#include "UA.h"

MANSCDPAgent::MANSCDPAgent(UA *ua) : Agent(ua)
{
    m_methodType = "MESSAGE";
    m_contentType = "Application/MANSCDP+Xml";

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

bool MANSCDPAgent::match(const std::string& methodType, const std::string& contentType)
{
    return contentType == "Application/MANSCDP+xml";
}

bool MANSCDPAgent::agent(const std::string& content)
{
    XMLDocument doc;
    
    if (doc.Parse(content.c_str()) != XML_SUCCESS)
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

bool MANSCDPAgent::send(int code, const XMLDocument &doc)
{
    XMLPrinter printer;
    doc.Print(&printer);

    return m_ua->sendResponse(code, m_contentType, printer.CStr());
}

bool MANSCDPAgent::agentControl(const PTZCmdRequest::Request& req)
{
    if (control)
    {
        DeviceControlResponse::Response res;
        control->process(req, res);

        XMLDocument doc;
        DeviceControlResponse::serialize(res, &doc);
        doc.Print();

        return send(200, doc);
    }

    return false;
}

bool MANSCDPAgent::agentControl(const TeleBootRequest::Request& req)
{
    return false;
}