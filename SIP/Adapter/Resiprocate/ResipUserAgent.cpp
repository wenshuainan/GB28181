#include <iostream>
#include <thread>
#include "ResipUserAgent.h"
#include "UA.h"
#include "MANSCDPContents.h"
#include "resip/dum/ClientPagerMessage.hxx"
#include "resip/dum/ServerPagerMessage.hxx"

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

void ResipUserAgent::onSuccess(resip::ClientRegistrationHandle h, const resip::SipMessage& response)
{
    
    SipAdapterMessage messageAdapter = {
        .instance = std::make_shared<resip::SipMessage>(response)
    };
    
    SipUserMessage messageApp;
    messageApp.setAdapter(messageAdapter);
    postRegistrationResponse(messageApp);
}

// Registration Handler ////////////////////////////////////////////////////////
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
    onSuccess(h, msg);

    return mRegistrationRetryDelayTime;
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
{}

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