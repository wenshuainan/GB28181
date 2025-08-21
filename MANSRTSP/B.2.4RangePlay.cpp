#include "B.2.4RangePlay.h"
#include "Agent/MANSRTSPAgent.h"

using namespace MANSRTSP;

bool RangePlay::match(const Message& req)
{
    if (strCaseCmp(req.getStartLine().getMethod(), "PLAY"))
    {
        auto range = req.getHeader("Range");
        auto scale = req.getHeader("Scale");
        return range && !scale;
    }
    return false;
}

bool RangePlay::handle(SessionPlayback& session, const Message& req)
{
    printf("MANSRTSP RangePlay request\n");
    int32_t range = 0;
    sscanf(req.getHeader("Range")->getParameter("npt")->getValue().c_str(), "%d-", &range);
    return session.rangePlay(range);
}