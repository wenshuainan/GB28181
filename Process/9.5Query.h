#ifndef QUERY_PROCESS_H
#define QUERY_PROCESS_H

#include "A.2.4Query.h"
#include "A.2.6Response.h"

class Query
{
public:
    Query() {}
    virtual ~Query() {}

public:
    virtual bool process(const CatalogQuery::Request& req, CatalogQueryResponse::Response& res) = 0;
    virtual bool process(const DeviceInfoQuery::Request& req, DeviceInfoQueryResponse::Response& res) = 0;
};

#endif