#ifndef MANSRTSP_RANGE_PLAY_H
#define MANSRTSP_RANGE_PLAY_H

#include "Request.h"

namespace MANSRTSP
{

class RangePlay : public Request
{
public:
    RangePlay(MANSRTSPAgent *agent) : Request(agent) {}

public:
    virtual bool match(const Message& req);
    virtual bool handle(const SessionIdentifier& id, const Message& req);
};

}

#endif