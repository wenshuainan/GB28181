#include "B.2.1Play.h"
#include "UA.h"
#include "Agent/MANSRTSPAgent.h"
#include "Agent/SessionAgent.h"

namespace MANSRTSP
{

bool Play::match(const Message& req)
{
    if (strCaseCmp(req.getStartLine().getMethod(), "PLAY"))
    {
        std::string range;
        std::string paramNow;
        std::string scale;
        for (auto header : req.getHeaders())
        {
            if (strCaseCmp(header.getName(), "Range"))
            {
                range = "Range";
                for (auto param : header.getParameters())
                {
                    if (strCaseCmp(param.getName(), "npt"))
                    {
                        paramNow = strCaseCmp(param.getValue(), "now-") ? "now-" : "";
                    }
                }
            }
            else if (strCaseCmp(header.getName(), "Scale"))
            {
                scale = "Scale";
            }
        }
        if (!range.empty() && !paramNow.empty() && scale.empty())
        {
            return true;
        }
    }
    return false;
}

bool Play::handle(SessionPlayback& session, const Message& req)
{
    return session.play();
}

}