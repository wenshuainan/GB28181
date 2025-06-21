#ifndef USER_AGENT_H
#define USER_AGENT_H

#include "Adapter.h"
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
private:
    std::vector<Agent *> agents;
    SIPAdapter *adapter;
    bool bThreadRun;

public:
    UA();
    ~UA();

private:
    void threadProc();

public:
    bool start();
    bool stop();

public:
    bool sendRequest(const std::string& methodType, const std::string& contentType, const std::string& content);
    bool sendResponse(int code, const std::string& contentType = "", const std::string& content = "");
};

#endif