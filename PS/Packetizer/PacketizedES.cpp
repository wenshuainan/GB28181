#include "PacketizedES.h"
#include "PacketizedAVC.h"
#include "PacketizedHEVC.h"
#include "PacketizedG711A.h"
#include "PacketizedAAC.h"

PES::PES(PSMux *mux)
    : m_mux(mux)
{}

PES::~PES()
{}

std::unique_ptr<PES> PES::create(ES_TYPE type, PSMux *mux)
{
    std::unique_ptr<PES> pes;

    if (mux == nullptr)
    {
        printf("mux == nullptr\n");
        return nullptr;
    }

    switch (type)
    {
    case AVC:
        pes = std::move(std::unique_ptr<PES>(new PacketizedAVC(mux)));
        break;
    case HEVC:
        pes = std::move(std::unique_ptr<PES>(new PacketizedHEVC(mux)));
        break;
    case G711A:
        pes = std::move(std::unique_ptr<PES>(new PacketizedG711A(mux)));
        break;
    case AAC:
        pes = std::move(std::unique_ptr<PES>(new PacketizedAAC(mux)));
        break;
    
    default:
        printf("PES not support %d\n", type);
        return nullptr;
    }

    return pes;
}