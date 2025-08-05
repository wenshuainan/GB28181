#include <arpa/inet.h>
#include <string.h>
#include <sys/time.h>
#include "Semantics.h"

BitStream::BitStream(int32_t lengthFiledOffset)
{
    offset = 0;
    length = 0;
    this->lengthFieldOffset = lengthFiledOffset;
}

BitStream::~BitStream()
{
    stream.clear();
}

int32_t BitStream::getLength() const
{
    return (offset + 7) / 8; // +7: 超出之后不足8bit也算一个字节
}

const uint8_t* BitStream::getData() const
{
    return stream.data();
}

void BitStream::updateLengthField()
{
    int32_t fieldlen = sizeof(this->length) * 8;

    if (offset < lengthFieldOffset + fieldlen)
    {
        return;
    }
    else
    {
        uint16_t len = ((offset - lengthFieldOffset - fieldlen) + 7) / 8; // (+7): 超出之后不足8bit也算一个字节
        if (len == length)
        {
            return;
        }
        else
        {
            length = len;
        }
    }

    uint16_t bigendian = htons(length); //大端序
    int32_t index = lengthFieldOffset / 8; //长度字段在字节流中的偏移

    stream[index] = *((uint8_t *)(&bigendian));
    stream[index + 1] = *((uint8_t *)(&bigendian) + 1);
}

void BitStream::write16be(int32_t nbits, uint16_t value, int32_t startbit)
{
    if (nbits <= 0 || startbit < 0 || startbit + nbits > 16)
    {
        return;
    }

    /* 低地址起第一个字节 */
    int32_t b1nbits = startbit > 8 ? nbits : nbits + startbit - 8;
    int32_t b1start = startbit > 8 ? startbit - 8 : 0;

    /* 低地址起第二个字节 */
    int32_t b2nbits = nbits - (b1nbits > 0 ? b1nbits : 0);
    int32_t b2start = startbit;

    write8(b1nbits, *((uint8_t *)(&value)), b1start);
    write8(b2nbits, *((uint8_t *)(&value) + 1), b2start);
}

void BitStream::write32be(int32_t nbits, uint32_t value, int32_t startbit)
{
    if (nbits <= 0 || startbit < 0 || startbit + nbits > 32)
    {
        return;
    }

    /* 低地址起第一个字 */
    int32_t w1nbits = startbit > 16 ? nbits : nbits + startbit - 16;
    int32_t w1start = startbit > 16 ? startbit - 16 : 0;

    /* 低地址起第二个字 */
    int32_t w2nbits = nbits - (w1nbits > 0 ? w1nbits : 0);
    int32_t w2start = startbit;

    write16be(w1nbits, *((uint16_t *)(&value)), w1start);
    write16be(w2nbits, *((uint16_t *)(&value) + 1), w2start);
}

void BitStream::clear()
{
    offset = 0;
    length = 0;
    stream.clear();
}

void BitStream::write8(int32_t nbits, uint8_t value, int32_t startbit)
{
    /* 参数非法 */
    if (nbits <= 0 || startbit < 0 || startbit + nbits > 8)
    {
        return;
    }

    int32_t index = offset / 8; //当前字节，从字节流开头的偏移
    int32_t bitoffset = offset % 8; //从当前字节最高位的偏移

    /* 扩充字节流空间 */
    if (stream.size() <= (uint32_t)index)
    {
        stream.resize(index + 128); // 多申请一些，防止每次都调用
    }

    uint8_t v1 = value & (0xFF << startbit); //去掉低位偏移
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
    if (lengthFieldOffset >= 0)
    {
        updateLengthField();
    }
}

void BitStream::write16(int32_t nbits, uint16_t value, int32_t startbit)
{
    if (nbits <= 0 || startbit < 0 || startbit + nbits > 16)
    {
        return;
    }

    uint16_t bigendian = htons(value); //大端序

    /* 低地址起第一个字节 */
    int32_t b1nbits = startbit > 8 ? nbits : nbits + startbit - 8;
    int32_t b1start = startbit > 8 ? startbit - 8 : 0;

    /* 低地址起第二个字节 */
    int32_t b2nbits = nbits - (b1nbits > 0 ? b1nbits : 0);
    int32_t b2start = startbit;

    write8(b1nbits, *((uint8_t *)(&bigendian)), b1start);
    write8(b2nbits, *(((uint8_t *)(&bigendian)) + 1), b2start);
}

void BitStream::write32(int32_t nbits, uint32_t value, int32_t startbit)
{
    if (nbits <= 0 || startbit < 0 || startbit + nbits > 32)
    {
        return;
    }

    uint32_t bigendian = htonl(value); //大端序

    /* 低地址起第一个字 */
    int32_t w1nbits = startbit > 16 ? nbits : nbits + startbit - 16;
    int32_t w1start = startbit > 16 ? startbit - 16 : 0;

    /* 低地址起第二个字 */
    int32_t w2nbits = nbits - (w1nbits > 0 ? w1nbits : 0);
    int32_t w2start = startbit;

    write16be(w1nbits, *((uint16_t *)(&bigendian)), w1start);
    write16be(w2nbits, *((uint16_t *)(&bigendian) + 1), w2start);
}

