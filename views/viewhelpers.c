#include <fstream>
#include <iostream>
#include <list>
#include <vdr/menu.h>
#include <vdr/videodir.h>
#include <sys/sysinfo.h>
#include "../services/scraper2vdr.h"
#include "../services/weatherforecast.h"
#include "../config.h"
#include "../libcore/timers.h"
#include "../libcore/helpers.h"
#include "viewhelpers.h"

cViewHelpers::cViewHelpers(void) {
    numDevices = 0;
    devicesInit = false;
    lSignalDisplay = 0;
    lSignalStrength = 0;
    lSignalQuality = 0;
    lastSecond = -1;
    lastMinute = -1;
    lastSystemLoad = 0.0;
    lastMemUsage = -1;
    lastEcmInfo.hops = -1;
    lastEcmInfo.ecmtime = -1;
    lastEcmInfo.caid = -1;
    lastEcmInfo.pid = -1;
    lastEcmInfo.prid = -1;
}

cViewHelpers::~cViewHelpers() {
    if (devicesInit) {
        delete[] lastSignalStrength;
        delete[] lastSignalQuality;
        delete[] recDevices;
    }
}

/******************************************************************
* Public Functions
******************************************************************/
bool cViewHelpers::SetDate(bool forced, stringmap &stringTokens, intmap &intTokens) {
    time_t t = time(0);   // get time now
    struct tm * now = localtime(&t);
    int min = now->tm_min;
    if (!forced && min == lastMinute) {
        return false;
    }
    lastMinute = min;

    intTokens.insert(pair<string, int>("year", now->tm_year + 1900));
    intTokens.insert(pair<string, int>("day", now->tm_mday));

    char monthname[20];
    char monthshort[10];
    strftime(monthshort, sizeof(monthshort), "%b", now);
    strftime(monthname, sizeof(monthname), "%B", now);

    stringTokens.insert(pair<string,string>("monthname", monthname));
    stringTokens.insert(pair<string,string>("monthnameshort", monthshort));
    stringTokens.insert(pair<string,string>("month", *cString::sprintf("%02d", now->tm_mon + 1)));
    stringTokens.insert(pair<string,string>("dayleadingzero", *cString::sprintf("%02d", now->tm_mday)));
    stringTokens.insert(pair<string,string>("dayname", *WeekDayNameFull(now->tm_wday)));
    stringTokens.insert(pair<string,string>("daynameshort", *WeekDayName(now->tm_wday)));
    stringTokens.insert(pair<string,string>("time", *TimeString(t)));

    return true;
}

bool cViewHelpers::SetTime(bool forced, stringmap &stringTokens, intmap &intTokens) {
    time_t t = time(0);   // get time now
    struct tm * now = localtime(&t);
    int sec = now->tm_sec;
    if (!forced && sec == lastSecond) {
        return false;
    }
    int min = now->tm_min;
    int hour = now->tm_hour;
    int hourMinutes = hour%12 * 5 + min / 12;

    intTokens.insert(pair<string, int>("sec", sec));
    intTokens.insert(pair<string, int>("min", min));
    intTokens.insert(pair<string, int>("hour", hour));
    intTokens.insert(pair<string, int>("hmins", hourMinutes));
    
    lastSecond = sec;
    return true;
}

bool cViewHelpers::SetSignal(bool forced, stringmap &stringTokens, intmap &intTokens) {
    bool done = false;
    time_t Now = time(NULL);
    if (Now != lSignalDisplay) {
        int SignalStrength = cDevice::ActualDevice()->SignalStrength();
        int SignalQuality = cDevice::ActualDevice()->SignalQuality();
        if (SignalStrength < 0) SignalStrength = 0;
        if (SignalQuality < 0) SignalQuality = 0;
        if ((SignalStrength == 0)&&(SignalQuality==0))
            return done;
        if ((lSignalStrength != SignalStrength) || (lSignalQuality != SignalQuality)) {
            intTokens.insert(pair<string,int>("signalstrength", SignalStrength));
            intTokens.insert(pair<string,int>("signalquality", SignalQuality));
            lSignalStrength = SignalStrength;
            lSignalQuality = SignalQuality;
            done = true;
        }
        lSignalDisplay = Now;
    }
    return done;
}

bool cViewHelpers::SetCurrentWeatherTokens(bool forced, stringmap &stringTokens, intmap &intTokens) {
    static cPlugin *pWeatherForecast = cPluginManager::GetPlugin("weatherforecast");
    if (!pWeatherForecast)
        return false;
    cServiceCurrentWeather currentWeather;
    if (!pWeatherForecast->Service("GetCurrentWeather", &currentWeather)) {
        return false;
    }
    stringTokens.insert(pair<string,string>("timestamp", currentWeather.timeStamp));
    stringTokens.insert(pair<string,string>("temperature", currentWeather.temperature));
    stringTokens.insert(pair<string,string>("apparenttemperature", currentWeather.apparentTemperature));
    stringTokens.insert(pair<string,string>("mintemperature", currentWeather.minTemperature));
    stringTokens.insert(pair<string,string>("maxtemperature", currentWeather.maxTemperature));
    stringTokens.insert(pair<string,string>("summary", currentWeather.summary));
    stringTokens.insert(pair<string,string>("icon", currentWeather.icon));
    stringTokens.insert(pair<string,string>("precipitationintensity", currentWeather.precipitationIntensity));
    intTokens.insert(pair<string,int>("precipitationprobability", currentWeather.precipitationProbability));
    stringTokens.insert(pair<string,string>("precipitationtype", currentWeather.precipitationType));
    intTokens.insert(pair<string,int>("humidity", currentWeather.humidity));
    stringTokens.insert(pair<string,string>("windspeed", currentWeather.windSpeed));
    intTokens.insert(pair<string,int>("windbearing", currentWeather.windBearing));
    stringTokens.insert(pair<string,string>("windbearingstring", currentWeather.windBearingString));
    stringTokens.insert(pair<string,string>("visibility", currentWeather.visibility));
    intTokens.insert(pair<string,int>("cloudcover", currentWeather.cloudCover));
    stringTokens.insert(pair<string,string>("pressure", currentWeather.pressure));
    stringTokens.insert(pair<string,string>("ozone", currentWeather.ozone));
    return true;
}

