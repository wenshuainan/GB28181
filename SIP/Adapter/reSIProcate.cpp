#include <iostream>
#include "reSIProcate.h"

reSIProcate::reSIProcate()
{}

reSIProcate::~reSIProcate()
{}

bool reSIProcate::init()
{
    return false;
}

bool reSIProcate::recv(Header& header, std::string& body)
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

bool reSIProcate::send(const Header& header, const std::string& body)
{
    std::cout << "reSIProcate::send" << std::endl;
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