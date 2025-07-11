#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include "UA.h"

UA::UA()
{
    bOnline = false;
}

UA::~UA()
{}

bool UA::recvRegistrationResponse(const SipMessageApp& res)
{
    return regAgent->agent(res);
}

bool UA::recvOutDialogRequest(const SipMessageApp& req)
{
    return mansAgent->agent(req);
}

bool UA::recvOutDialogResponse(const SipMessageApp& res, const SipMessageApp& req)
{
    return mansAgent->agent(res, req);
}

bool UA::recvSessionRequest(const SipMessageApp& req)
{
    return sessionAgent->agent(req);
}

void UA::setOnline()
{
    printf(">>>>>> %s:%d\n", __FILE__, __LINE__);
    bOnline = true;
    startKeepalive();
}

bool UA::start(const SipUserAgent::ClientInfo& client, const SipUserAgent::ServerInfo& server, const KeepaliveInfo &keepalive)
{
    keepaliveInfo = keepalive;
    
    /* 创建注册协议代理 */
    regAgent = std::make_shared<RegistrationAgent>(this);
    agents.push_back(regAgent);
    /* 创建MANSCDP协议代理 */
    mansAgent = std::make_shared<MANSCDPAgent>(this);
    agents.push_back(mansAgent);
    /* 创建媒体协议代理 */
    sessionAgent = std::make_shared<SessionAgent>(this);
    agents.push_back(sessionAgent);

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

#if 1
    /* 发起注册 */
    if (regAgent != nullptr)
    {
        return regAgent->start();
    }
    else
    {
        return false;
    }
#else
    XMLDocument doc;
    doc.Parse("<Notify><CmdType>Keepalive</CmdType><SN>43</SN><DeviceID>34020000001110000001</DeviceID><Status>OK</Status></Notify>");
    mansAgent->sendRequest(doc);
    return false;
#endif
}

bool UA::stop()
{
    agents.clear();
    sip->destroy();
    return true;
}

void UA::keepaliveProc()
{
    int32_t tick = 0x0FFFFFFF;

    while (bKeepaliveRunning)
    {
        auto timeoutCount = mansAgent->getKeepaliveTimeoutCount();
        if (timeoutCount > keepaliveInfo.timeoutCount)
        {
            // offline
        }

        if (tick - 3 > keepaliveInfo.interval)
        {
            tick = 0;
            printf(">>>>>> %s:%d\n", __FILE__, __LINE__);
            mansAgent->sendKeepalive();
        }

        usleep(1000000);
        tick++;
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

bool UA::updateStatus(const KeepAliveNotify::Request &notify)
{
    return mansAgent->sendKeepalive(&notify);
}