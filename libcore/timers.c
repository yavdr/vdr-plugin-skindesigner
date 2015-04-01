#include "timers.h"
#include "../services/epgsearch.h"
#include "../services/remotetimers.h"

static int CompareTimers(const void *a, const void *b) {
    return (*(const cTimer **)a)->Compare(**(const cTimer **)b);
}

cGlobalSortedTimers::cGlobalSortedTimers(bool forceRefresh) : cVector<const cTimer *>(Timers.Count()) {
    static bool initial = true;
    static cRemoteTimerRefresh *remoteTimerRefresh = NULL;
    localTimer = NULL;

    if (forceRefresh)
        initial = true;
    //check if remotetimers plugin is available
    static cPlugin* pRemoteTimers = cPluginManager::GetPlugin("remotetimers");

    cSchedulesLock SchedulesLock;
    const cSchedules *Schedules = cSchedules::Schedules(SchedulesLock);
    
    if (pRemoteTimers && initial) {
        cString errorMsg;
        pRemoteTimers->Service("RemoteTimers::RefreshTimers-v1.0", &errorMsg);
        initial = false;
    }

    for (cTimer *Timer = Timers.First(); Timer; Timer = Timers.Next(Timer)) {
        if (Timer->HasFlags(tfActive))
            Append(Timer);
    }

    //if remotetimers plugin is available, take timers also from him
    if (pRemoteTimers) {
        cTimer* remoteTimer = NULL;
        while (pRemoteTimers->Service("RemoteTimers::ForEach-v1.0", &remoteTimer) && remoteTimer != NULL) {
            remoteTimer->SetEventFromSchedule(Schedules); // make sure the event is current
            if (remoteTimer->HasFlags(tfActive))
                Append(remoteTimer);
        }
    }
  
    Sort(CompareTimers);
    
    int numTimers = Size();
    if (numTimers > 0) {
        localTimer = new bool[numTimers];
        for (int i=0; i < numTimers; i++) {
            if (!pRemoteTimers) {
                localTimer[i] = true;
            } else {
                localTimer[i] = false;
                for (cTimer *Timer = Timers.First(); Timer; Timer = Timers.Next(Timer)) {
                    if (Timer == At(i)) {
                        localTimer[i] = true;
                        break;
                    }
                }
            }
        }
    }

    if (pRemoteTimers && (remoteTimerRefresh == NULL))
        remoteTimerRefresh = new cRemoteTimerRefresh();
}

cGlobalSortedTimers::~cGlobalSortedTimers(void) {
    if (localTimer) {
        delete[] localTimer;
    }
}

bool cGlobalSortedTimers::IsRemoteTimer(int i) {
    if (!localTimer)
        return true;
    if (i >= Size())
        return true;
    return !(localTimer[i]);
}


int cGlobalSortedTimers::NumTimerConfilicts(void) {
    int numConflicts = 0;
    cPlugin *p = cPluginManager::GetPlugin("epgsearch");
    if (p) {
        Epgsearch_lastconflictinfo_v1_0 *serviceData = new Epgsearch_lastconflictinfo_v1_0;
        if (serviceData) {
            serviceData->nextConflict = 0;
            serviceData->relevantConflicts = 0;
            serviceData->totalConflicts = 0;
            p->Service("Epgsearch-lastconflictinfo-v1.0", serviceData);
            if (serviceData->relevantConflicts > 0) {
                numConflicts = serviceData->relevantConflicts;
            }
            delete serviceData;
        }
    }
    return numConflicts;
}

cRemoteTimerRefresh::cRemoteTimerRefresh(): cThread("skindesigner: RemoteTimers::RefreshTimers") {
    Start();
}

cRemoteTimerRefresh::~cRemoteTimerRefresh(void) {
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
}

void cRemoteTimerRefresh::Action(void) {    
    #define REFESH_INTERVALL_MS 30000
    while (Running()) {
        cCondWait::SleepMs(REFESH_INTERVALL_MS);      
        if (!cOsd::IsOpen()) {//make sure that no timer is currently being edited
            cGlobalSortedTimers(true);
            Timers.SetModified();
        }
    }
}
      