#include <iostream>
#include "ResipUserAgent.h"
#include "UA.h"
#include "MANSCDPContents.h"
#include "MANSRTSPContents.h"
#include "resip/dum/ClientPagerMessage.hxx"
#include "resip/dum/ServerPagerMessage.hxx"
#include "resip/dum/ServerInviteSession.hxx"
#include "resip/dum/Handle.hxx"
#include "resip/dum/Handles.hxx"
#include "resip/dum/AppDialogSet.hxx"
#include "resip/dum/ClientRegistration.hxx"

bool SipUserAgent::postRegistrationResponse(const SipUserMessage& res)
{
    return m_user->dispatchRegistrationResponse(res);
}

bool SipUserAgent::postMANSCDPRequest(const XMLDocument &req)
{
    return m_user->dispatchMANSCDPRequest(req);
}

bool SipUserAgent::postMANSCDPResult(int32_t code, const XMLDocument& cmd)
{
    return m_user->dispatchMANSCDPResult(code, cmd);
}

bool SipUserAgent::postSessionRequest(const SessionIdentifier& id, const SipUserMessage& req)
{
    return m_user->dispatchSessionRequest(id, req);
}

bool SipUserAgent::postMANSRTSPRequest(const SipUserMessage& req)
{
    return m_user->dispatchMANSRTSPRequest(req);
}

std::shared_ptr<SipUserAgent> SipUserAgent::create(UA *user, const ClientInfo& client, const ServerInfo& server)
{
    if (user == nullptr)
    {
        return nullptr;
    }

    std::shared_ptr<SipUserAgent> sip = std::make_shared<ResipUserAgent>(client, server);
    if (sip != nullptr)
    {
        sip->m_user = user;
    }

    return sip;
}

ResipUserAgent::ResipUserAgent(const SipUserAgent::ClientInfo& client, const SipUserAgent::ServerInfo& server)
    : BasicClientUserAgent(client)
{
    printf("++++++ ResipUserAgent\n");
    printf("client info:\nid=%s\nipv4=%s:%d\n", client.id.c_str(), client.ipv4.c_str(), client.port);
    printf("server info:\nid=%s\nipv4=%s:%d\n", server.id.c_str(), server.ipv4.c_str(), server.port);
    mServerUri.user() = server.id;
    mServerUri.host() = server.ipv4;
    mServerUri.port() = server.port;

    mRegistrationRetryDelayTime = client.interval;
}

ResipUserAgent::~ResipUserAgent()
{
    printf("------ ResipUserAgent\n");
}

bool ResipUserAgent::start()
{
    startup();
    return true;
}

bool ResipUserAgent::stop()
{
    mDum->forceShutdown(this);
    return true;
}

bool ResipUserAgent::threadFunc(int32_t interval)
{
    return process(interval);
}

const char* ResipUserAgent::getSipUser()
{
    return mAor.user().c_str();
}

