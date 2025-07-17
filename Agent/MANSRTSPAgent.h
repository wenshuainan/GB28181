#ifndef MANSRTSP_AGENT_H
#define MANSRTSP_AGENT_H

#include <vector>
#include "Agent.h"
#include "MANSRTSP/MANSRTSPCmd.h"

using namespace MANSRTSP;

class MANSRTSPAgent : public Agent
{
private:
    std::vector<std::shared_ptr<MANSRTSPCmd>> m_cmds;
};

#endif