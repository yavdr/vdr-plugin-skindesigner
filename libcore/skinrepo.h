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

enum eAction {
    eaUndefined,
    eaInstall,
    eaUpdate
};

// --- cSkinRepo -------------------------------------------------------------

class cSkinRepo : public cThread {
private:
    string name;
    eRepoType repoType;
    eAction action;
    string url;
    string author;
    vector<string> specialFonts;
    vector<string> supportedPlugins;
    vector< pair < string, string > > screenshots;
    //helpers for execution
    string command;
    string command2;
    string tempfile;
    int result;
    string skinPath;
    string themesPath;
    virtual void Action(void);
    void CreateThemeFiles(void);
public:
    cSkinRepo(void);
    virtual ~cSkinRepo(void);
    void SetName(string name) { this->name = name; };
    void SetRepoType(eRepoType type) { this->repoType = type; };
    void SetUrl(string url) { this->url = url; };
    void SetAuthor(string author) { this->author = author; };
    void SetSpecialFont(string font) { specialFonts.push_back(font); };
    void SetSupportedPlugin(string plugin) { supportedPlugins.push_back(plugin); };
    void SetScreenshot(string desc, string url) { screenshots.push_back(pair<string, string>(desc, url)); };
    eRepoType Type(void) { return repoType; };
    string Name(void) { return name; };
    string Author(void) { return author; };
    string Url(void) { return url; };
    vector<string> SpecialFonts(void) { return specialFonts; };
    vector<string> SupportedPlugins(void) { return supportedPlugins; };
    vector< pair < string, string > > Screenshots(void) { return screenshots; };
    void Install(string path, string themesPath);
    void Update(string path);
    bool InstallationFinished(void) { return !(Running()); };
    bool SuccessfullyInstalled(void) { if (result == 0) return true; return false; };
    bool SuccessfullyUpdated(void);
    void Debug(void);
};

// --- cSkinRepos -------------------------------------------------------------

class cSkinRepos {
private:
    string repoFile;
    xmlDocPtr doc;
    vector<cSkinRepo*> repos;
    vector<cSkinRepo*>::iterator repoIt;
    void ReadRepository(xmlNodePtr node);
public:
    cSkinRepos(void);
    virtual ~cSkinRepos(void);
    void Read(string path);
    int Count(void) { return repos.size(); };
    cSkinRepo *GetRepo(string name);
    void InitRepoIterator(void) { repoIt = repos.begin(); };
    cSkinRepo *GetNextRepo(void);
    void Debug(void);
};

#endif //__SKINREPO_H