bool cViewHelpers::SetDiscUsage(bool forced, stringmap &stringTokens, intmap &intTokens) {
    string vdrUsageString = *cVideoDiskUsage::String();
    int discUsage = cVideoDiskUsage::UsedPercent();
    bool discAlert = (discUsage > 95) ? true : false;
    string freeTime = *cString::sprintf("%02d:%02d", cVideoDiskUsage::FreeMinutes() / 60, cVideoDiskUsage::FreeMinutes() % 60);
    int freeGB = cVideoDiskUsage::FreeMB() / 1024;

    intTokens.insert(pair<string, int>("usedpercent", discUsage));
    intTokens.insert(pair<string, int>("freepercent", 100-discUsage));
    intTokens.insert(pair<string, int>("discalert", discAlert));
    intTokens.insert(pair<string, int>("freegb", freeGB));
    stringTokens.insert(pair<string,string>("freetime", freeTime));
    stringTokens.insert(pair<string,string>("vdrusagestring", vdrUsageString));
    return true;
}

bool cViewHelpers::SetSystemLoad(bool forced, stringmap &stringTokens, intmap &intTokens) {
    double systemLoad;
    if (getloadavg(&systemLoad, 1) > 0) {
        if (lastSystemLoad == systemLoad) {
            return false;
        }
        string load = *cString::sprintf("%.2f", systemLoad);
        int loadHand = systemLoad * 1000;
        int loadHandValue = 0;
        if (loadHand > 2500)
            loadHandValue = 2525;
        else {

            int loadHandDec = loadHand - (loadHand / 100) * 100;

            if (loadHandDec <= 12)
                loadHandDec = 0;
            else if (loadHandDec <= 37)
                loadHandDec = 25;
            else if (loadHandDec <= 62)
                loadHandDec = 50;
            else if (loadHandDec <= 87)
                loadHandDec = 75;
            else
                loadHandDec = 0;

            loadHandValue = loadHand / 100 * 100 + loadHandDec;
        }
        stringTokens.insert(pair<string,string>("load", load));
        intTokens.insert(pair<string,int>("loadhand", loadHandValue));
        lastSystemLoad = systemLoad;
    } else {
        return false;
    }
    return true;
}

bool cViewHelpers::SetSystemMemory(bool forced, stringmap &stringTokens, intmap &intTokens) {
    struct sysinfo memInfo;
    sysinfo (&memInfo);
    
    long long totalMem = memInfo.totalram;
    totalMem += memInfo.totalswap;
    totalMem *= memInfo.mem_unit;
    int totalMemMB = totalMem / 1024 / 1024;

    long long usedMem = memInfo.totalram - memInfo.freeram;
    usedMem += memInfo.totalswap - memInfo.freeswap;
    usedMem *= memInfo.mem_unit;
    int usedMemMB = usedMem / 1024 / 1024;

    if (lastMemUsage == usedMemMB) {
        return false;
    }
    lastMemUsage = usedMemMB;

    intTokens.insert(pair<string,int>("totalmem", totalMemMB));
    intTokens.insert(pair<string,int>("usedmem", usedMemMB));
    if (totalMemMB > 0)
        intTokens.insert(pair<string,int>("usedmempercent", usedMemMB * 100 / totalMemMB));
    return true;
}

bool cViewHelpers::SetSystemTemperatures(bool forced, stringmap &stringTokens, intmap &intTokens) {
    cString execCommand = cString::sprintf("cd \"%s/\"; \"%s/temperatures\"", SCRIPTFOLDER, SCRIPTFOLDER);
    system(*execCommand);

    string tempCPU, tempGPU;
    int cpu, gpu;

    cString itemFilename = cString::sprintf("%s/cpu", SCRIPTOUTPUTPATH );
    ifstream file(*itemFilename, ifstream::in);
    if( file.is_open() ) {
        std::getline(file, tempCPU);
        if (tempCPU.size() > 2) {
            cpu = atoi(tempCPU.substr(0,2).c_str());
        } else
            cpu = 0;
        file.close();
    } else {
        tempCPU = "0°C";
        cpu = 0;
    }

    itemFilename = cString::sprintf("%s/gpu", SCRIPTOUTPUTPATH );
    ifstream file2(*itemFilename, ifstream::in);
    if( file2.is_open() ) {
        std::getline(file2, tempGPU);
        if (tempGPU.size() > 2) {
            gpu = atoi(tempGPU.substr(0,2).c_str());
        } else
            gpu = 0;
        file2.close();
    } else {
        tempGPU = "0°C";
        gpu = 0;
    }

    intTokens.insert(pair<string,int>("cputemp", cpu));
    intTokens.insert(pair<string,int>("gputemp", gpu));
    return true;
}

bool cViewHelpers::SetDummy(bool forced, stringmap &stringTokens, intmap &intTokens) {
    return true;
}

/******************************************************************
* Protected Functions
******************************************************************/
void cViewHelpers::InitDevices(void) {
    numDevices = cDevice::NumDevices();
    lastSignalStrength = new int[numDevices];
    lastSignalQuality = new int[numDevices];
    recDevices = new bool[numDevices];
    for (int i=0; i<numDevices; i++) {
        lastSignalStrength[i] = 0;
        lastSignalQuality[i] = 0;
        recDevices[i] = false;
    }
    devicesInit = true;
}

