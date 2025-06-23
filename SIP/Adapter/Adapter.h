/*
 * sip adapter for resiprocate
 */

#ifndef SIP_ADAPTER_H
#define SIP_ADAPTER_H

#include <vector>
#include <string>
#include <map>
#include <algorithm>

struct CaseInsensitiveCompare {
    bool operator()(const std::string& a, const std::string& b) const
    {
        return std::lexicographical_compare(
            a.begin(), a.end(), b.begin(), b.end(),
            [](char a, char b) { return std::tolower(a) < std::tolower(b); }
        );
    }
};

class RequestLine
{
private:
    std::string m_method;
    std::string m_requestUri;
    std::string m_sipVersion;

public:
    RequestLine() : m_method(""), m_requestUri(""), m_sipVersion("") {}
    RequestLine(const std::string& method, const std::string& requestUri)
        : m_method(method), m_requestUri(requestUri)
    {
        m_sipVersion = "SIP/2.0";
    }
    ~RequestLine() {}
    const std::string& getMethod() const { return m_method; }
    const std::string& getRequestUri() const { return m_requestUri; }
};

class StatusLine
{
private:
    std::string m_sipVersion;
    int m_code;
    std::string m_reasonPhrase;

public:
    StatusLine() : m_sipVersion(""), m_code(0), m_reasonPhrase("") {}
    StatusLine(int code, const std::string& reasonPhrase)
        : m_code(code), m_reasonPhrase(reasonPhrase)
    {
        m_sipVersion = "SIP/2.0";
    }
    ~StatusLine() {}
    int getCode() const { return m_code; }
    const std::string& getReasonPhrase() const { return m_reasonPhrase; }
};

class HeaderField
{
private:
    std::string m_name;
    std::string m_value;
    std::map<std::string, std::string, CaseInsensitiveCompare> m_parameter;

public:
    HeaderField(const std::string& name, const std::string& value)
        : m_name(name), m_value(value)
    {}
    ~HeaderField() {}
    bool addParameter(std::string& name, std::string& value);
    const std::string& getName() const { return m_name; }
    const std::string& getValue() const { return m_value; }
    const std::string& getParameter(const std::string& name) const;
};

class Header
{
public:
     enum Type
     {
        Request,
        Response,
     };

private:
    Type m_type;
    RequestLine m_requestLine;
    StatusLine m_statusLine;
    std::multimap<std::string, std::vector<HeaderField>, CaseInsensitiveCompare> m_fields;

public:
    Header() {}
    ~Header() {}
    void setRequestLine(const std::string& method, const std::string& requestUri);
    void setStatusLine(int code, const std::string& reasonPhrase);
    void addField(const std::string& name, const std::vector<HeaderField>& fields);
    void addField(const std::string& name, const HeaderField& field);
    void addField(const std::string& name, const std::string& value);
    Type getType() const { return m_type; }
    const std::string& getMethod() const { return m_requestLine.getMethod(); }
    const std::string& getRequestUri() const { return m_requestLine.getRequestUri(); }
    int getCode() const { return m_statusLine.getCode(); }
    const std::string& getReasonPhrase() const { return m_statusLine.getReasonPhrase(); }
    const std::string& getValue(const std::string& name) const;
    const std::string& getParameterValue(const std::string& fieldName, const std::string& parameterName) const;
    const RequestLine& getRequestLine() const { return m_requestLine; }
    const StatusLine& getStatusLine() const { return m_statusLine; }
    const HeaderField& getField(const std::string& name) const;
};

class SIPAdapter
{
public:
    enum Transport
    {
        TRANSPORT_UDP,
        TRANSPORT_TCP,
        TRANSPORT_TLS
    };

    struct ClientInfo
    {
        std::string id;
    };

    struct ServerInfo
    {
        std::string domain;
        int port;
        std::string passwd;
    };

    struct Info
    {
        ClientInfo client;
        ServerInfo server;
        Transport transport;
    };

protected:
    Info m_info;
    
public:
    virtual bool init() = 0;
    virtual bool recv(Header& header, std::string& body) = 0;
    virtual bool send(const Header& header, const std::string& body) = 0;
    virtual bool genReqHeader(const std::string& method, const std::string& requestUri, Header& req) = 0;
    virtual bool genResHeader(const Header& req, int code, const std::string& reasonPhrase, Header& res) = 0;

public:
    SIPAdapter() {}
    virtual ~SIPAdapter() {}

public:
    static SIPAdapter* create(const Info& info);
};

#endif