#ifndef MANSRTSP_RANGE_PLAY_H
#define MANSRTSP_RANGE_PLAY_H

#include "CmdType.h"

namespace MANSRTSP
{

class RangePlay : public CmdTypeRequest
{
public:
    RangePlay(MANSRTSPAgent *agent) : CmdTypeRequest(agent) {}

public:
    virtual bool match(const Message& req);
    virtual bool handle(SessionPlayback& session, const Message& req);
};

}

#endif