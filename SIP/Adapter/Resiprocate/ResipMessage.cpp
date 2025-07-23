#include <strings.h>
#include <iostream>
#include <stdlib.h>
#include "SipAdapter.h"
#include "resip/stack/SipMessage.hxx"
#include "resip/stack/OctetContents.hxx"
#include "resip/stack/SdpContents.hxx"
#include "resip/stack/PlainContents.hxx"
#include "ResipUserAgent.h"
#include "MANSCDPContents.h"

SipUserMessage::SipUserMessage()
    : m_adapter(std::make_shared<SipAdapterMessage>())
{}

SipUserMessage::~SipUserMessage()
{}

void SipUserMessage::print() const
{
    if (m_adapter == nullptr || m_adapter->instance == nullptr)
    {
        return;
    }

    m_adapter->instance->encode(std::cout);
}

bool SipUserMessage::setAdapter(const SipAdapterMessage& adapter)
{
    if (m_adapter == nullptr)
    {
        return false;
    }
    
    m_adapter->instance = adapter.instance;
    return true;
}

const std::shared_ptr<SipAdapterMessage>& SipUserMessage::getAdapter() const
{
    return m_adapter;
}

const char* SipUserMessage::getMethod() const
{
    if (m_adapter == nullptr || m_adapter->instance == nullptr)
    {
        return "";
    }

    const std::shared_ptr<resip::SipMessage>& instance = m_adapter->instance;
    if (!instance->isRequest())
    {
        return "";
    }

    return instance->methodStr().c_str();
}

int SipUserMessage::getCode() const
{
    if (m_adapter == nullptr || m_adapter->instance == nullptr)
    {
        return 0;
    }

    const std::shared_ptr<resip::SipMessage>& instance = m_adapter->instance;
    if (!instance->isResponse())
    {
        return 0;
    }
    
    return instance->header(resip::h_StatusLine).statusCode();
}

const char* SipUserMessage::getReasonPhrase() const
{
    if (m_adapter == nullptr || m_adapter->instance == nullptr)
    {
        return "";
    }

    const std::shared_ptr<resip::SipMessage>& instance = m_adapter->instance;
    if (!instance->isResponse())
    {
        return "";
    }
    
    return instance->header(resip::h_StatusLine).reason().c_str();
}

const char* SipUserMessage::getUriUser() const
{
    if (m_adapter == nullptr || m_adapter->instance == nullptr)
    {
        return "";
    }

    const std::shared_ptr<resip::SipMessage>& instance = m_adapter->instance;
    if (!instance->isRequest())
    {
        return "";
    }

    return instance->header(resip::h_RequestLine).uri().user().c_str();
}

int32_t SipUserMessage::getSdpSessionVersion() const
{
    if (m_adapter == nullptr || m_adapter->instance == nullptr)
    {
        return 0;
    }

    const std::shared_ptr<resip::SipMessage>& instance = m_adapter->instance;
    resip::Contents *contents = instance->getContents();
    if (contents == nullptr)
    {
        return 0;
    }
    
    resip::SdpContents *sdp = dynamic_cast<resip::SdpContents*>(contents);
    if (sdp == nullptr)
    {
        return 0;
    }
    else
    {
        const resip::SdpContents::Session &session = sdp->session();
        return session.version();
    }
}

const char* SipUserMessage::getSdpSessionOwner() const
{
    return "";
}

const char* SipUserMessage::getSdpSessionName() const
{
    if (m_adapter == nullptr || m_adapter->instance == nullptr)
    {
        return "";
    }

    const std::shared_ptr<resip::SipMessage>& instance = m_adapter->instance;
    resip::Contents *contents = instance->getContents();
    if (contents == nullptr)
    {
        return "";
    }
    
    const resip::SdpContents *sdp = dynamic_cast<resip::SdpContents*>(contents);
    if (sdp == nullptr)
    {
        return "";
    }
    else
    {
        const resip::SdpContents::Session &session = sdp->session();
        return session.name().c_str();
    }
}

