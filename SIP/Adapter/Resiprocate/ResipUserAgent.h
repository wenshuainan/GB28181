#ifndef RESIPROCATE_ADAPTER_H
#define RESIPROCATE_ADAPTER_H

#include "SipAdapter.h"
#include "basicClientUserAgent.hxx"

struct SipAdapterMessage
{
    std::shared_ptr<resip::SipMessage> instance;
};

class ResipUserAgent : public SipUserAgent, public resip::BasicClientUserAgent
{
private:
    bool mbInit;
    resip::Uri mServerUri;
    resip::ClientPagerMessageHandle mKeepaliveHandle; // 向服务器发送keepalive命令
    resip::ClientPagerMessageHandle mMANSCDPResponseHandle; // 向服务器发送MANSCDP应答命令
    
public:
    ResipUserAgent(const SipUserAgent::ClientInfo& info, const SipUserAgent::ServerInfo& server);
    virtual ~ResipUserAgent();

private:
    void threadProc();

public:
    virtual bool init();
    virtual bool makeRegistrationRequest(SipUserMessage& req);
    virtual bool sendRegistration(const SipUserMessage& req);
    virtual bool sendKeepaliveRequest(const XMLDocument& notify);
    virtual bool sendMANSCDPResponse(const XMLDocument& res);

protected:
    // Registration Handler ////////////////////////////////////////////////////////
    virtual void onSuccess(resip::ClientRegistrationHandle h, const resip::SipMessage& response);
    virtual void onFailure(resip::ClientRegistrationHandle h, const resip::SipMessage& response);
    virtual void onRemoved(resip::ClientRegistrationHandle h, const resip::SipMessage& response);
    virtual int onRequestRetry(resip::ClientRegistrationHandle h, int retryMinimum, const resip::SipMessage& msg);

    // PagerMessageHandler //////////////////////////////////////////////////////////
    virtual void onSuccess(resip::ClientPagerMessageHandle, const resip::SipMessage& status);
    virtual void onFailure(resip::ClientPagerMessageHandle, const resip::SipMessage& status, std::unique_ptr<resip::Contents> contents);
    virtual void onMessageArrived(resip::ServerPagerMessageHandle, const resip::SipMessage& message);
};

#endif