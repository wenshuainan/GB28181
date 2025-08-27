#include <unistd.h>
#include <time.h>
#include "UA.h"
#include "RegistrationAgent.h"
#include "DevDate.h"

RegistrationAgent::RegistrationAgent(UA *ua)
    : Agent(ua)
{
    printf("++++++ RegistrationAgent\n");
    m_devDate = std::move(std::unique_ptr<DevDate>(new DevDate()));
    m_GBVerName = "X-GB-Ver";
    m_GBVerValue = "3.0";
    m_bStarted = false;
}

RegistrationAgent::~RegistrationAgent()
{
    printf("----- RegistrationAgent\n");
    stop();
}

bool RegistrationAgent::start()
{
    printf("Registration start\n");
    if (m_bStarted)
    {
        // printf("RegistrationAgent start error: started already\n");
        return false;
    }

    auto sip = m_ua->getSip();
    if (!sip)
    {
        return false;
    }

    m_bStarted = true;

    SipUserMessage message;
    sip->makeRegistration(message);

    printf("Registration GBVersion=(%s, %s)\n", m_GBVerName.c_str(), m_GBVerValue.c_str());
    /* 添加GB版本号扩展头域（附录I） */
    message.addExtensionField(m_GBVerName, m_GBVerValue);

    printf("send Registration request\n");
    return sip->sendRegistration(message);
}

bool RegistrationAgent::stop()
{
    printf("Registration stop\n");
    if (!m_bStarted)
    {
        printf("RegistrationAgent stop error: not started\n");
        return false;
    }
    else
    {
        m_bStarted = false;
    }

    if (m_ua->getState() == Registration::REGISTERED)
    {
        auto sip = m_ua->getSip();
        if (!sip)
        {
            return false;
        }
        SipUserMessage message;
        sip->makeRegistration(message);

        message.setExpires(0);

        printf("send Unregister request\n");
        return sip->sendRegistration(message);
    }
    return true;
}

bool RegistrationAgent::match(const std::string& method, const std::string& contentType)
{
    return false; //只作为Register Client，不处理Register请求
}

bool RegistrationAgent::agent(const SipUserMessage& message)
{
    printf("Registration agent:\n");
    message.print();
    int32_t code = message.getCode();
    m_ua->onRegistrationStatus(code, message.getReasonPhrase());

    /* SIP方式校时 */
    if (code == 200)
    {
        const char *date = message.getHeaderDate();
        if (date && strlen(date) > 0)
        {
            printf("set device time use header Date: %s\n", date);
            struct tm t;
            char *ret = strptime(date, "%Y-%m-%dT%H:%M:%S", &t);
            if (ret != nullptr && ret == date + 19)
            {
                m_devDate->setTime(mktime(&t));
            }
        }
    }
    return true;
}
