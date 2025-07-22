#include "B.2.2Pause.h"
#include "Agent/MANSRTSPAgent.h"

using namespace MANSRTSP;

bool Pause::match(const Message& req)
{
    if (strCaseCmp(req.getStartLine().getMethod(), "PAUSE"))
    {
        return true;
    }
    return false;
}

bool Pause::handle(const SessionIdentifier& id, const Message& req)
{
    bool ret = false;
    auto rtsp = m_agent->getMANSRTSPSession();
    if (rtsp != nullptr)
    {
        ret = rtsp->pause();
    }

    MANSRTSP::Message res(req, ret ? 200 : 400);
    m_agent->sendResponse(id, res);
    return ret;
}