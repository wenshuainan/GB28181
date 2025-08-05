#ifndef RECORD_QUERY_INTERFACE_H
#define RECORD_QUERY_INTERFACE_H

#include "MANSCDP/A.2.4Query.h"
#include "MANSCDP/A.2.6Response.h"

class RecordQuery
{
public:
    RecordQuery() {}
    virtual ~RecordQuery() {}

public:
    virtual bool queryRecordInfo(int32_t ch, const RecordInfoQuery::Request& req, RecordInfoQueryResponse& res) = 0;
};

#endif