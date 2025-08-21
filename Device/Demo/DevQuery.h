#ifndef DEV_QUERY_H
#define DEV_QUERY_H

#include "Interface/9.5Query.h"

class DevQuery : public Query
{
public:
    DevQuery(MANSCDPAgent *agent);
    virtual ~DevQuery();

public:
    virtual bool queryCatalog(int32_t ch, itemType& item);
    virtual bool queryDeviceInfo(DeviceInfoQueryResponse& res);
};

#endif