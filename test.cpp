#include <stdio.h>
#include <unistd.h>
#include "UA.h"

int main()
{
    UA ua;

    ua.start();

    while (1)
    {
        sleep(1);
    }

    return 0;
}