const char* SipUserMessage::getSdpSessionIpv4() const
{
    if (m_adapter == nullptr || m_adapter->instance == nullptr)
    {
        return "";
    }

    const std::shared_ptr<resip::SipMessage>& instance = m_adapter->instance;
    resip::Contents *contents = instance->getContents();
    if (contents == nullptr)
    {
        return "";
    }
    
    resip::SdpContents *sdp = dynamic_cast<resip::SdpContents*>(contents);
    if (sdp == nullptr)
    {
        return "";
    }
    
    const resip::SdpContents::Session &session = sdp->session();
    if (session.isConnection())
    {
        const resip::SdpContents::Session::Connection &connection = session.connection();
        return (connection.getAddressType() == resip::SdpContents::IP4)
                ? connection.getAddress().c_str()
                : "";
    }
    else
    {
        return "";
    }
}

const char* SipUserMessage::getSdpSessionUri() const
{
    if (m_adapter == nullptr || m_adapter->instance == nullptr)
    {
        return "";
    }

    const std::shared_ptr<resip::SipMessage>& instance = m_adapter->instance;
    resip::Contents *contents = instance->getContents();
    if (contents == nullptr)
    {
        return "";
    }
    
    resip::SdpContents *sdp = dynamic_cast<resip::SdpContents*>(contents);
    if (sdp == nullptr)
    {
        return "";
    }
    
    // const resip::SdpContents::Session &session = sdp->session();
    return "";
}

time_t SipUserMessage::getSdpTimeStart() const
{
    if (m_adapter == nullptr || m_adapter->instance == nullptr)
    {
        return 0;
    }
    
    const std::shared_ptr<resip::SipMessage>& instance = m_adapter->instance;
    resip::Contents *contents = instance->getContents();
    if (contents == nullptr)
    {
        return 0;
    }
    
    resip::SdpContents *sdp = dynamic_cast<resip::SdpContents*>(contents);
    if (sdp == nullptr)
    {
        return 0;
    }
    
    const resip::SdpContents::Session &session = sdp->session();
    const std::list<resip::SdpContents::Session::Time>& times = session.getTimes();
    if (times.size() == 0)
    {
        return 0;
    }
    
    return times.front().getStart();
}

time_t SipUserMessage::getSdpTimeEnd() const
{
    if (m_adapter == nullptr || m_adapter->instance == nullptr)
    {
        return 0;
    }
    
    const std::shared_ptr<resip::SipMessage>& instance = m_adapter->instance;
    resip::Contents *contents = instance->getContents();
    if (contents == nullptr)
    {
        return 0;
    }
    
    resip::SdpContents *sdp = dynamic_cast<resip::SdpContents*>(contents);
    if (sdp == nullptr)
    {
        return 0;
    }
    
    const resip::SdpContents::Session &session = sdp->session();
    const std::list<resip::SdpContents::Session::Time>& times = session.getTimes();
    if (times.size() == 0)
    {
        return 0;
    }
    
    return times.front().getStop();
}

int32_t SipUserMessage::getSdpMediaNum() const
{
    if (m_adapter == nullptr || m_adapter->instance == nullptr)
    {
        return 0;
    }
    
    const std::shared_ptr<resip::SipMessage>& instance = m_adapter->instance;
    resip::Contents *contents = instance->getContents();
    if (contents == nullptr)
    {
        return 0;
    }

    resip::SdpContents *sdp = dynamic_cast<resip::SdpContents*>(contents);
    if (sdp == nullptr)
    {
        return 0;
    }
    
    const resip::SdpContents::Session &session = sdp->session();
    const resip::SdpContents::Session::MediumContainer &container = session.media();
    return container.size();
}

const char* SipUserMessage::getSdpMediaType(int32_t index) const
{
    if (index < 0)
    {
        return "";
    }

    if (m_adapter == nullptr || m_adapter->instance == nullptr)
    {
        return "";
    }

    const std::shared_ptr<resip::SipMessage>& instance = m_adapter->instance;
    resip::Contents *contents = instance->getContents();
    if (contents == nullptr)
    {
        return "";
    }

    resip::SdpContents *sdp = dynamic_cast<resip::SdpContents*>(contents);
    if (sdp == nullptr)
    {
        return "";
    }

    const resip::SdpContents::Session &session = sdp->session();
    const resip::SdpContents::Session::MediumContainer &container = session.media();
    if ((uint32_t)index < container.size())
    {
        auto m = container.begin();
        std::advance(m, index);
        return m != container.end() ? m->name().c_str() : "";
    }
    else
    {
        return "";
    }
}