bool cViewHelpers::SetDevices(bool initial, bool light, intmap *intTokens, vector<stringmap> *devices) {
    if (!initial) {
        if (light)
            return false;
        //check if drawing is necessary
        bool changed = false;
        for (int i = 0; i < numDevices; i++) {
            const cDevice *device = cDevice::GetDevice(i);
            if (!device || !device->NumProvidedSystems()) {
                continue;
            }
            int signalStrength = device->SignalStrength();
            int signalQuality = device->SignalQuality();
            if ((signalStrength != lastSignalStrength[i]) || (signalQuality != lastSignalQuality[i])) {
                changed = true;
                break;
            }            
        }
        if (!changed) {
            return false;
        }
    }
    //check device which currently displays live tv
    int deviceLiveTV = -1;
    cDevice *primaryDevice = cDevice::PrimaryDevice();
    if (primaryDevice) {
        if (!primaryDevice->Replaying() || primaryDevice->Transferring())
            deviceLiveTV = cDevice::ActualDevice()->DeviceNumber();
        else
            deviceLiveTV = primaryDevice->DeviceNumber();
    }
    //check currently recording devices
    for (cTimer *timer = Timers.First(); timer; timer = Timers.Next(timer)) {
        if (!timer->Recording()) {
            continue;
        }
        if (cRecordControl *RecordControl = cRecordControls::GetRecordControl(timer)) {
            const cDevice *recDevice = RecordControl->Device();
            if (recDevice) {
                recDevices[recDevice->DeviceNumber()] = true;
            }
        }
    }
    int actualNumDevices = 0;
    for (int i = 0; i < numDevices; i++) {
        const cDevice *device = cDevice::GetDevice(i);
        if (!device || !device->NumProvidedSystems()) {
            continue;
        }
        
        stringmap deviceVals;
        stringstream strNum;
        strNum << actualNumDevices;
        actualNumDevices++;
        deviceVals.insert(pair< string, string >("devices[num]", strNum.str()));
        deviceVals.insert(pair< string, string >("devices[type]", *(device->DeviceType())));
        cCamSlot *camSlot = device->CamSlot();
        int camNumber = -1;
        if (camSlot) {
            camNumber = camSlot->SlotNumber();
            deviceVals.insert(pair< string, string >("devices[hascam]", "1"));
        } else {
            deviceVals.insert(pair< string, string >("devices[hascam]", "0"));
        }
        stringstream strCamNumber;
        strCamNumber << camNumber;
        deviceVals.insert(pair< string, string >("devices[cam]", strCamNumber.str()));

        int signalStrength = 0;
        int signalQuality = 0;
        if (!light) {
            signalStrength = device->SignalStrength();
            signalQuality = device->SignalQuality();
        }
        stringstream strStrength;
        strStrength << signalStrength;
        deviceVals.insert(pair< string, string >("devices[signalstrength]", strStrength.str()));
        stringstream strQuality;
        strQuality << signalQuality;
        deviceVals.insert(pair< string, string >("devices[signalquality]", strQuality.str()));

        deviceVals.insert(pair< string, string >("devices[livetv]", i == deviceLiveTV ? "1" : "0"));
        deviceVals.insert(pair< string, string >("devices[recording]", recDevices[i] ? "1" : "0"));

        const cChannel *channel = device->GetCurrentlyTunedTransponder();
        const cSource *source = (channel) ? Sources.Get(channel->Source()) : NULL;
        if (channel && channel->Number() > 0) {
            stringstream strChanNum;
            strChanNum << channel->Number();
            deviceVals.insert(pair< string, string >("devices[channelnumber]", strChanNum.str()));
            deviceVals.insert(pair< string, string >("devices[channelname]", channel->Name()));
            string channelID = *(channel->GetChannelID().ToString());
            deviceVals.insert(pair< string, string >("devices[channelid]", channelID));
            deviceVals.insert(pair< string, string >("devices[channellogoexists]", imgCache->LogoExists(channelID) ? "1" : "0"));
            deviceVals.insert(pair< string, string >("devices[istuned]", "1"));
        } else {
            deviceVals.insert(pair< string, string >("devices[channelnumber]", "0"));
            deviceVals.insert(pair< string, string >("devices[channelname]", ""));
            deviceVals.insert(pair< string, string >("devices[channelid]", ""));
            deviceVals.insert(pair< string, string >("devices[channellogoexists]", "0"));
            deviceVals.insert(pair< string, string >("devices[istuned]", "0"));
        }
    
        deviceVals.insert(pair< string, string >("devices[source]", source ? source->Description() : ""));

        devices->push_back(deviceVals);

        lastSignalStrength[i] = signalStrength;
        lastSignalQuality[i] = signalQuality;
    }

    intTokens->insert(pair<string, int>("numdevices", actualNumDevices));
    return true;
}

bool cViewHelpers::CheckNewMails(void) {
    static cPlugin *pMailbox = cPluginManager::GetPlugin("mailbox");
    if (!pMailbox) {
        return false;
    }
    bool newMail = false;
    if (pMailbox->Service("MailBox-HasNewMail-1.0", &newMail)) {
        return newMail;
    }
    return false;
}

