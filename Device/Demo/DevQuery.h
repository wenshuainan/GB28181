#ifndef DEV_QUERY_PROCESS_H
#define DEV_QUERY_PROCESS_H

#include "9.5Query.h"

class DevQuery : public Query
{
public:
    DevQuery();
    virtual ~DevQuery();

public:
    virtual bool process(const CatalogQuery::Request& req, CatalogQueryResponse::Response& res);
    virtual bool process(const DeviceInfoQuery::Request& req, DeviceInfoQueryResponse::Response& res);
};

#endif