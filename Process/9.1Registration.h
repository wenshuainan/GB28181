#ifndef REGISTRATION_H
#define REGISTRATION_H

#include "A.2.1GlobalTypes.h"

class Registration
{
protected:
    struct DevSIPInfo
    {
        std::string id;
        int expire;
        struct
        {
            std::string domain;
            int port;
            std::string passwd;
        } server;
    };

private:
    virtual bool getDevInfo(struct DevSIPInfo& info) = 0;
    virtual bool Succeed() = 0;
    virtual bool Failed(int sipCode) = 0;
};

#endif