int32_t SipUserMessage::getSdpMediaPort(int32_t index) const
{
    if (m_adapter == nullptr || m_adapter->instance == nullptr)
    {
        return -1;
    }

    const std::shared_ptr<resip::SipMessage>& instance = m_adapter->instance;
    resip::Contents *contents = instance->getContents();
    if (contents == nullptr)
    {
        return -1;
    }

    resip::SdpContents *sdp = dynamic_cast<resip::SdpContents*>(contents);
    if (sdp == nullptr)
    {
        return -1;
    }

    const resip::SdpContents::Session &session = sdp->session();
    const resip::SdpContents::Session::MediumContainer &container = session.media();
    if (index >= 0 && (uint32_t)index < container.size())
    {
        auto m = container.begin();
        std::advance(m, index);
        return m != container.end() ? m->port() : -1;
    }
    else
    {
        return -1;
    }
}

const char* SipUserMessage::getSdpMediaTransport(int32_t index) const
{
    if (m_adapter == nullptr || m_adapter->instance == nullptr)
    {
        return "";
    }

    const std::shared_ptr<resip::SipMessage>& instance = m_adapter->instance;
    resip::Contents *contents = instance->getContents();
    if (contents == nullptr)
    {
        return "";
    }

    resip::SdpContents *sdp = dynamic_cast<resip::SdpContents*>(contents);
    if (sdp == nullptr)
    {
        return "";
    }

    const resip::SdpContents::Session &session = sdp->session();
    const resip::SdpContents::Session::MediumContainer &container = session.media();
    if (index < 0 || (uint32_t)index >= container.size())
    {
        return "";
    }
    auto m = container.begin();
    std::advance(m, index);
    return m != container.end() ? m->protocol().c_str() : "";
}

int32_t SipUserMessage::getSdpMediaPayloadType(int32_t index, uint16_t type[10]) const
{
    if (m_adapter == nullptr || m_adapter->instance == nullptr)
    {
        return 0;
    }

    const std::shared_ptr<resip::SipMessage>& instance = m_adapter->instance;
    resip::Contents *contents = instance->getContents();
    if (contents == nullptr)
    {
        return 0;
    }

    resip::SdpContents *sdp = dynamic_cast<resip::SdpContents*>(contents);
    if (sdp == nullptr)
    {
        return 0;
    }

    const resip::SdpContents::Session &session = sdp->session();
    const resip::SdpContents::Session::MediumContainer &container = session.media();
    if (index < 0 || (uint32_t)index >= container.size())
    {
        return 0;
    }
    int i = 0;
    auto m = container.begin();
    std::advance(m, index);
    if (m != container.end())
    {
        const std::list<Data>& formats = m->getFormats();
        for (auto f : formats)
        {
            type[i++] = atoi(f.c_str());
            if (i >= 10)
            {
                break;
            }
        }
    }
    return i;
}

const char* SipUserMessage::getSdpMediaIpv4(int32_t index) const
{
    if (m_adapter == nullptr || m_adapter->instance == nullptr)
    {
        return "";
    }

    const std::shared_ptr<resip::SipMessage>& instance = m_adapter->instance;
    resip::Contents *contents = instance->getContents();
    if (contents == nullptr)
    {
        return "";
    }

    resip::SdpContents *sdp = dynamic_cast<resip::SdpContents*>(contents);
    if (sdp == nullptr)
    {
        return "";
    }

    const resip::SdpContents::Session &session = sdp->session();
    const resip::SdpContents::Session::MediumContainer &container = session.media();
    if (index < 0 || (uint32_t)index >= container.size())
    {
        return "";
    }
    auto m = container.begin();
    std::advance(m, index);
    if (m != container.end())
    {
        const std::list<resip::SdpContents::Session::Connection>& connections = m->getMediumConnections();
        if (!connections.empty())
        {
            auto c = connections.begin();
            std::advance(c, 0);
            return (c != connections.end() && c->getAddressType() == resip::SdpContents::IP4)
                    ? c->getAddress().c_str()
                    : "";
        }
        else if (session.isConnection())
        {
            const resip::SdpContents::Session::Connection& c = session.connection();
            return (c.getAddressType() == resip::SdpContents::IP4)
                    ? c.getAddress().c_str()
                    : "";
        }
    }
    
    return "";
}

