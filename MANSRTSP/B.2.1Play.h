#ifndef MANSRTSP_PLAY_H
#define MANSRTSP_PLAY_H

#include "Request.h"

namespace MANSRTSP
{

class Play : public Request
{
public:
    Play(MANSRTSPAgent* agent) : Request(agent) {}

public:
    virtual bool match(const Message& req);
    virtual bool handle(const Message& req);
};

}

#endif