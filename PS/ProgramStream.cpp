#include <arpa/inet.h>
#include <string.h>
#include <sys/time.h>
#include "ProgramStream.h"

BitStream::BitStream(int32_t lengthFiledOffset = 0)
{
    offset = 0;
    length = 0;
    this->lengthFieldOffset = lengthFiledOffset;
}

BitStream::~BitStream()
{}

int32_t BitStream::getLength() const
{
    return offset / 8;
}

void BitStream::updateLengthField()
{
    if (offset < lengthFieldOffset)
    {
        return;
    }
    else
    {
        uint16_t length = ((offset - lengthFieldOffset) + 8) / 9; // (x+8)/9: 超出之后不足8bit也算一个字节
        if (length == this->length)
        {
            return;
        }
        else
        {
            this->length = length;
        }
    }

    uint16_t bigendian = htons(length); //大端序
    int32_t index = lengthFieldOffset / 8; //长度字段在字节流中的偏移

    stream[index] = bigendian;
    stream[index + 1] = bigendian << 8;
}

void BitStream::write8(int32_t nbits, uint8_t value, int32_t startbit)
{
    int32_t index = offset / 8; //当前字节，从字节流开头的偏移
    int32_t bitoffset = offset % 8; //从当前字节最高位的偏移

    /* 参数非法 */
    if (nbits <= 0 || startbit < 0 || startbit + nbits > 8)
    {
        return;
    }

    /* 扩充字节流空间 */
    if (stream.size() <= index)
    {
        stream.resize(index + 10);
    }

    uint8_t v1 = value & (0xff << startbit); //去掉低位偏移
    uint8_t v2 = v1 << (8 - startbit - nbits); //取nbits的值，将nbits移到最高位
    uint8_t v3 = v2 >> bitoffset; //将nbits移到要赋值的字节偏移处
    stream[index] |= v3;

    /* 当前字节放不下nbits，将剩余的放到下一个字节 */
    int32_t nextn = nbits - (8 - bitoffset);
    if (nextn > 0)
    {
        stream[index + 1] = v2 << (nbits - nextn); //将已赋值的bits移除，并赋值给下一个字节
    }

    offset += nbits;

    /* 更新长度字段 */
    updateLengthField();
}

void BitStream::write16(int32_t nbits, uint16_t value, int32_t startbit)
{
    uint16_t bigendian = htons(value); //大端序

    /* 高位起第一个字节 */
    int32_t b1nbits = nbits + startbit - 8;
    int32_t b1start = startbit > 8 ? startbit - 8 : 0;

    /* 高位起第二个字节 */
    int32_t b2nbits = nbits - b1nbits;
    int32_t b2start = startbit;

    write8(b1nbits, (uint8_t)bigendian, b1start);
    write8(b2nbits, (uint8_t)(bigendian << 8), b2start);
}

void BitStream::write32(int32_t nbits, uint32_t value, int32_t startbit)
{
    uint32_t bigendian = htonl(value); //大端序

    /* 高位起第一个字 */
    int32_t w1nbits = nbits + startbit - 16;
    int32_t w1start = startbit > 16 ? startbit - 16 : 0;

    /* 高位起第二个字 */
    int32_t w2nbits = nbits - w1nbits;
    int32_t w2start = startbit;

    write16(w1nbits, (uint16_t)bigendian, w1start);
    write16(w2nbits, (uint16_t)(bigendian << 16), w2start);
}

void BitStream::write64(int32_t nbits, uint64_t value, int32_t startbit)
{
    uint32_t dw1 = value;
    uint32_t dw2 = value << 32;

    if (dw1 != htonl(dw1))
    {
        dw1 = htonl(dw2);
        dw2 = htonl(dw1);
    }

    /* 高位起第一个双字 */
    int32_t dw1nbits = nbits + startbit - 32;
    int32_t dw1start = startbit > 32 ? startbit - 32 : 0;

    /* 高位起第二个双字 */
    int32_t dw2nbits = nbits - dw1nbits;
    int32_t dw2start = startbit;

    write32(dw1nbits, dw1, dw1start);
    write32(dw2nbits, dw2, dw2start);
}

void BitStream::writeBytes(const std::vector<uint8_t>& data)
{
    int32_t index = offset / 8; //当前字节，从字节流开头的偏移
    int32_t bitoffset = offset % 8; //从当前字节最高位的偏移
    int32_t datasize = data.size();

    /* 参数非法 */
    if (datasize <= 0)
    {
        return;
    }

    /* 扩充字节流空间 */
    if (stream.size() <= index)
    {
        stream.resize(index + datasize + 10);
    }

    /* 将数据拷贝到字节流 */
    memcpy(stream.data() + index, data.data(), datasize);

    offset += datasize * 8;

    /* 更新长度字段 */
    updateLengthField();
}

