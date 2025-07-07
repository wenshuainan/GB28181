#include <string.h>
#include "RtpPayloadAvc.h"

RtpPayloadAvc::RtpPayloadAvc(RtpParticipant *participant, int32_t maxLen)
    : RtpPayload(participant, maxLen - 2) // -2 for FUA indicator and header
{
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

int32_t RtpPayloadAvc::format(const uint8_t *data, int32_t len)
{
    if (data == nullptr || len <= 0)
    {
        return 0;
    }

    if (cacheLen + len <= sizeof(cache))
    {
        memcpy(cache + cacheLen, data, len);
        cacheLen += len;
        return len;
    }

    if (cacheLen > 0)
    {
        uint8_t assemble[8] = {0};
        int32_t assembleLen = 0;

        memcpy(assemble, cache, cacheLen);
        assembleLen += cacheLen;

        int32_t dataLen = len > 4 ? 4 : len;
        memcpy(assemble + assembleLen, data, dataLen);
        assembleLen += dataLen;

        int i;
        for (i = 0; i < assembleLen - 4; i++)
        {
            if (assemble[i] == 0x00
                && assemble[i+1] == 0x00
                && assemble[i+2] == 0x00
                && assemble[i+3] == 0x01)
            {
                int j;
                for (j = 0; j < i; j++)
                {
                    if (formated.payload->size() >= maxLen)
                    {
                        pushFomated();
                        formated.payload = std::make_shared<std::vector<uint8_t>>();
                    }
                    formated.payload->push_back(assemble[j]);
                }
                formated.marker = 1;
                pushFomated();
                naluHeader = assemble[i+4];
                formated.payload = std::make_shared<std::vector<uint8_t>>();
                formated.bFirst = true;

                for (j = i + 4; j < assembleLen; j++)
                {
                    formated.payload->push_back(assemble[j]);
                }

                cacheLen = 0;

                return dataLen;
            }
        }

        for (i = 0; i < cacheLen; i++)
        {
            if (formated.payload->size() >= maxLen)
            {
                pushFomated();
                formated.payload = std::make_shared<std::vector<uint8_t>>();
            }
            formated.payload->push_back(cache[i]);
        }

        cacheLen = 0;
    }

    if (len <= sizeof(cache))
    {
        memcpy(cache, data, len);
        cacheLen = len;
        return len;
    }

    int parsed;
    for (parsed = 0; parsed < len - 4; parsed++)
    {
        if (data[parsed] == 0x00
            && data[parsed+1] == 0x00
            && data[parsed+2] == 0x00
            && data[parsed+3] == 0x01)
        {
            break;
        }
    }

    int cpyed = 0;
    while (cpyed < parsed)
    {
        int32_t ori = formated.payload->size();
        int wr = maxLen - ori;
        if (wr > parsed - cpyed)
        {
            wr = parsed - cpyed;
        }
        formated.payload->resize(ori + wr);
        memcpy(formated.payload->data() + ori, data + cpyed, wr);
        cpyed += wr;
        if (wr < parsed)
        {
            pushFomated();
        }
    }

    if (parsed == len - 4)
    {
        memcpy(cache, data + parsed, 4);
        cacheLen = 4;
        parsed += 4;
    }
    else
    {
        formated.marker = 1;
        pushFomated();
        naluHeader = data[parsed+4];
        formated.bFirst = true;
        formated.payload = std::make_shared<std::vector<uint8_t>>();

        cacheLen = 0;
        parsed += 5;
    }

    return parsed;
}
