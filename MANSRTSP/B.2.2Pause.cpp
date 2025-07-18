#include "B.2.2Pause.h"

using namespace MANSRTSP;

bool Pause::match(const Message& req)
{
    if (strCaseCmp(req.getStartLine().getMethod(), "PAUSE"))
    {
        return true;
    }
    return false;
}

bool Pause::handle(const Message& req)
{
    return false;
}