SystemHeader::SystemHeader()
    : bitstream(32)
{
    sytem_header_start_code = 0x000001BB;
    rate_bound = 0xFFFFFFFF;
    audio_bound = 1;
    fixed_flag = 0;
    CSPS_flag = 0;
    system_audio_lock_flag = 1;
    system_video_lock_flag = 1;
    video_bound = 1;
    packet_rate_restriction_flag = 0;
}

SystemHeader::~SystemHeader()
{}

void SystemHeader::toBitStream()
{
    toBitStream(bitstream);
}

void SystemHeader::toBitStream(BitStream& bitstream)
{
    bitstream.write32(32, sytem_header_start_code);
    bitstream.write16(16, header_length);
    bitstream.write8(1, 1);
    bitstream.write32(22, rate_bound);
    bitstream.write8(1, 1);
    bitstream.write8(6, audio_bound);
    bitstream.write8(1, fixed_flag);
    bitstream.write8(1, CSPS_flag);
    bitstream.write8(1, system_audio_lock_flag);
    bitstream.write8(1, system_video_lock_flag);
    bitstream.write8(1, 1);
    bitstream.write8(5, video_bound);
    bitstream.write8(1, packet_rate_restriction_flag);
    bitstream.write8(7, 0x7F);
    for (auto& it : streamTypes)
    {
        bitstream.write8(8, it.stream_id);
        if (it.stream_id == 0xB7)
        {
            bitstream.write8(2, 0x03);
            bitstream.write8(7, 0);
            bitstream.write8(7, it.stream_id_extension);
            bitstream.write8(8, 0xB6);
            bitstream.write8(2, 0x03);
            bitstream.write8(1, it.P_STD_buffer_bound_scale);
            bitstream.write16(13, it.P_STD_buffer_size_bound);
        }
        else
        {
            bitstream.write8(2, 0x03);
            bitstream.write8(1, it.P_STD_buffer_bound_scale);
            bitstream.write16(13, it.P_STD_buffer_size_bound);
        }
    }
}

const BitStream& SystemHeader::getBitStream() const
{
    return bitstream;
}

int32_t SystemHeader::getStreamLength() const
{}

void SystemHeader::addVideoStreamType(uint8_t stream_id)
{
    StreamType streamType;
    streamType.stream_id = stream_id;
    streamType.P_STD_buffer_bound_scale = 0;
    streamType.P_STD_buffer_size_bound = 100;
    streamTypes.push_back(streamType);
}

void SystemHeader::addAudioStreamType(uint8_t stream_id)
{
    StreamType streamType;
    streamType.stream_id = stream_id;
    streamType.P_STD_buffer_bound_scale = 1;
    streamType.P_STD_buffer_size_bound = 1000;
    streamTypes.push_back(streamType);
}

PackHeader::PackHeader()
{
    system_header = nullptr;
    pack_start_code = 0x000001BA;
    pack_stuffing_length = 0;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t tms = tv.tv_sec  * 1000 + tv.tv_usec / 1000;
    system_clock_reference_base = ((27000000 * tms) / 300) % (1 << 33);
    system_clock_reference_extension = (27000000 * tms) % 300;
}

PackHeader::~PackHeader()
{}

void PackHeader::toBitStream()
{
    toBitStream(bitstream);
}

void PackHeader::toBitStream(BitStream& bitstream)
{
    bitstream.write32(32, pack_start_code);
    bitstream.write8(2, 0x01);
    bitstream.write64(3, system_clock_reference_base, 30);
    bitstream.write8(1, 1);
    bitstream.write64(15, system_clock_reference_base, 15);
    bitstream.write8(1, 1);
    bitstream.write64(15, system_clock_reference_base, 0);
    bitstream.write8(1, 1);
    bitstream.write16(9, system_clock_reference_extension);
    bitstream.write8(1, 1);
    bitstream.write32(22, program_mux_rate);
    bitstream.write8(1, 1);
    bitstream.write8(1, 1);
    bitstream.write8(5, 0);
    bitstream.write8(3, pack_stuffing_length);
    for (auto it : pack_stuffing_bytes)
    {
        bitstream.write8(8, it);
    }
    if (system_header != nullptr)
    {
        system_header->toBitStream(bitstream);
    }
}

const BitStream& PackHeader::getBitStream() const
{
    return bitstream;
}

void PackHeader::setSystemHeader(std::shared_ptr<SystemHeader>& system_header)
{
    this->system_header = system_header;
}

void PackHeader::updateMuxRate(int32_t newPESPacketLength)
{
    uint32_t length = 0;

    length += system_header->getStreamLength();
    length += newPESPacketLength;

    program_mux_rate = length / 50;
}

