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
    PES *pes = nullptr;

    if (mux == nullptr)
    {
        printf("mux == nullptr\n");
        return nullptr;
    }

    switch (type)
    {
    case AVC:
        pes = new PacketizedAVC(mux);
        break;
    case HEVC:
        pes = new PacketizedHEVC(mux);
        break;
    case G711A:
        pes = new PacketizedG711A(mux);
        break;
    case AAC:
        pes = new PacketizedAAC(mux);
        break;
    
    default:
        printf("PES not support %d\n", type);
        return nullptr;
    }

    return std::unique_ptr<PES>(pes);
}