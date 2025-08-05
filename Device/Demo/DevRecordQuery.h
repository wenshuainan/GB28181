#ifndef DEV_RECORD_QUERY_H
#define DEV_RECORD_QUERY_H

#include "Interface/9.7RecordQuery.h"

class DevRecordQuery : public RecordQuery
{
public:
    DevRecordQuery();
    virtual ~DevRecordQuery();

public:
    virtual bool queryRecordInfo(int32_t ch, const RecordInfoQuery::Request& req, RecordInfoQueryResponse& res);
};

#endif