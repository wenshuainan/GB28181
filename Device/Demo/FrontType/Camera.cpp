#include "Camera.h"
#include "MANSCDP/A.3FrontDeviceControl.h"

Camera::Camera(const std::string &id, int32_t ch, Device *parent, MANSCDPAgent *agent)
    : Device(id, ch, parent)
    , MANSCDPDevice(agent)
    , m_bGuard(false)
{}

Camera::~Camera()
{}

bool Camera::controlPTZ(const PTZCommand *cmd)
{
    printf("Control PTZ pan(left=%d, right=%d) tilt(up=%d, down=%d) zoom(in=%d, out=%d)\n"
        , cmd->m_pan.left, cmd->m_pan.right, cmd->m_tilt.up, cmd->m_tilt.down, cmd->m_zoom.in, cmd->m_zoom.out);
    return true;
}

bool Camera::controlFI(const FICommand *cmd)
{
    printf("Control FI focus(near=%d, fat=%d) iris(small=%d, big=%d)\n"
        , cmd->m_focus.near, cmd->m_focus.far, cmd->m_iris.small, cmd->m_iris.big);
    return true;
}

bool Camera::startRecord(int32_t stream)
{
    printf("startRecord %d:%d\n", m_ch, stream);
    return true;
}

bool Camera::stopRecord(int32_t stream)
{
    printf("stopRecord %d:%d\n", m_ch, stream);
    return true;
}

bool Camera::setGuard()
{
    if (!m_bGuard)
    {
        return m_bGuard = true;
    }
    printf("Camera setGuard err\n");
    return false;
}

bool Camera::resetGuard()
{
    if (!m_bGuard)
    {
        return false;
    }

    printf("Camera resetGuard\n");
    m_bGuard = false;
    return true;
}

bool Camera::queryDeviceStatus(DeviceStatusQueryResponse& res)
{
    res.Online = DeviceStatusQueryResponse::ONLINE;
    res.Status = resultType::OK;
    return true;
}

bool Camera::queryCatalog(itemType& item)
{
    item.DeviceID = m_id;
    item.Name = "Camera 1";
    item.Manufacturer = "Manufacturer";
    item.Model = "Model";
    item.CivilCode = "CivilCode";
    item.Address = "Address";
    item.Parental = 1;
    item.ParentID = m_parent->getId();
    item.RegisterWay = 1;
    item.Secrecy = 0;
    item.Status = statusType::ON;

    return true;
}

bool Camera::getStatus()
{
    return true; // 状态正常
}

std::unique_ptr<RecordQuery::Handle> Camera::createHandle(const RecordInfoQuery::Request &req)
{
    return std::unique_ptr<RecordQuery::Handle>(new RecordQueryHandle(m_ch, this));
}

RecordQueryHandle::RecordQueryHandle(int32_t ch, MANSCDPDevice *device)
    : RecordQuery::Handle(ch)
    , m_device(device)
{
    printf("++++++ RecordQueryHandle\n");
}

RecordQueryHandle::~RecordQueryHandle()
{
    printf("------ RecordQueryHandle\n");
}

int32_t RecordQueryHandle::querySumNum()
{
    return 1;
}

bool RecordQueryHandle::queryRecordInfo(int32_t num, std::vector<itemFileType>& recordlist)
{
    itemFileType item;
    item.DeviceID = m_device->getId();
    item.Name = "Record 1";
    item.StartTime = "2025-08-27T00:01:00";
    item.EndTime = "2025-08-27T00:02:00";
    item.Secrecy = 0;
    recordlist.push_back(item);
    return true;
}