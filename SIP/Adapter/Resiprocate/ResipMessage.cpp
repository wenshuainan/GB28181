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

const std::string& SipGenericMessage::getMethod() const
{
    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        return m_adapter->instance->methodStr().toString();
    }
    else
    {
        static std::string empty;
        return empty;
    }
}

const std::string& SipGenericMessage::getRequestUri() const
{
    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        return m_adapter->instance->header(resip::h_RequestLine).uri().getAor().toString();
    }
    else
    {
        static std::string empty;
        return empty;
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

const std::string& SipGenericMessage::getReasonPhrase() const
{
    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        return m_adapter->instance->header(resip::h_StatusLine).reason().toString();
    }
    else
    {
        static std::string empty;
        return empty;
    }
}

const std::string& SipGenericMessage::getFieldValue(const std::string& name) const
{
    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        if (name == "Content-Type")
        {
            return m_adapter->instance->header(resip::h_ContentType).getHeaderField().getBuffer();
        }
        else if (name == "Content-Length")
        {
            return m_adapter->instance->header(resip::h_ContentLength).getHeaderField().getBuffer();
        }
        else if (name == "Call-ID")
        {
            // return m_adapter->instance->header(resip::h_CallId).getHeaderField().getBuffer();
            std::string tmp = m_adapter->instance->header(resip::h_CallId).value().toString();
            printf("<<<<<< call-id=[%s]\n", tmp.c_str());
            return tmp;
        }
    }

    static std::string empty;
    return empty;
}

const std::string& SipGenericMessage::getParameterValue(const std::string& fieldName, const std::string& parameterName) const
{}

const std::string& SipGenericMessage::getBody() const
{
    if (m_adapter != nullptr && m_adapter->instance != nullptr)
    {
        return m_adapter->instance->getRawBody().getBuffer();
    }
    else
    {
        static std::string empty;
        return empty;
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
        if (name == "Authorization")
        {
            m_adapter->instance->addHeader(resip::Headers::Authorization, headerName, headerLen, start, len);
        }
        else if (name == "Content-Type")
        {
            m_adapter->instance->addHeader(resip::Headers::ContentType, headerName, headerLen, start, len);
        }
        else if (name == "Content-Length")
        {
            m_adapter->instance->addHeader(resip::Headers::ContentLength, headerName, headerLen, start, len);
        }
        else
        {
            m_adapter->instance->addHeader(resip::Headers::UNKNOWN, headerName, headerLen, start, len);
        }
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