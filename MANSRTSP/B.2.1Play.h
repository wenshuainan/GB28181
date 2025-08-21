#ifndef MANSRTSP_PLAY_H
#define MANSRTSP_PLAY_H

#include "CmdType.h"

namespace MANSRTSP
{

class Play : public CmdTypeRequest
{
public:
    Play(MANSRTSPAgent* agent) : CmdTypeRequest(agent) {}

public:
    virtual bool match(const Message& req);
    virtual bool handle(SessionPlayback& session, const Message& req);
};

}

#endif