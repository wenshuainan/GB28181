#ifndef MANSRTSP_CMD_TYPE_H
#define MANSRTSP_CMD_TYPE_H

#include <string.h>
#include "B.1Message.h"
#include "Agent/SessionAgent.h"

class MANSRTSPAgent;

namespace MANSRTSP
{

class CmdTypeRequest
{
protected:
    MANSRTSPAgent* m_agent;

public:
    CmdTypeRequest(MANSRTSPAgent* agent) { m_agent = agent; }
    ~CmdTypeRequest() {}

protected:
    bool strCaseCmp(const std::string& str1, const std::string& str2)
    {
        return str1.length() == str2.length()
                && strcasecmp(str1.c_str(), str2.c_str()) == 0;
    }

public:
    virtual bool match(const Message& req) = 0;
    virtual bool handle(SessionPlayback& session, const Message& req) = 0;
};

}

#endif