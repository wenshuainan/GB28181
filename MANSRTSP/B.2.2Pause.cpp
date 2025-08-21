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

bool Pause::handle(SessionPlayback& session, const Message& req)
{
    printf("MANSRTSP Pause request\n");
    return session.pause();
}