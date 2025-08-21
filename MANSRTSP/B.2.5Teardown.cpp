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

bool Teardown::handle(SessionPlayback& session, const Message& req)
{
    printf("MANSRTSP TEARDOWN request\n");
    return session.teardown();
}