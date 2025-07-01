#ifndef PROGRAM_STREAM_H
#define PROGRAM_STREAM_H

#include <vector>
#include <memory>

class BitStream
{
public:
    void write(int32_t nbits, uint8_t value, int32_t valueoffset = 0);
    void write(int32_t nbits, uint16_t value, int32_t valueoffset = 0);
    void write(int32_t nbits, uint32_t value, int32_t valueoffset = 0);
    void write(int32_t nbits, uint64_t value, int32_t valueoffset = 0);
};

class SystemHeader
{
private:
    uint32_t sytem_header_start_code;
    uint16_t header_length;
    uint32_t rate_bound;
    uint8_t audio_bound;
    uint8_t fixed_flag;
    uint8_t CSPS_flag;
    uint8_t system_audio_lock_flag;
    uint8_t system_video_lock_flag;
    uint8_t video_bound;
    uint8_t packet_rate_restriction_flag;
    uint8_t reserved_bits;
    struct NextBits
    {
        uint8_t stream_id;
        union
        {
            struct
            {
                uint8_t stream_id_extension;
                uint8_t STD_buffer_bound_scale;
                uint16_t STD_buffer_size_bound;
            };
            struct
            {
                uint8_t STD_buffer_bound_scale;
                uint16_t STD_buffer_size_bound;
            };
        };
    };
    std::vector<NextBits> next_bits;
};

class PackHeader
{
private:
    uint32_t pack_start_code;
    uint64_t system_clock_reference_base;
    uint16_t system_clock_reference_extension;
    uint32_t program_mux_rate;
    uint8_t reserved;
    uint8_t pack_stuffing_length;
    std::vector<uint8_t> pack_stuffing_bytes;
    std::shared_ptr<SystemHeader> system_header;
};

class Descriptor
{
public:
    virtual void toBitStream(BitStream& bitstream) = 0;
};

class AVCVideoDescriptor : public Descriptor
{};

class HEVCVideoDescriptor : public Descriptor
{};

class MPEG2AACAudioDescriptor : public Descriptor
{};

class ProgramStreamMap
{
private:
    uint32_t packet_start_code_prefix;
    uint8_t map_stream_id;
    uint16_t program_stream_map_length;
    uint8_t current_next_indicator;
    uint8_t single_extension_stream_flag;
    uint8_t program_stream_map_version;
    uint8_t marker_bit;
    uint16_t program_stream_info_length;
    std::vector<std::shared_ptr<Descriptor>> descriptor;
    uint16_t elementry_stream_map_length;
    struct ElementaryStreamMap
    {
        uint8_t stream_type;
        uint16_t elementary_stream_id;
        uint16_t elementary_stream_info_length;
        uint8_t pseudo_descriptor_tag;
        uint8_t pseudo_descriptor_length;
        uint8_t marker_bit;
        uint8_t elementary_stream_id_extension;
        std::vector<std::shared_ptr<Descriptor>> descriptor;
    };
    std::vector<ElementaryStreamMap> elementary_stream_map;
};

class PESPacket
{
private:
    uint32_t packet_start_code_prefix;
    uint8_t stream_id;
    uint16_t PES_packet_length;
};

class Pack
{
private:
    PackHeader pack_header;
    std::vector<std::shared_ptr<PESPacket>> pes_packets;
};

#endif