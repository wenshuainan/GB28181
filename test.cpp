#include <stdio.h>
#include <unistd.h>
#include "UA.h"

int main()
{
    UA ua;

    UA::Info info;
    ua.start(info);

    while (1)
    {
        sleep(1);
    }

    return 0;
}