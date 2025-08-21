#ifndef MANSRTSP_MESSAGE_H
#define MANSRTSP_MESSAGE_H

#include <vector>
#include <string>

namespace MANSRTSP
{

class StartLine
{
private:
    std::string m_method;
    std::string m_version;
    int32_t m_code;
    std::string m_reason;

public:
    StartLine() {}
    StartLine(int32_t code, const std::string& version)
        : m_version(version), m_code(code)
    {
        switch (m_code)
        {
        case 200:
            m_reason = "OK";
            break;
        
        default:
            break;
        }
    };

private:
    int32_t parseMethod(const char* data, int32_t size);
    int32_t parseVersion(const char* data, int32_t size);

public:
    int32_t parse(const char* data, int32_t size);
    bool encode(std::string& data) const;
    const std::string& getMethod() const { return m_method; }
    const std::string& getVersion() const { return m_version; }
    StartLine& operator=(const StartLine& other)
    {
        this->m_method = other.m_method;
        this->m_version = other.m_version;
        this->m_code = other.m_code;
        this->m_reason = other.m_reason;
        return *this;
    }
};

class Header
{
public:
    class Parameter
    {
        friend Header;
        
    private:
        std::string m_name;
        std::string m_value;

    private:
        int32_t parseName(const char* data, int32_t size);
        int32_t parseValue(const char* data, int32_t size);

    public:
        int32_t parse(const char* data, int32_t size);
        bool encode(std::string& data) const;
        const std::string& getName() const { return m_name; }
        const std::string& getValue() const { return m_value; }
        int32_t getInt() const;
        float getFloat() const;
        Parameter& operator=(const Parameter& other)
        {
            this->m_name = other.m_name;
            this->m_value = other.m_value;
            return *this;
        }
    };
    
private:
    std::string m_name;
    std::string m_value;
    std::vector<Parameter> m_parameters;

private:
    int32_t parseName(const char* data, int32_t size);
    int32_t parseValue(const char* data, int32_t size);
    int32_t parseParameters(const char* data, int32_t size);

public:
    int32_t parse(const char* data, int32_t size);
    bool encode(std::string& data) const;
    const std::string& getName() const { return m_name; }
    const std::string& getValue() const { return m_value; }
    int32_t getInt() const;
    float getFloat() const;
    const Parameter* getParameter(const char* name) const;
    bool isExist(const char* param) const;
    Header& operator=(const Header& other)
    {
        this->m_name = other.m_name;
        this->m_value = other.m_value;
        this->m_parameters = other.m_parameters;
        return *this;
    }
};

class Message
{
private:
    StartLine m_startline;
    std::vector<Header> m_headers;
    std::string m_body;

public:
    Message() {}
    Message(const Message& req, int32_t code)
        : m_startline(code, req.getStartLine().getVersion())
        , m_headers(req.getHeaders())
    {}

public:
    int32_t parse(const char* data, int32_t size);
    bool encode(std::string& data) const;
    const StartLine& getStartLine() const { return m_startline; }
    const Header* getHeader(const char* name) const;
    const std::vector<Header>& getHeaders() const { return m_headers; }
    bool isExist(const char* name) const;
    Message& operator=(const Message& other)
    {
        this->m_startline = other.m_startline;
        this->m_headers = other.m_headers;
        this->m_body = other.m_body;
        return *this;
    }
};

}

#endif