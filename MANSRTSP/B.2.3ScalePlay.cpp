#include "B.2.3ScalePlay.h"

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

bool ScalePlay::handle(const Message& req)
{
    return false;
}