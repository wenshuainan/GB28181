#include <iostream>
#include "DevDownload.h"

DevDownload::DevDownload(int32_t ch)
    : Download(ch)
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

DevDownload::~DevDownload()
{
    if (m_testVideo)
    {
        fclose(m_testVideo);
        m_testVideo = nullptr;
    }
}

bool DevDownload::seek(time_t timestamp)
{
    return false;
}

int32_t DevDownload::readVideo(uint8_t *data, int32_t size)
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

int32_t DevDownload::readAudio(uint8_t *data, int32_t size)
{
    return 0;
}
