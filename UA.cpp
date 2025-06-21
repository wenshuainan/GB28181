#include <thread>
#include <unistd.h>
#include "UA.h"
#include "MANSCDPAgent.h"
#include "RegistrationAgent.h"
#include "MediaAgent.h"
#include "DevControl.h"

UA::UA()
{
    RegistrationAgent *registrationAgent = new RegistrationAgent(this);
    MANSCDPAgent *manscdpAgent = new MANSCDPAgent(this);
    MediaAgent *mediaAgent = new MediaAgent(this);

    if (manscdpAgent != nullptr)
    {
        manscdpAgent->control = new DevControl();
    }

    agents.push_back(registrationAgent);
    agents.push_back(manscdpAgent);
    agents.push_back(mediaAgent);
}

UA::~UA()
{
    for (auto agent : agents)
    {
        delete agent;
    }
}

void UA::threadProc()
{
    adapter = SIPAdapter::create();

    while (bThreadRun)
    {
        Header header;
        std::string body;
        if (adapter->recv(header, body))
        {
            const std::string methodType = header.getRequestLine().getMethod();
            const std::string contentType = header.getField("Content-type").getValue();

            for (auto agent : agents)
            {
                if (agent->match(methodType, contentType))
                {
                    agent->agent(body);
                }
            }
        }

        sleep(30);
    }
}

bool UA::start()
{
    bThreadRun = true;
    std::thread t(&UA::threadProc, this);
    t.detach();
    return true;
}

bool UA::stop()
{
    bThreadRun = false;
    return true;
}

bool UA::sendRequest(const std::string& methodType, const std::string& contentType, const std::string& content)
{
    Header header;
    header.setRequestLine(methodType, "sip:127.0.0.1:5060");
    header.addField("Content-type", contentType);
    header.addField("Content-length", std::to_string(content.length()));

    return adapter->send(header, content);
}

bool UA::sendResponse(int code, const std::string& contentType, const std::string& content)
{
    Header header;
    header.setStatusLine(code, "sip:127.0.0.1:5060");
    header.addField("From", "sip:1000@192.168.1.101");
    header.addField("To", "sip:1000@192.168.1.101");
    header.addField("Call-ID", "2000");
    header.addField("CSeq", "2 INVITE");
    header.addField("Contact", "sip:1000@192.168.1.101");
    header.addField("Content-type", contentType);
    header.addField("Content-length", std::to_string(content.length()));

    return adapter->send(header, content);
}