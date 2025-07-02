#ifndef PACK_PROGRAM_STREAM_H
#define PACK_PROGRAM_STREAM_H

#include <functional>
#include "ProgramStream.h"
#include "PackElementaryStream.h"

class PackProgramStream
{
private:
    std::vector<std::shared_ptr<PackES>> programES;
    std::vector<std::shared_ptr<Pack>> pack;

private:
    std::function<void (uint8_t *, int32_t)> streamCallback;

public:
    PackProgramStream();
    ~PackProgramStream();

public:
    std::shared_ptr<Pack> makePack();
    std::shared_ptr<SystemHeader> makeSystemHeader();
    std::shared_ptr<ProgramStreamMap> makePSM();
    void addPack(const std::shared_ptr<Pack>& pack);

public:
    void setStreamCallback(std::function<void (uint8_t *, int32_t)> callback);
    void intput(PackES::ES_TYPE type, uint8_t *data, int32_t size);
}

#endif