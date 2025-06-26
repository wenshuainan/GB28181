#ifndef USER_AGENT_H
#define USER_AGENT_H

#include "SipAdapter.h"
#include "Agent.h"
#include "9.1Registration.h"
#include "9.2Play.h"
#include "9.3Control.h"
#include "9.4Alarm.h"
#include "9.5Query.h"
#include "9.6Status.h"
#include "9.7Search.h"
#include "9.8Playback.h"
#include "9.9Download.h"
#include "9.10Date.h"
#include "9.11Subscription.h"
#include "9.12Voice.h"
#include "9.13Upgrade.h"
#include "9.14Snapshot.h"

#define DEBUG_LOG std::cout << "GB28181: " << __FILE__ << ":" << __LINE__ << " "

class UA
{
public:
    struct Info
    {
        SipUserAgent::Info sipInfo;
    };
    
private:
    std::vector<Agent *> agents;
    SipUserAgent *sip;

public:
    UA();
    ~UA();

public:
    bool postRecved(const SipMessageApp& message);

public:
    bool start(const Info& info);
    bool stop();
    SipUserAgent* getSip() { return sip; }
};

#endif