void BitStream::write64(int32_t nbits, uint64_t value, int32_t startbit)
{
    if (nbits <= 0 || startbit < 0 || startbit + nbits > 64)
    {
        return;
    }

    uint32_t dw1 = value >> 32;
    uint32_t dw2 = value;

    dw1 = htonl(dw1);
    dw2 = htonl(dw2);

    /* 低地址起第一个双字 */
    int32_t dw1nbits = startbit > 32 ? nbits : nbits + startbit - 32;
    int32_t dw1start = startbit > 32 ? startbit - 32 : 0;

    /* 低地址起第二个双字 */
    int32_t dw2nbits = nbits - (dw1nbits > 0 ? dw1nbits : 0);
    int32_t dw2start = startbit;

    write32be(dw1nbits, dw1, dw1start);
    write32be(dw2nbits, dw2, dw2start);
}

void BitStream::writeBytes(const std::vector<uint8_t>& data)
{
    int32_t index = offset / 8; //当前字节，从字节流开头的偏移
    // int32_t bitoffset = offset % 8; //从当前字节最高位的偏移
    int32_t datalen = data.size();

    /* 参数非法 */
    if (datalen <= 0)
    {
        return;
    }

    /* 扩充字节流空间 */
    if ((uint32_t)index + datalen > stream.size())
    {
        stream.resize(index + datalen);
    }

    /* 将数据拷贝到字节流 */
    memcpy(stream.data() + index, data.data(), datalen);
    offset += (datalen * 8);

    /* 更新长度字段 */
    if (lengthFieldOffset >= 0)
    {
        updateLengthField();
    }
}

void BitStream::writeBitStream(const BitStream& bitstream)
{
    int32_t index = offset / 8; //当前字节，从字节流开头的偏移
    // int32_t bitoffset = offset % 8; //从当前字节最高位的偏移
    int32_t writelen = bitstream.getLength();

    if (writelen <= 0)
    {
        return;
    }

    /* 扩充字节流空间 */
    if ((uint32_t)index + writelen > stream.size())
    {
        stream.resize(index + writelen);
    }

    /* 将数据拷贝到字节流 */
    memcpy(stream.data() + index, bitstream.stream.data(), writelen);
    offset += (writelen * 8);

    /* 更新长度字段 */
    if (lengthFieldOffset >= 0)
    {
        updateLengthField();
    }
}

SystemHeader::SystemHeader()
    : m_bitstream(32)
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
    m_bitstream.clear();
    toBitStream(m_bitstream);
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
    for (auto it : streamTypes)
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
    return m_bitstream;
}

int32_t SystemHeader::getBitStreamLength() const
{
    return m_bitstream.getLength();
}

void SystemHeader::addVideoStreamType(uint8_t stream_id)
{
    for (auto it : streamTypes)
    {
        if (it.stream_id == stream_id)
        {
            return;
        }
    }
    
    StreamType streamType;
    streamType.stream_id = stream_id;
    streamType.P_STD_buffer_bound_scale = 0;
    streamType.P_STD_buffer_size_bound = 100;

    streamTypes.push_back(streamType);
}

void SystemHeader::addAudioStreamType(uint8_t stream_id)
{
    for (auto it : streamTypes)
    {
        if (it.stream_id == stream_id)
        {
            return;
        }
    }

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

    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t tms = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    system_clock_reference_base = ((27000 * tms) / 300) % (1ULL << 33);
    system_clock_reference_extension = (27000 * tms) % 300;

    program_mux_rate = 0;

    pack_stuffing_length = 0;
}

PackHeader::~PackHeader()
{}

void PackHeader::toBitStream()
{
    m_bitstream.clear();
    toBitStream(m_bitstream);
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
    bitstream.write32(22, (program_mux_rate + 49) / 50);
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
        system_header->toBitStream();
        updateMuxRate(system_header->getBitStreamLength());
        bitstream.writeBitStream(system_header->getBitStream());
    }
}

const BitStream& PackHeader::getBitStream() const
{
    return m_bitstream;
}

void PackHeader::setSystemHeader(const std::shared_ptr<SystemHeader>& systemheader)
{
    system_header = systemheader;
}

void PackHeader::updateMuxRate(int32_t addedLength)
{
    program_mux_rate += addedLength;
}

