#ifndef PROGRAM_STREAM_H
#define PROGRAM_STREAM_H

#include <vector>
#include <memory>

class BitStream
{
public:
    void write(int nbits, unsigned char value);
    void write(int nbits, unsigned short value);
    void write(int nbits, unsigned int value);
    void write(int nbits, unsigned long long value, int valueoffset);
};

class SystemHeader
{};

class PackHeader
{};

class ProgramStreamMap
{};

class PESPacket
{
private:
    std::vector<std::shared_ptr<ProgramStreamMap>> program_stream_maps;
};

class Pack
{
private:
    PackHeader pack_header;
    std::vector<std::shared_ptr<PESPacket>> pes_packets;
};

#endif