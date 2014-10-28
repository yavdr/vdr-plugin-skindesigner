#ifndef __FEMONRECEIVER_H
#define __FEMONRECEIVER_H

#include <vdr/thread.h>
#include <vdr/receiver.h>

class cFemonReceiver : public cReceiver, public cThread {
    private:
        cMutex            m_Mutex;
        cCondWait         m_Sleep;
        bool              m_Active;

        cRingBufferLinear m_VideoBuffer;
        cTsToPes          m_VideoAssembler;
        int               m_VideoType;
        int               m_VideoPid;
        int               m_VideoPacketCount;
        double            m_VideoBitrate;

        cRingBufferLinear m_AudioBuffer;
        cTsToPes          m_AudioAssembler;
        int               m_AudioPid;
        int               m_AudioPacketCount;
        double            m_AudioBitrate;
        bool              m_AudioValid;

        cRingBufferLinear m_AC3Buffer;
        cTsToPes          m_AC3Assembler;
        int               m_AC3Pid;
        int               m_AC3PacketCount;
        double            m_AC3Bitrate;
        bool              m_AC3Valid;

    protected:
        virtual void Activate(bool On);
        virtual void Receive(uchar *Data, int Length);
        virtual void Action(void);

    public:
        cFemonReceiver(const cChannel* Channel, int ATrack, int DTrack);
        virtual ~cFemonReceiver();
        void Deactivate(void);

        double VideoBitrate(void)         { cMutexLock MutexLock(&m_Mutex);
            return m_VideoBitrate; };                // bit/s
        double AudioBitrate(void)         { cMutexLock MutexLock(&m_Mutex);
            return m_AudioBitrate; };                // bit/s
        double AC3Bitrate(void)           { cMutexLock MutexLock(&m_Mutex);
            return m_AC3Bitrate; };                  // bit/s
};

#endif //__FEMONRECEIVER_H

