#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include "UA.h"

UA::UA()
{
    m_bOnline = false;
}

UA::~UA()
{}

void UA::keepaliveProc()
{
    int32_t tick = 0x0FFFFFFF;

    while (m_bKeepaliveRunning)
    {
        auto timeoutCount = m_mansAgent->getKeepaliveTimeoutCount();
        if (timeoutCount > m_keepaliveInfo.timeoutCount)
        {
            // offline
        }

        if (tick - 3 > m_keepaliveInfo.interval)
        {
            tick = 0;
            m_mansAgent->sendKeepaliveRequest();
        }

        usleep(1000000);
        tick++;
    }
}

bool UA::dispatchRegistrationResponse(const SipUserMessage& res)
{
    return m_registAgent->agent(res);
}

bool UA::dispatchSessionRequest(const SipUserMessage& req)
{
    return m_sessionAgent->agent(req);
}

bool UA::dispatchMANSCDPRequest(const XMLDocument &req)
{
    return m_mansAgent->agent(req);
}

bool UA::dispatchKeepaliveResponse(int32_t code)
{
    return m_mansAgent->recvedKeepaliveResponse(code);
}

void UA::setOnline()
{
    m_bOnline = true;
    startKeepalive();
}

bool UA::startKeepalive()
{
    m_bKeepaliveRunning = true;
    m_keepaliveThread = new std::thread(&UA::keepaliveProc, this);
    return true;
}

bool UA::stopKeepalive()
{
    m_bKeepaliveRunning = false;
    if (m_keepaliveThread != nullptr)
    {
        if (m_keepaliveThread->joinable())
        {
            m_keepaliveThread->join();
        }
        delete m_keepaliveThread;
        m_keepaliveThread = nullptr;
    }
    return true;
}

bool UA::start(const SipUserAgent::ClientInfo& client, const SipUserAgent::ServerInfo& server, const KeepaliveInfo &keepalive)
{
    m_keepaliveInfo = keepalive;
    
    /* 创建注册协议代理 */
    m_registAgent = std::make_shared<RegistrationAgent>(this);
    m_agents.push_back(m_registAgent);
    /* 创建MANSCDP协议代理 */
    m_mansAgent = std::make_shared<MANSCDPAgent>(this);
    m_agents.push_back(m_mansAgent);
    /* 创建媒体协议代理 */
    m_sessionAgent = std::make_shared<SessionAgent>(this);
    m_agents.push_back(m_sessionAgent);

    /* 创建sip用户代理 */
    m_sip = SipUserAgent::create(this, client, server);
    if (m_sip == nullptr)
    {
        m_agents.clear();
        return false;
    }

    /* 初始化sip用户代理 */
    if (m_sip->init() == false)
    {
        m_sip->destroy();
        m_agents.clear();
        return false;
    }

    /* 发起注册 */
    if (m_registAgent != nullptr)
    {
        return m_registAgent->start();
    }
    else
    {
        return false;
    }
}

bool UA::stop()
{
    startKeepalive();
    m_agents.clear();
    m_sip->destroy();
    m_bOnline = false;
    return true;
}

bool UA::updateStatus(const KeepAliveNotify::Request &notify)
{
    return m_mansAgent->sendKeepaliveRequest(&notify);
}