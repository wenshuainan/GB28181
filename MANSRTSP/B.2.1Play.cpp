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
        bool bNptNow = false;
        bool bScale = false;

        auto range = req.getHeader("Range");
        if (range)
        {
            auto param = range->getParameter("npt");
            if (param && param->getValue() == "now-")
            {
                bNptNow = true;
            }
        }
        auto scale = req.getHeader("Scale");
        bScale = scale != nullptr;

        return bNptNow && !bScale;
    }
    return false;
}

bool Play::handle(SessionPlayback& session, const Message& req)
{
    printf("MANSRTSP Play request\n");
    return session.play();
}

}