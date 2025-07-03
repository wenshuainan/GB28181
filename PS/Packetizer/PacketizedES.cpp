#include "PacketizedES.h"
#include "PacketizedAVC.h"

PES::PES(PSMux *mux)
    : mux(mux)
{}

PES::~PES()
{}

std::shared_ptr<PES> PES::create(ES_TYPE type, PSMux *mux)
{
    std::shared_ptr<PES> pes = nullptr;

    if (mux == nullptr)
    {
        return nullptr;
    }

    switch (type)
    {
    case AVC:
        pes = std::make_shared<PacketizedAVC>(mux);
        break;
    
    default:
        return nullptr;
    }

    return pes;
}