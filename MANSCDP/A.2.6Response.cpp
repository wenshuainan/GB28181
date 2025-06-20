#include "A.2.6Response.h"

bool ResponseDeviceControl::serialize(const Response& response, XMLElement *xml)
{
    XMLElement *xmlCmdType = xml->GetDocument()->NewElement("CmdType");
    xmlCmdType->SetText("DeviceControl");
    xml->InsertEndChild(xmlCmdType);

    XMLElement *xmlSN = xml->GetDocument()->NewElement("SN");
    xmlSN->SetText(response.SN.getValue());
    xml->InsertEndChild(xmlSN);

    XMLElement *xmlDeviceID = xml->GetDocument()->NewElement("DeviceID");
    xmlDeviceID->SetText(response.DeviceID.getStr().c_str());
    xml->InsertEndChild(xmlDeviceID);

    XMLElement *xmlResult = xml->GetDocument()->NewElement("Result");
    xmlResult->SetText(response.Result.getStr().c_str());
    xml->InsertEndChild(xmlResult);

    return true;
}