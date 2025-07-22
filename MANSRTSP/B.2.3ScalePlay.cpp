#include "B.2.3ScalePlay.h"
#include "Agent/MANSRTSPAgent.h"

using namespace MANSRTSP;

bool ScalePlay::match(const Message& req)
{
    if (strCaseCmp(req.getStartLine().getMethod(), "PLAY"))
    {
        std::string scale;
        std::string range;
        for (auto header : req.getHeaders())
        {
            if (strCaseCmp(header.getName(), "Scale"))
            {
                scale = "Scale";
            }
            if (strCaseCmp(header.getName(), "Range"))
            {
                range = "Range";
                break;
            }
        }
        if (!scale.empty() && range.empty())
        {
            return true;
        }
    }
    return false;
}

bool ScalePlay::handle(const SessionIdentifier& id, const Message& req)
{
    bool ret = false;
    auto rtsp = m_agent->getMANSRTSPSession();
    if (rtsp != nullptr)
    {
        ret = rtsp->scalePlay();
    }

    MANSRTSP::Message res(req, ret ? 200 : 400);
    m_agent->sendResponse(id, res);
    return ret;
}