#include "PacketizedES.h"
#include "PacketizedAVC.h"
#include "PacketizedHEVC.h"
#include "PacketizedG711A.h"

PES::PES(PSMux *mux)
    : m_mux(mux)
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
    case HEVC:
        pes = std::make_shared<PacketizedHEVC>(mux);
        break;
    case G711A:
        pes = std::make_shared<PacketizedG711A>(mux);
        break;
    
    default:
        return nullptr;
    }

    return pes;
}