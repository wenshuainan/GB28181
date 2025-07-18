#include "B.2.5Teardown.h"

using namespace MANSRTSP;

bool Teardown::match(const Message& req)
{
    if (strCaseCmp(req.getStartLine().getMethod(), "TEARDOWN"))
    {
        return true;
    }
    return false;
}

bool Teardown::handle(const Message& req)
{
    return false;
}