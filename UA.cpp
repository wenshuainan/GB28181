#include <thread>
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

void UA::threadProc()
{
    while (bThreadRun)
    {
        SipGenericMessage message;
        if (sip->recv(message))
        {
            auto type = message.getType();
            if (type == SipGenericMessage::Request)
            {
                const std::string method = message.getMethod();
                const std::string contentType = message.getFieldValue("Content-Type");

                for (auto agent : agents)
                {
                    if (agent->match(method, contentType))
                    {
                        agent->agent(message);
                        break;
                    }
                }
            }
            else if (type == SipGenericMessage::Response)
            {
                const std::string callID = message.getFieldValue("Call-ID");

                for (auto agent : agents)
                {
                    if (agent->match(callID))
                    {
                        agent->agent(message);
                        break;
                    }
                }
            }
            else
            {
            }
        }

        sleep(10);
    }
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
    sip = SipUserAgent::create(info.sipInfo);
    if (sip == nullptr)
    {
        return false;
    }

    /* 创建主线程 */
    bThreadRun = true;
    std::thread t(&UA::threadProc, this);
    t.detach();

    /* 初始化sip用户代理 */
    sip->init();

    /* 发起注册 */
    regAgent->start();

    return true;
}

bool UA::stop()
{
    bThreadRun = false;
    return true;

    for (auto agent : agents)
    {
        delete agent;
    }
}
