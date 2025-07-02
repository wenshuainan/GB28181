#ifndef PACK_AVC_H
#define PACK_AVC_H

#include "PackElementaryStream.h"

class PackAVC : public PackES
{
private:
    uint8_t naluHeader;
    uint8_t cache[3];
    int32_t cachelen;
    std::shared_ptr<PESPacket> pesPacket;
    std::shared_ptr<Pack> packing;

public:
    PackAVC();
    virtual ~PackAVC();

public:
    int32_t pack(uint8_t *data, uint32_t size);

private:
    std::shared_ptr<PESPacket>& makePESPacket(uint8_t naluType);
};

#endif