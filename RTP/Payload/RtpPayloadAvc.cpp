#include <string.h>
#include "RtpPayloadAvc.h"

RtpPayloadAvc::RtpPayloadAvc(RtpParticipant *participant, int32_t maxLen)
    : RtpPayload(participant, maxLen)
{
    bNaluHeaderNextTime = false;
    formated.bFirst = false;
    formated.marker = 0;
    formated.payload = nullptr;
}

RtpPayloadAvc::~RtpPayloadAvc()
{}

uint8_t RtpPayloadAvc::makeFUAIndicator(uint8_t naluHeader)
{
    uint8_t indicator = 0;

    indicator = naluHeader & 0x07;
    indicator |= 28 << 3; // FU-A = 28

    return indicator;
}

uint8_t RtpPayloadAvc::makeFUAHeader(uint8_t start, uint8_t end, uint8_t naluType)
{
    uint8_t header = 0;

    header = start;
    header |= end << 1;
    header |= naluType << 3;

    return header;
}

void RtpPayloadAvc::pushFomated()
{
    if (formated.payload == nullptr)
    {
        return;
    }

    formated.payload->at(0) = makeFUAIndicator(naluHeader);
    formated.payload->at(1) = makeFUAHeader(formated.bFirst ? 1 : 0, formated.marker, naluHeader & 0x1f);

    participant->pushPayload(formated);

    formated.bFirst = false;
    formated.marker = 0;
}

int32_t RtpPayloadAvc::format(uint8_t *data, int32_t len)
{
    if (len <= 0)
    {
        return len;
    }

    if (tailLen + len <= sizeof(lastTimeTail))
    {
        memcpy(lastTimeTail + tailLen, data, len);
        tailLen += len;
        return len;
    }
    else if (tailLen == sizeof(lastTimeTail))
    {
        if (len > 0
            && lastTimeTail[0] == 0x00 && lastTimeTail[1] == 0x00 && lastTimeTail[2] == 0x00
            && data[0] == 0x01)
        {
            formated.marker = 1;
            pushFomated();
            formated.bFirst = true;
            formated.payload = std::make_shared<std::vector<uint8_t>>();
            tailLen = 0;
            return 1;
        }
        else if (len > 1
            && lastTimeTail[1] == 0x00 && lastTimeTail[2] == 0x00
            && data[0] == 0x00 && data[1] == 0x01)
        {
            formated.marker = 1;
            pushFomated();
            formated.bFirst = true;
            formated.payload = std::make_shared<std::vector<uint8_t>>();
            formated.payload->push_back(lastTimeTail[0]);
            tailLen = 0;
            return 2;
        }
        else if (len > 2
            && lastTimeTail[2] == 0x00
            && data[0] == 0x00 && data[1] == 0x00 && data[2] == 0x01)
        {
            formated.marker = 1;
            pushFomated();
            formated.bFirst = true;
            formated.payload = std::make_shared<std::vector<uint8_t>>();
            formated.payload->push_back(lastTimeTail[0]);
            formated.payload->push_back(lastTimeTail[1]);
            tailLen = 0;
            return 3;
        }
        else
        {
            if (formated.payload->size() + tailLen <= maxLen)
            {
                formated.payload->push_back(lastTimeTail[0]);
                formated.payload->push_back(lastTimeTail[1]);
                formated.payload->push_back(lastTimeTail[2]);
            }
            else
            {
                pushFomated();
                formated.bFirst = true;
                formated.payload = std::make_shared<std::vector<uint8_t>>();
                formated.payload->push_back(lastTimeTail[0]);
                formated.payload->push_back(lastTimeTail[1]);
                formated.payload->push_back(lastTimeTail[2]);
            }

            tailLen = 0;
        }
    }

    if (bNaluHeaderNextTime)
    {
        naluHeader = data[0];
    }

    /* find start code: 0x00 0x00 0x00 0x01 */
    int parsed;
    for (parsed = 0; parsed < len - 3; parsed++)
    {
        if (data[parsed] == 0x00
            && data[parsed+1] == 0x00
            && data[parsed+2] == 0x00
            && data[parsed+3] == 0x01)
        {
            break;
        }
    }

    int f = 0;
    while (f < parsed)
    {
        int32_t s = formated.payload->size();
        int p = maxLen - s;
        if (p > parsed - f)
        {
            p = parsed - f;
        }
        formated.payload->resize(s + p);
        mempcpy(formated.payload->data() + s, data + f, p);
        f += p;
        if (p < parsed)
        {
            pushFomated();
        }
    }

    if (parsed == len - 3)
    {
        memccpy(lastTimeTail, data + parsed, 3);
        tailLen = 3;
    }
    else
    {
        formated.marker = 1;
        pushFomated();
        formated.bFirst = true;
        formated.payload = std::make_shared<std::vector<uint8_t>>();
        tailLen = 0;

        if (parsed + 4 < len)
        {
            naluHeader = data[parsed+4];
            bNaluHeaderNextTime = false;
        }
        else
        {
            bNaluHeaderNextTime = true;
        }

        parsed += 4;
    }

    return parsed;
}
