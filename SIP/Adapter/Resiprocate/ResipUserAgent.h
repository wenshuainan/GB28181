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
    virtual bool makeReqMessage(SipMessageApp& req, const std::string& method);
    virtual bool makeResMessage(SipMessageApp& res, const SipMessageApp& req, int code, const std::string& reasonPhrase = "");

public:
    void makeResponse(resip::SipMessage& response, 
                        const resip::SipMessage& request, 
                        int responseCode, 
                        const resip::Data& reason = resip::Data::Empty) const;

private:
    void threadProc();

protected:
    // Registration Handler ////////////////////////////////////////////////////////
    virtual void onSuccess(resip::ClientRegistrationHandle h, const resip::SipMessage& response);
    virtual void onFailure(resip::ClientRegistrationHandle h, const resip::SipMessage& response);
    virtual void onRemoved(resip::ClientRegistrationHandle h, const resip::SipMessage& response);
    virtual int onRequestRetry(resip::ClientRegistrationHandle h, int retryMinimum, const resip::SipMessage& msg);

    // OutOfDialogHandler //////////////////////////////////////////////////////////
    virtual void onSuccess(resip::ClientOutOfDialogReqHandle, const resip::SipMessage& response);
    virtual void onFailure(resip::ClientOutOfDialogReqHandle, const resip::SipMessage& response);
    virtual void onReceivedRequest(resip::ServerOutOfDialogReqHandle, const resip::SipMessage& request);
};

#endif