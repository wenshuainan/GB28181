#ifndef RESIPROCATE_ADAPTER_H
#define RESIPROCATE_ADAPTER_H

#include <memory>
#include "../SipAdapter.h"
#include "basicClientUserAgent.hxx"

struct SipAdapterMessage
{
    std::shared_ptr<resip::SipMessage> instance;
};

class ResipUserAgent : public SipUserAgent, public resip::BasicClientUserAgent
{
private:
    resip::Uri mServerUri;
    
public:
    ResipUserAgent(const SipUserAgent::ClientInfo& info, const SipUserAgent::ServerInfo& server);
    virtual ~ResipUserAgent();

public:
    virtual bool start();
    virtual bool stop();
    virtual bool threadFunc(int32_t interval);
    virtual const char* getSipUser();
    virtual bool makeRegistration(SipUserMessage& req);
    virtual bool sendRegistration(const SipUserMessage& req);
    virtual bool sendMANSCDP(const XMLDocument& cmd);
    virtual bool makeSessionResponse(const SipUserMessage& req, SipUserMessage& res, int32_t code);
    virtual bool sendSessionResponse(const SessionIdentifier& id, const SipUserMessage& res);
    virtual bool sendSessionMessage(const SessionIdentifier& id, const XMLDocument& cmd);
    virtual bool sendMANSRTSP(const SessionIdentifier& id, const MANSRTSP::Message& cmd);
    virtual bool removeSession(const SessionIdentifier& id);

protected:
    // Registration Handler ////////////////////////////////////////////////////////
    virtual void onSuccess(resip::ClientRegistrationHandle h, const resip::SipMessage& response);
    virtual void onFailure(resip::ClientRegistrationHandle h, const resip::SipMessage& response);
    virtual void onRemoved(resip::ClientRegistrationHandle h, const resip::SipMessage& response);
    virtual int onRequestRetry(resip::ClientRegistrationHandle h, int retryMinimum, const resip::SipMessage& msg);

    // Invite Session Handler /////////////////////////////////////////////////////
    virtual void onNewSession(resip::ServerInviteSessionHandle h, resip::InviteSession::OfferAnswerType oat, const resip::SipMessage& msg);
    virtual void onConnectedConfirmed(resip::InviteSessionHandle, const resip::SipMessage &msg);
    virtual void onTerminated(resip::InviteSessionHandle h, resip::InviteSessionHandler::TerminatedReason reason, const resip::SipMessage* msg);
    virtual void onOffer(resip::InviteSessionHandle handle, const resip::SipMessage& msg, const resip::SdpContents& offer);
    virtual void onInfo(resip::InviteSessionHandle, const resip::SipMessage& msg);

    // PagerMessageHandler //////////////////////////////////////////////////////////
    virtual void onSuccess(resip::ClientPagerMessageHandle, const resip::SipMessage& status);
    virtual void onFailure(resip::ClientPagerMessageHandle, const resip::SipMessage& status, std::unique_ptr<resip::Contents> contents);
    virtual void onMessageArrived(resip::ServerPagerMessageHandle, const resip::SipMessage& message);
};

#endif