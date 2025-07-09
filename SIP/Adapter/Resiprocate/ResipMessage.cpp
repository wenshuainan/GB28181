#include <strings.h>
#include <iostream>
#include "SipAdapter.h"
#include "resip/stack/SipMessage.hxx"
#include "resip/stack/OctetContents.hxx"
#include "resip/stack/SdpContents.hxx"
#include "resip/stack/PlainContents.hxx"
#include "ResipUserAgent.h"

SipMessageApp::SipMessageApp()
    : m_adapter(std::make_shared<SipMessageAdapter>())
{}

SipMessageApp::~SipMessageApp()
{}

void SipMessageApp::print() const
{
    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        m_adapter->instance->encode(std::cout);
    }
}

const std::shared_ptr<SipMessageAdapter>& SipMessageApp::getAdapter() const
{
    return m_adapter;
}

SipMessageApp::Type SipMessageApp::getType() const
{
    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        return m_adapter->instance->isRequest() ?
                SipMessageApp::Type::Request
                : SipMessageApp::Type::Response;
    }
}

const char* SipMessageApp::getMethod() const
{
    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        return m_adapter->instance->methodStr().c_str();
    }
    else
    {
        return "";
    }
}

int SipMessageApp::getCode() const
{
    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        return m_adapter->instance->header(resip::h_StatusLine).statusCode();
    }
    else
    {
        return 0;
    }
}

const char* SipMessageApp::getReasonPhrase() const
{
    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        return m_adapter->instance->header(resip::h_StatusLine).reason().c_str();
    }
    else
    {
        return "";
    }
}

const char* SipMessageApp::getContentType() const
{
    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        resip::Mime& type = m_adapter->instance->header(resip::h_ContentType);

        if (type == resip::Mime("MANSCDP", "xml"))
        {
            return "MANSCDP/xml";
        }
        else if (type == resip::Mime("application", "sdp"))
        {
            return "application/sdp";
        }
        else if (type == resip::Mime("text", "plain"))
        {
            return "text/plain";
        }
    }

    return "";
}

const char* SipMessageApp::getCallID() const
{
    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        return m_adapter->instance->header(resip::h_CallId).value().c_str();
    }
    else
    {
        return "";
    }
}

const char* SipMessageApp::getBody() const
{
    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        resip::Contents *contents = m_adapter->instance->getContents();
        if (contents)
        {
            const resip::Mime& type = contents->getType();

            if (type == resip::Mime("application", "octet-stream"))
            {
                resip::OctetContents *octet = dynamic_cast<resip::OctetContents*>(contents);
                if (octet)
                {
                    return octet->octets().c_str();
                }
            }
            else if (type == resip::Mime("application", "sdp"))
            {
            }
            else if (type == resip::Mime("text", "plain"))
            {
                resip::PlainContents *plain = dynamic_cast<resip::PlainContents*>(contents);
                if (plain)
                {
                    return plain->text().c_str();
                }
            }
        }
    }
    else
    {
        return "";
    }
}

int32_t SipMessageApp::getSdpSessionVersion() const
{
    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        resip::Contents *contents = m_adapter->instance->getContents();
        if (contents)
        {
            resip::SdpContents *sdp = dynamic_cast<resip::SdpContents*>(contents);
            if (sdp)
            {
                const resip::SdpContents::Session &session = sdp->session();
                return session.version();
            }
        }
    }

    return 0;
}

const char* SipMessageApp::getSdpSessionOwner() const
{
    return "";
}

const char* SipMessageApp::getSdpSessionName() const
{
    return "";
}

const char* SipMessageApp::getSdpSessionIpv4() const
{
    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        resip::Contents *contents = m_adapter->instance->getContents();
        if (contents)
        {
            resip::SdpContents *sdp = dynamic_cast<resip::SdpContents*>(contents);
            if (sdp)
            {
                const resip::SdpContents::Session &session = sdp->session();
                if (session.isConnection())
                {
                    const resip::SdpContents::Session::Connection &connection = session.connection();
                    return (connection.getAddressType() == resip::SdpContents::IP4)
                            ? connection.getAddress().c_str()
                            : "";
                }
            }
        }
    }

    return "";
}

int32_t SipMessageApp::getSdpMediaNum() const
{
    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        resip::Contents *contents = m_adapter->instance->getContents();
        if (contents)
        {
            resip::SdpContents *sdp = dynamic_cast<resip::SdpContents*>(contents);
            if (sdp)
            {
                const resip::SdpContents::Session &session = sdp->session();
                const resip::SdpContents::Session::MediumContainer &container = session.media();
                return container.size();
            }
        }
    }

    return 0;
}

int32_t SipMessageApp::getSdpMediaPort(int32_t index) const
{
    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        resip::Contents *contents = m_adapter->instance->getContents();
        if (contents)
        {
            resip::SdpContents *sdp = dynamic_cast<resip::SdpContents*>(contents);
            if (sdp)
            {
                const resip::SdpContents::Session &session = sdp->session();
                const resip::SdpContents::Session::MediumContainer &container = session.media();
                if (index >= 0 && index < container.size())
                {
                    auto m = container.begin();
                    std::advance(m, index);
                    return m != container.end() ? m->port() : 0;
                }
            }
        }
    }

    return 0;
}