bool ResipUserAgent::makeRegistration(SipUserMessage& req)
{
    /* 释放之前的注册资源 */
    if (mRegHandle.isValid())
    {
        resip::AppDialogSetHandle handle = mRegHandle->getAppDialogSet();
        handle->end();
        mRegHandle->stopRegistering();
    }

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

bool ResipUserAgent::sendMANSCDP(const XMLDocument& cmd)
{
    resip::ClientPagerMessageHandle handle = mDum->makePagerMessage(resip::NameAddr(mServerUri));
    if (!handle.isValid())
    {
        printf("make pager message error\n");
        return false;
    }
    std::unique_ptr<resip::Contents> contents(new MANSCDPContents(cmd));
    resip::Data debug;
    {
        resip::DataStream s(debug);
        contents->encodeParsed(s);
    }
    printf("send MANSCDP cmd:\n%s\n", debug.c_str());
    handle.get()->page(std::move(contents));
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
    resip::Helper::makeResponse(*instance, *(reqAdapter->instance), code);

    if (code == 200)
    {
        resip::SdpContents sdp = *reqSdp;
        sdp.session().clearMedium();
        instance->setContents(&sdp);
    }

    SipAdapterMessage adapter = {
        .instance = instance
    };
    res.setAdapter(adapter);
    return true;
}

bool ResipUserAgent::sendSessionResponse(const SessionIdentifier& id, const SipUserMessage& res)
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

    resip::InviteSession *session = (resip::InviteSession *)id;
    session->provideAnswer(*sdp);
    resip::ServerInviteSession* uas = dynamic_cast<resip::ServerInviteSession*>(session);
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

bool ResipUserAgent::sendSessionMessage(const SessionIdentifier& id, const XMLDocument& cmd)
{
    MANSCDPContents contents(cmd);
    resip::Data debug;
    {
        resip::DataStream s(debug);
        contents.encodeParsed(s);
    }
    printf("send session Notify:\n%s\n", debug.c_str());
    resip::InviteSession *session = (resip::InviteSession *)id;
    session->message(contents);
    return true;
}

bool ResipUserAgent::sendMANSRTSP(const SessionIdentifier& id, const MANSRTSP::Message& cmd)
{
    MANSRTSPContents contents(cmd);
    resip::Data debug;
    {
        resip::DataStream s(debug);
        contents.encodeParsed(s);
    }
    printf("send MANSRTSP Response:\n%s\n", debug.c_str());
    resip::InviteSession *session = (resip::InviteSession *)id;
    session->acceptNIT(200, &contents);
    return true;
}

// Registration Handler ////////////////////////////////////////////////////////
void ResipUserAgent::onSuccess(resip::ClientRegistrationHandle h, const resip::SipMessage& response)
{
    mRegHandle = h;
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
{}

void ResipUserAgent::onConnectedConfirmed(resip::InviteSessionHandle h, const resip::SipMessage &msg)
{
    SipAdapterMessage adapter = {
        .instance = std::make_shared<resip::SipMessage>(msg)
    };

    SipUserMessage user;
    user.setAdapter(adapter);
    postSessionRequest((SessionIdentifier)(h.get()), user);
}

void ResipUserAgent::onTerminated(resip::InviteSessionHandle h, resip::InviteSessionHandler::TerminatedReason reason, const resip::SipMessage* msg)
{
    std::cout << "terminated reason: " << reason << std::endl;

    SipAdapterMessage adapter = {
        .instance = std::make_shared<resip::SipMessage>(*msg)
    };

    SipUserMessage user;
    user.setAdapter(adapter);
    postSessionRequest((SessionIdentifier)(h.get()), user);
}

void ResipUserAgent::onOffer(resip::InviteSessionHandle handle, const resip::SipMessage& msg, const resip::SdpContents& offer)
{
    SipAdapterMessage adapter = {
        .instance = std::make_shared<resip::SipMessage>(msg)
    };

    SipUserMessage user;
    user.setAdapter(adapter);
    postSessionRequest((SessionIdentifier)(handle.get()), user);
}

void ResipUserAgent::onInfo(resip::InviteSessionHandle, const resip::SipMessage& msg)
{
    SipAdapterMessage adapter = {
        .instance = std::make_shared<resip::SipMessage>(msg)
    };

    SipUserMessage user;
    user.setAdapter(adapter);
    postMANSRTSPRequest(user);
}

// PagerMessageHandler //////////////////////////////////////////////////////////
void ResipUserAgent::onSuccess(resip::ClientPagerMessageHandle h, const resip::SipMessage& status)
{
    resip::SipMessage& sipMessage = h.get()->getMessageRequest();
    resip::Contents *contents = sipMessage.getContents();
    if (contents)
    {
        MANSCDPContents *cdp = dynamic_cast<MANSCDPContents *>(contents);
        if (cdp)
        {
            postMANSCDPResult(status.header(resip::h_StatusLine).statusCode(), cdp->xml());
        }
    }
    h.get()->end();
}

void ResipUserAgent::onFailure(resip::ClientPagerMessageHandle h, const resip::SipMessage& status, std::unique_ptr<resip::Contents> contents)
{
    onSuccess(h, status);
}

void ResipUserAgent::onMessageArrived(resip::ServerPagerMessageHandle h, const resip::SipMessage& message)
{
    auto ok = h->accept();
    h->send(std::move(ok));

    resip::Contents *contents = message.getContents();
    if (contents != nullptr)
    {
        resip::Data debug = contents->getBodyData();
        printf("onMessageArrived:\n%s\n", debug.c_str());
        MANSCDPContents *cdp = dynamic_cast<MANSCDPContents *>(contents);
        postMANSCDPRequest(cdp->xml());
    }
}