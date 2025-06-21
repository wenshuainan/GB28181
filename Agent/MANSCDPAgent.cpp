#include "MANSCDPAgent.h"
#include "tinyxml2.h"
#include "A.2.6Response.h"

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

bool MANSCDPAgent::match(std::string& methodType, std::string& contentType)
{
    return contentType == "Application/MANSCDP+Xml";
}

bool MANSCDPAgent::agent(std::string& content)
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

bool MANSCDPAgent::agentControl(const PTZCmdRequest::Request& req)
{
    if (control)
    {
        DeviceControlResponse::Response res;
        control->process(req, res);
        XMLDocument doc;
        DeviceControlResponse::serialize(res, &doc);
        doc.Print();
        // return ua->send(xml);
    }

    return false;
}

bool MANSCDPAgent::agentControl(const TeleBootRequest::Request& req)
{
    return false;
}