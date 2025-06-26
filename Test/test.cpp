#include <stdio.h>
#include <unistd.h>
#include "UA.h"

int main()
{
    UA ua;

    UA::Info info;
    info.sipInfo.server.id = "000";
    info.sipInfo.server.domain = "10.12.13.136";
    info.sipInfo.server.transport = SipUserAgent::TRANSPORT_UDP;
    info.sipInfo.server.port = 5060;
    info.sipInfo.client.id = "102";
    info.sipInfo.client.passwd = "102";
    info.sipInfo.client.expire = 86400;
    info.sipInfo.interval = 60;

    ua.start(info);

    while (1)
    {
        sleep(1);
    }

    return 0;
}