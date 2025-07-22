#ifndef MANSRTSP_SCALE_PLAY_H
#define MANSRTSP_SCALE_PLAY_H

#include "Request.h"

namespace MANSRTSP
{

class ScalePlay : public Request
{
public:
    ScalePlay(MANSRTSPAgent* agent) : Request(agent) {}

public:
    virtual bool match(const Message& req);
    virtual bool handle(const SessionIdentifier& id, const Message& req);
};

}

#endif