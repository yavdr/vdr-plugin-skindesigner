#ifndef __SKINREPO_H
#define __SKINREPO_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlerror.h>
#include <vdr/plugin.h>

using namespace std;

enum eRepoType {
    rtUndefined,
    rtGit,
    rtZipUrl
};

// --- cSkinRepo -------------------------------------------------------------

class cSkinRepo : public cThread {
private:
    string name;
    eRepoType repoType;
    string url;
    vector<string> specialFonts;
    vector<string> supportedPlugins;
    vector< pair < string, string > > screenshots;
    //helpers for execution
    string command;
    string tempfile;
    int result;
    virtual void Action(void);
public:
    cSkinRepo(void);
    virtual ~cSkinRepo(void);
    void SetName(string name) { this->name = name; };
    void SetRepoType(eRepoType type) { this->repoType = type; };
    void SetUrl(string url) { this->url = url; };
    void SetSpecialFont(string font) { specialFonts.push_back(font); };
    void SetSupportedPlugin(string plugin) { supportedPlugins.push_back(plugin); };
    void SetScreenshot(string desc, string url) { screenshots.push_back(pair<string, string>(desc, url)); };
    string Name(void) { return name; };
    void Install(string path);
    bool InstallationFinished(void) { return !(Running()); };
    void Debug(void);
};

// --- cSkinRepos -------------------------------------------------------------

class cSkinRepos {
private:
    string repoFile;
    xmlDocPtr doc;
    vector<cSkinRepo*> repos;
    void ReadRepository(xmlNodePtr node);
public:
    cSkinRepos(void);
    virtual ~cSkinRepos(void);
    void Read(string path);
    cSkinRepo *GetRepo(string name);
    void Debug(void);
};

#endif //__SKINREPO_H
