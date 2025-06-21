#include "Adapter.h"
#include "reSIProcate.h"

const std::string& HeaderField::getParameter(const std::string& name) const
{
    auto it = m_parameter.find(name);
    if (it != m_parameter.end())
    {
        return it->second;
    }
    else
    {
        static std::string empty;
        return empty;
    }
}

bool HeaderField::addParameter(std::string& name, std::string& value)
{
    auto pair = m_parameter.insert(std::pair<std::string, std::string>(name, value));
    return pair.second;
}

bool Header::setRequestLine(const RequestLine& requestLine)
{
    m_requestLine = requestLine;
    m_type = Request;
    return true;
}

bool Header::setRequestLine(const std::string& method, const std::string& requestUri)
{
    m_requestLine = RequestLine(method, requestUri);
    m_type = Request;
    return true;
}

bool Header::setStatusLine(const StatusLine& statusLine)
{
    m_statusLine = statusLine;
    m_type = Response;
    return true;
}

bool Header::setStatusLine(int code, const std::string& reasonPhrase)
{
    m_statusLine = StatusLine(code, reasonPhrase);
    m_type = Response;
    return true;
}

void Header::addField(const std::string& name, const std::vector<HeaderField>& fields)
{
    m_fields.insert(std::pair<std::string, std::vector<HeaderField>>(name, fields));
}

void Header::addField(const std::string& name, const HeaderField& field)
{
    m_fields.insert(std::pair<std::string, std::vector<HeaderField>>(name, std::vector<HeaderField>({field})));
}

void Header::addField(const std::string& name, const std::string& value)
{
    m_fields.insert(std::pair<std::string, std::vector<HeaderField>>(name, std::vector<HeaderField>({HeaderField(name, value)})));
}

const HeaderField& Header::getField(const std::string& name) const
{
    auto it = m_fields.find(name);
    if (it != m_fields.end())
    {
        return it->second[0];
    }
    else
    {
        static HeaderField empty("", "");
        return empty;
    }
}

SIPAdapter* SIPAdapter::create()
{
    return new reSIProcate();
}