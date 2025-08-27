#include "MANSCDPAgent.h"
#include "tinyxml2/tinyxml2.h"
#include "UA.h"
#include "MANSCDP/A.2.3Control.h"
#include "MANSCDP/A.2.4Query.h"

MANSCDPAgent::MANSCDPAgent(UA *ua) : Agent(ua)
{
    printf("++++++ MANSCDPAgent\n");
    m_cmdRequests.push_back(std::move(std::unique_ptr<ControlReuest>(new ControlReuest(this))));
    m_cmdRequests.push_back(std::move(std::unique_ptr<QueryRequest>(new QueryRequest(this))));
}

MANSCDPAgent::~MANSCDPAgent()
{
    printf("----- MANSCDPAgent\n");
}

bool MANSCDPAgent::match(const std::string& method, const std::string& contentType)
{
    return strCaseCmp(method, "MESSAGE")
            && strCaseCmp(contentType, "Application/MANSCDP+xml");
}

bool MANSCDPAgent::agent(const SipUserMessage& message)
{
    return false;
}

bool MANSCDPAgent::agent(const XMLDocument &cmdReq)
{
    const XMLElement *rootElement = cmdReq.RootElement();
    if (!rootElement)
    {
        return false;
    }
    
    std::string rootName = rootElement->Name();
    printf("MANSCDP request: %s\n", rootName.c_str());
    for (auto& i : m_cmdRequests)
    {
        if (i->match(rootName))
        {
            return i->dispatch(rootElement);
        }
    }

    printf("not match\n");
    return false;
}

bool MANSCDPAgent::agent(int32_t code, const XMLDocument& cmd)
{
    const XMLElement *rootElement = cmd.RootElement();
    if (!rootElement)
    {
        return false;
    }

    printf("MANSCDP result: <%s>\n", rootElement->Name());
    for (auto i = m_handlers.begin(); i != m_handlers.end(); ++i)
    {
        if ((*i)->match(rootElement))
        {
            printf("matched %s\n", rootElement->FirstChildElement("CmdType")->GetText());
            (*i)->handle(code);
            m_handlers.erase(i);
            return true;
        }
    }

    printf("not match\n");
    return false;
}

bool MANSCDPAgent::sendCmd(const XMLDocument& cmd, std::shared_ptr<MessageResultHandler> handler)
{
    auto sip = m_ua->getSip();
    if (!sip)
    {
        return false;
    }

    if (sip->sendMANSCDP(cmd))
    {
        if (handler)
        {
            printf("add cmd message handler:<%s>\n", cmd.RootElement()->FirstChildElement("CmdType")->GetText());
            m_handlers.push_back(handler);
        }
        return true;
    }
    return false;
}

MANSCDPDevice* MANSCDPAgent::getDevice(const std::string& id)
{
    return dynamic_cast<MANSCDPDevice*>(m_ua->getDevice(id));
}