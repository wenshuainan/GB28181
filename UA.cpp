#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include "UA.h"
#include "Agent/Agent.h"
#include "Agent/RegistrationAgent.h"
#include "Agent/MANSCDPAgent.h"
#include "Agent/SessionAgent.h"
#include "Agent/MANSRTSPAgent.h"

UA::UA()
{
    m_bStarted = false;
    m_bOnline = false;
}

UA::~UA()
{
    stop();
}

void UA::proc()
{
    int32_t katick = 0x0FFFFFFF;
    int32_t tickinterval = 1000000;

    while (m_bStarted)
    {
        if (m_bOnline)
        {
            auto timeoutCount = m_cdpAgent->getKeepaliveTimeoutCount();
            if (timeoutCount > m_kaInfo.timeoutCount)
            {
                m_bOnline = false;
                m_registAgent->stop();
                continue;
            }

            if (katick + 3 > m_kaInfo.interval)
            {
                katick = 0;
                m_cdpAgent->sendKeepaliveNotify();
            }

            katick++;
        }
        else
        {
            katick = 0x0FFFFFFF;

            /* 注册 */
            if (m_registAgent != nullptr)
            {
                m_registAgent->start();
            }
        }

        usleep(tickinterval);
    }

    /* 注销 */
    if (m_registAgent != nullptr)
    {
        m_registAgent->stop();
    }
}

bool UA::dispatchRegistrationResponse(const SipUserMessage& res)
{
    return m_registAgent->agent(res);
}

bool UA::dispatchSessionRequest(const SessionIdentifier& id, const SipUserMessage& req)
{
    return m_sessionAgent->agent(id, req);
}

bool UA::dispatchMANSCDPRequest(const XMLDocument &req)
{
    return m_cdpAgent->agent(req);
}

bool UA::dispatchKeepaliveResponse(int32_t code)
{
    return m_cdpAgent->recvedKeepaliveResponse(code);
}

bool UA::dispatchMANSRTSPRequest(const MANSRTSP::Message& req)
{
    return m_rtspAgent->dispatchRequest(req);
}

void UA::setStatus(bool online)
{
    m_bOnline = online;
}

bool UA::start(const SipUserAgent::ClientInfo& client,
                const SipUserAgent::ServerInfo& server,
                const KeepaliveInfo &keepalive
            )
{
    if (m_bStarted)
    {
        return false;
    }
    
    /* 创建注册协议代理 */
    m_registAgent = std::make_shared<RegistrationAgent>(this);
    m_agents.push_back(m_registAgent);
    /* 创建MANSCDP协议代理 */
    m_cdpAgent = std::make_shared<MANSCDPAgent>(this);
    m_agents.push_back(m_cdpAgent);
    /* 创建INVITE会话代理 */
    m_sessionAgent = std::make_shared<SessionAgent>(this);
    m_agents.push_back(m_sessionAgent);
    /* 创建MANSRTSP协议代理 */
    m_rtspAgent = std::make_shared<MANSRTSPAgent>(this);
    m_agents.push_back(m_rtspAgent);

    /* 创建sip用户代理 */
    m_sip = SipUserAgent::create(this, client, server);
    if (m_sip == nullptr)
    {
        m_agents.clear();
        return m_bStarted = false;
    }

    m_kaInfo = keepalive;

    /* 初始化sip用户代理 */
    if (m_sip->init() == false)
    {
        m_sip->destroy();
        m_agents.clear();
        return m_bStarted = false;
    }

    /* 创建状态维护线程 */
    m_bStarted = true;
    m_thread = std::make_shared<std::thread>(&UA::proc, this);
    if (m_thread == nullptr)
    {
        m_sip->destroy();
        m_agents.clear();
        return m_bStarted = false;
    }
    else
    {
        return true;
    }
}

bool UA::stop()
{
    if (!m_bStarted)
    {
        return false;
    }

    m_bStarted = false;
    m_thread->join();

    m_agents.clear();
    m_sip->destroy();
    m_bOnline = false;

    return true;
}

bool UA::getStatus() const
{
    return m_bOnline;
}

bool UA::updateStatus(const KeepAliveNotify::Notify &notify)
{
    return m_cdpAgent->sendKeepaliveNotify(&notify);
}