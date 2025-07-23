#ifndef MANSRTSP_PAUSE_H
#define MANSRTSP_PAUSE_H

#include "Request.h"

namespace MANSRTSP
{

class Pause : public Request
{
public:
    Pause(MANSRTSPAgent *agent) : Request(agent) {}
    
public:
    virtual bool match(const Message& req);
    virtual bool handle(SessionPlayback& session, const Message& req);
};

}
#endif