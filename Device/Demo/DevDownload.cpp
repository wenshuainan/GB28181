#include <iostream>
#include "DevDownload.h"

DevDownload::DevDownload(int32_t ch)
    : Download(ch)
{}

DevDownload::~DevDownload()
{
    stop();
}

void DevDownload::setTime(time_t start, time_t end)
{
    std::cout << "setTime" << " start=" << start << " end=" << end << std::endl;
}

bool DevDownload::setSpeed(int speed)
{
    std::cout << "setSpeed" << " speed=" << speed << std::endl;
    return true;
}

uint64_t DevDownload::getFileSize()
{
    return 10000;
}

bool DevDownload::start()
{
    // const char *name = "./assets/128x128.h264";
    const char *name = "./assets/nature.h264";

    m_testVideo = fopen(name, "rb");
    if (m_testVideo == nullptr)
    {
        std::cout << "failed open video " << name << std::endl;
        return false;
    }
    return true;
}

bool DevDownload::stop()
{
    if (m_testVideo)
    {
        fclose(m_testVideo);
        m_testVideo = nullptr;
    }
    return true;
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

bool DevDownload::isFileEnd()
{
    return feof(m_testVideo) != 0;
}

PES::ES_TYPE DevDownload::getVideoType()
{
    return PES::AVC;
}

PES::ES_TYPE DevDownload::getAudioType()
{
    return PES::G711A;
}
