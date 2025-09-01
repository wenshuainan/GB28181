#include <unistd.h>
#include <time.h>
#include <sys/prctl.h>
#include "UA.h"
#include "Agent/RegistrationAgent.h"
#include "Agent/MANSCDPAgent.h"
#include "MANSCDP/A.2.5Notify.h"
#include "MANSRTSP/B.1Message.h"
#include "FrontType/Camera.h"
#include "FrontType/AlarmIn.h"
#include "FrontType/AlarmOut.h"

UA::UA()
    : Device("", -1)
    , MANSCDPDevice(nullptr)
    , m_bStarted(false)
{}

UA::~UA()
{
    stop();
}

void UA::threadProc()
{
    prctl(PR_SET_NAME, "UA");
    
    struct timespec keepaliveTime = {-m_keepaliveInfo.interval, 0}; // 保活信息发送时间
                                                                    // 使用负数初始化，使得程序刚启动就上线后可以立即发送

    while (m_bStarted)
    {
        if (m_state == REGISTERED)
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
            /* 创建前端外围设备 */
            if (m_devices.empty())
            {
                if (!createDevices())
                {
                    printf("create devices failed\n");
                    continue;
                }
            }
            /* 判断保活超时次数是否超出用户的配置，超出后认为离线 */
            if (m_timeoutCount >= m_keepaliveInfo.timeoutCount)
            {
                printf("UA offline, keepalive too many timeouts(%d)\n", m_timeoutCount);
                m_state = UNREGISTERED;
                keepaliveTime.tv_sec = 0;
                m_timeoutCount = 0;
                m_regAgent.reset(); // 重新发起注册
                continue;
            }
            /* 配置的保活间隔时间到，提前3s发送保活包 */
            struct timespec nowTime;
            clock_gettime(CLOCK_MONOTONIC, &nowTime);
            if (nowTime.tv_sec - keepaliveTime.tv_sec >= m_keepaliveInfo.interval - 3)
            {
                printf("UA send keepalive\n");
                keepaliveTime = nowTime;
                sendKeepalive();
            }
        }
        else
        {
            /* 离线状态，清理资源 */
            m_devices.clear();
            m_cdpAgent.reset();

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
        printf("UA online: %s\n", m_state == REGISTERED ? "true" : "false");
    }

    if (m_regAgent)
    {
        m_regAgent->stop();
    }
}

bool UA::createDevices()
{
    for (std::size_t i = 0; i < m_frontDevice.camera.size(); i++)
    {
        auto& id = m_frontDevice.camera[i];
        m_devices[id] = std::move(std::unique_ptr<Device>(new Camera(id, i, this, m_cdpAgent.get())));
    }
    for (std::size_t i = 0; i < m_frontDevice.alarmIn.size(); i++)
    {
        auto& id = m_frontDevice.alarmIn[i];
        m_devices[id] = std::move(std::unique_ptr<Device>(new AlarmIn(id, i, this, m_cdpAgent.get())));
    }
    for (std::size_t i = 0; i < m_frontDevice.alarmOut.size(); i++)
    {
        auto& id = m_frontDevice.alarmOut[i];
        m_devices[id] = std::move(std::unique_ptr<Device>(new AlarmOut(id, i, this, m_cdpAgent.get())));
    }
    return true;
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
    Device *device = getDevice(req.getRequestUser());
    if (!device)
    {
        return false;
    }

    SessionDevice *sessionDevice = dynamic_cast<SessionDevice*>(device);
    if (sessionDevice)
    {
        SessionAgent *session = sessionDevice->getSessionAgent();
        if (!session)
        {
            return false;
        }
        return session->agent(id, req);
    }
    return false;
}

bool UA::dispatchMANSRTSPRequest(const SipUserMessage& req)
{
    printf("diapatch MANSRTSP request\n");
    Device *device = getDevice(req.getRequestUser());
    if (!device)
    {
        return false;
    }

    const MANSRTSP::Message* message = req.getMANSRTSP();
    if (!message)
    {
        return false;
    }

    SessionDevice *sessionDevice = dynamic_cast<SessionDevice*>(device);
    if (sessionDevice)
    {
        SessionAgent *session = sessionDevice->getSessionAgent();
        if (!session)
        {
            return false;
        }
        return session->dispatchMANSRTSP(*message);
    }
    return false;
}

