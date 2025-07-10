#include <iostream>
#include <unistd.h>
#include "UA.h"
#include "RegistrationAgent.h"
#include "MANSCDPAgent.h"
#include "MediaAgent.h"

UA::UA()
{}

UA::~UA()
{}

bool UA::postRecved(const SipMessageApp& message)
{
    auto type = message.getType();

    if (type == SipMessageApp::Request)
    {
        const std::string method = message.getMethod();
        const std::string contentType = message.getContentType();
        DEBUG_LOG << "Method: " << method << " Content-Type: " << contentType << std::endl;

        for (auto agent : agents)
        {
            if (agent->match(method, contentType))
            {
                return agent->agent(message);
            }
        }

        DEBUG_LOG << "Dismatched request body: " << std::endl << "===" << message.getBody() << "===" << std::endl;
    }
    else if (type == SipMessageApp::Response)
    {
        const std::string callID = message.getCallID();
        DEBUG_LOG << "Call-ID: " << callID << std::endl;

        for (auto agent : agents)
        {
            if (agent->match(callID))
            {
                return agent->agent(message);
            }
        }

        DEBUG_LOG << "Dismatched response" << std::endl;
    }
    else
    {
        DEBUG_LOG << "Unknown message type" << std::endl;
    }

    return false;
}

bool UA::start(const SipUserAgent::ClientInfo& client, const SipUserAgent::ServerInfo& server)
{
    /* 创建注册协议代理 */
    agents.push_back(std::make_shared<RegistrationAgent>(this));
    /* 创建MANSCDP协议代理 */
    agents.push_back(std::make_shared<MANSCDPAgent>(this));
    /* 创建媒体协议代理 */
    agents.push_back(std::make_shared<MediaAgent>(this));

    /* 创建sip用户代理 */
    sip = SipUserAgent::create(this, client, server);
    if (sip == nullptr)
    {
        agents.clear();
        return false;
    }

    /* 初始化sip用户代理 */
    if (sip->init() == false)
    {
        sip->destroy();
        agents.clear();
        return false;
    }

    /* 发起注册 */
    const std::shared_ptr<RegistrationAgent> regAgent = std::dynamic_pointer_cast<RegistrationAgent>(agents[0]);
    if (regAgent != nullptr)
    {
        return regAgent->start();
    }
    else
    {
        return false;
    }
}

bool UA::stop()
{
    agents.clear();
    sip->destroy();
    return true;
}

void UA::keepaliveProc()
{
    while (bKeepaliveRunning)
    {
        KeepAliveNotify::Request req;
        statusProcess->getStatus(req);
        XMLDocument xmldocReq;
        KeepAliveNotify::serialize(req, xmldocReq);
        const std::shared_ptr<MANSCDPAgent> agent = std::dynamic_pointer_cast<MANSCDPAgent>(agents[1]);
        agent->agent(xmldocReq);
    }
}

bool UA::startKeepalive()
{
    bKeepaliveRunning = true;
    keepaliveThread = new std::thread(&UA::keepaliveProc, this);
    return true;
}

bool UA::stopKeepalive()
{
    bKeepaliveRunning = false;
    if (keepaliveThread != nullptr)
    {
        if (keepaliveThread->joinable())
        {
            keepaliveThread->join();
        }
        delete keepaliveThread;
    }
    return true;
}

bool UA::updateStatus(const KeepAliveNotify &notify)
{
    XMLDocument xmldocReq;
    KeepAliveNotify::serialize(req, xmldocReq);
    const std::shared_ptr<MANSCDPAgent> agent = std::dynamic_pointer_cast<MANSCDPAgent>(agents[1]);
    agent->agent(xmldocReq);
}