void cViewHelpers::SetScraperTokens(const cEvent *event, const cRecording *recording, stringmap &stringTokens, intmap &intTokens, map < string, vector<stringmap> > &loopTokens) {
    static cPlugin *pScraper = GetScraperPlugin();
    if (!pScraper || (!event && !recording)) {
        intTokens.insert(pair<string,int>("ismovie", false));
        intTokens.insert(pair<string,int>("isseries", false));
        return;
    }

    ScraperGetEventType getType;
    getType.event = event;
    getType.recording = recording;
    if (!pScraper->Service("GetEventType", &getType)) {
        intTokens.insert(pair<string,int>("ismovie", false));
        intTokens.insert(pair<string,int>("isseries", false));
        return;
    }

    if (getType.type == tMovie) {
        cMovie movie;
        movie.movieId = getType.movieId;
        pScraper->Service("GetMovie", &movie);
        intTokens.insert(pair<string,int>("ismovie", true));
        intTokens.insert(pair<string,int>("isseries", false));

        stringTokens.insert(pair<string,string>("movietitle", movie.title));
        stringTokens.insert(pair<string,string>("movieoriginalTitle", movie.originalTitle));
        stringTokens.insert(pair<string,string>("movietagline", movie.tagline));
        stringTokens.insert(pair<string,string>("movieoverview", movie.overview));
        stringTokens.insert(pair<string,string>("moviegenres", movie.genres));
        stringTokens.insert(pair<string,string>("moviehomepage", movie.homepage));
        stringTokens.insert(pair<string,string>("moviereleasedate", movie.releaseDate));
        stringstream pop;
        pop << movie.popularity;
        stringTokens.insert(pair<string,string>("moviepopularity", pop.str()));
        stringstream vote;
        vote << movie.voteAverage;
        stringTokens.insert(pair<string,string>("movievoteaverage", pop.str()));
        stringTokens.insert(pair<string,string>("posterpath", movie.poster.path));
        stringTokens.insert(pair<string,string>("fanartpath", movie.fanart.path));
        stringTokens.insert(pair<string,string>("collectionposterpath", movie.collectionPoster.path));
        stringTokens.insert(pair<string,string>("collectionfanartpath", movie.collectionFanart.path));

        intTokens.insert(pair<string,int>("movieadult", movie.adult));
        intTokens.insert(pair<string,int>("moviebudget", movie.budget));
        intTokens.insert(pair<string,int>("movierevenue", movie.revenue));
        intTokens.insert(pair<string,int>("movieruntime", movie.runtime));
        intTokens.insert(pair<string,int>("posterwidth", movie.poster.width));
        intTokens.insert(pair<string,int>("posterheight", movie.poster.height));
        intTokens.insert(pair<string,int>("fanartwidth", movie.fanart.width));
        intTokens.insert(pair<string,int>("fanartheight", movie.fanart.height));
        intTokens.insert(pair<string,int>("collectionposterwidth", movie.collectionPoster.width));
        intTokens.insert(pair<string,int>("collectionposterheight", movie.collectionPoster.height));
        intTokens.insert(pair<string,int>("collectionfanartwidth", movie.collectionFanart.width));
        intTokens.insert(pair<string,int>("collectionfanartheight", movie.collectionFanart.height));

        vector< map< string, string > > actors;
        for (vector<cActor>::iterator act = movie.actors.begin(); act != movie.actors.end(); act++) {
            map< string, string > actor;
            actor.insert(pair<string, string>("actors[name]", (*act).name));
            actor.insert(pair<string, string>("actors[role]", (*act).role));
            actor.insert(pair<string, string>("actors[thumb]", (*act).actorThumb.path));
            stringstream actWidth, actHeight;
            actWidth << (*act).actorThumb.width;
            actHeight << (*act).actorThumb.height;
            actor.insert(pair<string, string>("actors[thumbwidth]", actWidth.str()));
            actor.insert(pair<string, string>("actors[thumbheight]", actHeight.str()));
            actors.push_back(actor);
        }
        loopTokens.insert(pair<string, vector< map< string, string > > >("actors", actors));

    } else if (getType.type == tSeries) {
        cSeries series;
        series.seriesId = getType.seriesId;
        series.episodeId = getType.episodeId;
        pScraper->Service("GetSeries", &series);
        intTokens.insert(pair<string,int>("ismovie", false));
        intTokens.insert(pair<string,int>("isseries", true));
        //Series Basics
        stringTokens.insert(pair<string,string>("seriesname", series.name));
        stringTokens.insert(pair<string,string>("seriesoverview", series.overview));
        stringTokens.insert(pair<string,string>("seriesfirstaired", series.firstAired));
        stringTokens.insert(pair<string,string>("seriesnetwork", series.network));
        stringTokens.insert(pair<string,string>("seriesgenre", series.genre));
        stringstream rating;
        rating << series.rating;
        stringTokens.insert(pair<string,string>("seriesrating", rating.str()));
        stringTokens.insert(pair<string,string>("seriesstatus", series.status));
        //Episode Information
        intTokens.insert(pair<string,int>("episodenumber", series.episode.number));
        intTokens.insert(pair<string,int>("episodeseason", series.episode.season));
        stringTokens.insert(pair<string,string>("episodetitle", series.episode.name));
        stringTokens.insert(pair<string,string>("episodefirstaired", series.episode.firstAired));
        stringTokens.insert(pair<string,string>("episodegueststars", series.episode.guestStars));
        stringTokens.insert(pair<string,string>("episodeoverview", series.episode.overview));
        stringstream eprating;
        eprating << series.episode.rating;
        stringTokens.insert(pair<string,string>("episoderating", eprating.str()));
        intTokens.insert(pair<string,int>("episodeimagewidth", series.episode.episodeImage.width));
        intTokens.insert(pair<string,int>("episodeimageheight", series.episode.episodeImage.height));
        stringTokens.insert(pair<string,string>("episodeimagepath", series.episode.episodeImage.path));
        //Seasonposter
        intTokens.insert(pair<string,int>("seasonposterwidth", series.seasonPoster.width));
        intTokens.insert(pair<string,int>("seasonposterheight", series.seasonPoster.height));
        stringTokens.insert(pair<string,string>("seasonposterpath", series.seasonPoster.path));

        //Posters
        int current = 1;
        for(vector<cTvMedia>::iterator poster = series.posters.begin(); poster != series.posters.end(); poster++) {
            stringstream labelWidth, labelHeight, labelPath;
            labelWidth << "seriesposter" << current << "width";
            labelHeight << "seriesposter" << current << "height";
            labelPath << "seriesposter" << current << "path";

            intTokens.insert(pair<string,int>(labelWidth.str(), (*poster).width));
            intTokens.insert(pair<string,int>(labelHeight.str(), (*poster).height));
            stringTokens.insert(pair<string,string>(labelPath.str(), (*poster).path));
            current++;
        }
        if (current < 3) {
            for (; current < 4; current++) {
                stringstream labelWidth, labelHeight, labelPath;
                labelWidth << "seriesposter" << current << "width";
                labelHeight << "seriesposter" << current << "height";
                labelPath << "seriesposter" << current << "path";
                
                intTokens.insert(pair<string,int>(labelWidth.str(), 0));
                intTokens.insert(pair<string,int>(labelHeight.str(), 0));
                stringTokens.insert(pair<string,string>(labelPath.str(), ""));
            }
        }

        //Banners
        current = 1;
        for(vector<cTvMedia>::iterator banner = series.banners.begin(); banner != series.banners.end(); banner++) {
            stringstream labelWidth, labelHeight, labelPath;
            labelWidth << "seriesbanner" << current << "width";
            labelHeight << "seriesbanner" << current << "height";
            labelPath << "seriesbanner" << current << "path";
            
            intTokens.insert(pair<string,int>(labelWidth.str(), (*banner).width));
            intTokens.insert(pair<string,int>(labelHeight.str(), (*banner).height));
            stringTokens.insert(pair<string,string>(labelPath.str(), (*banner).path));
            current++;
        }
        if (current < 3) {
            for (; current < 4; current++) {
                stringstream labelWidth, labelHeight, labelPath;
                labelWidth << "seriesbanner" << current << "width";
                labelHeight << "seriesbanner" << current << "height";
                labelPath << "seriesbanner" << current << "path";
                
                intTokens.insert(pair<string,int>(labelWidth.str(), 0));
                intTokens.insert(pair<string,int>(labelHeight.str(), 0));
                stringTokens.insert(pair<string,string>(labelPath.str(), ""));
            }
        }
        
        //Fanarts
        current = 1;
        for(vector<cTvMedia>::iterator fanart = series.fanarts.begin(); fanart != series.fanarts.end(); fanart++) {
            stringstream labelWidth, labelHeight, labelPath;
            labelWidth << "seriesfanart" << current << "width";
            labelHeight << "seriesfanart" << current << "height";
            labelPath << "seriesfanart" << current << "path";
            
            intTokens.insert(pair<string,int>(labelWidth.str(), (*fanart).width));
            intTokens.insert(pair<string,int>(labelHeight.str(), (*fanart).height));
            stringTokens.insert(pair<string,string>(labelPath.str(), (*fanart).path));
            current++;
        }
        if (current < 3) {
            for (; current < 4; current++) {
                stringstream labelWidth, labelHeight, labelPath;
                labelWidth << "seriesfanart" << current << "width";
                labelHeight << "seriesfanart" << current << "height";
                labelPath << "seriesfanart" << current << "path";
                
                intTokens.insert(pair<string,int>(labelWidth.str(), 0));
                intTokens.insert(pair<string,int>(labelHeight.str(), 0));
                stringTokens.insert(pair<string,string>(labelPath.str(), ""));
            }
        }
        
        //Actors
       vector< map< string, string > > actors;
        for (vector<cActor>::iterator act = series.actors.begin(); act != series.actors.end(); act++) {
            map< string, string > actor;
            actor.insert(pair<string, string>("actors[name]", (*act).name));
            actor.insert(pair<string, string>("actors[role]", (*act).role));
            actor.insert(pair<string, string>("actors[thumb]", (*act).actorThumb.path));
            stringstream actWidth, actHeight;
            actWidth << (*act).actorThumb.width;
            actHeight << (*act).actorThumb.height;
            actor.insert(pair<string, string>("actors[thumbwidth]", actWidth.str()));
            actor.insert(pair<string, string>("actors[thumbheight]", actHeight.str()));
            actors.push_back(actor);
        }
        loopTokens.insert(pair<string, vector< map< string, string > > >("actors", actors));

    } else {
        intTokens.insert(pair<string,int>("ismovie", false));
        intTokens.insert(pair<string,int>("isseries", false));
    }

}

