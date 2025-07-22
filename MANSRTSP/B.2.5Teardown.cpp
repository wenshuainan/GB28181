#include "B.2.5Teardown.h"
#include "Agent/MANSRTSPAgent.h"

using namespace MANSRTSP;

bool Teardown::match(const Message& req)
{
    if (strCaseCmp(req.getStartLine().getMethod(), "TEARDOWN"))
    {
        return true;
    }
    return false;
}

bool Teardown::handle(const SessionIdentifier& id, const Message& req)
{
    bool ret = false;
    auto rtsp = m_agent->getMANSRTSPSession();
    if (rtsp != nullptr)
    {
        ret = rtsp->teardown();
    }

    MANSRTSP::Message res(req, ret ? 200 : 400);
    m_agent->sendResponse(id, res);
    return ret;
}