#include "displaychannel.h"
#include "libcore/timers.h"

cSDDisplayChannel::cSDDisplayChannel(cTemplate *channelTemplate, bool WithInfo) {
    channelView = NULL;
    if (firstDisplay) {
        firstDisplay = false;
        doOutput = false;
        return;
    } else if (!channelTemplate) {
        esyslog("skindesigner: displayChannel no valid template - aborting");
        doOutput = false;
        return;
    } else {
        doOutput = true;
    }
    groupSep = false;
    present = NULL;
    currentLast = 0;
    channelChange = false;
    initial = true;
    devicesLast = cTimeMs::Now();

    channelView = new cDisplayChannelView(channelTemplate->GetRootView());
    if (!channelView->createOsd()) {
        doOutput = false;
        return;
    }
    channelView->DrawBackground();
    channelView->DrawSignalBackground();
}

cSDDisplayChannel::~cSDDisplayChannel() {
    if (channelView)
        delete channelView;
}

void cSDDisplayChannel::SetChannel(const cChannel *Channel, int Number) {
    if (!doOutput)
        return;

    channelChange = true;
    groupSep = false;

    cString ChannelNumber("");
    cString ChannelName("");
    cString ChannelID("");

    if (Channel) {
        ChannelName = Channel->Name() ? Channel->Name() : "";
        ChannelID = Channel->GetChannelID().ToString();
        if (!Channel->GroupSep()) {
            ChannelNumber = cString::sprintf("%d%s", Channel->Number(), Number ? "-" : "");
        } else {
            groupSep = true;
        }
    } else if (Number) {
        ChannelNumber = cString::sprintf("%d-", Number);
    } else {
        ChannelName = ChannelString(NULL, 0);
    }
    channelView->ClearChannel();
    channelView->ClearEPGInfo();
    channelView->ClearStatusIcons();
    channelView->ClearChannelGroups();
    channelView->ClearScraperContent();
    channelView->ClearAudioInfo();
    if (!groupSep) {
        channelView->DrawChannel(ChannelNumber, ChannelName, ChannelID, (Number > 0)?true:false);
        channelView->DrawProgressBarBack();
        channelView->DrawSignalBackground();
        if (Channel) {
            channelView->DrawStatusIcons(Channel);
            channelView->DrawAudioInfo();
        }
    } else {
        channelView->ClearSignal();
        channelView->ClearSignalBackground();
        channelView->ClearProgressBar();
        channelView->ClearProgressBarBack();
        if (Channel)
            channelView->DrawChannelGroups(Channel, ChannelName);
    }

}

void cSDDisplayChannel::SetEvents(const cEvent *Present, const cEvent *Following) {
    if (!doOutput)
        return;

    present = Present;
    channelView->ClearProgressBar();
    if (!groupSep) {
        channelView->ClearEPGInfo();
    }

    cGlobalSortedTimers SortedTimers;// local and remote timers

    bool recPresent = false;
    if (Present) {
        if (!groupSep) {
            SetProgressBar(Present);
        }
        eTimerMatch TimerMatch = tmNone;
        const cTimer *Timer = Timers.GetMatch(Present, &TimerMatch);
        if (Timer && Timer->Recording()) {
            recPresent = true;
        }
        for (int i = 0; i < SortedTimers.Size() && !recPresent; i++) 
            if (const cTimer *Timer = SortedTimers[i]) 
                if (Timer->Channel()->GetChannelID() == Present->ChannelID()) 
                    if (const cEvent *timerEvent = Timer->Event())
                        if (Present->EventID() == timerEvent->EventID())
                            recPresent = Timer->Recording();
        
    }
    bool recFollowing = false;
    if (Following) {
        recFollowing = Following->HasTimer();
        for (int i = 0; i < SortedTimers.Size() && !recFollowing; i++) 
            if (const cTimer *Timer = SortedTimers[i]) 
                if (Timer->Channel()->GetChannelID() == Following->ChannelID()) 
                    if (const cEvent *timerEvent = Timer->Event())
                        if (Following->EventID() == timerEvent->EventID())
                            recFollowing = true;
    }

    if (Present || Following) {
        channelView->DrawEPGInfo(Present, Following, recPresent, recFollowing);
        channelView->DrawScraperContent(Present);
    }
}

void cSDDisplayChannel::SetProgressBar(const cEvent *present) {
    int Current = 0;
    int Total = 0;
    time_t t = time(NULL);
    if (t > present->StartTime())
        Current = t - present->StartTime();
    Total = present->Duration();
    if ((Current > currentLast + 3) || initial || channelChange) {
        currentLast = Current;
        cString start = present->GetTimeString();
        cString stop = present->GetEndTimeString();
        channelView->DrawProgressBar(start, stop, Current, Total);
    }
}


void cSDDisplayChannel::SetMessage(eMessageType Type, const char *Text) {
    if (!doOutput)
        return;
    channelView->ClearChannel();
    channelView->ClearEPGInfo();
    channelView->ClearStatusIcons();
    channelView->ClearScreenResolution();
    channelView->ClearProgressBar();
    channelView->ClearProgressBarBack();
    channelView->ClearSignal();
    channelView->ClearSignalBackground();
    channelView->ClearScraperContent();
    channelView->ClearAudioInfo();
    channelView->DisplayMessage(Type, Text);
    groupSep = true;
}

void cSDDisplayChannel::Flush(void) {
    if (!doOutput)
        return;

    if (initial) {
        channelView->DrawCurrentWeather();
        channelView->DrawCustomTokens();
    }

    if (initial || channelChange) {
        channelView->DrawDate();
    }
    channelView->DrawTime();

    if (present) {
        SetProgressBar(present);
    } else {
        channelView->ClearProgressBar();
    }

    if (!groupSep) {
        channelView->DrawScreenResolution();
        channelView->DrawSignal();
        channelView->DrawAudioInfo();
        if (initial || cTimeMs::Now() - devicesLast  > 500) {
            channelView->DrawDevices(initial);
            devicesLast = cTimeMs::Now();
        }
    } else {
        channelView->ClearStatusIcons();
        channelView->ClearScreenResolution();
        channelView->ClearSignal();
        channelView->ClearSignalBackground();
        channelView->ClearDevices();
    }

    if (initial) {
        channelView->DoStart();
    }

    initial = false;
    channelChange = false;
    channelView->Flush();
}