#ifndef DEV_RECORD_QUERY_H
#define DEV_RECORD_QUERY_H

#include "Interface/9.7RecordQuery.h"

class DevRecordQuery : public RecordQuery
{
public:
    class DevHandle : public RecordQuery::Handle
    {
        friend class DevRecordQuery;

    public:
        DevHandle(MANSCDPAgent *agent, int32_t ch);
        virtual ~DevHandle();

    public:
        virtual int32_t querySumNum();
        virtual bool queryRecordInfo(int32_t num, std::vector<itemFileType>& recordlist);
    };

public:
    DevRecordQuery(MANSCDPAgent *agent);
    virtual ~DevRecordQuery();

public:
    virtual std::unique_ptr<RecordQuery::Handle> create(int32_t ch, const RecordInfoQuery::Request &req);
};

#endif