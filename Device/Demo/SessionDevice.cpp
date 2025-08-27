#include "SessionDevice.h"
#include "UA.h"

SessionDevice::SessionDevice()
    : Device("", -1)
{
    printf("++++++ SessionDevice\n");
    UA *ua = dynamic_cast<UA*>(m_parent);
    m_agent = std::move(std::unique_ptr<SessionAgent>(new SessionAgent(ua, m_ch)));
}

SessionDevice::~SessionDevice()
{
    printf("------ SessionDevice\n");
}

SessionAgent* SessionDevice::getSessionAgent() const
{
    return m_agent.get();
}