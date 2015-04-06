#ifndef __NOPACITY_TIMERS_H
#define __NOPACITY_TIMERS_H

#include <vdr/timers.h>
#include <vdr/plugin.h>

class cGlobalSortedTimers : public cVector<const cTimer *> {
    private:
        bool *localTimer;
    public:
        cGlobalSortedTimers(bool forceRefresh = false);
        virtual ~cGlobalSortedTimers(void);
        bool IsRemoteTimer(int i);
        int NumTimerConfilicts(void);
};

class cRemoteTimerRefresh: public cThread {
    protected:
        virtual void Action(void);
    public:
        cRemoteTimerRefresh(void);
        virtual ~cRemoteTimerRefresh(void);
};
#endif //__NOPACITY_TIMERS_H
