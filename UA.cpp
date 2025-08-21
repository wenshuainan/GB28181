#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include "UA.h"
#include "Agent/Agent.h"
#include "Agent/RegistrationAgent.h"
#include "Agent/MANSCDPAgent.h"
#include "Agent/SessionAgent.h"
#include "Agent/MANSRTSPAgent.h"
#include "MANSCDP/A.2.5Notify.h"
#include "MANSRTSP/B.1Message.h"
#include "DevStatus.h"

UA::UA()
    : m_bStarted(false)
    , m_bOnline(false)
{}

UA::~UA()
{
    stop();
}

void UA::threadProc()
{
    std::unique_ptr<DevStatus> devStatus; // 状态信息报送
    struct timespec keepaliveTime = {0, 0}; // 保活信息发送时间

    while (m_bStarted)
    {
        /* 如果提前创建Agent，离线后需要清理许多状态。所以在线后重新创建新的，离线时直接销毁 */
        if (m_bOnline)
        {
            /* 创建MANSCDP协议代理 */
            if (!m_cdpAgent)
            {
                if (!(m_cdpAgent = std::move(std::unique_ptr<MANSCDPAgent>(new MANSCDPAgent(this)))))
                {
                    printf("MANSCDPAgent create failed\n");
                    continue;
                }
            }
            /* 创建每个通道的INVITE会话代理 */
            if (m_sessionAgent.empty())
            {
                for (auto& i : m_channels)
                {
                    std::unique_ptr<SessionAgent> sessionAgent(new SessionAgent(this, i.second));
                    if (!sessionAgent)
                    {
                        printf("SessionAgent create failed\n");
                        m_sessionAgent.clear();
                        continue;
                    }
                    m_sessionAgent.push_back(std::move(sessionAgent));
                }
            }
            /* 创建心跳保活 */
            if (!devStatus)
            {
                if (!(devStatus = std::move(std::unique_ptr<DevStatus>(new DevStatus(m_cdpAgent.get())))))
                {
                    printf("DevStatus create failed\n");
                    continue;
                }
            }
            /* 判断保活超时次数是否超出用户的配置，超出后认为离线 */
            auto count = devStatus->getTimeoutCount();
            if (count >= m_keepaliveInfo.timeoutCount)
            {
                printf("UA offline, keepalive too many timeouts(%d)\n", count);
                m_bOnline = false;
                keepaliveTime.tv_sec = 0;
                devStatus.reset();
                m_sessionAgent.clear();
                m_cdpAgent.reset();
                m_regAgent.reset();
                continue;
            }
            /* 配置的保活间隔时间到，提前3s发送保活包 */
            struct timespec nowTime;
            clock_gettime(CLOCK_MONOTONIC, &nowTime);
            if (nowTime.tv_sec - keepaliveTime.tv_sec >= m_keepaliveInfo.interval - 3)
            {
                printf("UA send keepalive\n");
                keepaliveTime = nowTime;
                devStatus->sendKeepalive();
            }
        }
        else
        {
            /* 创建注册协议代理 */
            if (!m_regAgent)
            {
                m_regAgent = std::move(std::unique_ptr<RegistrationAgent>(new RegistrationAgent(this)));
                if (m_regAgent)
                {
                    m_regAgent->start();
                }
                else
                {
                    printf("RegistrationAgent create failed\n");
                    continue;
                }
            }
        }

        m_sip->threadFunc(1000);
        printf("UA online: %s\n", m_bOnline ? "true" : "false");
    }

    if (m_bOnline)
    {
        m_regAgent->stop();
    }
}

bool UA::dispatchRegistrationResponse(const SipUserMessage& res)
{
    printf("diapatch register response\n");
    if (m_regAgent)
    {
        return m_regAgent->agent(res);
    }
    return false;
}

bool UA::dispatchMANSCDPRequest(const XMLDocument &req)
{
    printf("diapatch MANSCDP request\n");
    if (m_cdpAgent)
    {
        return m_cdpAgent->agent(req);
    }
    return false;
}

