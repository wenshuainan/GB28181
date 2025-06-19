#ifndef PROXY_H
#define PROXY_H

#include "Process/9.1Registration.h"
#include "Process/9.2Play.h"
#include "Process/9.3Control.h"
#include "Process/9.4Alarm.h"
#include "Process/9.5Query.h"
#include "Process/9.6Status.h"
#include "Process/9.7Search.h"
#include "Process/9.8Playback.h"
#include "Process/9.9Download.h"
#include "Process/9.10Date.h"
#include "Process/9.11Subscription.h"
#include "Process/9.12Voice.h"
#include "Process/9.13Upgrade.h"
#include "Process/9.14Snapshot.h"

class Proxy
{
private:
    Registration *registration;
    Play *play;
    Control *control;

private:
    std::vector<RequestType *> requestTypes;

public:
    Proxy();
    ~Proxy();

public:
    void setProcess(Registration *registration);
    void setProcess(Play *play);
    void setProcess(Control *control);

public:
    bool processControl(const PTZCmdRequest::Request& req);
    bool processControl(const TeleBootRequest::Request& req);
};

#endif