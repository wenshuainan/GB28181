#ifndef RESIPROCATE_ADAPTER_H
#define RESIPROCATE_ADAPTER_H

#include "Adapter.h"
#include "UserAgent.hxx"

class Resiprocate : public SIPAdapter
{
private:
    resip::UserAgent userAgent;

public:
    Resiprocate(const SIPAdapter::Info& info);
    virtual ~Resiprocate();

    virtual bool init();
    virtual bool recv(Header& header, std::string& body);
    virtual bool send(const Header& header, const std::string& body);
    virtual bool genReqHeader(const std::string& method, const std::string& requestUri, Header& req);
    virtual bool genResHeader(const Header& req, int code, const std::string& reasonPhrase, Header& res);
};

#endif