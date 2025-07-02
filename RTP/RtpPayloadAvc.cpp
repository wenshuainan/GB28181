#include <string.h>
#include "RtpPayloadAvc.h"

RtpPayloadAvc::RtpPayloadAvc()
{
    startBit = 0;
    endBit = 0;
    bNaluHeaderNextTime = false;
}

RtpPayloadAvc::~RtpPayloadAvc()
{}

unsigned char RtpPayloadAvc::makeFUAIndicator()
{
    unsigned char indicator = 0;

    indicator = naluHeader & 0x07;
    indicator |= 28 << 3;

    return indicator;
}

unsigned char RtpPayloadAvc::makeFUAHeader()
{
    unsigned char header = 0;

    header = startBit;
    header |= endBit << 1;
    header |= (naluHeader & 0xF8);

    return header;
}

bool RtpPayloadAvc::makeFUA(RtpPacket& packet)
{
    unsigned char indicator = makeFUAIndicator();
    unsigned char header = makeFUAHeader();

    packet.write(&indicator, 1, 0);
    packet.write(&header, 1, 1);

    return true;
}

int RtpPayloadAvc::format(char *data, int len, RtpPacket& packet)
{
    int freeSpace = packet.getFreeSpace();

    if (tailLen + len <= sizeof(lastTimeTail))
    {
        memcpy(lastTimeTail + tailLen, data, len);
        tailLen += len;

        return len;
    }

    if (tailLen == sizeof(lastTimeTail))
    {
        if (len > 0 && lastTimeTail[0] == 0x00 && lastTimeTail[1] == 0x00 && lastTimeTail[2] == 0x00 && data[0] == 0x01)
        {
            endBit = 1;
            makeFUA(packet);
            packet.setFinished();
            tailLen = 0;
            return 1; // 1 byte data[0]
        }
        else if (len > 1 && lastTimeTail[1] == 0x00 && lastTimeTail[2] == 0x00 && data[0] == 0x00 && data[1] == 0x01)
        {
            if (freeSpace >= 1)
            {
                packet.write(lastTimeTail, 1);
                endBit = 1;
                makeFUA(packet);
                packet.setFinished();
                tailLen = 0;
                return 2; // 2 byte data[0] data[1]
            }
            else
            {
                packet.setFinished();
                return 0;
            }
        }
        else if (len > 2 && lastTimeTail[2] == 0x00 && data[0] == 0x00 && data[1] == 0x00 && data[2] == 0x01)
        {
            if (freeSpace >= 2)
            {
                packet.write(lastTimeTail, 2);
                endBit = 1;
                makeFUA(packet);
                packet.setFinished();
                tailLen = 0;
                return 3; // 3 byte data[0] data[1] data[2]
            }
            else
            {
                packet.setFinished();
                return 0;
            }
        }
        else
        {
            if (freeSpace >= tailLen)
            {
                packet.write(lastTimeTail, tailLen);
                tailLen = 0;
            }
        }
    }

    if (bNaluHeaderNextTime)
    {
        naluHeader = data[0];
    }

    /* find start code: 0x00 0x00 0x00 0x01 */
    int i;
    for (i = 0; i < len - 3 && i < freeSpace; i++)
    {
        if (data[i] == 0x00 && data[i+1] == 0x00
            && data[i+2] == 0x00 && data[i+3] == 0x01)
        {
            endBit = 1;
            break;
        }
    }

    packet.write(data, i);
    makeFUA(packet);

    if (endBit)
    {
        startBit = 1;
        endBit = 0;
        packet.setFinished();
        if (i + 4 < len)
        {
            naluHeader = data[i+4];
        }
        else
        {
            bNaluHeaderNextTime = true;
        }
        tailLen = 0;

        return i + 3;
    }
    else
    {
        startBit = 0;
        if (i == len - 3)
        {
            memcpy(lastTimeTail, data + i, 3);
            tailLen = 3;
            return len;
        }
        if (i == freeSpace)
        {
            packet.setFinished();
            return i;
        }
        return i;
    }
}
