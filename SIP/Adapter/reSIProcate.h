#ifndef RESIPROCATE_ADAPTER_H
#define RESIPROCATE_ADAPTER_H

#include "Adapter.h"

class reSIProcate : public SIPAdapter
{
public:
    reSIProcate();
    virtual ~reSIProcate();

    virtual bool init();
    virtual bool recv(Header& header, std::string& body);
    virtual bool send(const Header& header, const std::string& body);
};

#endif