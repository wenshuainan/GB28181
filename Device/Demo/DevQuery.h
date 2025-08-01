#ifndef DEV_QUERY_H
#define DEV_QUERY_H

#include "Interface/9.5Query.h"

class DevQuery : public Query
{
public:
    DevQuery();
    virtual ~DevQuery();

public:
    virtual bool handle(int32_t ch, itemType& item);
    virtual bool handle(DeviceInfoQueryResponse& res);
};

#endif