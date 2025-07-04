#ifndef PS_SEMANTICS_H
#define PS_SEMANTICS_H

#include <vector>
#include <memory>

class BitStream
{
private:
    std::vector<uint8_t> stream;
    int32_t offset;
    uint16_t length;
    int32_t lengthFieldOffset;

public:
    BitStream(int32_t lengthFiledOffset = -1);
    virtual ~BitStream();
    int32_t getLength() const;
    const uint8_t* getData() const;

private:
    void updateLengthField();
    void write16be(int32_t nbits, uint16_t value, int32_t startbit = 0);
    void write32be(int32_t nbits, uint32_t value, int32_t startbit = 0);

public:
    void clear();
    void write8(int32_t nbits, uint8_t value, int32_t startbit = 0);
    void write16(int32_t nbits, uint16_t value, int32_t startbit = 0);
    void write32(int32_t nbits, uint32_t value, int32_t startbit = 0);
    void write64(int32_t nbits, uint64_t value, int32_t startbit = 0);
    void writeBytes(const std::vector<uint8_t>& data);
    void writeBitStream(const BitStream& bitstream);
};

class SystemHeader
{
private:
    BitStream bitstream;

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
    struct StreamType
    {
        uint8_t stream_id;
        uint8_t stream_id_extension;
        uint8_t P_STD_buffer_bound_scale;
        uint16_t P_STD_buffer_size_bound;
    };
    std::vector<StreamType> streamTypes;

public:
    SystemHeader();
    virtual ~SystemHeader();
    void toBitStream();
    void toBitStream(BitStream& bitstream);
    const BitStream& getBitStream() const;
    int32_t getBitStreamLength() const;

public:
    void addVideoStreamType(uint8_t stream_id);
    void addAudioStreamType(uint8_t stream_id);
};

class PackHeader
{
private:
    BitStream bitstream;

private:
    uint32_t pack_start_code;
    uint64_t system_clock_reference_base;
    uint16_t system_clock_reference_extension;
    uint32_t program_mux_rate;
    uint8_t reserved;
    uint8_t pack_stuffing_length;
    std::vector<uint8_t> pack_stuffing_bytes;
    std::shared_ptr<SystemHeader> system_header;

public:
    PackHeader();
    virtual ~PackHeader();
    void toBitStream();
    void toBitStream(BitStream& bitstream);
    const BitStream& getBitStream() const;

public:
    void setSystemHeader(const std::shared_ptr<SystemHeader>& system_header);
    void updateMuxRate(int32_t addedLength);
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

class PESPacket
{
private:
    BitStream bitstream;

private:
    uint32_t packet_start_code_prefix;
    uint8_t stream_id;
    uint16_t PES_packet_length;
    uint8_t PES_scrambling_control;
    uint8_t PES_priority;
    uint8_t data_alignment_indicator;
    uint8_t copyright;
    uint8_t original_or_copy;
    uint8_t PTS_DTS_flags;
    uint8_t ESCR_flag;
    uint8_t ES_rate_flag;
    uint8_t DSM_trick_mode_flag;
    uint8_t additional_copy_info_flag;
    uint8_t PES_CRC_flag;
    uint8_t PES_extension_flag;
    uint8_t PES_header_data_length;
    uint64_t PTS;
    uint64_t DTS;
    uint64_t ESCR_base;
    uint16_t ESCR_extension;
    uint32_t ES_rate;
    uint8_t trick_mode_control;
    uint8_t field_id;
    uint8_t intra_slice_refresh;
    uint8_t frequency_truncation;
    uint8_t rep_cntrl;
    uint8_t additional_copy_info;
    uint8_t previous_PES_packet_CRC;
    uint8_t PES_private_data_flag;
    uint8_t pack_header_field_flag;
    uint8_t program_packet_sequence_counter_flag;
    uint8_t P_STD_buffer_flag;
    uint8_t PES_extension_flag_2;
    std::vector<uint8_t> PES_private_data;
    uint8_t pack_field_length;
    uint8_t program_packet_sequence_counter;
    uint8_t MPEG1_MPEG2_identifier;
    uint8_t original_stuff_length;
    uint8_t P_STD_buffer_scale;
    uint16_t P_STD_buffer_size;
    uint8_t PES_extension_field_length;
    uint8_t stream_id_extension_flag;
    uint8_t stream_id_extension;
    uint8_t tref_extension_flag;
    uint64_t TREF;
    std::vector<uint8_t> stuffing_byte;
    std::vector<uint8_t> PES_packet_data_byte;
    std::vector<uint8_t> padding_byte;

public:
    PESPacket(uint8_t stream_id);
    virtual ~PESPacket();
    virtual void toBitStream();
    virtual void toBitStream(BitStream& bitstream);
    virtual const BitStream& getBitStream() const;
    int32_t getBitStreamLength() const;

public:
    int32_t writeDataByte(const uint8_t* data, int32_t size);
};

class ProgramStreamMap : public PESPacket
{
private:
    BitStream bitstream;

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
    uint16_t elementary_stream_map_length;
    struct ElementaryStream
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
    std::vector<ElementaryStream> elementary_stream_map;
    uint32_t CRC_32;

public:
    ProgramStreamMap();
    virtual ~ProgramStreamMap();
    void toBitStream();
    void toBitStream(BitStream& bitstream);
    const BitStream& getBitStream() const;

public:
    void addElementaryStream(uint8_t stream_type, uint8_t elementary_stream_id);
};

class Pack
{
private:
    BitStream bitstream;

private:
    PackHeader pack_header;
    std::vector<std::shared_ptr<PESPacket>> PES_packet;

public:
    Pack();
    virtual ~Pack();
    void toBitStream();
    void toBitStream(BitStream& bitstream);
    const BitStream& getBitStream() const;

public:
    void addSystemHeader(const std::shared_ptr<SystemHeader>& system_header);
    void addPESPacket(const std::shared_ptr<PESPacket>& PES_packet);
    const std::vector<std::shared_ptr<PESPacket>>& getPESPacket() const;
};

#endif