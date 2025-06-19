#include "Proxy.h"

Proxy::Proxy()
{
    registration = nullptr;
    play = nullptr;
    control = nullptr;

    requestTypes.push_back(new ControlReuest());
}

Proxy::~Proxy()
{
    for (auto i : requestTypes)
    {
        delete i;
    }
}

void Proxy::setProcess(Registration *registration)
{
    this->registration = registration;
}

void Proxy::setProcess(Play *play)
{
    this->play = play;
}

void Proxy::setProcess(Control *control)
{
    this->control = control;
}

bool Proxy::processControl(const PTZCmdRequest::Request& req)
{
    if (control)
    {
        DeviceControlResponse::Response res;
        control->process(req, res);
        //序列化res && 发送
    }

    return false;
}

bool Proxy::processControl(const TeleBootRequest::Request& req)
{
}