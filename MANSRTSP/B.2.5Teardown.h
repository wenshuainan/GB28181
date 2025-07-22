#ifndef MANSRTSP_TEARDOWN_H
#define MANSRTSP_TEARDOWN_H

#include "Request.h"

namespace MANSRTSP
{

class Teardown : public Request
{
public:
    Teardown(MANSRTSPAgent *agent) : Request(agent) {}

public:
    virtual bool match(const Message& req);
    virtual bool handle(const SessionIdentifier& id, const Message& req);
};

}

#endif