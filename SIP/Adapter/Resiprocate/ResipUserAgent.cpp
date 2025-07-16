#include <iostream>
#include <thread>
#include "ResipUserAgent.h"
#include "UA.h"
#include "MANSCDPContents.h"
#include "resip/dum/ClientPagerMessage.hxx"
#include "resip/dum/ServerPagerMessage.hxx"
#include "resip/dum/ServerInviteSession.hxx"

bool SipUserAgent::postRegistrationResponse(const SipUserMessage& res)
{
    return m_user->dispatchRegistrationResponse(res);
}

bool SipUserAgent::postKeepaliveResponse(int32_t code)
{
    return m_user->dispatchKeepaliveResponse(code);
}

bool SipUserAgent::postMANSCDPRequest(const XMLDocument &req)
{
    return m_user->dispatchMANSCDPRequest(req);
}

bool SipUserAgent::postSessionRequest(const SipUserMessage& req)
{
    return m_user->dispatchSessionRequest(req);
}

std::shared_ptr<SipUserAgent> SipUserAgent::create(UA *user, const ClientInfo& client, const ServerInfo& server)
{
    std::shared_ptr<SipUserAgent> sip = std::make_shared<ResipUserAgent>(client, server);

    if (sip != nullptr)
    {
        sip->m_user = user;
    }

    return sip;
}

bool SipUserAgent::destroy()
{
    return false;
}

ResipUserAgent::ResipUserAgent(const SipUserAgent::ClientInfo& client, const SipUserAgent::ServerInfo& server)
    : BasicClientUserAgent(client)
{
    mRegistrationRetryDelayTime = client.interval;

    mServerUri.user() = server.id;
    mServerUri.host() = server.ipv4;
    mServerUri.port() = server.port;
}

ResipUserAgent::~ResipUserAgent()
{}

void ResipUserAgent::threadProc()
{
    while (mbInit)
    {
        process(1000);
    }
}

bool ResipUserAgent::init()
{
    mKeepaliveHandle = mDum->makePagerMessage(resip::NameAddr(mServerUri));
    mMANSCDPResponseHandle = mDum->makePagerMessage(resip::NameAddr(mServerUri));

    startup();

    mbInit = true;
    std::thread t(&ResipUserAgent::threadProc, this);
    t.detach();

    return true;
}

bool ResipUserAgent::makeRegistrationRequest(SipUserMessage& req)
{
    std::shared_ptr<resip::SipMessage> reg = mDum->makeRegistration(resip::NameAddr(mAor));
    if (reg == nullptr)
    {
        return false;
    }

    /* set with GB28181 required format */
    reg->header(resip::h_RequestLine).uri() = mServerUri;

    SipAdapterMessage adapter = {
        .instance = reg
    };
    req.setAdapter(adapter);
    return true;
}

bool ResipUserAgent::sendRegistration(const SipUserMessage& req)
{
    const std::shared_ptr<SipAdapterMessage> &adapter = req.getAdapter();
    if (adapter == nullptr || adapter->instance == nullptr)
    {
        return false;
    }

    mDum->send(adapter->instance);
    return true;
}

bool ResipUserAgent::sendKeepaliveRequest(const XMLDocument &notify)
{
    if (!mKeepaliveHandle.isValid())
    {
        return false;
    }

    std::unique_ptr<resip::Contents> contents(new MANSCDPContents(notify));
    mKeepaliveHandle.get()->page(std::move(contents));
    return true;
}

bool ResipUserAgent::sendMANSCDPResponse(const XMLDocument &res)
{
    if (!mMANSCDPResponseHandle.isValid())
    {
        return false;
    }

    std::unique_ptr<resip::Contents> contents(new MANSCDPContents(res));
    mMANSCDPResponseHandle.get()->page(std::move(contents));
    return true;
}

bool ResipUserAgent::makeSessionResponse(const SipUserMessage& req, SipUserMessage& res, int32_t code)
{
    std::shared_ptr<SipAdapterMessage> reqAdapter = req.getAdapter();
    if (reqAdapter == nullptr || reqAdapter->instance == nullptr)
    {
        return false;
    }

    resip::Contents *reqContents = reqAdapter->instance->getContents();
    if (reqContents == nullptr)
    {
        return false;
    }
    resip::SdpContents *reqSdp = dynamic_cast<resip::SdpContents *>(reqContents);
    if (reqSdp == nullptr)
    {
        return false;
    }

    std::shared_ptr<resip::SipMessage> instance = std::make_shared<resip::SipMessage>();
    Helper::Helper::makeResponse(*instance, *(reqAdapter->instance), code);

    resip::SdpContents sdp = *reqSdp;
    sdp.session().clearMedium();
    instance->setContents(&sdp);

    SipAdapterMessage adapter = {
        .instance = instance
    };
    res.setAdapter(adapter);
    return true;
}

