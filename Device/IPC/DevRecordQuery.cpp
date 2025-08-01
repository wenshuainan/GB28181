#include <iostream>
#include "DevRecordQuery.h"

DevRecordQuery::DevRecordQuery()
{}

DevRecordQuery::~DevRecordQuery()
{}

bool DevRecordQuery::handle(int32_t ch, const RecordInfoQuery::Request& req, RecordInfoQueryResponse& res)
{
    res.Name = "Camera1";
    res.SumNum = 1;
    res.RecordList.Num = 1;
    res.RecordList.Item.resize(1);
    res.RecordList.Item[0].DeviceID = req.DeviceID;
    res.RecordList.Item[0].Name = "Camera1";
    res.RecordList.Item[0].StartTime = "2025-07-18T12:00:00";
    res.RecordList.Item[0].EndTime = "2025-07-18T16:00:00";
    res.RecordList.Item[0].Secrecy = 0;

    return true;
}