std::shared_ptr<SipUserAgent> UA::getSip() const
{
    return m_sip;
}

Device* UA::getDevice(const std::string& id)
{
    if (m_state != REGISTERED)
    {
        return nullptr;
    }
    
    if (id == m_id)
    {
        return this;
    }

    auto it = m_devices.find(id);
    if (it != m_devices.end())
    {
        return it->second.get();
    }
    return nullptr;
}

void UA::onRegistrationStatus(int32_t code, const std::string& sipReasonPhrase)
{
    printf("Registration sip status %d(%s) last state=%d\n", code, sipReasonPhrase.c_str(), m_state);
    switch (code)
    {
    case 200:
        if (m_state == REGISTERED) // 注销
        {
            m_state = UNREGISTERED;
        }
        else
        {
            m_state = REGISTERED;
        }
        break;
    case 400:
    case 401:
    case 403:
    case 408:
        m_state = REGISTER_FAILED;
        break;
    
    default:
        m_state = UNREGISTERED;
        break;
    }
    printf("Registration new state=%d\n", m_state);
}

bool UA::queryDeviceStatus(DeviceStatusQueryResponse& res)
{
    res.Online = DeviceStatusQueryResponse::ONLINE;
    res.Status = resultType::OK;
    res.DeviceTime = time(NULL);

    res.AlarmStatus.Item.resize(m_devices.size());
    for (auto& device : m_devices)
    {
        MANSCDPDevice *dev = dynamic_cast<MANSCDPDevice*>(device.second.get());
        if (dev)
        {
            int32_t num = res.AlarmStatus.Num.getInt();
            if (dev->queryAlarmStatus(res.AlarmStatus.Item[num]))
            {
                res.AlarmStatus.Num++;
            }
        }
    }
    if (res.AlarmStatus.Num != m_devices.size())
    {
        res.AlarmStatus.Item.resize(res.AlarmStatus.Num.getInt());
    }
    return true;
}

bool UA::queryCatalog(std::vector<itemType>& items)
{
    items.resize(m_devices.size());
    int i = 0;
    for (auto& device : m_devices)
    {
        MANSCDPDevice *dev = dynamic_cast<MANSCDPDevice*>(device.second.get());
        if (dev)
        {
            dev->queryCatalog(items[i]);
        }
        i++;
    }
    return true;
}

bool UA::queryDeviceInfo(DeviceInfoQueryResponse& res)
{
    res.DeviceName = "IPC";
    res.Manufacturer = "Manufacturer";
    res.Model = "Model";
    res.Firmware = "Firmware 1.0";

    return true;
}

bool UA::getStatus(std::vector<std::string>& offDevices)
{
    for (auto& device : m_devices)
    {
        MANSCDPDevice *dev = dynamic_cast<MANSCDPDevice*>(device.second.get());
        if (dev)
        {
            if (!dev->getStatus())
            {
                offDevices.push_back(device.first);
            }
        }
    }
    return true;
}

bool UA::sendKeepalive()
{
    MANSCDPAgent *agent = m_cdpAgent.get();
    std::shared_ptr<KeepaliveNotify> keepalive = agent->createCmdMessage<KeepaliveNotify>(agent, this);
    if (keepalive && keepalive->notify(keepalive))
    {
        addSentCount();
        return true;
    }
    return false;
}

bool UA::start(const SipUserAgent::ClientInfo& client,
               const SipUserAgent::ServerInfo& server,
               const KeepaliveInfo& keepalive,
               const FrontDevice& frontDevice)
{
    printf("UA starting...\n");
    if (m_bStarted)
    {
        printf("UA start error: started already\n");
        return false;
    }

    this->m_id = client.id;
    m_keepaliveInfo = keepalive;
    m_frontDevice = frontDevice;

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
    m_state = UNREGISTERED;

    printf("release resources...\n");
    m_regAgent.reset();
    m_cdpAgent.reset();
    m_devices.clear();
    m_sip.reset();

    printf("UA stopped\n");
    return true;
}
