#include "DevDownload.h"

DevDownload::DevDownload()
{
    m_testVideo = fopen("./assets/128x128.h264", "rb");
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

int32_t DevDownload::read(uint8_t *data, int32_t size)
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