uint32_t SipUserMessage::getSdpMediaSSRC(int32_t index) const
{
    if (index < 0)
    {
        return 0;
    }

    if (m_adapter == nullptr || m_adapter->instance == nullptr)
    {
        return 0;
    }

    const std::shared_ptr<resip::SipMessage>& instance = m_adapter->instance;
    resip::Contents *contents = instance->getContents();
    if (contents == nullptr)
    {
        return 0;
    }

    resip::SdpContents *sdp = dynamic_cast<resip::SdpContents*>(contents);
    if (sdp == nullptr)
    {
        return 0;
    }

    const resip::SdpContents::Session &session = sdp->session();
    const resip::SdpContents::Session::MediumContainer &container = session.media();
    if ((uint32_t)index < container.size())
    {
        auto m = container.begin();
        std::advance(m, index);
        return m != container.end() ? m->ssrc() : 0;
    }
    else
    {
        return 0;
    }
}

bool SipUserMessage::setSdpMediaNum(int32_t num)
{
    if (num <= 0)
    {
        return false;
    }

    if (m_adapter == nullptr || m_adapter->instance == nullptr)
    {
        return false;
    }

    const std::shared_ptr<resip::SipMessage>& instance = m_adapter->instance;
    resip::Contents *contents = instance->getContents();
    if (contents == nullptr)
    {
        return false;
    }

    resip::SdpContents *sdp = dynamic_cast<resip::SdpContents*>(contents);
    if (sdp == nullptr)
    {
        return false;
    }

    resip::SdpContents::Session &session = sdp->session();
    resip::SdpContents::Session::MediumContainer &container = session.media();
    int32_t size = container.size();
    if (num <= size)
    {
        return false;
    }
    int i;
    for (i = size; i < num; i++)
    {
        session.addMedium(resip::SdpContents::Session::Medium());
    }
    return true;
}

bool SipUserMessage::setSdpMediaType(int32_t index, const char *type)
{
    if (index < 0 || type == nullptr || type[0] == 0)
    {
        return false;
    }

    if (m_adapter == nullptr || m_adapter->instance == nullptr)
    {
        return false;
    }

    const std::shared_ptr<resip::SipMessage>& instance = m_adapter->instance;
    resip::Contents *contents = instance->getContents();
    if (contents == nullptr)
    {
        return false;
    }

    resip::SdpContents *sdp = dynamic_cast<resip::SdpContents*>(contents);
    if (sdp == nullptr)
    {
        return false;
    }

    resip::SdpContents::Session &session = sdp->session();
    resip::SdpContents::Session::MediumContainer &container = session.media();
    int32_t size = container.size();
    if (index >= size)
    {
        return false;
    }
    auto i = container.begin();
    std::advance(i, index);
    i->name() = type;
    return true;
}

bool SipUserMessage::setSdpMediaPort(int32_t index, int32_t port)
{
    if (index < 0 || port < 0)
    {
        return false;
    }

    if (m_adapter == nullptr || m_adapter->instance == nullptr)
    {
        return false;
    }

    const std::shared_ptr<resip::SipMessage>& instance = m_adapter->instance;
    resip::Contents *contents = instance->getContents();
    if (contents == nullptr)
    {
        return false;
    }

    resip::SdpContents *sdp = dynamic_cast<resip::SdpContents*>(contents);
    if (sdp == nullptr)
    {
        return false;
    }

    resip::SdpContents::Session &session = sdp->session();
    resip::SdpContents::Session::MediumContainer &container = session.media();
    int32_t size = container.size();
    if (index >= size)
    {
        return false;
    }
    auto i = container.begin();
    std::advance(i, index);
    i->port() = port;
    return true;
}

