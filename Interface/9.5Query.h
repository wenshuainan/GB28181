#ifndef QUERY_INTERFACE_H
#define QUERY_INTERFACE_H

#include "MANSCDP/A.2.4Query.h"
#include "MANSCDP/A.2.6Response.h"

class Query
{
public:
    Query() {}
    virtual ~Query() {}

public:
    virtual bool handle(int32_t ch, itemType& item) = 0;
    virtual bool handle(DeviceInfoQueryResponse& res) = 0;
};

#endif