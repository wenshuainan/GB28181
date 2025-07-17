#include <iostream>
#include <ctype.h>
#include <algorithm>
#include "B.1MANSRTSP.h"

StartLine::StartLine()
{}

StartLine::~StartLine()
{}

int32_t StartLine::parseMethod(const char* data, int32_t size)
{
    int32_t i = 0;
    while (i < size && !isblank(data[i]))
    {
        // std::cout << i << data[i] << "-->[" << m_method << "]" << m_method.length() << std::endl;
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
    std::cout << "startline-" << std::endl;
    std::cout << "\tmethod=" << m_method << std::endl;
    std::cout << "\tversion=" << m_version << std::endl;
    return std::min(i, size);
}

bool StartLine::encode(std::string& data)
{
    data += m_method;
    data += " ";
    data += m_version;
    data += "\r\n";
    return true;
}

Header::Parameter::Parameter()
{}

Header::Parameter::~Parameter()
{}

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
            || (i + 1 < size && !(data[i] == '\r' && data[i+1] == '\n')))
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
        return std::min(i + 2, size);
    }
}

int32_t Header::Parameter::parse(const char* data, int32_t size)
{
    int32_t i = 0;
    i = parseName(data, size);
    i += parseValue(data + i, size - i);
    return std::min(i, size);
}

bool Header::Parameter::encode(std::string& data)
{
    data += m_name;
    data += "=";
    data += m_value;
    return true;
}

Header::Header()
{}

Header::~Header()
{}

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
    std::cout << __FILE__ << ":" << __LINE__ << "data=" << data << " size=" << size << std::endl;
    int32_t i = 0;
    while ((i < size && data[i] != ';')
            && (i < size && data[i] != '=')
            && (i + 1 < size && !(data[i] == '\r' && data[i+1] == '\n')))
    {
        std::cout << i << data[i] << "-->[" << m_value << "]" << m_value.length() << std::endl;
        m_value.push_back(data[i]);
        i++;
    }
    m_value.erase(std::remove_if(m_value.begin(), m_value.end(), ::isspace), m_value.end());

    if (data[i] == ';')
    {
        return std::min(i + 1, size);
    }
    else if (data[i] == '=')
    {
        m_value.clear();
        return 0;
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
    return std::min(i, size);
}

int32_t Header::parse(const char* data, int32_t size)
{
    int32_t i = 0;
    std::cout << __FILE__ << ":" << __LINE__ << std::endl;
    i = parseName(data, size);
    std::cout << __FILE__ << ":" << __LINE__ << std::endl;
    i += parseValue(data + i, size - i);
    std::cout << __FILE__ << ":" << __LINE__ << "> data=" << data + i << " size=" << size << std::endl;
    i += parseParameters(data + i, size - i);
    std::cout << "header-" << std::endl;
    std::cout << "\tname=" << m_name << std::endl;
    std::cout << "\tvalue=" << m_value << std::endl;
    for (auto param : m_parameters)
    {
        std::cout << "\tparameter-" << std::endl;
        std::cout << "\t\tname=" << param.m_name << std::endl;
        std::cout << "\t\tvalue=" << param.m_value << std::endl;
    }
    
    return std::min(i, size);
}

bool Header::encode(std::string& data)
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

MANSRTSP::MANSRTSP()
{}

MANSRTSP::~MANSRTSP()
{}

int32_t MANSRTSP::parse(const char* data, int32_t size)
{
    int32_t i = 0;
    std::cout << __FILE__ << ":" << __LINE__ << std::endl;
    i = m_startline.parse(data, size);
    std::cout << __FILE__ << ":" << __LINE__ << std::endl;
    while (i + 1 < size && !(data[i] == '\r' && data[i+1] == '\n'))
    {
        m_headers.push_back(Header());
        std::cout << __FILE__ << ":" << __LINE__ << std::endl;
        i += m_headers.back().parse(data + i, size - i);
        std::cout << __FILE__ << ":" << __LINE__ << std::endl;
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

bool MANSRTSP::encode(std::string& data)
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

    return true;
}