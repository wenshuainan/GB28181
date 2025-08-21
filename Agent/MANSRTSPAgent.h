#ifndef MANSRTSP_AGENT_H
#define MANSRTSP_AGENT_H

#include <vector>
#include <memory>
#include "Agent.h"
#include "MANSRTSP/CmdType.h"
#include "SessionAgent.h"

class MANSRTSPAgent : public Agent
{
private:
    /* 所有MANSRTSP协议定义的请求
     * PLAY、PAUS、TEARDOWN
     */
    std::vector<std::unique_ptr<MANSRTSP::CmdTypeRequest>> m_requests;
    SessionPlayback *m_session;

public:
    MANSRTSPAgent(UA *ua, SessionPlayback *session);
    ~MANSRTSPAgent();

public:
    virtual bool match(const std::string& method, const std::string& contentType);
    virtual bool agent(const SipUserMessage& message);
    virtual bool agent(const SessionIdentifier& id, const MANSRTSP::Message& message);
    bool sendResponse(const SessionIdentifier& id, const MANSRTSP::Message& message);
};

#endif