PESPacket::PESPacket(uint8_t streamid)
    : m_bitstream(32)
{
    packet_start_code_prefix = 0x000001;
    stream_id = streamid;
    PES_packet_length = 0;
    PES_scrambling_control = 0;
    PES_priority = 1;
    data_alignment_indicator = 1;
    copyright = 0;
    PTS_DTS_flags = 0x02;
    ESCR_flag = 0;
    ES_rate_flag = 0;
    DSM_trick_mode_flag = 0;
    additional_copy_info_flag = 0;
    PES_CRC_flag = 0;
    PES_extension_flag = 0;
    PES_header_data_length = 0; // PES header任选字段和填充字段所占的字节数，根据前面各个flag选择
    if (PTS_DTS_flags == 0x02)
    {
        PES_header_data_length += 5;
    }
    else if (PTS_DTS_flags == 0x03)
    {
        PES_header_data_length += 10;
    }
    if (ESCR_flag == 1)
    {
        PES_header_data_length += 6;
    }
    if (ES_rate_flag == 1)
    {
        PES_header_data_length += 3;
    }
    if (DSM_trick_mode_flag == 1)
    {
        PES_header_data_length += 1;
    }
    if (additional_copy_info_flag == 1)
    {
        PES_header_data_length += 1;
    }
    if (PES_CRC_flag == 1)
    {
        PES_header_data_length += 2;
    }
    if (PES_extension_flag == 1)
    {
        PES_header_data_length += 1;
        // ...
    }

    //PTS_DTS_flags = 0x02;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t tms = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    PTS = ((27000 * tms) / 300) % (1ULL << 33);

    //PTS_DTS_flags = 0x03;
    //DTS if have video B frame ...

    /* if PES_extension_flag == 0 other extension flag no need set */
    // PES_private_data_flag = 0;
    // pack_header_field_flag = 0;
    // program_packet_sequence_counter_flag = 0;
}

PESPacket::~PESPacket()
{}

void PESPacket::toBitStream()
{
    m_bitstream.clear();
    toBitStream(m_bitstream);
}

void PESPacket::toBitStream(BitStream& bitstream)
{
    bitstream.write32(24, packet_start_code_prefix);
    bitstream.write8(8, stream_id);
    bitstream.write16(16, PES_packet_length);
    bitstream.write8(2, 0x02);
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
    return m_bitstream;
}

int32_t PESPacket::getBitStreamLength() const
{
    return m_bitstream.getLength();
}

int32_t PESPacket::writeDataByte(const uint8_t* data, int32_t size)
{
    int32_t ori = PES_packet_data_byte.size();
    int32_t input = ori + size > 65000 ? 65000 - ori : size;

    PES_packet_data_byte.resize(ori + input);
    memcpy(PES_packet_data_byte.data() + ori, data, input);

    return input;
}

ProgramStreamMap::ProgramStreamMap()
    : PESPacket(0), m_bitstream(32)
{
    packet_start_code_prefix = 0x000001;
    map_stream_id = 0xBC;
    program_stream_map_length = 0;
    current_next_indicator = 1;
    single_extension_stream_flag = 0;
    program_stream_map_version = 1;
    program_stream_info_length = 0;
    elementary_stream_map_length = 0;
    CRC_32 = 0;
}

ProgramStreamMap::~ProgramStreamMap()
{}

void ProgramStreamMap::toBitStream()
{
    m_bitstream.clear();
    toBitStream(m_bitstream);
}

void ProgramStreamMap::toBitStream(BitStream& bitstream)
{
    bitstream.write32(24, packet_start_code_prefix);
    bitstream.write8(8, map_stream_id);
    bitstream.write16(16, program_stream_map_length);
    bitstream.write8(1, current_next_indicator);
    bitstream.write8(1, single_extension_stream_flag);
    bitstream.write8(1, 0);
    bitstream.write8(5, program_stream_map_version);
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
            for (auto itdesc : it.descriptor)
            {
                itdesc->toBitStream(bitstream);
            }
        }
    }
    bitstream.write32(32, CRC_32);
}

const BitStream& ProgramStreamMap::getBitStream() const
{
    return m_bitstream;
}

void ProgramStreamMap::addElementaryStream(uint8_t stream_type, uint8_t elementary_stream_id)
{
    for (auto it : elementary_stream_map)
    {
        if (it.stream_type == stream_type)
        {
            return;
        }
    }
    
    ElementaryStream elementary_stream;
    elementary_stream.stream_type = stream_type;
    elementary_stream.elementary_stream_id = elementary_stream_id;
    elementary_stream.elementary_stream_info_length = 0;

    elementary_stream_map.push_back(elementary_stream);
    elementary_stream_map_length += sizeof(elementary_stream.stream_type);
    elementary_stream_map_length += sizeof(elementary_stream.elementary_stream_id);
    elementary_stream_map_length += sizeof(elementary_stream.elementary_stream_info_length);
}

Pack::Pack()
{}

Pack::~Pack()
{}

void Pack::toBitStream()
{
    m_bitstream.clear();
    toBitStream(m_bitstream);
}

void Pack::toBitStream(BitStream& bitstream)
{
    for (auto it : PES_packet)
    {
        it->toBitStream();
        pack_header.updateMuxRate(it->getBitStreamLength());
    }

    pack_header.toBitStream(bitstream);
}

const BitStream& Pack::getBitStream() const
{
    return m_bitstream;
}

void Pack::addSystemHeader(const std::shared_ptr<SystemHeader>& system_header)
{
    pack_header.setSystemHeader(system_header);
}

void Pack::addPESPacket(const std::shared_ptr<PESPacket>& packet)
{
    PES_packet.push_back(packet);
}

const std::vector<std::shared_ptr<PESPacket>>& Pack::getPESPacket() const
{
    return PES_packet;
}