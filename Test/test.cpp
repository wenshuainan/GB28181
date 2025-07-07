#include <stdio.h>
#include <unistd.h>
#include "UA.h"

#include <sys/prctl.h>
#include <pthread.h>
#include "PSMux.h"
#include "Packetizer/PacketizedAVC.h"
#include "Semantics.h"

#include "RTP/RtpParticipant.h"
static RtpParticipant *s_rtp_participant = NULL;

static int s_stream_flag = 0;
static void *stream_proc(void *arg)
{
    FILE *stream = NULL;
    // const char *filename = "./assets/nature.h264";
    // const char *filename = "./assets/240x320.h264";
    // const char *filename = "./assets/240x3202.h264";
    const char *filename = "./assets/128x128.h264";
    // const char *filename = "./assets/1920x1080.h264";
    // const char *filename = "./assets/cuc_ieschool.h264";
    // const char *filename = "./assets/birds.h264";

    prctl(PR_SET_NAME, "simulate_stream");

    // PES *pes = (PacketizedAVC *)arg;
    RtpParticipant *participant = (RtpParticipant *)arg;

    while (s_stream_flag)
    {
        if (stream == NULL)
        {
            stream = fopen(filename, "rb");
            if (stream == NULL)
            {
                printf("fopen %s err %d\n", filename, errno);
                break;
            }
        }

        unsigned char buf[1024] = {0};
        int len = fread(buf, 1, sizeof(buf), stream);
        if (len <= 0)
        {
            printf("simulate stream replay...\n");
            fclose(stream);
            stream = NULL;

            break;
        }

        int wrlen = 0;
        while (wrlen < len && s_stream_flag)
        {
            // int wr = pes->packetized(buf + wrlen, len - wrlen);
            int wr = participant->format(buf + wrlen, len - wrlen);
            printf(">>>>>> %s:%d wr=%d\n", __FILE__, __LINE__, wr);
            if (wr < 0)
            {
                printf("simulate write data failed\n");
                break;
            }
            wrlen += wr;
        }
    }

    if (stream != NULL)
    {
        fclose(stream);
    }

    return NULL;
}

void ps_stream_callback(const uint8_t *data, int32_t size)
{
    // printf("~~~~~~ %p %u\n", data, size);
#if 0
    static FILE *file = fopen("./stream.ps", "wb");
    if (file)
    {
        fwrite(data, 1, size, file);
        fflush(file);
    }
#else
    s_rtp_participant->format(data, size);
#endif
}

int main()
{
#if 0
    UA ua;

    UA::Info info;
    info.sipInfo.server.id = "000";
    info.sipInfo.server.domain = "10.12.13.136";
    info.sipInfo.server.transport = SipUserAgent::TRANSPORT_UDP;
    info.sipInfo.server.port = 5060;
    info.sipInfo.client.id = "102";
    info.sipInfo.client.passwd = "102";
    info.sipInfo.client.expire = 86400;
    info.sipInfo.interval = 60;

    ua.start(info);
#elif 1
    RtpParticipant::Participant participant = {
        .destination = {"10.12.13.136", 5000},
        .netType = RtpNet::Type::UDP,
        .payloadType = RtpPayload::Type::H264,
        .SSRC = 0x99000000
    };
    RtpParticipant rtpParticipant(participant);
    rtpParticipant.start();
    s_rtp_participant = &rtpParticipant;

    PSMux mux;
    mux.setStreamCallback(ps_stream_callback);
    mux.start();

    PacketizedAVC packetizer(&mux);
    pthread_t tid;
    s_stream_flag = 1;
    // pthread_create(&tid, NULL, stream_proc, &packetizer);
    pthread_create(&tid, NULL, stream_proc, &rtpParticipant);
#else
    BitStream bs;
    // bs.write8(1, 0x80, 7);
    // bs.write8(2, 0x40, 6);
    // bs.write8(5, 0x14, 2);
    // bs.write64(64, 0x9900000000000011);
    // bs.write16(8, 0x1200, 8);
    // bs.write16(8, 0x0120, 4);
    // bs.write32(8, 0x00120000, 16);
    // bs.write16(8, 0x0012);
    // bs.write16(4, 0x0100, 8);
    // bs.write32(4, 0x10001200, 28);
    bs.write64(12, 0x12000000000001ba);
    printf("length = %d\n", bs.getLength());
    int i;
    for (i = 0; i < bs.getLength(); i++)
    {
        printf("%02x ", bs.getData()[i]);
    }
    printf("\n");
#endif

    while (1)
    {
        sleep(1);
    }

    return 0;
}