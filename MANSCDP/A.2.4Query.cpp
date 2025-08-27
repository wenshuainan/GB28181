#include "A.2.4Query.h"
#include "A.2.6Response.h"
#include "Agent/MANSCDPAgent.h"
#include "MANSCDPDevice.h"

QueryRequest::QueryRequest(MANSCDPAgent *agent)
{
    printf("++++++ QueryRequest\n");
    spec.push_back(std::move(std::unique_ptr<DeviceStatusQuery>(new DeviceStatusQuery(agent))));
    spec.push_back(std::move(std::unique_ptr<CatalogQuery>(new CatalogQuery(agent))));
    spec.push_back(std::move(std::unique_ptr<DeviceInfoQuery>(new DeviceInfoQuery(agent))));
    spec.push_back(std::move(std::unique_ptr<RecordInfoQuery>(new RecordInfoQuery(agent))));
}

QueryRequest::~QueryRequest()
{
    printf("----- QueryRequest\n");
}

bool QueryRequest::match(const std::string& ReqType)
{
    return ReqType == "Query";
}

bool QueryRequest::dispatch(const XMLElement *xmlReq)
{
    printf("MANSCDP <Query> request\n");
    for (auto& i : spec)
    {
        if (i->match(xmlReq))
        {
            return i->handle(xmlReq);
        }
    }

    printf("Not handled Query\n");
    return false;
}

DeviceStatusQuery::DeviceStatusQuery(MANSCDPAgent *agent)
    : CmdTypeSpecRequest(agent)
{
    printf("++++++ DeviceStatusQuery\n");
}

DeviceStatusQuery::~DeviceStatusQuery()
{
    printf("----- DeviceStatusQuery\n");
}

bool DeviceStatusQuery::parse(const XMLElement *xmlReq, Request& req)
{
    const XMLElement *xmlCmdType = xmlReq->FirstChildElement("CmdType");
    if (xmlCmdType)
    {
        req.CmdType = xmlCmdType->GetText();
    }
    else
    {
        printf("CmdType not found\n");
        return false;
    }

    const XMLElement *xmlSN = xmlReq->FirstChildElement("SN");
    if (xmlSN)
    {
        req.SN = xmlSN->GetText();
    }
    else
    {
        printf("SN not found\n");
        return false;
    }

    const XMLElement *xmlDeviceID = xmlReq->FirstChildElement("DeviceID");
    if (xmlDeviceID)
    {
        req.DeviceID = xmlDeviceID->GetText();
    }
    else
    {
        printf("DeviceID not found\n");
        return false;
    }
    
    return true;
}

bool DeviceStatusQuery::match(const XMLElement *xmlReq)
{
    const XMLElement *xmlCmdType = xmlReq->FirstChildElement("CmdType");
    if (xmlCmdType)
    {
        return std::string(xmlCmdType->GetText()) == "DeviceStatus";
    }
    else
    {
        return false;
    }
}

bool DeviceStatusQuery::handle(const XMLElement *xmlReq)
{
    printf("MANSCDP <DeviceStatus> Request\n");
    Request req;
    if (!parse(xmlReq, req))
    {
        printf("parse failed\n");
        return false;
    }
    
    MANSCDPDevice *dev = m_agent->getDevice(req.DeviceID.getStr());
    if (!dev)
    {
        printf("device not found\n");
        return false;
    }

    std::shared_ptr<DeviceStatusQueryResponse> res = m_agent->createCmdMessage<DeviceStatusQueryResponse>(m_agent, req);
    if (res)
    {
        res->Result = dev->queryDeviceStatus(*res);
        return res->response();
    }
    return false;
}

CatalogQuery::CatalogQuery(MANSCDPAgent *agent)
    : CmdTypeSpecRequest(agent)
{
    printf("++++++ CatalogQuery\n");
}

CatalogQuery::~CatalogQuery()
{
    printf("----- CatalogQuery\n");
}

