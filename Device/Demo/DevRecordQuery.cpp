#include <iostream>
#include "DevRecordQuery.h"

DevRecordQuery::DevRecordQuery()
{}

DevRecordQuery::~DevRecordQuery()
{}

bool DevRecordQuery::handle(const RecordInfoQuery::Request& req, RecordInfoQueryResponse::Response& res)
{
    std::cout << "DevRecordQuery::handle" << std::endl;
    std::cout << "CMD:" << req.CmdType.getValue() << std::endl;
    std::cout << "SN:" << req.SN.getValue() << std::endl;
    std::cout << "DeviceID:" << req.DeviceID.getValue() << std::endl;

    res.CmdType = req.CmdType;
    res.SN = req.SN;
    res.DeviceID = req.DeviceID;
    res.Name = "Camera1";
    res.SumNum = 1;
    res.RecordList.Num = 1;
    res.RecordList.Item.resize(res.RecordList.Num.getValue());
    res.RecordList.Item[0].DeviceID = req.DeviceID;
    res.RecordList.Item[0].Name = "Camera1";
    res.RecordList.Item[0].StartTime = "2025-07-18T12:00:00";
    res.RecordList.Item[0].EndTime = "2025-07-18T16:00:00";
    res.RecordList.Item[0].Secrecy = 0;

    return true;
}