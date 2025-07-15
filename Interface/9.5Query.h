#ifndef QUERY_INTERFACE_H
#define QUERY_INTERFACE_H

#include "A.2.4Query.h"
#include "A.2.6Response.h"

class Query
{
public:
    Query() {}
    virtual ~Query() {}

public:
    virtual bool handle(const CatalogQuery::Request& req, CatalogQueryResponse::Response& res) = 0;
    virtual bool handle(const DeviceInfoQuery::Request& req, DeviceInfoQueryResponse::Response& res) = 0;
};

#endif