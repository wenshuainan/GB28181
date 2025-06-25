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

class UA
{
public:
    struct Info
    {
        SipUserAgent::Info sipInfo;
        int interval;
    };
    
private:
    std::vector<Agent *> agents;
    SipUserAgent *sip;
    bool bThreadRun;

public:
    UA();
    ~UA();

private:
    void threadProc();

public:
    bool start(const Info& info);
    bool stop();
    SipUserAgent* getSip() { return sip; }
};

#endif