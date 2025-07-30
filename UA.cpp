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
    int32_t kaTick = 0x0FFFFFFF; //保活计数器，判断保活间隔
                                 //使用最大值初始化，上线后立即发送
    int32_t tickInterval = 1000000; //单位：微秒

    while (m_bStarted)
    {
        if (m_bOnline)
        {
            auto timeoutCount = m_cdpAgent->getKeepaliveTimeoutCount();
            if (timeoutCount >= m_kaInfo.timeoutCount)
            {
                m_bOnline = false;
                m_registAgent->reset();
                continue;
            }

            if (kaTick + 3 > m_kaInfo.interval)
            {
                kaTick = 0;
                m_cdpAgent->makeKeepaliveNotify();
            }

            kaTick++;
        }
        else
        {
            kaTick = 0x0FFFFFFF;

            /* 注册 */
            if (m_registAgent != nullptr)
            {
                m_registAgent->start();
            }
        }

        usleep(tickInterval);
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
    if (m_bOnline)
    {
        m_cdpAgent->clearKeepaliveTimeoutCount();
    }
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

    m_kaInfo = keepalive;

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