bool UA::dispatchMANSCDPResult(int32_t code, const XMLDocument &cmd)
{
    printf("diapatch MANSCDP cmd result\n");
    if (m_cdpAgent)
    {
        return m_cdpAgent->agent(code, cmd);
    }
    return false;
}

bool UA::dispatchSessionRequest(const SessionIdentifier& id, const SipUserMessage& req)
{
    printf("diapatch session request\n");
    int32_t ch = getChannel(req.getRequestUser());
    if (ch < 0)
    {
        return false;
    }

    if (!m_sessionAgent.empty())
    {
        return m_sessionAgent[ch]->agent(id, req);
    }
    return false;
}

bool UA::dispatchMANSRTSPRequest(const SipUserMessage& req)
{
    printf("diapatch MANSRTSP request\n");
    int32_t ch = getChannel(req.getRequestUser());
    if (ch < 0)
    {
        return false;
    }
    
    const MANSRTSP::Message* message = req.getMANSRTSP();
    if (message != nullptr)
    {
        if (!m_sessionAgent.empty())
        {
            return m_sessionAgent[ch]->dispatchMANSRTSP(*message);
        }
    }
    return false;
}

void UA::setOnline(bool online)
{
    printf("UA ======================> %s\n", online ? "online" : "offline");
    m_bOnline = online;
}

const std::unordered_map<std::string, int32_t>& UA::getChannels() const
{
    return m_channels;
}

int32_t UA::getChannel(const std::string& id) const
{
    auto it = m_channels.find(id);
    if (it != m_channels.end())
    {
        return it->second;
    }
    else
    {
        printf("get channel error: invalid id:%s\n", id.c_str());
        return -1;
    }
}

std::shared_ptr<SipUserAgent> UA::getSip() const
{
    return m_sip;
}

bool UA::start(const SipUserAgent::ClientInfo& client,
               const SipUserAgent::ServerInfo& server,
               const KeepaliveInfo &keepalive,
               const std::vector<std::string>& catalogIds)
{
    printf("UA starting...\n");
    if (m_bStarted)
    {
        printf("UA start error: started already\n");
        return false;
    }

    m_keepaliveInfo = keepalive;

    auto size = catalogIds.size();
    if (size == 0)
    {
        printf("UA start error: size of catalogIds\n");
        return false;
    }
    /* 将目录Id与通道号关联 */
    int i;
    for (i = 0; (std::size_t)i < size; i++)
    {
        m_channels[catalogIds[i]] = i;
    }
    /* 创建sip用户代理 */
    m_sip = SipUserAgent::create(this, client, server);
    if (m_sip)
    {
        m_sip->start();
    }
    else
    {
        printf("UA start error: m_sip\n");
        return false;
    }
    /* 创建状态维护线程 */
    m_bStarted = true;
    m_thread = std::move(std::unique_ptr<std::thread>(new std::thread(&UA::threadProc, this)));
    if (!m_thread)
    {
        printf("UA start error: m_thread\n");
        return m_bStarted = false;
    }
    else
    {
        printf("UA start succeed\n");
        return true;
    }
}

bool UA::stop()
{
    printf("UA stopping...\n");
    if (!m_bStarted)
    {
        printf("UA stop error: started already\n");
        return false;
    }

    /* 停止SIP用户代理 */
    m_sip->stop();

    /* 停止状态维护线程 */
    printf("join thread...\n");
    m_bStarted = false;
    m_thread->join();
    m_thread.reset();

    printf("release resources...\n");
    m_sessionAgent.clear();
    m_regAgent.reset();
    m_cdpAgent.reset();
    m_channels.clear();
    m_sip.reset();

    printf("UA stopped\n");
    m_bOnline = false;
    return true;
}

bool UA::getOnline() const
{
    return m_bOnline;
}
