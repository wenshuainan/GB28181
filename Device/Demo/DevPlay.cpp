#include <stdio.h>
#include "DevPlay.h"

DevPlay::DevPlay()
{
    m_testVideo = fopen("./assets/128x128.h264", "rb");
}

DevPlay::~DevPlay()
{}

bool DevPlay::getVideo(Coded &coded)
{
    if (m_testVideo != nullptr)
    {
        int32_t size = fread(m_videoBuf, 1, sizeof(m_videoBuf), m_testVideo);
        if (size <= 0)
        {
            return false;
        }

        coded.data = m_videoBuf;
        coded.size = size;
        coded.bFramed = false;
        coded.userdata = 0;

        return true;
    }

    return false;
}

bool DevPlay::getAudio(Coded &coded)
{
    (void) coded;
    return false;
}

bool DevPlay::putCoded(const Coded &coded)
{
    (void) coded;
    return true;
}