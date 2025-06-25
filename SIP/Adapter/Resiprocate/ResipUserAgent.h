#ifndef RESIPROCATE_ADAPTER_H
#define RESIPROCATE_ADAPTER_H

#include "SipAdapter.h"
#include "basicClientUserAgent.hxx"

struct SipMessageAdapter
{
    std::shared_ptr<resip::SipMessage> instance;
};

class ResipUserAgent : public SipUserAgent, public resip::BasicClientUserAgent
{
private:
    bool bInit;
    
public:
    ResipUserAgent(const SipUserAgent::Info& info);
    virtual ~ResipUserAgent();

public:
    virtual bool init();
    virtual bool recv(SipGenericMessage& message);
    virtual bool send(const SipGenericMessage& message);
    virtual bool genReqMessage(SipGenericMessage& req, const std::string& method, const std::string& requestUri = "");
    virtual bool genResMessage(SipGenericMessage& res, const SipGenericMessage& req, int code, const std::string& reasonPhrase = "");

public:
    void makeResponse(resip::SipMessage& response, 
                        const resip::SipMessage& request, 
                        int responseCode, 
                        const resip::Data& reason = resip::Data::Empty) const;

private:
    void threadProc();
};

#endif