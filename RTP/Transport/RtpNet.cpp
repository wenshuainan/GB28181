#include "RtpNet.h"
#include "RtpOverTcp.h"
#include "RtpOverUdp.h"

RtpNet::RtpNet(int localPort)
{}

RtpNet::~RtpNet()
{}

std::shared_ptr<RtpNet> RtpNet::create(Type type, int localPort)
{
    std::shared_ptr<RtpNet> net = nullptr;

    switch (type)
    {
    case UDP:
        net = std::make_shared<RtpOverUdp>();
        break;
    case TCP_ACTIVE:
        net = std::make_shared<RtpOverTcp>();
        break;
    case TCP_PASSIVE:
        break;
    
    default:
        break;
    }

    return net;
}