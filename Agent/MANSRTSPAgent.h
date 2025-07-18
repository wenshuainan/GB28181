#ifndef MANSRTSP_AGENT_H
#define MANSRTSP_AGENT_H

#include <vector>
#include "Agent.h"
#include "MANSRTSP/Request.h"

using namespace MANSRTSP;

class MANSRTSPAgent : public Agent
{
private:
    std::vector<std::shared_ptr<Request>> m_requests;

public:
    MANSRTSPAgent(UA *ua);
    ~MANSRTSPAgent();

public:
    virtual bool match(const std::string& method, const std::string& contentType);
    virtual bool agent(const SipUserMessage& message);

public:
    bool dispatchRequest(const MANSRTSP::Message& message);
};

#endif