bool ResipUserAgent::sendSessionResponse(const SipUserMessage& res)
{
    const std::shared_ptr<SipAdapterMessage> adapter = res.getAdapter();
    if (adapter == nullptr || adapter->instance == nullptr)
    {
        return false;
    }

    const std::shared_ptr<resip::SipMessage> instance = adapter->instance;
    const resip::Contents *contents = adapter->instance->getContents();
    if (contents == nullptr)
    {
        return false;
    }

    const resip::SdpContents *sdp = dynamic_cast<const resip::SdpContents*>(contents);
    if (sdp == nullptr)
    {
        return false;
    }

    mInviteSessionHandle->provideAnswer(*sdp);
    ServerInviteSession* uas = dynamic_cast<ServerInviteSession*>(mInviteSessionHandle.get());
    if(uas && !uas->isAccepted())
    {
        uas->accept();
        return true;
    }
    else
    {
        return false;
    }
}

// Registration Handler ////////////////////////////////////////////////////////
void ResipUserAgent::onSuccess(resip::ClientRegistrationHandle h, const resip::SipMessage& response)
{
    (void) h;
    SipAdapterMessage adapter = {
        .instance = std::make_shared<resip::SipMessage>(response)
    };
    
    SipUserMessage user;
    user.setAdapter(adapter);
    postRegistrationResponse(user);
}

void ResipUserAgent::onFailure(resip::ClientRegistrationHandle h, const resip::SipMessage& response)
{
    onSuccess(h, response);
}

void ResipUserAgent::onRemoved(resip::ClientRegistrationHandle h, const resip::SipMessage& response)
{
    onSuccess(h, response);
}

int ResipUserAgent::onRequestRetry(resip::ClientRegistrationHandle h, int retryMinimum, const resip::SipMessage& msg)
{
    std::cout << "retryMinimum=" << retryMinimum << std::endl;

    onSuccess(h, msg);

    return mRegistrationRetryDelayTime;
}

// Invite Session Handler /////////////////////////////////////////////////////
void ResipUserAgent::onNewSession(resip::ServerInviteSessionHandle h, resip::InviteSession::OfferAnswerType oat, const resip::SipMessage& msg)
{
    (void) h;
    (void) oat;
    (void) msg;
}

void ResipUserAgent::onConnectedConfirmed(InviteSessionHandle, const SipMessage &msg)
{
    SipAdapterMessage adapter = {
        .instance = std::make_shared<resip::SipMessage>(msg)
    };

    SipUserMessage user;
    user.setAdapter(adapter);
    postSessionRequest(user);
}

void ResipUserAgent::onTerminated(resip::InviteSessionHandle h, resip::InviteSessionHandler::TerminatedReason reason, const resip::SipMessage* msg)
{
    if (h != mInviteSessionHandle)
    {
        return;
    }
    std::cout << "terminated reason: " << reason << std::endl;

    SipAdapterMessage adapter = {
        .instance = std::make_shared<resip::SipMessage>(*msg)
    };

    SipUserMessage user;
    user.setAdapter(adapter);
    postSessionRequest(user);
}

void ResipUserAgent::onOffer(resip::InviteSessionHandle handle, const resip::SipMessage& msg, const resip::SdpContents& offer)
{
    (void) offer;
    mInviteSessionHandle = handle;

    SipAdapterMessage adapter = {
        .instance = std::make_shared<resip::SipMessage>(msg)
    };

    SipUserMessage user;
    user.setAdapter(adapter);
    postSessionRequest(user);
}

// PagerMessageHandler //////////////////////////////////////////////////////////
void ResipUserAgent::onSuccess(ClientPagerMessageHandle h, const SipMessage& status)
{
    if (h == mKeepaliveHandle)
    {
        postKeepaliveResponse(status.header(resip::h_StatusLine).statusCode());
    }
}

void ResipUserAgent::onFailure(ClientPagerMessageHandle h, const SipMessage& status, std::unique_ptr<Contents> contents)
{
    (void) h;
    (void) status;
    (void) contents;
}

void ResipUserAgent::onMessageArrived(ServerPagerMessageHandle h, const SipMessage& message)
{
    auto ok = h->accept();
    h->send(std::move(ok));

    Contents *contents = message.getContents();
    if (contents != nullptr)
    {
        MANSCDPContents *mans = dynamic_cast<MANSCDPContents *>(contents);
        postMANSCDPRequest(mans->xml());
    }
}