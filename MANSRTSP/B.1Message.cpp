#include <iostream>
#include <ctype.h>
#include <algorithm>
#include "B.1Message.h"

using namespace MANSRTSP;

int32_t StartLine::parseMethod(const char* data, int32_t size)
{
    int32_t i = 0;
    while (i < size && !isblank(data[i]))
    {
        m_method.push_back(data[i]);
        i++;
    }
    m_method.erase(std::remove_if(m_method.begin(), m_method.end(), ::isspace), m_method.end());
    return std::min(i + 1, size);
}

int32_t StartLine::parseVersion(const char* data, int32_t size)
{
    int32_t i = 0;
    while (i + 1 < size && !(data[i] == '\r' && data[i+1] == '\n'))
    {
        m_version.push_back(data[i]);
        i++;
    }
    m_version.erase(std::remove_if(m_version.begin(), m_version.end(), ::isspace), m_version.end());
    return std::min(i + 2, size);
}

int32_t StartLine::parse(const char* data, int32_t size)
{
    int32_t i = 0;
    i = parseMethod(data, size);
    i += parseVersion(data + i, size - i);
    std::cout << "├── STARTLINE" << std::endl;
    std::cout << "│   ├── METHOD=" << m_method << std::endl;
    std::cout << "│   └── VERSION=" << m_version << std::endl;
    return std::min(i, size);
}

bool StartLine::encode(std::string& data) const
{
    data += m_version;
    data += " ";
    data += std::to_string(m_code);
    data += " ";
    data += m_reason;
    data += "\r\n";
    return true;
}

int32_t Header::Parameter::parseName(const char* data, int32_t size)
{
    int32_t i = 0;
    while (i < size && data[i] != '=')
    {
        m_name.push_back(data[i]);
        i++;
    }
    m_name.erase(std::remove_if(m_name.begin(), m_name.end(), ::isspace), m_name.end());
    return std::min(i + 1, size);
}

int32_t Header::Parameter::parseValue(const char* data, int32_t size)
{
    int32_t i = 0;
    while ((i < size && data[i] != ';')
            && (i + 1 < size && !(data[i] == '\r' && data[i+1] == '\n')))
    {
        m_value.push_back(data[i]);
        i++;
    }
    m_value.erase(std::remove_if(m_value.begin(), m_value.end(), ::isspace), m_value.end());

    if (data[i] == ';')
    {
        return std::min(i + 1, size);
    }
    else
    {
        return std::min(i, size);
    }
}

int32_t Header::Parameter::parse(const char* data, int32_t size)
{
    int32_t i = 0;
    i = parseName(data, size);
    i += parseValue(data + i, size - i);
    return std::min(i, size);
}

bool Header::Parameter::encode(std::string& data) const
{
    data += m_name;
    data += "=";
    data += m_value;
    return true;
}

int32_t Header::parseName(const char* data, int32_t size)
{
    int32_t i = 0;
    while (i < size && data[i] != ':')
    {
        m_name.push_back(data[i]);
        i++;
    }
    m_name.erase(std::remove_if(m_name.begin(), m_name.end(), ::isspace), m_name.end());
    return std::min(i + 1, size);
}

int32_t Header::parseValue(const char* data, int32_t size)
{
    int32_t i = 0;
    while ((i < size && data[i] != ';')
            && (i < size && data[i] != '=')
            && (i + 1 < size && !(data[i] == '\r' && data[i+1] == '\n')))
    {
        m_value.push_back(data[i]);
        i++;
    }
    m_value.erase(std::remove_if(m_value.begin(), m_value.end(), ::isspace), m_value.end());

    if (data[i] == ';')
    {
        i += 1;
        i += parseParameters(data + i, size - i);
        return std::min(i + 1, size);
    }
    else if (data[i] == '=')
    {
        m_value.clear();
        i = parseParameters(data, size); // no value, begin from data again
        return std::min(i, size);
    }
    else
    {
        return std::min(i + 2, size);
    }
}

int32_t Header::parseParameters(const char* data, int32_t size)
{
    int32_t i = 0;
    while (i + 1 < size && !(data[i] == '\r' && data[i+1] == '\n'))
    {
        m_parameters.push_back(Parameter());
        i += m_parameters.back().parse(data + i, size - i);
    }
    return std::min(i + 2, size);
}

int32_t Header::parse(const char* data, int32_t size)
{
    int32_t i = 0;
    i = parseName(data, size);
    i += parseValue(data + i, size - i);
    if (i < size)
    {
        std::cout << "├── HEADER" << std::endl;
        std::cout << "│   ├── NAME=" << m_name << std::endl;
        std::cout << "│   " << (m_parameters.empty() ? "└── " : "├── ") << "VALUE=" << m_value << std::endl;
    }
    else
    {
        std::cout << "└── HEADER" << std::endl;
        std::cout << "    ├── NAME=" << m_name << std::endl;
        std::cout << "    " << (m_parameters.empty() ? "└── " : "├── ") << "VALUE=" << m_value << std::endl;
    }
    
    for (std::size_t k = 0; k < m_parameters.size(); k++)
    {
        if (k < m_parameters.size() - 1)
        {
            std::cout << (i < size ? "│" : " ") << "   ├── PARAMETER" << std::endl;
            std::cout << (i < size ? "│" : " ") << "   │   ├── NAME=" << m_parameters[k].m_name << std::endl;
            std::cout << (i < size ? "│" : " ") << "   │   └── VALUE=" << m_parameters[k].m_value << std::endl;
        }
        else
        {
            std::cout << (i < size ? "│" : " ") << "   └── PARAMETER" << std::endl;
            std::cout << (i < size ? "│" : " ") << "       ├── NAME=" << m_parameters[k].m_name << std::endl;
            std::cout << (i < size ? "│" : " ") << "       └── VALUE=" << m_parameters[k].m_value << std::endl;
        }
    }
    
    return std::min(i, size);
}

bool Header::encode(std::string& data) const
{
    data += m_name;
    data += ": ";
    data += m_value;
    std::size_t size = data.size();
    for (auto& param : m_parameters)
    {
        if (size > 2 && (data[size-2] != ':' && data[size-1] != ' '))
        {
            data += "; ";
        }
        param.encode(data);
    }
    data += "\r\n";
    return true;
}

int32_t Message::parse(const char* data, int32_t size)
{
    int32_t i = 0;
    i = m_startline.parse(data, size);
    while (i + 1 < size && !(data[i] == '\r' && data[i+1] == '\n'))
    {
        m_headers.push_back(Header());
        i += m_headers.back().parse(data + i, size - i);
    }

    if (i + 2 < size)
    {
        i += 2;
    }
    else
    {
        i = size;
    }

    if (i < size)
    {
        m_body = data + i;
        i = size;
    }

    return std::min(i, size);
}

bool Message::encode(std::string& data) const
{
    m_startline.encode(data);
    for (auto& header : m_headers)
    {
        header.encode(data);
    }

    data += "\r\n";

    if (!m_body.empty())
    {
        data += m_body;
    }

    std::cout << "encoded:" << std::endl;
    std::cout << data << std::endl;

    return true;
}