PESPacket::PESPacket()
    : bitstream(32)
{
    packet_start_code_prefix = 0x000001;
}

PESPacket::~PESPacket()
{}

void PESPacket::toBitStream()
{
    toBitStream(bitstream);
}

void PESPacket::toBitStream(BitStream& bitstream)
{
    bitstream.write32(24, packet_start_code_prefix);
    bitstream.write8(8, stream_id);
    bitstream.write16(16, PES_packet_length);
    bitstream.write8(2, 0x02); // '10b'
    bitstream.write8(2, PES_scrambling_control);
    bitstream.write8(1, PES_priority);
    bitstream.write8(1, data_alignment_indicator);
    bitstream.write8(1, copyright);
    bitstream.write8(1, original_or_copy);
    bitstream.write8(2, PTS_DTS_flags);
    bitstream.write8(1, ESCR_flag);
    bitstream.write8(1, ES_rate_flag);
    bitstream.write8(1, DSM_trick_mode_flag);
    bitstream.write8(1, additional_copy_info_flag);
    bitstream.write8(1, PES_CRC_flag);
    bitstream.write8(1, PES_extension_flag);
    bitstream.write8(8, PES_header_data_length);
    if (PTS_DTS_flags == 0x02)
    {
        bitstream.write8(4, 0x02);
        bitstream.write64(3, PTS, 30);
        bitstream.write8(1, 1);
        bitstream.write64(15, PTS, 15);
        bitstream.write8(1, 1);
        bitstream.write64(15, PTS);
        bitstream.write8(1, 1);
    }
    if (PTS_DTS_flags == 0x03)
    {
        bitstream.write8(4, 0x03);
        bitstream.write64(3, PTS, 30);
        bitstream.write8(1, 1);
        bitstream.write64(15, PTS, 15);
        bitstream.write8(1, 1);
        bitstream.write64(15, PTS);
        bitstream.write8(1, 1);
        bitstream.write8(4, 0x01);
        bitstream.write64(3, DTS, 30);
        bitstream.write8(1, 1);
        bitstream.write64(15, DTS, 15);
        bitstream.write8(1, 1);
        bitstream.write64(15, DTS);
        bitstream.write8(1, 1);
    }
    if (ESCR_flag == 1)
    {
        bitstream.write8(2, 0);
        bitstream.write64(3, ESCR_base, 30);
        bitstream.write8(1, 1);
        bitstream.write64(15, ESCR_base, 15);
        bitstream.write8(1, 1);
        bitstream.write64(15, ESCR_base);
        bitstream.write8(1, 1);
        bitstream.write16(9, ESCR_extension);
        bitstream.write8(1, 1);
    }
    if (ES_rate_flag == 1)
    {
        bitstream.write8(1, 0);
        bitstream.write32(22, ES_rate);
        bitstream.write8(1, 1);
    }
    if (DSM_trick_mode_flag == 1)
    {
        bitstream.write8(3, trick_mode_control);
        if (trick_mode_control == 0x00)
        {
            bitstream.write8(2, field_id);
            bitstream.write8(1, intra_slice_refresh);
            bitstream.write8(2, frequency_truncation);
        }
        else if (trick_mode_control == 0x01)
        {
            bitstream.write8(5, rep_cntrl);
        }
        else if (trick_mode_control == 0x02)
        {
            bitstream.write8(2, field_id);
            bitstream.write8(3, 0);
        }
        else if (trick_mode_control == 0x03)
        {
            bitstream.write8(2, field_id);
            bitstream.write8(1, intra_slice_refresh);
            bitstream.write8(2, frequency_truncation);
        }
        else if (trick_mode_control == 0x04)
        {
            bitstream.write8(5, rep_cntrl);
        }
        else
        {
            bitstream.write8(5, 0);
        }
    }
    if (additional_copy_info_flag == 1)
    {
        bitstream.write8(1, 1);
        bitstream.write8(7, additional_copy_info);
    }
    if (PES_CRC_flag == 1)
    {
        bitstream.write16(16, previous_PES_packet_CRC);
    }
    if (PES_extension_flag == 1)
    {
        bitstream.write8(1, PES_private_data_flag);
        bitstream.write8(1, pack_header_field_flag);
        bitstream.write8(1, program_packet_sequence_counter_flag);
        bitstream.write8(1, P_STD_buffer_flag);
        bitstream.write8(3, 0);
        bitstream.write8(1, PES_extension_flag_2);
        if (PES_private_data_flag == 1)
        {
            // bitstream.write(8, PES_private_data);
        }
        if (pack_header_field_flag == 1)
        {
            bitstream.write8(8, pack_field_length);
        }
        if (program_packet_sequence_counter_flag == 1)
        {
            bitstream.write8(1, 1);
            bitstream.write8(7, program_packet_sequence_counter);
            bitstream.write8(1, 1);
            bitstream.write8(1, MPEG1_MPEG2_identifier);
            bitstream.write8(6, original_stuff_length);
        }
        if (P_STD_buffer_flag == 1)
        {
            bitstream.write8(2, 0x01);
            bitstream.write8(1, P_STD_buffer_scale);
            bitstream.write16(13, P_STD_buffer_size);
        }
        if (PES_extension_flag_2 == 1)
        {
            bitstream.write8(1, 1);
            bitstream.write8(7, PES_extension_field_length);
            bitstream.write8(1, stream_id_extension_flag);
            if (stream_id_extension_flag == 0)
            {
                bitstream.write8(7, stream_id_extension);
            }
            else
            {
                bitstream.write8(6, 0);
                bitstream.write8(1, tref_extension_flag);
                if (tref_extension_flag == 0)
                {
                    bitstream.write8(4, 0);
                    bitstream.write64(3, TREF, 30);
                    bitstream.write8(1, 1);
                    bitstream.write64(15, TREF, 15);
                    bitstream.write8(1, 1);
                    bitstream.write64(15, TREF);
                    bitstream.write8(1, 1);
                }
            }
            // for (i = 0; i < N3; i++)
            // {
            //     reserved
            // }
        }
    }
    // for (i < 0; i < N1; i++)
    // {
    //     stuffing_byte
    // }
    bitstream.writeBytes(PES_packet_data_byte);
}

