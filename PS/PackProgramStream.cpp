#include "PackProgramStream.h"

PackProgramStream::PackProgramStream()
{}

PackProgramStream::~PackProgramStream()
{}

std::shared_ptr<Pack> PackProgramStream::makePack()
{
    std::shared_ptr<Pack> pack = std::make_shared<Pack>();
    if (pack == nullptr)
    {
        return nullptr;
    }

    return pack;
}

std::shared_ptr<SystemHeader> PackProgramStream::makeSystemHeader()
{}

std::shared_ptr<ProgramStreamMap> PackProgramStream::makePSM()
{}

void PackProgramStream::addPack(const std::shared_ptr<Pack>& pack)
{
    this->pack.push_back(pack);
}

void PackProgramStream::setStreamCallback(std::function<void (uint8_t *, int32_t)> callback)
{
    streamCallback = callback;
}

void PackProgramStream::intput(PackES::ES_TYPE type, uint8_t *data, int32_t size)
{}