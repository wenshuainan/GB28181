#ifndef RTP_ADAPTER_H
#define RTP_ADAPTER_H

class RtpAdapter
{
public:
    struct Info
    {
    };

public:
    RtpAdapter(const Info& info);
    virtual ~RtpAdapter();
};

#endif