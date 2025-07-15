#include "A.2.4Query.h"
#include "A.2.6Response.h"
#include "Agent/MANSCDPAgent.h"

QueryRequest::QueryRequest(MANSCDPAgent *agent, Query *query)
{
    spec.push_back(std::make_shared<CatalogQuery>(agent, query));
    spec.push_back(std::make_shared<DeviceInfoQuery>(agent, query));
}

QueryRequest::~QueryRequest()
{
    spec.clear();
}

bool QueryRequest::match(const std::string& ReqType)
{
    return ReqType == "Query";
}

bool QueryRequest::dispatch(const XMLElement *xmlReq)
{
    for (auto& i : spec)
    {
        if (i->match(xmlReq))
        {
            return i->handle(xmlReq);
        }
    }

    return false;
}

CatalogQuery::CatalogQuery(MANSCDPAgent *agent, Query *query)
    : CmdTypeSpecRequest(agent, query)
{}

CatalogQuery::~CatalogQuery()
{}

bool CatalogQuery::deserialize(const XMLElement *xmlReq, Request& req)
{
    const XMLElement *xmlCmdType = xmlReq->FirstChildElement("CmdType");
    if (xmlCmdType)
    {
        req.CmdType = xmlCmdType->GetText();
    }
    else
    {
        return false;
    }

    const XMLElement *xmlSN = xmlReq->FirstChildElement("SN");
    if (xmlSN)
    {
        req.SN = xmlSN->GetText();
    }
    else
    {
        return false;
    }

    const XMLElement *xmlDeviceID = xmlReq->FirstChildElement("DeviceID");
    if (xmlDeviceID)
    {
        req.DeviceID = xmlDeviceID->GetText();
    }
    else
    {
        return false;
    }

    return true;
}

bool CatalogQuery::match(const XMLElement *xmlReq)
{
    const XMLElement *xmlCmdType = xmlReq->FirstChildElement("CmdType");
    if (!xmlCmdType)
    {
        return false;
    }

    return std::string(xmlCmdType->GetText()) == "Catalog";
}

bool CatalogQuery::handle(const XMLElement *xmlReq)
{
    Request req;
    if (!deserialize(xmlReq, req))
    {
        return false;
    }

    CatalogQueryResponse::Response res;
    if (m_query->handle(req, res))
    {
        XMLDocument doc;
        CatalogQueryResponse::serialize(res, &doc);
        return m_agent->sendResponseCmd(doc);
    }

    return false;
}

DeviceInfoQuery::DeviceInfoQuery(MANSCDPAgent *agent, Query *query)
    : CmdTypeSpecRequest(agent, query)
{}

DeviceInfoQuery::~DeviceInfoQuery()
{}

bool DeviceInfoQuery::deserialize(const XMLElement *xmlReq, Request& req)
{
    const XMLElement *xmlCmdType = xmlReq->FirstChildElement("CmdType");
    if (xmlCmdType)
    {
        req.CmdType = xmlCmdType->GetText();
    }
    else
    {
        return false;
    }

    const XMLElement *xmlSN = xmlReq->FirstChildElement("SN");
    if (xmlSN)
    {
        req.SN = xmlSN->GetText();
    }
    else
    {
        return false;
    }

    const XMLElement *xmlDeviceID = xmlReq->FirstChildElement("DeviceID");
    if (xmlDeviceID)
    {
        req.DeviceID = xmlDeviceID->GetText();
    }
    else
    {
        return false;
    }

    return true;
}

bool DeviceInfoQuery::match(const XMLElement *xmlReq)
{
    const XMLElement *xmlCmdType = xmlReq->FirstChildElement("CmdType");
    if (!xmlCmdType)
    {
        return false;
    }

    return std::string(xmlCmdType->GetText()) == "DeviceInfo";
}

bool DeviceInfoQuery::handle(const XMLElement *xmlReq)
{
    Request req;
    if (!deserialize(xmlReq, req))
    {
        return false;
    }

    DeviceInfoQueryResponse::Response res;
    if (m_query->handle(req, res))
    {
        XMLDocument doc;
        DeviceInfoQueryResponse::serialize(res, &doc);
        return m_agent->sendResponseCmd(doc);
    }

    return false;
}