#include <iostream>
#include <thread>
#include "ResipUserAgent.h"
#include "UA.h"

std::shared_ptr<SipUserAgent> SipUserAgent::create(UA *app, const ClientInfo& client, const ServerInfo& server)
{
    std::shared_ptr<SipUserAgent> sip = std::make_shared<ResipUserAgent>(client, server);

    if (sip != nullptr)
    {
        sip->app = app;
    }

    return sip;
}

bool SipUserAgent::destroy()
{}

bool SipUserAgent::postApp(UA *app, const SipMessageApp& message)
{
    return app->postRecved(message);
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
    while (bInit)
    {
        process(1000);
    }
}

bool ResipUserAgent::init()
{
    startup();

    bInit = true;
    std::thread t(&ResipUserAgent::threadProc, this);
    t.detach();

    return true;
}

bool ResipUserAgent::recv(SipMessageApp& message)
{
    return false;
}

bool ResipUserAgent::send(const SipMessageApp& message)
{
    const std::shared_ptr<SipMessageAdapter> &adapter = message.getAdapter();
    if (adapter == nullptr || adapter->instance == nullptr)
    {
        return false;
    }

    mDum->send(adapter->instance);
    return true;
}

bool ResipUserAgent::genReqMessage(SipMessageApp& req, const std::string& method)
{
    if (req.getAdapter() == nullptr)
    {
        return false;
    }

    if (method == "REGISTER")
    {
        std::shared_ptr<resip::SipMessage> message = mDum->makeRegistration(resip::NameAddr(mAor));
        if (message == nullptr)
        {
            return false;
        }

        /* set with GB28181 required format */
        message->header(resip::h_RequestLine).uri() = mServerUri;

        SipMessageAdapter adapter = {
            .instance = message
        };
        req.setAdapter(adapter);
    }
    else
    {
        std::shared_ptr<resip::SipMessage> message = mDum->makeOutOfDialogRequest(resip::NameAddr(mServerUri), resip::MESSAGE);
        if (message == nullptr)
        {
            return false;
        }

        SipMessageAdapter adapter = {
            .instance = message
        };
        req.setAdapter(adapter);
    }

    return true;
}

bool ResipUserAgent::genResMessage(SipMessageApp& res, const SipMessageApp& req, int code, const std::string& reasonPhrase)
{
    const std::shared_ptr<SipMessageAdapter> &reqAdapter = req.getAdapter();
    if (reqAdapter == nullptr || reqAdapter->instance == nullptr)
    {
        return false;
    }

    SipMessageAdapter adapter;
    adapter.instance = std::make_shared<resip::SipMessage>();
    if (adapter.instance != nullptr)
    {
        makeResponse(*(adapter.instance), *(reqAdapter->instance), code, reasonPhrase);
        res.setAdapter(adapter);
        return true;
    }
    else
    {
        return false;
    }
}

void
ResipUserAgent::makeResponse(resip::SipMessage& response,
                                 const resip::SipMessage& request,
                                 int responseCode,
                                 const resip::Data& reason) const
{
   resip_assert(request.isRequest());
   resip::Helper::makeResponse(response, request, responseCode, reason);
}

void ResipUserAgent::postAdapter(const resip::SipMessage& sipMessage)
{
    SipMessageApp messageApp;
    SipMessageAdapter messageAdapter;

    messageAdapter.instance = std::make_shared<resip::SipMessage>(sipMessage);
    messageApp.setAdapter(messageAdapter);
    
    postApp(this->app, messageApp);
}

void ResipUserAgent::onSuccess(resip::ClientRegistrationHandle h, const resip::SipMessage& response)
{
    postAdapter(response);
}

void ResipUserAgent::onFailure(resip::ClientRegistrationHandle h, const resip::SipMessage& response)
{
    postAdapter(response);
}

void ResipUserAgent::onRemoved(resip::ClientRegistrationHandle h, const resip::SipMessage& response)
{
    postAdapter(response);
}

int ResipUserAgent::onRequestRetry(resip::ClientRegistrationHandle h, int retryMinimum, const resip::SipMessage& msg)
{
    postAdapter(msg);

    return mRegistrationRetryDelayTime;
}

void ResipUserAgent::onMessageArrived(resip::ServerPagerMessageHandle, const resip::SipMessage& message)
{
    postAdapter(message);
}