void cViewHelpers::SetPosterBanner(const cEvent *event, stringmap &stringTokens, intmap &intTokens) {
    static cPlugin *pScraper = GetScraperPlugin();
    if (!pScraper) {
        return;
    }

    ScraperGetPosterBanner call;
    call.event = event;
    if (pScraper->Service("GetPosterBanner", &call)) {
        int mediaWidth = 0;
        int mediaHeight = 0;
        string mediaPath = "";
        bool isBanner = false;
        int posterWidth = 0;
        int posterHeight = 0;
        string posterPath = "";
        bool hasPoster = false;
        int bannerWidth = 0;
        int bannerHeight = 0;
        string bannerPath = "";
        bool hasBanner = false;

        if ((call.type == tSeries) && call.banner.path.size() > 0) {
            mediaWidth = call.banner.width;
            mediaHeight = call.banner.height;
            mediaPath = call.banner.path;
            isBanner = true;
            bannerWidth = mediaWidth;
            bannerHeight = mediaHeight;
            bannerPath = mediaPath;
            hasBanner = true;

            ScraperGetPoster callPoster;
            callPoster.event = event;
            callPoster.recording = NULL;
            if (pScraper->Service("GetPoster", &callPoster)) {
                posterWidth = callPoster.poster.width;
                posterHeight = callPoster.poster.height;
                posterPath = callPoster.poster.path;
                hasPoster = true;
            }
        } else if (call.type == tMovie && call.poster.path.size() > 0 && call.poster.height > 0) {
            mediaWidth = call.poster.width;
            mediaHeight = call.poster.height;
            mediaPath = call.poster.path;
            posterWidth = call.poster.width;
            posterHeight = call.poster.height;
            posterPath = call.poster.path;
            hasPoster = true;
        } else
            return;

        intTokens.insert(pair<string,int>("mediawidth", mediaWidth));
        intTokens.insert(pair<string,int>("mediaheight", mediaHeight));
        intTokens.insert(pair<string,int>("isbanner", isBanner));
        stringTokens.insert(pair<string,string>("mediapath", mediaPath));
        intTokens.insert(pair<string,int>("posterwidth", posterWidth));
        intTokens.insert(pair<string,int>("posterheight", posterHeight));
        stringTokens.insert(pair<string,string>("posterpath", posterPath));
        intTokens.insert(pair<string,int>("hasposter", hasPoster));
        intTokens.insert(pair<string,int>("bannerwidth", bannerWidth));
        intTokens.insert(pair<string,int>("bannerheight", bannerHeight));
        stringTokens.insert(pair<string,string>("bannerpath", bannerPath));
        intTokens.insert(pair<string,int>("hasbanner", hasBanner));
    }
}

