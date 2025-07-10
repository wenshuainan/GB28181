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
    resip::Uri mServerUri;
    
public:
    ResipUserAgent(const SipUserAgent::ClientInfo& info, const SipUserAgent::ServerInfo& server);
    virtual ~ResipUserAgent();

public:
    virtual bool init();
    virtual bool recv(SipMessageApp& message);
    virtual bool send(const SipMessageApp& message);
    virtual bool genReqMessage(SipMessageApp& req, const std::string& method);
    virtual bool genResMessage(SipMessageApp& res, const SipMessageApp& req, int code, const std::string& reasonPhrase = "");

public:
    void makeResponse(resip::SipMessage& response, 
                        const resip::SipMessage& request, 
                        int responseCode, 
                        const resip::Data& reason = resip::Data::Empty) const;

private:
    void threadProc();
    void postAdapter(const resip::SipMessage& sipMessage);

protected:
    // Registration Handler ////////////////////////////////////////////////////////
    void onSuccess(resip::ClientRegistrationHandle h, const resip::SipMessage& response);
    void onFailure(resip::ClientRegistrationHandle h, const resip::SipMessage& response);
    void onRemoved(resip::ClientRegistrationHandle h, const resip::SipMessage& response);
    int onRequestRetry(resip::ClientRegistrationHandle h, int retryMinimum, const resip::SipMessage& msg);

    void onMessageArrived(resip::ServerPagerMessageHandle, const resip::SipMessage& message);
};

#endif