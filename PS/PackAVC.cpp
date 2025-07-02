#include "PackAVC.h"
#include "PackProgramStream.h"

PackAVC::PackAVC()
{}

PackAVC::~PackAVC()
{}

int32_t PackAVC::pack(uint8_t *data, uint32_t size)
{
    int len;
    int i;
    for (i = 0; i < size; i++)
    {
        if (data[i] == 0x00 && data[i+1] == 0x00 && data[i+2] == 0x00 && data[i+3] == 0x01)
        {
            break;
        }
    }

    if (pesPacket != nullptr)
    {
        //todo 没放完继续循环放
        len = pesPacket->inputDataByte(data, i);
        packing->addPESPacket(pesPacket); //结束后放到pack
        programStream->addPack(packing); //新的帧生成老的pack放回
    }

    pesPacket = makePESPacket(naluHeader & 0x1F);
    programStream->makePack(); //新的帧生成新的pack
    return len;
}

std::shared_ptr<PESPacket>& PackAVC::makePESPacket(uint8_t naluType)
{}