#ifndef DEV_QUERY_H
#define DEV_QUERY_H

#include "Interface/9.5Query.h"

class DevQuery : public Query
{
public:
    DevQuery();
    virtual ~DevQuery();

public:
    virtual bool handle(const CatalogQuery::Request& req, CatalogQueryResponse::Response& res);
    virtual bool handle(const DeviceInfoQuery::Request& req, DeviceInfoQueryResponse::Response& res);
    virtual bool handle(const RecordInfoQuery::Request& req, RecordInfoQueryResponse::Response& res);
};

#endif