void cViewHelpers::SetTimers(map<string,int> *intTokens, map<string,string> *stringTokens, vector<map<string,string> > *timers) {
    cGlobalSortedTimers SortedTimers;// local and remote timers
    int numTimers = SortedTimers.Size();

    intTokens->insert(pair<string, int>("numtimers", numTimers));

    int numTimerConflicts = SortedTimers.NumTimerConfilicts();
    intTokens->insert(pair<string, int>("numtimerconflicts", numTimerConflicts));

    for (int i=0; i<15; i++) {
        stringstream name;
        name << "timer" << i+1 << "exists";
        if (i < numTimers) {
            intTokens->insert(pair<string, int>(name.str(), true));
        } else {
            intTokens->insert(pair<string, int>(name.str(), false));
        }
    }

    for (int i = 0; i < numTimers; i++) {
        if (i >=15)
            break;
        map< string, string > timerVals;
        const cTimer *Timer = SortedTimers[i];
        string isRemoteTimer = SortedTimers.IsRemoteTimer(i) ? "1" : "0";
        const cEvent *event = Timer->Event();
        if (event) {
            timerVals.insert(pair< string, string >("timers[title]", event->Title()));
        } else {
            const char *File = Setup.FoldersInTimerMenu ? NULL : strrchr(Timer->File(), FOLDERDELIMCHAR);
            if (File && strcmp(File + 1, TIMERMACRO_TITLE) && strcmp(File + 1, TIMERMACRO_EPISODE))
                File++;
            else
                File = Timer->File();
            timerVals.insert(pair< string, string >("timers[title]", File));            
        }
        const cChannel *channel = Timer->Channel();
        if (channel) {
            timerVals.insert(pair< string, string >("timers[channelname]", channel->Name()));
            stringstream chanNum;
            chanNum << channel->Number();
            timerVals.insert(pair< string, string >("timers[channelnumber]", chanNum.str()));
            string channelID = *(channel->GetChannelID().ToString());
            timerVals.insert(pair< string, string >("timers[channelid]", channelID));
            bool logoExists = imgCache->LogoExists(channelID);
            timerVals.insert(pair< string, string >("timers[channellogoexists]", logoExists ? "1" : "0"));
        } else {
            timerVals.insert(pair< string, string >("timers[channelname]", ""));
            timerVals.insert(pair< string, string >("timers[channelnumber]", "0"));
            timerVals.insert(pair< string, string >("timers[channelid]", ""));  
            timerVals.insert(pair< string, string >("timers[channellogoexists]", "0"));
        }
        
        timerVals.insert(pair< string, string >("timers[recording]", Timer->Recording() ? "1" : "0"));

        cString timerDate("");
        if (Timer->Recording()) {
            timerDate = cString::sprintf("-%s", *TimeString(Timer->StopTime()));
        } else {
            time_t Now = time(NULL);
            cString Today = WeekDayName(Now);
            cString Time = TimeString(Timer->StartTime());
            cString Day = WeekDayName(Timer->StartTime());
            if (Timer->StartTime() > Now + 6 * SECSINDAY) {
                time_t ttm = Timer->StartTime();
                struct tm * timerTime = localtime(&ttm);
                timerDate = cString::sprintf("%02d.%02d %s", timerTime->tm_mday, timerTime->tm_mon + 1, *Time);
            } else if (strcmp(Day, Today) != 0)
                timerDate = cString::sprintf("%s %s", *Day, *Time);
            else
                timerDate = Time;
            if (Timer->Flags() & tfVps)
                timerDate = cString::sprintf("VPS %s", *timerDate);
        }
        timerVals.insert(pair< string, string >("timers[datetime]", *timerDate));
        timerVals.insert(pair< string, string >("timers[isremotetimer]", isRemoteTimer));
        
        timers->push_back(timerVals);
    }
}

void cViewHelpers::SetLastRecordings(map<string,int> *intTokens, map<string,string> *stringTokens, vector<stringmap> *lastRecordings) {

    cGlobalSortedTimers SortedTimers;// local and remote timers
    int numTimers = SortedTimers.Size();
    //set number of timers so that it is possible to adapt this viewelement accordingly
    intTokens->insert(pair<string, int>("numtimers", numTimers));

    list<cRecording*> orderedRecs;

    for (cRecording *recording = Recordings.First(); recording; recording = Recordings.Next(recording)) {
        if (orderedRecs.size() == 0) {
            orderedRecs.push_back(recording);
            continue;
        }
        bool inserted = false;
        for (list<cRecording*>::iterator it = orderedRecs.begin(); it != orderedRecs.end(); it++) {
            const cRecording *orderedRec = *it;
            if (recording->Start() >= orderedRec->Start()) {
                orderedRecs.insert(it, recording);
                inserted = true;
                break;
            }
        }
        if (!inserted) {
            orderedRecs.push_back(recording);
        }
    }

    int found = 0;
    for (list<cRecording*>::iterator it = orderedRecs.begin(); it != orderedRecs.end(); it++) {
        const cRecording *recording = *it;
#if APIVERSNUM >= 20101
        if (recording->IsInUse()) {
            continue;
        }
#endif
        map< string, string > recVals;
        string recFullPath = recording->Name() ? recording->Name() : "";
        string recName = "";
        string recPath = "";
        RecName(recFullPath, recName, recPath);
        stringstream recDuration;
        int dur = recording->LengthInSeconds()/60;
        recDuration << dur;
        string posterPath = "";
        int posterWidth = 0;
        int posterHeight = 0;
        bool hasPoster = false;
        RecPoster(recording, posterWidth, posterHeight, posterPath, hasPoster);
        stringstream sPosterWidth;
        sPosterWidth << posterWidth;
        stringstream sPosterHeight;
        sPosterHeight << posterHeight;
        string sHasPoster = hasPoster ? "1" : "0";
        recVals.insert(pair< string, string >("recordings[name]", recName));
        recVals.insert(pair< string, string >("recordings[seriesname]", recPath));
        recVals.insert(pair< string, string >("recordings[date]", *ShortDateString(recording->Start())));
        recVals.insert(pair< string, string >("recordings[time]", *TimeString(recording->Start())));
        recVals.insert(pair< string, string >("recordings[duration]", recDuration.str()));
        recVals.insert(pair< string, string >("recordings[hasposter]", sHasPoster));
        recVals.insert(pair< string, string >("recordings[posterpath]", posterPath));
        recVals.insert(pair< string, string >("recordings[posterwidth]", sPosterWidth.str()));
        recVals.insert(pair< string, string >("recordings[posterheight]", sPosterHeight.str()));
        lastRecordings->push_back(recVals);
        found++;
        if (found == 5)
            break;
    }
}

void cViewHelpers::SetMenuHeader(eMenuCategory cat, string menuTitle, stringmap &stringTokens, intmap &intTokens) {
    stringTokens.insert(pair<string,string>("title", menuTitle));
    stringTokens.insert(pair<string,string>("vdrversion", VDRVERSION));

    //check for standard menu entries
    bool hasIcon = false;

    string icon = imgCache->GetIconName(menuTitle, cat);
    if (imgCache->MenuIconExists(icon))
        hasIcon = true;

    stringTokens.insert(pair<string,string>("icon", icon));
    intTokens.insert(pair<string,int>("hasicon", hasIcon));

    //Disc Usage
    string vdrUsageString = *cVideoDiskUsage::String();
    int discUsage = cVideoDiskUsage::UsedPercent();
    bool discAlert = (discUsage > 95) ? true : false;
    string freeTime = *cString::sprintf("%02d:%02d", cVideoDiskUsage::FreeMinutes() / 60, cVideoDiskUsage::FreeMinutes() % 60);
    int freeGB = cVideoDiskUsage::FreeMB() / 1024;

    intTokens.insert(pair<string, int>("usedpercent", discUsage));
    intTokens.insert(pair<string, int>("freepercent", 100-discUsage));
    intTokens.insert(pair<string, int>("discalert", discAlert));
    intTokens.insert(pair<string, int>("freegb", freeGB));
    stringTokens.insert(pair<string,string>("freetime", freeTime));
    stringTokens.insert(pair<string,string>("vdrusagestring", vdrUsageString));
}

