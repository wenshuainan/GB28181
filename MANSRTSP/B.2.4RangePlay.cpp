#include "B.2.4RangePlay.h"
#include "Agent/MANSRTSPAgent.h"

using namespace MANSRTSP;

bool RangePlay::match(const Message& req)
{
    if (strCaseCmp(req.getStartLine().getMethod(), "PLAY"))
    {
        std::string range;
        std::string scale;
        for (auto header : req.getHeaders())
        {
            if (strCaseCmp(header.getName(), "Range"))
            {
                range = "Range";
            }
            else if (strCaseCmp(header.getName(), "Scale"))
            {
                scale = "Scale";
                break;
            }
        }
        if (!range.empty() && scale.empty())
        {
            return true;
        }
    }
    return false;
}

bool RangePlay::handle(SessionPlayback& session, const Message& req)
{
    return session.rangePlay();
}