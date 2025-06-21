#include "A.2.6Response.h"

bool CmdResponse::serialize(const Response& response, XMLDocument *doc)
{
    XMLElement *rootElement = doc->NewElement("Response");
    if (rootElement != nullptr)
    {
        return doc->InsertEndChild(rootElement) != nullptr;
    }
    else
    {
        return false;
    }
}

bool DeviceControlResponse::serialize(const Response& response, XMLDocument *doc)
{
    if (!CmdResponse::serialize(response, doc))
    {
        return false;
    }

    XMLNode *rootElement = doc->FirstChild();
    if (rootElement == nullptr)
    {
        return false;
    }

    XMLElement *xmlCmdType = doc->NewElement("CmdType");
    xmlCmdType->SetText("DeviceControl");
    rootElement->InsertEndChild(xmlCmdType);

    XMLElement *xmlSN = doc->NewElement("SN");
    xmlSN->SetText(response.SN.getValue());
    rootElement->InsertEndChild(xmlSN);

    XMLElement *xmlDeviceID = doc->NewElement("DeviceID");
    xmlDeviceID->SetText(response.DeviceID.getStr().c_str());
    rootElement->InsertEndChild(xmlDeviceID);

    XMLElement *xmlResult = doc->NewElement("Result");
    xmlResult->SetText(response.Result.getStr().c_str());
    rootElement->InsertEndChild(xmlResult);

    return true;
}