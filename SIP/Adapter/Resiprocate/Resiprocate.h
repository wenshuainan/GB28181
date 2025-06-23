#ifndef RESIPROCATE_ADAPTER_H
#define RESIPROCATE_ADAPTER_H

#include "Adapter.h"

class Resiprocate : public SIPAdapter
{
public:
    Resiprocate();
    virtual ~Resiprocate();

    virtual bool init();
    virtual bool recv(Header& header, std::string& body);
    virtual bool send(const Header& header, const std::string& body);
    virtual bool genReqHeader(const std::string& method, Header& req);
    virtual bool genResHeader(const Header& req, int code, const std::string& reasonPhrase, Header& res);
};

#endif