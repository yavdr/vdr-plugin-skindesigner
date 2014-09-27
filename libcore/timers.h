#ifndef __NOPACITY_TIMERS_H
#define __NOPACITY_TIMERS_H

#include <vdr/timers.h>
#include <vdr/plugin.h>

class cGlobalSortedTimers : public cVector<const cTimer *> {
    public:
        cGlobalSortedTimers(bool forceRefresh = false);
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
