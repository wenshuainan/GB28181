#include <stdio.h>
#include <iostream>
#include "DevPlay.h"

DevPlay::DevPlay(int32_t ch)
    : Play(ch)
{}

DevPlay::~DevPlay()
{
    stop();
}

bool DevPlay::start()
{
    const char *name = "./assets/128x128.h264";
    // const char *name = "./assets/nature.h264";
    // const char *name = "./assets/352x288.h265";
    // const char *name = "./assets/640x360.h265";
    // const char *audio = "./assets/alaw.raw";
    const char *audio = "./assets/test.aac";

    m_testVideo = fopen(name, "rb");
    if (m_testVideo == nullptr)
    {
        std::cout << "failed open video " << name << std::endl;
        return false;
    }

    m_testAudio = fopen(audio, "rb");
    if (m_testAudio == nullptr)
    {
        fclose(m_testVideo);
        m_testVideo = nullptr;
        
        std::cout << "failed open audio " << audio << std::endl;
        return false;
    }

    return true;
}

bool DevPlay::stop()
{
    if (m_testVideo)
    {
        fclose(m_testVideo);
        m_testVideo = nullptr;
    }
    return true;
}

int32_t DevPlay::getVideo(uint8_t *data, int32_t size)
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

int32_t DevPlay::getAudio(uint8_t *data, int32_t size)
{
    if (m_testAudio != nullptr)
    {
        return fread(data, 1, 1024, m_testAudio);
    }
    else
    {
        return 0;
    }
}

PES::ES_TYPE DevPlay::getVideoType()
{
    return PES::AVC;
}

PES::ES_TYPE DevPlay::getAudioType()
{
    return PES::AAC;
}