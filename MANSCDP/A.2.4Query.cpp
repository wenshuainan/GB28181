#include "A.2.4Query.h"
#include "A.2.6Response.h"
#include "Agent/MANSCDPAgent.h"

QueryRequest::QueryRequest(MANSCDPAgent *agent, Query *query, RecordQuery *recordQuery)
{
    spec.push_back(std::make_shared<CatalogQuery>(agent, query));
    spec.push_back(std::make_shared<DeviceInfoQuery>(agent, query));
    spec.push_back(std::make_shared<RecordInfoQuery>(agent, recordQuery));
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

bool CatalogQuery::parse(const XMLElement *xmlReq, Request& req)
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
    if (xmlCmdType)
    {
        return std::string(xmlCmdType->GetText()) == "Catalog";
    }
    else
    {
        return false;
    }
}

bool CatalogQuery::handle(const XMLElement *xmlReq)
{
    Request req;
    if (!parse(xmlReq, req))
    {
        return false;
    }

    CatalogQueryResponse::Response res;
    if (m_query->handle(req, res))
    {
        XMLDocument doc;
        CatalogQueryResponse::encode(res, &doc);
        return m_agent->sendResponseCmd(doc);
    }

    return false;
}

DeviceInfoQuery::DeviceInfoQuery(MANSCDPAgent *agent, Query *query)
    : CmdTypeSpecRequest(agent, query)
{}

DeviceInfoQuery::~DeviceInfoQuery()
{}

bool DeviceInfoQuery::parse(const XMLElement *xmlReq, Request& req)
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
    if (xmlCmdType)
    {
        return std::string(xmlCmdType->GetText()) == "DeviceInfo";
    }
    else
    {
        return false;
    }
}

bool DeviceInfoQuery::handle(const XMLElement *xmlReq)
{
    Request req;
    if (!parse(xmlReq, req))
    {
        return false;
    }

    DeviceInfoQueryResponse::Response res;
    if (m_query->handle(req, res))
    {
        XMLDocument doc;
        DeviceInfoQueryResponse::encode(res, &doc);
        return m_agent->sendResponseCmd(doc);
    }

    return false;
}

RecordInfoQuery::RecordInfoQuery(MANSCDPAgent *agent, RecordQuery *recordQuery)
    : CmdTypeSpecRequest(agent, recordQuery)
{}

RecordInfoQuery::~RecordInfoQuery()
{}

bool RecordInfoQuery::parse(const XMLElement *xmlReq, Request& req)
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

    const XMLElement *xmlStartTime = xmlReq->FirstChildElement("StartTime");
    if (xmlStartTime)
    {
        req.StartTime = xmlStartTime->GetText();
    }
    else
    {
        return false;
    }

    const XMLElement *xmlEndTime = xmlReq->FirstChildElement("EndTime");
    if (xmlEndTime)
    {
        req.EndTime = xmlEndTime->GetText();
    }
    else
    {
        return false;
    }

    return true;
}

bool RecordInfoQuery::match(const XMLElement *xmlReq)
{
    const XMLElement *xmlCmdType = xmlReq->FirstChildElement("CmdType");
    if (xmlCmdType)
    {
        return std::string(xmlCmdType->GetText()) == "RecordInfo";
    }
    else
    {
        return false;
    }
}

bool RecordInfoQuery::handle(const XMLElement *xmlReq)
{
    Request req;
    if (!parse(xmlReq, req))
    {
        return false;
    }

    RecordInfoQueryResponse::Response res;
    if (m_recordQuery->handle(req, res))
    {
        XMLDocument doc;
        RecordInfoQueryResponse::encode(res, &doc);
        return m_agent->sendResponseCmd(doc);
    }
    else
    {
        return false;
    }
}