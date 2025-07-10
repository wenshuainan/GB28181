#include <iostream>
#include "DevQuery.h"

DevQuery::DevQuery()
{}

DevQuery::~DevQuery()
{}

bool DevQuery::process(const CatalogQuery::Request& req, CatalogQueryResponse::Response& res)
{
    std::cout << "DevQuery::process" << std::endl;
    std::cout << "CMD:" << req.CmdType.getValue() << std::endl;
    std::cout << "SN:" << req.SN.getValue() << std::endl;
    std::cout << "DeviceID:" << req.DeviceID.getValue() << std::endl;

    res.CmdType = req.CmdType;
    res.SN = req.SN;
    res.DeviceID = req.DeviceID;
    res.SumNum = 1;
    res.DeviceList.Num = 1;
    res.DeviceList.item.resize(res.DeviceList.Num.getValue());
    res.DeviceList.item[0].DeviceID = req.DeviceID;
    res.DeviceList.item[0].Name = "DemoDev";
    res.DeviceList.item[0].Manufacturer = "???";
    res.DeviceList.item[0].Model = "???";
    res.DeviceList.item[0].CivilCode = "12345678";
    res.DeviceList.item[0].Address = "???";
    res.DeviceList.item[0].Parental = 0;
    res.DeviceList.item[0].ParentID = "1234567890";
    res.DeviceList.item[0].RegisterWay = 1;
    res.DeviceList.item[0].Secrecy = 0;

    return true;
}

bool DevQuery::process(const DeviceInfoQuery::Request& req, DeviceInfoQueryResponse::Response& res)
{
    std::cout << "DevQuery::process" << std::endl;
    std::cout << "CMD:" << req.CmdType.getValue() << std::endl;
    std::cout << "SN:" << req.SN.getValue() << std::endl;
    std::cout << "DeviceID:" << req.DeviceID.getValue() << std::endl;

    res.CmdType = req.CmdType;
    res.SN = req.SN;
    res.DeviceID = req.DeviceID;
    res.DeviceName = "DemoDev";
    res.Result = resultType::OK;
    res.Manufacturer = "???";
    res.Model = "???";
    res.Firmware = "1.0";

    return true;
}