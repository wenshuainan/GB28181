#ifndef MANSRTSP_REQUEST_H
#define MANSRTSP_REQUEST_H

#include <string.h>
#include <memory>
#include "B.1Message.h"
#include "Agent/SessionAgent.h"

class MANSRTSPAgent;

namespace MANSRTSP
{

class Request
{
protected:
    MANSRTSPAgent* m_agent;

public:
    Request(MANSRTSPAgent* agent) { m_agent = agent; }
    ~Request() {}

protected:
    bool strCaseCmp(const std::string& str1, const std::string& str2)
    {
        return str1.length() == str2.length()
                && strcasecmp(str1.c_str(), str2.c_str()) == 0;
    }

public:
    virtual bool match(const Message& req) = 0;
    virtual bool handle(const Message& req) = 0;
};

}

#endif