void cViewHelpers::SetCurrentSchedule(string recName, stringmap &stringTokens, intmap &intTokens) {
    cDevice *device = cDevice::PrimaryDevice();
    const cChannel *channel = NULL;
    if (!device->Replaying() || device->Transferring()) {
        channel = Channels.GetByNumber(device->CurrentChannel());
    }
    if (channel) {
        SetCurrentScheduleFromChannel(channel, stringTokens, intTokens);
    } else {
        if (recName.size() == 0)
            return;
        const cRecording *recording = new cRecording(recName.c_str());
        if (recording) {
            SetCurrentScheduleFromRecording(recording, stringTokens, intTokens);
            delete recording;
        }
    }
}

bool cViewHelpers::SetEcmInfos(int channelSid, stringmap &stringTokens, intmap &intTokens) {
    static cPlugin *pDVBApi = cPluginManager::GetPlugin("dvbapi");
    if (!pDVBApi)
        return false;

    sDVBAPIEcmInfo ecmInfo;
    ecmInfo.sid = channelSid;

    if (!pDVBApi->Service("GetEcmInfo", &ecmInfo)) {
        return false;
    }

    if (ecmInfo.hops < 0 || ecmInfo.ecmtime <= 0)
        return false;
    if (CompareECMInfos(&ecmInfo))
        return false;
    lastEcmInfo = ecmInfo;

    intTokens.insert(pair<string,int>("caid", ecmInfo.caid));
    intTokens.insert(pair<string,int>("pid", ecmInfo.pid));
    intTokens.insert(pair<string,int>("prid", ecmInfo.prid));
    intTokens.insert(pair<string,int>("ecmtime", ecmInfo.ecmtime));
    intTokens.insert(pair<string,int>("hops", ecmInfo.hops));

    stringTokens.insert(pair<string,string>("cardsystem", *ecmInfo.cardsystem ? *ecmInfo.cardsystem : ""));
    stringTokens.insert(pair<string,string>("reader", *ecmInfo.reader ? *ecmInfo.reader : ""));
    stringTokens.insert(pair<string,string>("from", *ecmInfo.from ? *ecmInfo.from : ""));
    stringTokens.insert(pair<string,string>("protocol", *ecmInfo.protocol ? *ecmInfo.protocol : ""));

    return true;
}

/********************************************************************************
* Private Functions
********************************************************************************/

void cViewHelpers::RecName(string &path, string &name, string &folder) {
    size_t delim = path.find_last_of('~');
    if (delim == string::npos) {
        name = path;
        if (name.find('%') == 0) {
            name = name.substr(1);
        }
        return;
    }
    name = path.substr(delim+1);
    if (name.find('%') == 0) {
        name = name.substr(1);
    }
    folder = path.substr(0, delim);
    size_t delim2 = folder.find_last_of('~');
    if (delim2 == string::npos) {
        return;
    }
    folder = folder.substr(delim2+1);
}

void cViewHelpers::RecPoster(const cRecording *rec, int &posterWidth, int &posterHeight, string &path, bool &hasPoster) {
    static cPlugin *pScraper = GetScraperPlugin();
    if (!pScraper)
        return;
    ScraperGetPoster callPoster;
    callPoster.event = NULL;
    callPoster.recording = rec;
    if (pScraper->Service("GetPoster", &callPoster)) {
        posterWidth = callPoster.poster.width;
        posterHeight = callPoster.poster.height;
        path = callPoster.poster.path;
        hasPoster = true;
    }
}

void cViewHelpers::SetCurrentScheduleFromChannel(const cChannel *channel, stringmap &stringTokens, intmap &intTokens) {
    const cEvent *event = NULL;
    cSchedulesLock SchedulesLock;
    if (const cSchedules *Schedules = cSchedules::Schedules(SchedulesLock))
        if (const cSchedule *Schedule = Schedules->GetSchedule(channel))
            event = Schedule->GetPresentEvent();
    if (!event)
        return;

    intTokens.insert(pair<string,int>("islivetv", 1));
    stringTokens.insert(pair<string,string>("title", (event->Title())?event->Title():""));
    stringTokens.insert(pair<string,string>("subtitle", (event->ShortText())?event->ShortText():""));
    stringTokens.insert(pair<string,string>("start", *event->GetTimeString()));
    stringTokens.insert(pair<string,string>("stop", *event->GetEndTimeString()));
    intTokens.insert(pair<string,int>("duration", event->Duration() / 60));
    intTokens.insert(pair<string,int>("durationhours", event->Duration() / 3600));
    stringTokens.insert(pair<string,string>("durationminutes", *cString::sprintf("%.2d", (event->Duration() / 60)%60)));
    intTokens.insert(pair<string,int>("elapsed", (int)round((time(NULL) - event->StartTime())/60)));
    intTokens.insert(pair<string,int>("remaining", (int)round((event->EndTime() - time(NULL))/60)));

    int mediaWidth = 0;
    int mediaHeight = 0;
    string mediaPath = "";
    bool isBanner = false;
    int posterWidth = 0;
    int posterHeight = 0;
    string posterPath = "";
    bool hasPoster = false;
    int bannerWidth = 0;
    int bannerHeight = 0;
    string bannerPath = "";
    bool hasBanner = false;
    static cPlugin *pScraper = GetScraperPlugin();
    if (pScraper) {
        ScraperGetPosterBanner call;
        call.event = event;
        if (pScraper->Service("GetPosterBanner", &call)) {
            if ((call.type == tSeries) && call.banner.path.size() > 0) {
                mediaWidth = call.banner.width;
                mediaHeight = call.banner.height;
                mediaPath = call.banner.path;
                isBanner = true;
                bannerWidth = mediaWidth;
                bannerHeight = mediaHeight;
                bannerPath = mediaPath;
                hasBanner = true;
                ScraperGetPoster callPoster;
                callPoster.event = event;
                callPoster.recording = NULL;
                if (pScraper->Service("GetPoster", &callPoster)) {
                    posterWidth = callPoster.poster.width;
                    posterHeight = callPoster.poster.height;
                    posterPath = callPoster.poster.path;
                    hasPoster = true;
                }
            } else if (call.type == tMovie && call.poster.path.size() > 0 && call.poster.height > 0) {
                mediaWidth = call.poster.width;
                mediaHeight = call.poster.height;
                mediaPath = call.poster.path;
                posterWidth = call.poster.width;
                posterHeight = call.poster.height;
                posterPath = call.poster.path;
                hasPoster = true;
            }
        }
    }
    intTokens.insert(pair<string,int>("mediawidth", mediaWidth));
    intTokens.insert(pair<string,int>("mediaheight", mediaHeight));
    intTokens.insert(pair<string,int>("isbanner", isBanner));
    stringTokens.insert(pair<string,string>("mediapath", mediaPath));
    intTokens.insert(pair<string,int>("posterwidth", posterWidth));
    intTokens.insert(pair<string,int>("posterheight", posterHeight));
    stringTokens.insert(pair<string,string>("posterpath", posterPath));
    intTokens.insert(pair<string,int>("hasposter", hasPoster));
    intTokens.insert(pair<string,int>("bannerwidth", bannerWidth));
    intTokens.insert(pair<string,int>("bannerheight", bannerHeight));
    stringTokens.insert(pair<string,string>("bannerpath", bannerPath));
    intTokens.insert(pair<string,int>("hasbanner", hasBanner));
}

