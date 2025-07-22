#ifndef DEV_RECORD_QUERY_H
#define DEV_RECORD_QUERY_H

#include "Interface/9.7RecordQuery.h"

class DevRecordQuery : public RecordQuery
{
public:
    DevRecordQuery();
    virtual ~DevRecordQuery();

public:
    virtual bool handle(const RecordInfoQuery::Request& req, RecordInfoQueryResponse::Response& res);
};

#endif