bool CatalogQuery::parse(const XMLElement *xmlReq, Request& req)
{
    const XMLElement *xmlCmdType = xmlReq->FirstChildElement("CmdType");
    if (xmlCmdType)
    {
        req.CmdType = xmlCmdType->GetText();
    }
    else
    {
        printf("CmdType not found\n");
        return false;
    }

    const XMLElement *xmlSN = xmlReq->FirstChildElement("SN");
    if (xmlSN)
    {
        req.SN = xmlSN->GetText();
    }
    else
    {
        printf("SN not found\n");
        return false;
    }

    const XMLElement *xmlDeviceID = xmlReq->FirstChildElement("DeviceID");
    if (xmlDeviceID)
    {
        req.DeviceID = xmlDeviceID->GetText();
    }
    else
    {
        printf("DeviceID not found\n");
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
    printf("MANSCDP <Catalog> Request\n");
    Request req;
    if (!parse(xmlReq, req))
    {
        printf("parse failed\n");
        return false;
    }

    MANSCDPDevice *dev = m_agent->getDevice(req.DeviceID.getStr());
    if (!dev)
    {
        printf("device not found\n");
        return false;
    }

    std::vector<itemType> items;
    dev->queryCatalog(items);
    if (items.empty())
    {
        printf("queryCatalog failed\n");
        return false;
    }

    std::shared_ptr<CatalogQueryResponse> first = m_agent->createCmdMessage<CatalogQueryResponse>(m_agent, req);
    if (!first)
    {
        printf("Create first CatalogQueryResponse failed\n");
        return false;
    }
    int sumNum = items.size();
    int num = 0;
    std::shared_ptr<CatalogQueryResponse> next = first;
    while (num < sumNum)
    {
        next->SumNum = sumNum;
        int size = (sumNum - num) > 2 ? 2 : (sumNum - num); // 一次传输2条记录，< MTU
        next->DeviceList.Num = size;
        next->DeviceList.Item.resize(size);
        int i;
        for (i = 0; i < size; i++)
        {
            next->DeviceList.Item[i] = items[num++];
        }
        if (num >= sumNum)
        {
            break;
        }
        next->m_next = m_agent->createCmdMessage<CatalogQueryResponse>(m_agent, req);
        next = next->m_next;
        if (!next)
        {
            printf("Create next CatalogQueryResponse failed\n");
            return false;
        }
    }
    return first->response(first);
}

DeviceInfoQuery::DeviceInfoQuery(MANSCDPAgent *agent)
    : CmdTypeSpecRequest(agent)
{
    printf("++++++ DeviceInfoQuery\n");
}

DeviceInfoQuery::~DeviceInfoQuery()
{
    printf("----- DeviceInfoQuery\n");
}

bool DeviceInfoQuery::parse(const XMLElement *xmlReq, Request& req)
{
    const XMLElement *xmlCmdType = xmlReq->FirstChildElement("CmdType");
    if (xmlCmdType)
    {
        req.CmdType = xmlCmdType->GetText();
    }
    else
    {
        printf("CmdType not found\n");
        return false;
    }

    const XMLElement *xmlSN = xmlReq->FirstChildElement("SN");
    if (xmlSN)
    {
        req.SN = xmlSN->GetText();
    }
    else
    {
        printf("SN not found\n");
        return false;
    }

    const XMLElement *xmlDeviceID = xmlReq->FirstChildElement("DeviceID");
    if (xmlDeviceID)
    {
        req.DeviceID = xmlDeviceID->GetText();
    }
    else
    {
        printf("DeviceID not found\n");
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
    printf("MANSCDP <DeviceInfo> Request\n");
    Request req;
    if (!parse(xmlReq, req))
    {
        printf("parse failed\n");
        return false;
    }

    MANSCDPDevice *dev = m_agent->getDevice(req.DeviceID.getStr());
    if (!dev)
    {
        printf("device not found\n");
        return false;
    }

    std::shared_ptr<DeviceInfoQueryResponse> res = m_agent->createCmdMessage<DeviceInfoQueryResponse>(m_agent, req);
    if (!res)
    {
        return false;
    }
    res->Result = dev->queryDeviceInfo(*res);
    return res->response();
}

RecordInfoQuery::RecordInfoQuery(MANSCDPAgent *agent)
    : CmdTypeSpecRequest(agent)
{
    printf("++++++ RecordInfoQuery\n");
}

RecordInfoQuery::~RecordInfoQuery()
{
    printf("------ RecordInfoQuery\n");
}

bool RecordInfoQuery::parse(const XMLElement *xmlReq, Request& req)
{
    const XMLElement *xmlCmdType = xmlReq->FirstChildElement("CmdType");
    if (xmlCmdType)
    {
        req.CmdType = xmlCmdType->GetText();
    }
    else
    {
        printf("CmdType not found\n");
        return false;
    }

    const XMLElement *xmlSN = xmlReq->FirstChildElement("SN");
    if (xmlSN)
    {
        req.SN = xmlSN->GetText();
    }
    else
    {
        printf("SN not found\n");
        return false;
    }

    const XMLElement *xmlDeviceID = xmlReq->FirstChildElement("DeviceID");
    if (xmlDeviceID)
    {
        req.DeviceID = xmlDeviceID->GetText();
    }
    else
    {
        printf("DeviceID not found\n");
        return false;
    }

    const XMLElement *xmlStartTime = xmlReq->FirstChildElement("StartTime");
    if (xmlStartTime)
    {
        req.StartTime = xmlStartTime->GetText();
    }
    else
    {
        printf("StartTime not found\n");
        return false;
    }

    const XMLElement *xmlEndTime = xmlReq->FirstChildElement("EndTime");
    if (xmlEndTime)
    {
        req.EndTime = xmlEndTime->GetText();
    }
    else
    {
        printf("EndTime not found\n");
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
    printf("MANSCDP <RecordInfo> Request\n");
    Request req;
    if (!parse(xmlReq, req))
    {
        printf("parse failed\n");
        return false;
    }

    MANSCDPDevice *dev = m_agent->getDevice(req.DeviceID.getStr());
    if (!dev)
    {
        printf("device not found\n");
        return false;
    }

    std::unique_ptr<RecordQuery::Handle> handle = dev->createHandle(req);
    if (!handle)
    {
        printf("RecordQuery handle create failed\n");
        return false;
    }

    std::shared_ptr<RecordInfoQueryResponse> first = m_agent->createCmdMessage<RecordInfoQueryResponse>(m_agent, req);
    if (!first)
    {
        printf("create first RecordInfoQueryResponse failed\n");
        return false;
    }
    int32_t sumNum = handle->querySumNum();
    int32_t num = 0;
    std::shared_ptr<RecordInfoQueryResponse> next = first;
    while (num < sumNum)
    {
        next->SumNum = sumNum;
        handle->queryRecordInfo(2, next->RecordList.Item); // 一次传输2条记录，< MTU
        next->RecordList.Num = next->RecordList.Item.size();
        num += next->RecordList.Num.getInt();
        if (next->RecordList.Item.size() == 0 || num >= sumNum)
        {
            break;
        }
        next->m_next = m_agent->createCmdMessage<RecordInfoQueryResponse>(m_agent, req);
        next = next->m_next;
        if (!next)
        {
            printf("create next RecordInfoQueryResponse failed\n");
            return false;
        }
    }
    return first->response(first);
}