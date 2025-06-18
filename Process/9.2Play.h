#ifndef PLAY_H
#define PLAY_H

class Play
{
protected:
    struct SDP
    {};
    
private:
    virtual bool Invite(const struct SDP& sdpRemote, struct SDP& sdpLocal) = 0;
    virtual bool ACK() = 0;
    virtual bool BYE() = 0;
};

#endif