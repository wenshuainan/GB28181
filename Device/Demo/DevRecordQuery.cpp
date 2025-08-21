#include <iostream>
#include "DevRecordQuery.h"
#include "Agent/MANSCDPAgent.h"

DevRecordQuery::DevHandle::DevHandle(MANSCDPAgent *agent, int32_t ch)
    : RecordQuery::Handle(agent, ch)
{}

DevRecordQuery::DevHandle::~DevHandle()
{}

int32_t DevRecordQuery::DevHandle::querySumNum()
{
    return 1;
}

bool DevRecordQuery::DevHandle::queryRecordInfo(int32_t num, std::vector<itemFileType>& recordlist)
{
    recordlist.resize(1);
    recordlist[0].DeviceID = m_agent->getDeviceId(m_ch);
    recordlist[0].Name = "Camera1";
    recordlist[0].StartTime = "2025-08-21T12:00:00";
    recordlist[0].EndTime = "2025-08-21T16:00:00";
    recordlist[0].Secrecy = 0;
    return true;
}

DevRecordQuery::DevRecordQuery(MANSCDPAgent *agent)
    : RecordQuery(agent)
{}

DevRecordQuery::~DevRecordQuery()
{}

std::unique_ptr<RecordQuery::Handle> DevRecordQuery::create(int32_t ch, const RecordInfoQuery::Request &req)
{
    return std::unique_ptr<RecordQuery::Handle>(new DevHandle(m_agent, ch));
}