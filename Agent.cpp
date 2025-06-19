#include "Agent.h"

Agent::Agent()
{
    registration = nullptr;
    play = nullptr;
    control = nullptr;

    requestTypes.push_back(new ControlReuest());
}

Agent::~Agent()
{
    for (auto i : requestTypes)
    {
        delete i;
    }
}

void Agent::setProcess(Registration *registration)
{
    this->registration = registration;
}

void Agent::setProcess(Play *play)
{
    this->play = play;
}

void Agent::setProcess(Control *control)
{
    this->control = control;
}

bool Agent::processControl(const PTZCmdRequest::Request& req)
{
    if (control)
    {
        DeviceControlResponse::Response res;
        control->process(req, res);
        //序列化res && 发送
    }

    return false;
}

bool Agent::processControl(const TeleBootRequest::Request& req)
{
}