void cViewHelpers::SetCurrentScheduleFromRecording(const cRecording *recording, stringmap &stringTokens, intmap &intTokens) {
    intTokens.insert(pair<string,int>("islivetv", 0));

    string recFullName = recording->Name() ? recording->Name() : "";
    string recName = "";
    string recFolder = "";
    RecName(recFullName, recName, recFolder);

    stringTokens.insert(pair<string,string>("title", recName));
    const cRecordingInfo *info = recording->Info();
    if (info) {
        stringTokens.insert(pair<string,string>("subtitle", info->ShortText() ? info->ShortText() : ""));
    } else {
        stringTokens.insert(pair<string,string>("subtitle", recFolder));
    }
    stringTokens.insert(pair<string,string>("start", ""));
    stringTokens.insert(pair<string,string>("stop", ""));
    intTokens.insert(pair<string,int>("duration", recording->LengthInSeconds() / 60));
    intTokens.insert(pair<string,int>("durationhours", recording->LengthInSeconds() / 3600));
    stringTokens.insert(pair<string,string>("durationminutes", *cString::sprintf("%.2d", (recording->LengthInSeconds() / 60)%60)));
    intTokens.insert(pair<string,int>("elapsed", 0));
    intTokens.insert(pair<string,int>("remaining", 0));

    int mediaWidth = 0;
    int mediaHeight = 0;
    string mediaPath = "";
    bool isBanner = false;
    int posterWidth = 0;
    int posterHeight = 0;
    string posterPath = "";
    bool hasPoster = false;
    int bannerWidth = 0;
    int bannerHeight = 0;
    string bannerPath = "";
    bool hasBanner = false;
    static cPlugin *pScraper = GetScraperPlugin();
    if (pScraper) {
        ScraperGetPosterBannerV2 call;
        call.event = NULL;
        call.recording = recording;
        if (pScraper->Service("GetPosterBannerV2", &call)) {
            if ((call.type == tSeries) && call.banner.path.size() > 0) {
                mediaWidth = call.banner.width;
                mediaHeight = call.banner.height;
                mediaPath = call.banner.path;
                isBanner = true;
                bannerWidth = mediaWidth;
                bannerHeight = mediaHeight;
                bannerPath = mediaPath;
                hasBanner = true;
                ScraperGetPoster callPoster;
                callPoster.event = NULL;
                callPoster.recording = recording;
                if (pScraper->Service("GetPoster", &callPoster)) {
                    posterWidth = callPoster.poster.width;
                    posterHeight = callPoster.poster.height;
                    posterPath = callPoster.poster.path;
                    hasPoster = true;
                }
            } else if (call.type == tMovie && call.poster.path.size() > 0 && call.poster.height > 0) {
                mediaWidth = call.poster.width;
                mediaHeight = call.poster.height;
                mediaPath = call.poster.path;
                posterWidth = call.poster.width;
                posterHeight = call.poster.height;
                posterPath = call.poster.path;
                hasPoster = true;
            }
        }
    }
    intTokens.insert(pair<string,int>("mediawidth", mediaWidth));
    intTokens.insert(pair<string,int>("mediaheight", mediaHeight));
    intTokens.insert(pair<string,int>("isbanner", isBanner));
    stringTokens.insert(pair<string,string>("mediapath", mediaPath));
    intTokens.insert(pair<string,int>("posterwidth", posterWidth));
    intTokens.insert(pair<string,int>("posterheight", posterHeight));
    stringTokens.insert(pair<string,string>("posterpath", posterPath));
    intTokens.insert(pair<string,int>("hasposter", hasPoster));
    intTokens.insert(pair<string,int>("bannerwidth", bannerWidth));
    intTokens.insert(pair<string,int>("bannerheight", bannerHeight));
    stringTokens.insert(pair<string,string>("bannerpath", bannerPath));
    intTokens.insert(pair<string,int>("hasbanner", hasBanner));
}

bool cViewHelpers::CompareECMInfos(sDVBAPIEcmInfo *ecmInfo) {
    if (ecmInfo->caid != lastEcmInfo.caid)
        return false;
    if (ecmInfo->pid != lastEcmInfo.pid)
        return false;
    if (ecmInfo->prid != lastEcmInfo.prid)
        return false;
    if (ecmInfo->ecmtime != lastEcmInfo.ecmtime)
        return false;
    if (ecmInfo->hops != lastEcmInfo.hops)
        return false;
    return true;
}
