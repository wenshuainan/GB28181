#ifndef RECORD_QUERY_INTERFACE_H
#define RECORD_QUERY_INTERFACE_H

#include <memory>
#include "MANSCDP/A.2.4Query.h"
#include "MANSCDP/A.2.6Response.h"

class MANSCDPAgent;

class RecordQuery
{
public:
    class Handle
    {
    protected:
        MANSCDPAgent *m_agent;
        int32_t m_ch;

    public:
        Handle(MANSCDPAgent *agent, int32_t ch)
            : m_agent(agent)
            , m_ch(ch)
        {}
        virtual ~Handle() {}

    public:
        virtual int32_t querySumNum() = 0;
        virtual bool queryRecordInfo(int32_t num, std::vector<itemFileType>& recordlist) = 0;
    };

protected:
    MANSCDPAgent *m_agent;
    
public:
    RecordQuery(MANSCDPAgent *agent) : m_agent(agent) {}
    virtual ~RecordQuery() {}

public:
    virtual std::unique_ptr<Handle> create(int32_t ch, const RecordInfoQuery::Request &req) = 0;
};

#endif