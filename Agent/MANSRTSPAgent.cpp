#include "MANSRTSPAgent.h"
#include "MANSRTSP/B.2.1Play.h"
#include "MANSRTSP/B.2.2Pause.h"
#include "MANSRTSP/B.2.3ScalePlay.h"
#include "MANSRTSP/B.2.4RangePlay.h"
#include "MANSRTSP/B.2.5Teardown.h"

MANSRTSPAgent::MANSRTSPAgent(UA *ua)
    : Agent(ua)
{
    m_requests.push_back(std::make_shared<MANSRTSP::Play>(this));
    m_requests.push_back(std::make_shared<MANSRTSP::Pause>(this));
    m_requests.push_back(std::make_shared<MANSRTSP::ScalePlay>(this));
    m_requests.push_back(std::make_shared<MANSRTSP::RangePlay>(this));
    m_requests.push_back(std::make_shared<MANSRTSP::Teardown>(this));
}

MANSRTSPAgent::~MANSRTSPAgent()
{
    m_requests.clear();
}

bool MANSRTSPAgent::match(const std::string& method, const std::string& contentType)
{
    return strCaseCmp(method, "INFO")
            && strCaseCmp(contentType, "Application/MANSRTSP");
}

bool MANSRTSPAgent::agent(const SipUserMessage& message)
{
    return false;
}

bool MANSRTSPAgent::dispatchRequest(const MANSRTSP::Message& message)
{
    for (auto req : m_requests)
    {
        if (req->match(message))
        {
            return req->handle(message);
        }
    }
    return false;
}