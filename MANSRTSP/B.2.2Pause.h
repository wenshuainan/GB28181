#ifndef MANSRTSP_PAUSE_H
#define MANSRTSP_PAUSE_H

#include "CmdType.h"

namespace MANSRTSP
{

class Pause : public CmdTypeRequest
{
public:
    Pause(MANSRTSPAgent *agent) : CmdTypeRequest(agent) {}
    
public:
    virtual bool match(const Message& req);
    virtual bool handle(SessionPlayback& session, const Message& req);
};

}
#endif