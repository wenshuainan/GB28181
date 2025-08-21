#include "B.2.3ScalePlay.h"
#include "Agent/MANSRTSPAgent.h"

using namespace MANSRTSP;

bool ScalePlay::match(const Message& req)
{
    if (strCaseCmp(req.getStartLine().getMethod(), "PLAY"))
    {
        auto scale = req.getHeader("Scale");
        auto range = req.getHeader("Range");
        return scale && !range;
    }
    return false;
}

bool ScalePlay::handle(SessionPlayback& session, const Message& req)
{
    printf("MANSRTSP ScalePlay request\n");
    return session.scalePlay(req.getHeader("Scale")->getFloat());
}