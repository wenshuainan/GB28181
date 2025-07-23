#include "DevPlayback.h"

DevPlayback::DevPlayback()
{
    m_testVideo = fopen("./assets/128x128.h264", "rb");
}

DevPlayback::~DevPlayback()
{
    if (m_testVideo)
    {
        fclose(m_testVideo);
        m_testVideo = nullptr;
    }
}

bool DevPlayback::seek(int32_t ch, time_t timestamp)
{
    return false;
}

int32_t DevPlayback::read(int32_t ch, uint8_t *data, int32_t size)
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
