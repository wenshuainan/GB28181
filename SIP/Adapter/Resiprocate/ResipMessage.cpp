#include <strings.h>
#include "SipAdapter.h"
#include "resip/stack/SipMessage.hxx"
#include "ResipUserAgent.h"

SipGenericMessage::SipGenericMessage()
    : m_adapter(std::make_shared<SipMessageAdapter>())
{}

SipGenericMessage::~SipGenericMessage()
{}

const std::shared_ptr<SipMessageAdapter>& SipGenericMessage::getAdapter() const
{
    return m_adapter;
}

SipGenericMessage::Type SipGenericMessage::getType() const
{
    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        return m_adapter->instance->isRequest() ?
                SipGenericMessage::Type::Request
                : SipGenericMessage::Type::Response;
    }
}

const char* SipGenericMessage::getMethod() const
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

const char* SipGenericMessage::getRequestUri() const
{
    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        return m_adapter->instance->header(resip::h_RequestLine).uri().getAor().c_str();
    }
    else
    {
        return "";
    }
}

int SipGenericMessage::getCode() const
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

const char* SipGenericMessage::getReasonPhrase() const
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

const char* SipGenericMessage::getFieldValue(const std::string& name) const
{
    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        if (strcasecmp(name.c_str(), "Content-Type") == 0)
        {
            return m_adapter->instance->header(resip::h_ContentType).getHeaderField().getBuffer();
        }
        else if (strcasecmp(name.c_str(), "Content-Length") == 0)
        {
            return m_adapter->instance->header(resip::h_ContentLength).comment().c_str();
        }
        else if (strcasecmp(name.c_str(), "Call-ID") == 0)
        {
            return m_adapter->instance->header(resip::h_CallId).value().c_str();
        }
    }

    return "";
}

const char* SipGenericMessage::getParameterValue(const std::string& fieldName, const std::string& parameterName) const
{}

const char* SipGenericMessage::getBody() const
{
    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        return m_adapter->instance->getRawBody().getBuffer();
    }
    else
    {
        return "";
    }
}

void SipGenericMessage::setAdapter(const SipMessageAdapter& adapter)
{
    if (m_adapter != nullptr)
    {
        m_adapter->instance = adapter.instance;
    }
}

void SipGenericMessage::addField(const std::string& name, const std::string& value)
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

void SipGenericMessage::addParameter(const std::string& fieldName, const std::string& parameterName, const std::string& value)
{}

void SipGenericMessage::setBody(const std::string& body)
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

void SipGenericMessage::print() const
{
    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        m_adapter->instance->encode(std::cout);
    }
}