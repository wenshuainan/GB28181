#ifndef MANSRTSP_SCALE_PLAY_H
#define MANSRTSP_SCALE_PLAY_H

#include "CmdType.h"

namespace MANSRTSP
{

class ScalePlay : public CmdTypeRequest
{
public:
    ScalePlay(MANSRTSPAgent* agent) : CmdTypeRequest(agent) {}

public:
    virtual bool match(const Message& req);
    virtual bool handle(SessionPlayback& session, const Message& req);
};

}

#endif