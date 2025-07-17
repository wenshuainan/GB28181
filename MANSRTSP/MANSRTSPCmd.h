#ifndef MANSRTSP_CMD_H
#define MANSRTSP_CMD_H

#include "B.1MANSRTSP.h"

class MANSRTSPAgent;
class SessionAgent;

namespace MANSRTSP
{

class MANSRTSPCmd
{
private:
    MANSRTSPAgent* m_agent;
    SessionAgent* m_session;

public:
    MANSRTSPCmd();
    ~MANSRTSPCmd();

public:
    virtual bool match(const MANSRTSP& req) = 0;
    virtual bool handle(const MANSRTSP& req) = 0;
};

}

#endif