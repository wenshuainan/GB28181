#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include "UA.h"
#include "Agent/Agent.h"
#include "Agent/RegistrationAgent.h"
#include "Agent/MANSCDPAgent.h"
#include "Agent/SessionAgent.h"
#include "Agent/MANSRTSPAgent.h"
#include "MANSCDP/A.2.5Notify.h"
#include "MANSRTSP/B.1Message.h"

UA::UA()
    : m_bStarted(false), m_bOnline(false)
{}

UA::~UA()
{
    stop();
}

void UA::threadProc()
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
                m_cdpAgent->makeKeepaliveNotify();
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

bool UA::dispatchKeepaliveResponse(int32_t code)
{
    return m_cdpAgent->recvedKeepaliveResponse(code);
}

bool UA::dispatchMANSCDPRequest(const XMLDocument &req)
{
    return m_cdpAgent->agent(req);
}

bool UA::dispatchSessionRequest(const SessionIdentifier& id, const SipUserMessage& req)
{
    int32_t ch = getChNum(req.getRequestUser());
    if (ch < 0)
    {
        return false;
    }

    return m_sessionAgent[ch]->agent(id, req);
}

bool UA::dispatchMANSRTSPRequest(const SipUserMessage& req)
{
    int32_t ch = getChNum(req.getRequestUser());
    if (ch < 0)
    {
        return false;
    }
    
    const MANSRTSP::Message* message = req.getMANSRTSP();
    if (message != nullptr)
    {
        return m_rtspAgent->agent(*(m_sessionAgent[ch]), *message);
    }
    else
    {
        return false;
    }
}

void UA::setOnline(bool online)
{
    m_bOnline = online;
}

int32_t UA::getChNum(const std::string& id) const
{
    auto it = m_channels.find(id);
    if (it != m_channels.end())
    {
        return it->second;
    }
    else
    {
        return -1;
    }
}

bool UA::start(const SipUserAgent::ClientInfo& client,
    const SipUserAgent::ServerInfo& server,
    const KeepaliveInfo &keepalive,
    const std::vector<std::string>& catalogIds
)
{
    if (m_bStarted)
    {
        return false;
    }

    auto size = catalogIds.size();
    if (size == 0)
    {
        return false;
    }
    /* 将目录Id与通道号关联并创建每个通道的INVITE会话代理 */
    int i;
    for (i = 0; (std::size_t)i < size; i++)
    {
        m_channels[catalogIds[i]] = i;
        std::shared_ptr<SessionAgent> sessionAgent = std::make_shared<SessionAgent>(this, i);
        m_sessionAgent.push_back(sessionAgent);
    }
    
    /* 创建注册协议代理 */
    m_registAgent = std::make_shared<RegistrationAgent>(this);
    /* 创建MANSCDP协议代理 */
    m_cdpAgent = std::make_shared<MANSCDPAgent>(this);
    /* 创建MANSRTSP协议代理 */
    m_rtspAgent = std::make_shared<MANSRTSPAgent>(this);

    /* 创建sip用户代理 */
    m_sip = SipUserAgent::create(this, client, server);
    if (m_sip == nullptr)
    {
        return m_bStarted = false;
    }

    m_kaInfo = keepalive;

    /* 创建状态维护线程 */
    m_bStarted = true;
    m_thread = std::make_shared<std::thread>(&UA::threadProc, this);
    if (m_thread == nullptr)
    {
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

    m_sip = nullptr;
    m_sessionAgent.clear();
    m_registAgent = nullptr;
    m_cdpAgent = nullptr;
    m_rtspAgent = nullptr;
    m_bOnline = false;

    return true;
}

bool UA::getOnline() const
{
    return m_bOnline;
}

bool UA::sendStatusNotify()
{
    return m_cdpAgent->makeKeepaliveNotify();
}

Alarm* UA::getAlarm()
{
    return m_cdpAgent->m_devAlarm.get();
}