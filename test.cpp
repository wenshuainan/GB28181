#include <stdio.h>
#include <unistd.h>
#include "UA.h"

int main()
{
    UA ua;

    std::string data = "<Control> \
                        <CmdType>DeviceControl</CmdType> \
                        <SN>99</SN> \
                        <DeviceID>123456</DeviceID> \
                        <PTZCmd>66666666</PTZCmd> \
                        <PTZCmdParams> \
                        <PresetName>abc</PresetName> \
                        <CruiseTrackName>def</CruiseTrackName> \
                        </PTZCmdParams> \
                        </Control>";

    ua.read(data);

    while (1)
    {
        sleep(1);
    }

    return 0;
}