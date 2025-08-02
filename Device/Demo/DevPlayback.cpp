#include <iostream>
#include "DevPlayback.h"

DevPlayback::DevPlayback(int32_t ch)
    : Playback(ch)
{
    const char *name = "./assets/128x128.h264";
    // const char *name = "./assets/nature.h264";

    m_testVideo = fopen(name, "rb");
    if (m_testVideo == nullptr)
    {
        std::cout << "failed open video " << name << std::endl;
        return;
    }
}

DevPlayback::~DevPlayback()
{
    if (m_testVideo)
    {
        fclose(m_testVideo);
        m_testVideo = nullptr;
    }
}

bool DevPlayback::seek(time_t timestamp)
{
    return false;
}

int32_t DevPlayback::readVideo(uint8_t *data, int32_t size)
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

int32_t DevPlayback::readAudio(uint8_t *data, int32_t size)
{
    return 0;
}