const BitStream& PESPacket::getBitStream() const
{
    return bitstream;
}

int32_t PESPacket::inputDataByte(const uint8_t* data, int32_t size)
{
    int32_t ori = PES_packet_data_byte.size();
    int32_t input = ori + size > 65000 ? 65000 - ori : size;

    PES_packet_data_byte.resize(ori + input);
    memcpy(PES_packet_data_byte.data(), data, input);

    return input;
}

int32_t PESPacket::getStreamLength() const
{
    return bitstream.getLength();
}

ProgramStreamMap::ProgramStreamMap()
    : bitstream(32)
{
    packet_start_code_prefix = 0x000001;
    map_stream_id = 0xBC;
}

ProgramStreamMap::~ProgramStreamMap()
{}

void ProgramStreamMap::toBitStream()
{
    toBitStream(bitstream);
}

void ProgramStreamMap::toBitStream(BitStream& bitstream)
{
    bitstream.write32(24, packet_start_code_prefix);
    bitstream.write8(8, map_stream_id);
    bitstream.write16(16, program_stream_map_length);
    bitstream.write8(1, current_next_indicator);
    bitstream.write8(1, single_extension_stream_flag);
    bitstream.write8(1, 0);
    bitstream.write8(5, 1);
    bitstream.write8(7, 0);
    bitstream.write8(1, 1);
    bitstream.write16(16, program_stream_info_length);
    for (auto it : descriptor)
    {
        it->toBitStream(bitstream);
    }
    bitstream.write16(16, elementary_stream_map_length);
    for (auto it : elementary_stream_map)
    {
        bitstream.write8(8, it.stream_type);
        bitstream.write8(8, it.elementary_stream_id);
        bitstream.write16(16, it.elementary_stream_info_length);
        if (it.elementary_stream_id == 0xFD &&
            single_extension_stream_flag == 0)
        {
            bitstream.write8(8, it.pseudo_descriptor_tag);
            bitstream.write8(8, it.pseudo_descriptor_length);
            bitstream.write8(1, 1);
            bitstream.write8(7, it.elementary_stream_id_extension);
            for (size_t i = 3; i < it.descriptor.size(); i++)
            {
                it.descriptor[i]->toBitStream(bitstream);
            }
        }
        else
        {
            for (auto it : it.descriptor)
            {
                it->toBitStream(bitstream);
            }
        }
    }
    bitstream.write32(32, CRC_32);
}

const BitStream& ProgramStreamMap::getBitStream() const
{
    return bitstream;
}

Pack::Pack()
{}

Pack::~Pack()
{}

void Pack::toBitStream()
{
    toBitStream(bitstream);
}

void Pack::toBitStream(BitStream& bitstream)
{
    pack_header.toBitStream(bitstream);

    for (auto it : PES_packet)
    {
        it->toBitStream(bitstream);
    }
}

const BitStream& Pack::getBitStream() const
{
    return bitstream;
}

void Pack::addPESPacket(const std::shared_ptr<PESPacket>& PES_packet)
{
    this->PES_packet.push_back(PES_packet);

    pack_header.updateMuxRate(PES_packet->getStreamLength());
}