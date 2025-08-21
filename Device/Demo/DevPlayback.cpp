#include <iostream>
#include "DevPlayback.h"

DevPlayback::DevPlayback(int32_t ch)
    : Playback(ch)
{}

DevPlayback::~DevPlayback()
{
    stop();
}

void DevPlayback::setTime(time_t start, time_t end)
{
    std::cout << "setTime" << " start=" << start << " end=" << end << std::endl;
}

bool DevPlayback::start()
{
    const char *name = "./assets/128x128.h264";
    // const char *name = "./assets/nature.h264";

    m_testVideo = fopen(name, "rb");
    if (m_testVideo == nullptr)
    {
        std::cout << "failed open video " << name << std::endl;
        return false;
    }
    return true;
}

bool DevPlayback::stop()
{
    if (m_testVideo)
    {
        fclose(m_testVideo);
        m_testVideo = nullptr;
    }
    return true;
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

bool DevPlayback::isFileEnd()
{
    return feof(m_testVideo) != 0;
}

PES::ES_TYPE DevPlayback::getVideoType()
{
    return PES::AVC;
}

PES::ES_TYPE DevPlayback::getAudioType()
{
    return PES::G711A;
}

bool DevPlayback::play()
{
    printf("DevPlayback::play\n");
    return true;
}

bool DevPlayback::play(int32_t start)
{
    printf("DevPlayback::play %d\n", start);
    return true;
}

bool DevPlayback::play(int32_t start, int32_t end)
{
    printf("DevPlayback::play %d %d\n", start, end);
    return true;
}

bool DevPlayback::pause()
{
    printf("DevPlayback::pause\n");
    return true;
}

bool DevPlayback::teardown()
{
    printf("DevPlayback::teardown\n");
    return true;
}

bool DevPlayback::setSpeed(float speed)
{
    printf("DevPlayback::setSpeed %f\n", speed);
    return true;
}
