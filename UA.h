#ifndef GB28181_USER_AGENT_H
#define GB28181_USER_AGENT_H

#include <vector>
#include <memory>
#include "SipAdapter.h"
#include "Agent.h"

#define DEBUG_LOG std::cout << "GB28181: " << __FILE__ << ":" << __LINE__ << " "

class UA
{
public:
    struct Info
    {
        SipUserAgent::Info sipInfo;
    };
    
private:
    std::vector<std::shared_ptr<Agent>> agents;
    std::shared_ptr<SipUserAgent> sip;

public:
    UA();
    ~UA();

public:
    bool postRecved(const SipMessageApp& message);

public:
    bool start(const Info& info);
    bool stop();
    const std::shared_ptr<SipUserAgent>& getSip() { return sip; }
};

#endif