const char* SipMessageApp::getSdpMediaTransport(int32_t index) const
{
    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        resip::Contents *contents = m_adapter->instance->getContents();
        if (contents)
        {
            resip::SdpContents *sdp = dynamic_cast<resip::SdpContents*>(contents);
            if (sdp)
            {
                const resip::SdpContents::Session &session = sdp->session();
                const resip::SdpContents::Session::MediumContainer &container = session.media();
                if (index >= 0 && index < container.size())
                {
                    auto m = container.begin();
                    std::advance(m, index);
                    return m != container.end() ? m->protocol().c_str() : "";
                }
            }
        }
    }

    return "";
}

int32_t SipMessageApp::getSdpMediaPayloadType(int32_t index) const
{
    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        resip::Contents *contents = m_adapter->instance->getContents();
        if (contents)
        {
            resip::SdpContents *sdp = dynamic_cast<resip::SdpContents*>(contents);
            if (sdp)
            {
                const resip::SdpContents::Session &session = sdp->session();
                const resip::SdpContents::Session::MediumContainer &container = session.media();
                if (index >= 0 && index < container.size())
                {
                    auto m = container.begin();
                    std::advance(m, index);
                    if (m != container.end())
                    {
                        const resip::SdpContents::Session::Medium::CodecContainer &codecs = m->codecs();
                        if (codecs.size() > 0)
                        {
                            auto c = codecs.begin();
                            std::advance(c, 0);
                            return c != codecs.end() ? c->payloadType() : 0;
                        }
                    }
                }
            }
        }
    }

    return 0;
}

const char* SipMessageApp::getSdpMediaIpv4(int32_t index) const
{
    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        resip::Contents *contents = m_adapter->instance->getContents();
        if (contents)
        {
            resip::SdpContents *sdp = dynamic_cast<resip::SdpContents*>(contents);
            if (sdp)
            {
                const resip::SdpContents::Session &session = sdp->session();
                const resip::SdpContents::Session::MediumContainer &container = session.media();
                if (index >= 0 && index < container.size())
                {
                    auto m = container.begin();
                    std::advance(m, index);
                    if (m != container.end())
                    {
                        const std::list<resip::SdpContents::Session::Connection> &connections = m->getConnections();
                        if (connections.size() > 0)
                        {
                            auto c = connections.begin();
                            std::advance(c, 0);
                            return (c != connections.end() && c->getAddressType() == resip::SdpContents::IP4)
                                    ? c->getAddress().c_str()
                                    : "";
                        }
                    }
                }
            }
        }
    }

    return "";
}

uint32_t SipMessageApp::getSdpMediaSSRC(int32_t index) const
{
    return 0;
}

void SipMessageApp::setAdapter(const SipMessageAdapter& adapter)
{
    if (m_adapter != nullptr)
    {
        m_adapter->instance = adapter.instance;
    }
}

void SipMessageApp::addField(const std::string& name, const std::string& value)
{
    if (name.empty() || value.empty())
    {
        return;
    }

    const char *headerName = name.c_str();
    int headerLen = name.length();
    const char *start = value.c_str();
    int len = value.length();

    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        resip::Headers::Type type = resip::Headers::UNKNOWN;

        if (strcasecmp(headerName, "Authorization") == 0)
        {
            type = resip::Headers::Authorization;
        }
        else if (strcasecmp(headerName, "Content-Type") == 0)
        {
            type = resip::Headers::ContentType;
        }
        else if (strcasecmp(headerName, "Content-Length") == 0)
        {
            type = resip::Headers::ContentLength;
        }
        else
        {
            type = resip::Headers::UNKNOWN;
        }

        /* 先移除现有的再添加 */
        m_adapter->instance->remove(type);
        m_adapter->instance->addHeader(type, headerName, headerLen, start, len);
    }
}

void SipMessageApp::addParameter(const std::string& fieldName, const std::string& parameterName, const std::string& value)
{}

void SipMessageApp::setBody(const std::string& body)
{
    if (body.empty())
    {
        return;
    }
    
    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        m_adapter->instance->setBody(body.c_str(), body.length());
    }
}

bool SipMessageApp::setSdp(int32_t version, const std::string& owner, const std::string& name)
{}

bool SipMessageApp::setSdpConnectionIpv4(const std::string& ipv4)
{}

bool SipMessageApp::addSdpMedia(int32_t num)
{}

bool SipMessageApp::setSdpMediaPort(int32_t index, int32_t port)
{}

bool SipMessageApp::setSdpMediaTransport(int32_t index, const std::string& transport)
{}

bool SipMessageApp::setSdpMediaPayloadType(int32_t index, int32_t payloadType)
{}