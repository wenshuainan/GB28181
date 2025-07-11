#include <iostream>
#include <thread>
#include "ResipUserAgent.h"
#include "UA.h"
#include "resip/dum/ClientOutOfDialogReq.hxx"

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

bool SipUserAgent::postRegistrationResponse(const SipMessageApp& res)
{
    return app->recvRegistrationResponse(res);
}

bool SipUserAgent::postOutDialogRequest(const SipMessageApp& req)
{
    return app->recvOutDialogRequest(req);
}

bool SipUserAgent::postOutDialogResponse(const SipMessageApp& res, const SipMessageApp& req)
{
    return app->recvOutDialogResponse(res, req);
}

bool SipUserAgent::postSessionRequest(const SipMessageApp& req)
{
    return app->recvSessionRequest(req);
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

    std::cout << "Debug: SipMessageAdapter reference count = " << adapter.use_count() << std::endl;
    mDum->send(adapter->instance);
    return true;
}

bool ResipUserAgent::makeReqMessage(SipMessageApp& req, const std::string& method)
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
        return true;
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
        return true;
    }

    return false;
}

bool ResipUserAgent::makeResMessage(SipMessageApp& res, const SipMessageApp& req, int code, const std::string& reasonPhrase)
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

void ResipUserAgent::onSuccess(resip::ClientRegistrationHandle h, const resip::SipMessage& response)
{
    
    SipMessageAdapter messageAdapter = {
        .instance = std::make_shared<resip::SipMessage>(response)
    };
    
    SipMessageApp messageApp;
    messageApp.setAdapter(messageAdapter);
    postRegistrationResponse(messageApp);
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
    onSuccess(h, msg);

    return mRegistrationRetryDelayTime;
}

void ResipUserAgent::onSuccess(resip::ClientOutOfDialogReqHandle h, const resip::SipMessage& response)
{
    SipMessageAdapter adapterRes = {
        .instance = std::make_shared<resip::SipMessage>(response)
    };
    SipMessageAdapter adapterReq = {
        .instance = std::make_shared<resip::SipMessage>(h->getRequest())
    };
    
    SipMessageApp AppRes;
    AppRes.setAdapter(adapterRes);
    SipMessageApp AppReq;
    AppReq.setAdapter(adapterReq);
    postOutDialogResponse(AppRes, AppReq);
}

void ResipUserAgent::onFailure(resip::ClientOutOfDialogReqHandle h, const resip::SipMessage& response)
{
    onSuccess(h, response);
}

void ResipUserAgent::onReceivedRequest(resip::ServerOutOfDialogReqHandle h, const resip::SipMessage& request)
{
    SipMessageAdapter adapterReq = {
        .instance = std::make_shared<resip::SipMessage>(request)
    };
    
    SipMessageApp AppReq;
    AppReq.setAdapter(adapterReq);
    postOutDialogRequest(AppReq);
}