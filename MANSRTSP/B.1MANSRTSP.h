#ifndef DESCRIPTION_H
#define DESCRIPTION_H

#include <vector>
#include <string>

class StartLine
{
private:
    std::string m_method;
    std::string m_version;
    int32_t m_code;
    std::string m_reason;

public:
    StartLine();
    ~StartLine();

private:
    int32_t parseMethod(const char* data, int32_t size);
    int32_t parseVersion(const char* data, int32_t size);

public:
    int32_t parse(const char* data, int32_t size);
    bool encode(std::string& data);
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

    public:
        Parameter();
        ~Parameter();

    private:
        int32_t parseName(const char* data, int32_t size);
        int32_t parseValue(const char* data, int32_t size);

    public:
        int32_t parse(const char* data, int32_t size);
        bool encode(std::string& data);
    };
    
private:
    std::string m_name;
    std::string m_value;
    std::vector<Parameter> m_parameters;

public:
    Header();
    ~Header();

private:
    int32_t parseName(const char* data, int32_t size);
    int32_t parseValue(const char* data, int32_t size);
    int32_t parseParameters(const char* data, int32_t size);

public:
    int32_t parse(const char* data, int32_t size);
    bool encode(std::string& data);
};

class MANSRTSP
{
private:
    StartLine m_startline;
    std::vector<Header> m_headers;
    std::string m_body;

public:
    MANSRTSP();
    // MANSRTSP(const std::string& method);
    // MANSRTSP(const MANSRTSP& req, int32_t code);
    ~MANSRTSP();

public:
    int32_t parse(const char* data, int32_t size);
    bool encode(std::string& data);
};

#endif