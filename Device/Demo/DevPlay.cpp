#include <stdio.h>
#include "DevPlay.h"

DevPlay::DevPlay()
{
    m_testVideo = fopen("./assets/128x128.h264", "rb");
    // m_testVideo = fopen("./assets/nature.h264", "rb");
}

DevPlay::~DevPlay()
{
    if (m_testVideo)
    {
        fclose(m_testVideo);
        m_testVideo = nullptr;
    }
}

int32_t DevPlay::getVideo(int32_t ch, uint8_t *data, int32_t size)
{
    if (m_testVideo != nullptr)
    {
        return fread(data, 1, size, m_testVideo);
    }
    else
    {
        return 0;
    }
}

int32_t DevPlay::getAudio(int32_t ch, uint8_t *data, int32_t size)
{
    return 0;
}