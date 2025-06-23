#include <iostream>
#include "Resiprocate.h"

Resiprocate::Resiprocate()
{}

Resiprocate::~Resiprocate()
{}

bool Resiprocate::init()
{
    return false;
}

bool Resiprocate::recv(Header& header, std::string& body)
{
    header.setRequestLine("MESSAGE", "sip:1000@192.168.1.100");
    header.addField("From", "sip:1000@192.168.1.100");
    header.addField("To", "sip:1000@192.168.1.100");
    header.addField("Call-ID", "1000");
    header.addField("CSeq", "1 INVITE");
    header.addField("Contact", "sip:1000@192.168.1.100");
    header.addField("Content-type", "Application/MANSCDP+xml");
    header.addField("Content-Length", "150");

    body ="<Control> \
            <CmdType>DeviceControl</CmdType> \
            <SN>99</SN> \
            <DeviceID>123456</DeviceID> \
            <PTZCmd>66666666</PTZCmd> \
            <PTZCmdParams> \
            <PresetName>abc</PresetName> \
            <CruiseTrackName>def</CruiseTrackName> \
            </PTZCmdParams> \
            </Control>";

    return true;
}

bool Resiprocate::send(const Header& header, const std::string& body)
{
    std::cout << "Resiprocate::send" << std::endl;
    std::cout << header.getStatusLine().getCode() << std::endl;
    std::cout << header.getField("From").getValue() << std::endl;
    std::cout << header.getField("To").getValue() << std::endl;
    std::cout << header.getField("Call-ID").getValue() << std::endl;
    std::cout << header.getField("CSeq").getValue() << std::endl;
    std::cout << header.getField("Contact").getValue() << std::endl;
    std::cout << header.getField("Content-Type").getValue() << std::endl;
    std::cout << header.getField("Content-Length").getValue() << std::endl;

    std::cout << body << std::endl;

    return true;
}

bool Resiprocate::genReqHeader(const std::string& method, Header& req)
{
    std::cout << "Resiprocate::genReqHeader" << std::endl;
    req.setRequestLine(method, "sip:1000@192.168.1.100");
    req.addField("From", "sip:1000@192.168.1.100");
    req.addField("To", "sip:1000@192.168.1.100");
    req.addField("Call-ID", "1000");
    req.addField("CSeq", "1 INVITE");
    req.addField("Contact", "sip:1000@192.168.1.100");
    req.addField("Content-type", "Application/MANSCDP+xml");
    req.addField("Content-Length", "150");

    return true;
}

bool Resiprocate::genResHeader(const Header& req, int code, const std::string& reasonPhrase, Header& res)
{
    std::cout << "Resiprocate::genResHeader" << std::endl;
    res.setStatusLine(code, reasonPhrase);
    res.addField("From", "sip:1000@192.168.1.101");
    res.addField("To", "sip:1000@192.168.1.101");
    res.addField("Call-ID", "1001");
    res.addField("CSeq", "2 INVITE");
    res.addField("Contact", "sip:1000@192.168.1.101");

    return true;
}