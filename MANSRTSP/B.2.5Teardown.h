#ifndef MANSRTSP_TEARDOWN_H
#define MANSRTSP_TEARDOWN_H

#include "CmdType.h"

namespace MANSRTSP
{

class Teardown : public CmdTypeRequest
{
public:
    Teardown(MANSRTSPAgent *agent) : CmdTypeRequest(agent) {}

public:
    virtual bool match(const Message& req);
    virtual bool handle(SessionPlayback& session, const Message& req);
};

}

#endif