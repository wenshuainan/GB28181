#include <stdio.h>
#include <unistd.h>
#include "UA.h"

int main()
{
    UA ua;

    UA::Info info;
    info.sipInfo.server.domain = "192.168.0.100";
    info.sipInfo.server.transport = SipUserAgent::TRANSPORT_UDP;
    info.sipInfo.server.port = 5060;
    info.sipInfo.client.id = "101";
    info.sipInfo.client.passwd = "101";
    info.interval = 60;
    info.expire = 3600;

    ua.start(info);

    while (1)
    {
        sleep(1);
    }

    return 0;
}