#include <iostream>
#include "DevQuery.h"

DevQuery::DevQuery()
{}

DevQuery::~DevQuery()
{}

bool DevQuery::handle(const CatalogQuery::Request& req, CatalogQueryResponse::Response& res)
{
    std::cout << "DevQuery::handle" << std::endl;
    std::cout << "CMD:" << req.CmdType.getValue() << std::endl;
    std::cout << "SN:" << req.SN.getValue() << std::endl;
    std::cout << "DeviceID:" << req.DeviceID.getValue() << std::endl;

    res.CmdType = req.CmdType;
    res.SN = req.SN;
    res.DeviceID = req.DeviceID;
    res.SumNum = 1;
    res.DeviceList.Num = 1;
    res.DeviceList.Item.resize(res.DeviceList.Num.getValue());
    res.DeviceList.Item[0].DeviceID = "34020000001310000001";
    res.DeviceList.Item[0].Name = "DemoDev";
    res.DeviceList.Item[0].Manufacturer = "Manufacturer 1";
    res.DeviceList.Item[0].Model = "Model 1";
    res.DeviceList.Item[0].CivilCode = "CivilCode 1";
    res.DeviceList.Item[0].Address = "Address 1";
    res.DeviceList.Item[0].Parental = 1;
    res.DeviceList.Item[0].ParentID = req.DeviceID.getStr();
    res.DeviceList.Item[0].RegisterWay = 1;
    res.DeviceList.Item[0].Secrecy = 0;
    res.DeviceList.Item[0].Status = statusType::ON;

    return true;
}

bool DevQuery::handle(const DeviceInfoQuery::Request& req, DeviceInfoQueryResponse::Response& res)
{
    std::cout << "DevQuery::handle" << std::endl;
    std::cout << "CMD:" << req.CmdType.getValue() << std::endl;
    std::cout << "SN:" << req.SN.getValue() << std::endl;
    std::cout << "DeviceID:" << req.DeviceID.getValue() << std::endl;

    res.CmdType = req.CmdType;
    res.SN = req.SN;
    res.DeviceID = req.DeviceID;
    res.DeviceName = "DemoDev";
    res.Result = resultType::OK;
    res.Manufacturer = "Manufacturer 1";
    res.Model = "Model 1";
    res.Firmware = "Firmware 1.0";

    return true;
}
