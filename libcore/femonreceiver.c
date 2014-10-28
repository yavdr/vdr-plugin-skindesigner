#include <unistd.h>
#include "../config.h"
#include "femonreceiver.h"

cFemonReceiver::cFemonReceiver(const cChannel *Channel, int ATrack, int DTrack)
    : cReceiver(Channel),
    cThread("femon receiver"),
    m_Mutex(),
    m_Sleep(),
    m_Active(false),
    m_VideoBuffer(KILOBYTE(512), TS_SIZE, false, "Femon video"),
    m_VideoType(Channel ? Channel->Vtype(): 0),
    m_VideoPid(Channel ? Channel->Vpid() : 0),
    m_VideoPacketCount(0),
    m_AudioBuffer(KILOBYTE(256), TS_SIZE, false, "Femon audio"),
    m_AudioPid(Channel ? Channel->Apid(ATrack) : 0),
    m_AudioPacketCount(0),
    m_AC3Buffer(KILOBYTE(256), TS_SIZE, false, "Femon AC3"),
    m_AC3Pid(Channel ? Channel->Dpid(DTrack) : 0),
    m_AC3PacketCount(0)
{
    SetPids(NULL);
    AddPid(m_VideoPid);
    AddPid(m_AudioPid);
    AddPid(m_AC3Pid);

    m_VideoBuffer.SetTimeouts(0, 100);
    m_AudioBuffer.SetTimeouts(0, 100);
    m_AC3Buffer.SetTimeouts(0, 100);

    m_VideoBitrate = 0.0;
    m_AudioBitrate = 0.0;
    m_AC3Bitrate = 0.0;
}

cFemonReceiver::~cFemonReceiver(void)
{
    Deactivate();
}

void cFemonReceiver::Deactivate(void)
{
    Detach();
    if (m_Active) {
        m_Active = false;
        m_Sleep.Signal();
        if (Running())
            Cancel(3);
    }
}

void cFemonReceiver::Activate(bool On)
{
    if (On)
        Start();
    else
        Deactivate();
}

void cFemonReceiver::Receive(uchar *Data, int Length)
{
    // TS packet length: TS_SIZE
    if (Running() && (*Data == TS_SYNC_BYTE) && (Length == TS_SIZE)) {
        int len, pid = TsPid(Data);
        if (pid == m_VideoPid) {
            ++m_VideoPacketCount;
            len = m_VideoBuffer.Put(Data, Length);
            if (len != Length) {
                m_VideoBuffer.ReportOverflow(Length - len);
                m_VideoBuffer.Clear();
            }
        }
        else if (pid == m_AudioPid) {
            ++m_AudioPacketCount;
            len = m_AudioBuffer.Put(Data, Length);
            if (len != Length) {
                m_AudioBuffer.ReportOverflow(Length - len);
                m_AudioBuffer.Clear();
            }
        }
        else if (pid == m_AC3Pid) {
            ++m_AC3PacketCount;
            len = m_AC3Buffer.Put(Data, Length);
            if (len != Length) {
                m_AC3Buffer.ReportOverflow(Length - len);
                m_AC3Buffer.Clear();
            }
        }
    }
}

void cFemonReceiver::Action(void)
{
    cTimeMs calcPeriod(0);
    m_Active = true;
    bool init = true;
    while (Running() && m_Active) {
        uint8_t *Data;
        double timeout;
        int Length;
        bool processed = false;

        // process available video data
        while ((Data = m_VideoBuffer.Get(Length))) {
            if (!m_Active || (Length < TS_SIZE))
                break;
            Length = TS_SIZE;
            if (*Data != TS_SYNC_BYTE) {
                for (int i = 1; i < Length; ++i) {
                    if (Data[i] == TS_SYNC_BYTE) {
                        Length = i;
                        break;
                    }
                }
                m_VideoBuffer.Del(Length);
                continue;
            }
            processed = true;
            if (TsPayloadStart(Data)) {
                m_VideoAssembler.Reset();
            }
            m_VideoAssembler.PutTs(Data, Length);
            m_VideoBuffer.Del(Length);
        }

        // process available audio data
        while ((Data = m_AudioBuffer.Get(Length))) {
            if (!m_Active || (Length < TS_SIZE))
                break;
            Length = TS_SIZE;
            if (*Data != TS_SYNC_BYTE) {
                for (int i = 1; i < Length; ++i) {
                    if (Data[i] == TS_SYNC_BYTE) {
                        Length = i;
                        break;
                    }
                }
                m_AudioBuffer.Del(Length);
                continue;
            }
            processed = true;
            m_AudioAssembler.PutTs(Data, Length);
            m_AudioBuffer.Del(Length);
        }

        // process available dolby data
        while ((Data = m_AC3Buffer.Get(Length))) {
            if (!m_Active || (Length < TS_SIZE))
                break;
            Length = TS_SIZE;
            if (*Data != TS_SYNC_BYTE) {
                for (int i = 1; i < Length; ++i) {
                    if (Data[i] == TS_SYNC_BYTE) {
                        Length = i;
                        break;
                    }
                }
                m_AC3Buffer.Del(Length);
                continue;
            }
            processed = true;
            m_AC3Assembler.PutTs(Data, Length);
            m_AC3Buffer.Del(Length);
        }

        // calculate bitrates
        timeout = double(calcPeriod.Elapsed());
        if (m_Active && (init || (timeout >= (100.0 * config.bitrateCalcInterval )))) {
            // TS packet 188 bytes - 4 byte header; MPEG standard defines 1Mbit = 1000000bit
            // PES headers should be compensated!
            m_VideoBitrate     = (1000.0 * 8.0 * 184.0 * m_VideoPacketCount) / timeout;
            m_VideoPacketCount = 0;
            m_AudioBitrate     = (1000.0 * 8.0 * 184.0 * m_AudioPacketCount) / timeout;
            m_AudioPacketCount = 0;
            m_AC3Bitrate       = (1000.0 * 8.0 * 184.0 * m_AC3PacketCount)   / timeout;
            m_AC3PacketCount   = 0;
            calcPeriod.Set(0);
            init = false;
        }

        if (!processed)
            m_Sleep.Wait(10); // to avoid busy loop and reduce cpu load
    }
}
