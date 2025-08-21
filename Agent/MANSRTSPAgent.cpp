#include "UA.h"
#include "MANSRTSPAgent.h"
#include "MANSRTSP/B.2.1Play.h"
#include "MANSRTSP/B.2.2Pause.h"
#include "MANSRTSP/B.2.3ScalePlay.h"
#include "MANSRTSP/B.2.4RangePlay.h"
#include "MANSRTSP/B.2.5Teardown.h"

MANSRTSPAgent::MANSRTSPAgent(UA *ua, SessionPlayback *session)
    : Agent(ua)
    , m_session(session)
{
    printf("++++++ MANSRTSPAgent\n");
    m_requests.push_back(std::move(std::unique_ptr<MANSRTSP::Play>(new MANSRTSP::Play(this))));
    m_requests.push_back(std::move(std::unique_ptr<MANSRTSP::Pause>(new MANSRTSP::Pause(this))));
    m_requests.push_back(std::move(std::unique_ptr<MANSRTSP::ScalePlay>(new MANSRTSP::ScalePlay(this))));
    m_requests.push_back(std::move(std::unique_ptr<MANSRTSP::RangePlay>(new MANSRTSP::RangePlay(this))));
    m_requests.push_back(std::move(std::unique_ptr<MANSRTSP::Teardown>(new MANSRTSP::Teardown(this))));
}

MANSRTSPAgent::~MANSRTSPAgent()
{
    printf("----- MANSRTSPAgent\n");
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

bool MANSRTSPAgent::agent(const SessionIdentifier& id, const MANSRTSP::Message& message)
{
    printf("MANSRTSP agent message\n");
    int code = 400;
    for (auto& req : m_requests)
    {
        if (req->match(message))
        {
            code = req->handle(*m_session, message) ? 200 : 400;
        }
    }

    printf("method: %s code=%d\n", message.getStartLine().getMethod().c_str(), code);
    MANSRTSP::Message res(message, code);
    return sendResponse(id, res);
}

bool MANSRTSPAgent::sendResponse(const SessionIdentifier& id, const MANSRTSP::Message& message)
{
    printf("send MANSRTSP response\n");
    auto sip = m_ua->getSip();
    if (sip)
    {
        return sip->sendMANSRTSP(id, message);
    }
    return false;
}