bool SipUserMessage::setSdpMediaTransport(int32_t index, const char *transport)
{
    if (index < 0 || transport == nullptr ||  transport[0] == 0)
    {
        return false;
    }

    if (m_adapter == nullptr || m_adapter->instance == nullptr)
    {
        return false;
    }

    const std::shared_ptr<resip::SipMessage>& instance = m_adapter->instance;
    resip::Contents *contents = instance->getContents();
    if (contents == nullptr)
    {
        return false;
    }

    resip::SdpContents *sdp = dynamic_cast<resip::SdpContents*>(contents);
    if (sdp == nullptr)
    {
        return false;
    }

    resip::SdpContents::Session &session = sdp->session();
    resip::SdpContents::Session::MediumContainer &container = session.media();
    int32_t size = container.size();
    if (index >= size)
    {
        return false;
    }
    auto i = container.begin();
    std::advance(i, index);
    i->protocol() = transport;
    return true;
}

bool SipUserMessage::setSdpMediaPayloadType(int32_t index, int32_t type)
{
    if (index < 0)
    {
        return false;
    }

    if (m_adapter == nullptr || m_adapter->instance == nullptr)
    {
        return false;
    }

    const std::shared_ptr<resip::SipMessage>& instance = m_adapter->instance;
    resip::Contents *contents = instance->getContents();
    if (contents == nullptr)
    {
        return false;
    }

    resip::SdpContents *sdp = dynamic_cast<resip::SdpContents*>(contents);
    if (sdp == nullptr)
    {
        return false;
    }

    resip::SdpContents::Session &session = sdp->session();
    resip::SdpContents::Session::MediumContainer &container = session.media();
    int32_t size = container.size();
    if (index >= size)
    {
        return false;
    }
    auto i = container.begin();
    std::advance(i, index);
    i->addFormat(resip::Data(type));
    return true;
}

bool SipUserMessage::addSdpMediaAttr(int32_t index, const char *key, const char *value)
{
    if (index < 0 || key == nullptr || key[0] == 0)
    {
        return false;
    }

    if (m_adapter == nullptr || m_adapter->instance == nullptr)
    {
        return false;
    }

    const std::shared_ptr<resip::SipMessage>& instance = m_adapter->instance;
    resip::Contents *contents = instance->getContents();
    if (contents == nullptr)
    {
        return false;
    }

    resip::SdpContents *sdp = dynamic_cast<resip::SdpContents*>(contents);
    if (sdp == nullptr)
    {
        return false;
    }

    resip::SdpContents::Session &session = sdp->session();
    resip::SdpContents::Session::MediumContainer &container = session.media();
    int32_t size = container.size();
    if (index >= size)
    {
        return false;
    }
    auto i = container.begin();
    std::advance(i, index);
    if (value != nullptr && value[0] != 0)
    {
        i->addAttribute(key, value);
    }
    else
    {
        i->addAttribute(key);
    }
    return true;
}

bool SipUserMessage::setSdpMediaSSRC(int32_t index, uint32_t value)
{
    if (index < 0)
    {
        return false;
    }

    if (m_adapter == nullptr || m_adapter->instance == nullptr)
    {
        return false;
    }

    const std::shared_ptr<resip::SipMessage>& instance = m_adapter->instance;
    resip::Contents *contents = instance->getContents();
    if (contents == nullptr)
    {
        return false;
    }

    resip::SdpContents *sdp = dynamic_cast<resip::SdpContents*>(contents);
    if (sdp == nullptr)
    {
        return false;
    }

    resip::SdpContents::Session &session = sdp->session();
    resip::SdpContents::Session::MediumContainer &container = session.media();
    int32_t size = container.size();
    if (index >= size)
    {
        return false;
    }
    auto i = container.begin();
    std::advance(i, index);
    i->ssrc() = value;
    return true;
}

bool SipUserMessage::setExpires(int32_t expires)
{
    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        m_adapter->instance->header(resip::h_Expires).value() = expires;
        return true;
    }

    return false;
}

bool SipUserMessage::addExtensionField(const std::string& name, const std::string& value)
{
    if (name.empty() || value.empty())
    {
        return false;
    }

    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        const char *headerName = name.c_str();
        int headerLen = name.length();
        const char *start = value.c_str();
        int len = value.length();

        m_adapter->instance->addHeader(resip::Headers::UNKNOWN, headerName, headerLen, start, len);
        return true;
    }

    return false;
}
