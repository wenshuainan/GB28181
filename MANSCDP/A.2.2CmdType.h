#ifndef CMD_TYPE_H
#define CMD_TYPE_H

#include "A.2.1GlobalTypes.h"

class RequestType
{
public:
    virtual bool match(const std::string& name) = 0;
    virtual bool process(XMLElement& ele) = 0;
};

class ResponseType
{};

#endif