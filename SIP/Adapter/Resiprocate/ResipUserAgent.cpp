#include <iostream>
#include <thread>
#include "ResipUserAgent.h"

SipUserAgent* SipUserAgent::create(const SipUserAgent::Info& info)
{
    return new ResipUserAgent(info);
}

ResipUserAgent::ResipUserAgent(const SipUserAgent::Info& info)
    : BasicClientUserAgent(info)
{}

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

bool ResipUserAgent::recv(SipGenericMessage& message)
{
    return false;
}

bool ResipUserAgent::send(const SipGenericMessage& message)
{
    printf(">>>>>> %s:%d\n", __FILE__, __LINE__);
    const std::shared_ptr<SipMessageAdapter> &adapter = message.getAdapter();
    if (adapter == nullptr || adapter->instance == nullptr)
    {
        printf(">>>>>> %s:%d\n", __FILE__, __LINE__);
        return false;
    }

    printf(">>>>>> %s:%d\n", __FILE__, __LINE__);
    mDum->send(adapter->instance);
    printf(">>>>>> %s:%d\n", __FILE__, __LINE__);
    return true;
}

bool ResipUserAgent::genReqMessage(SipGenericMessage& req, const std::string& method, const std::string& requestUri)
{
    if (req.getAdapter() == nullptr)
    {
        return false;
    }

    if (method == "REGISTER")
    {
        SipMessageAdapter adapter;
        adapter.instance = mDum->makeRegistration(resip::NameAddr(mAor), nullptr);
        req.setAdapter(adapter);
    }
    else
    {
        resip::Uri uri = resip::BasicClientCmdLineParser::toUri(requestUri.c_str(), "");
        resip::NameAddr target(uri);
        resip::MethodTypes meth;

        if (method == "MESSAGE")
        {
            meth = resip::MESSAGE;
        }
        else if (method == "INFO")
        {
            meth = resip::INFO;
        }

        SipMessageAdapter adapter;
        adapter.instance = mDum->makeOutOfDialogRequest(target, meth, nullptr);
        req.setAdapter(adapter);
    }

    return true;
}

bool ResipUserAgent::genResMessage(SipGenericMessage& res, const SipGenericMessage& req, int code, const std::string& reasonPhrase)
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