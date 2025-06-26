#include <iostream>
#include <unistd.h>
#include "UA.h"
#include "MANSCDPAgent.h"
#include "RegistrationAgent.h"
#include "MediaAgent.h"
#include "DevControl.h"
#include "DevRegistration.h"

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

bool UA::start(const Info& info)
{
    /* 创建register代理 */
    RegistrationAgent *regAgent = new RegistrationAgent(this);
    if (regAgent != nullptr)
    {
        regAgent->registration = new DevRegistration(); // 创建register设备处理对象
        agents.push_back(regAgent);
    }

    /* 创建MANSCDP协议代理 */
    MANSCDPAgent *cdpAgent = new MANSCDPAgent(this);
    if (cdpAgent != nullptr)
    {
        cdpAgent->control = new DevControl(); // 创建MANSCDP设备处理对象
        agents.push_back(cdpAgent);
    }

    /* 创建媒体代理 */
    MediaAgent *mediaAgent = new MediaAgent(this);
    if (mediaAgent != nullptr)
    {
        agents.push_back(mediaAgent);
    }

    /* 创建sip用户代理 */
    sip = SipUserAgent::create(this, info.sipInfo);
    if (sip == nullptr)
    {
        return false;
    }

    /* 初始化sip用户代理 */
    sip->init();

    /* 发起注册 */
    regAgent->start();

    return true;
}

bool UA::stop()
{
    for (auto agent : agents)
    {
        delete agent;
    }

    return true;
}
