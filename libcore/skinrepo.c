#include <iostream>
#include <fstream>
#include "skinrepo.h"
#include "../libcore/helpers.h"

using namespace std;

// --- cSkinRepo -------------------------------------------------------------

cSkinRepo::cSkinRepo(void) {
    name = "";
    repoType = rtUndefined;
    action = eaUndefined;
    url = "";
    author = "unknown";
    minSDVersion = "0.0.1";
    command = "";
    command2 = "";
    tempfile = "";
    result = -1;
    skinPath = "";
    themesPath = "";
}

cSkinRepo::~cSkinRepo() {
}

bool cSkinRepo::Valid(void) {
    if (!name.size())
        return false;
    if (repoType == rtUndefined)
        return false;
    if (!url.size())
        return false;
    return true;
}

void cSkinRepo::Install(string path, string themesPath) {
    if (Running())
        return;
    action = eaInstall;
    this->skinPath = path + name;
    this->themesPath = themesPath;
    if (repoType == rtGit) {

        command = *cString::sprintf("git clone --depth=1 --progress %s %s", url.c_str(), skinPath.c_str());
        tempfile = *cString::sprintf("gitclone_%s_%ld.out", name.c_str(), time(0));

        dsyslog("skindesigner: installing skin from Git, command: %s, logfile: %s", command.c_str(), tempfile.c_str());

        Start();

    } else if (repoType == rtZipUrl) {

        size_t hit = url.find_last_of('/');
        if (hit == string::npos)
            return;
        string filename = url.substr(hit+1);
        
        command = *cString::sprintf("wget -P /tmp/ %s", url.c_str());
        command2 = *cString::sprintf("unzip /tmp/%s -d %s", filename.c_str(), path.c_str());

        dsyslog("skindesigner: installing skin from Zip, command: %s, %s", command.c_str(), command2.c_str());
        
        Start();
    }
}

void cSkinRepo::Update(string path) {
    if (Running())
        return;
    action = eaUpdate;
    this->skinPath = path + name;
    if (repoType == rtGit) {

        command = *cString::sprintf("cd %s; git pull", skinPath.c_str());
        tempfile = *cString::sprintf("gitpull_%s_%ld.out", name.c_str(), time(0));

        dsyslog("skindesigner: updating skin from Git, command: %s, logfile: /tmp/%s", command.c_str(), tempfile.c_str());

        Start();

    } else if (repoType == rtZipUrl) {

        //TODO

    }
}

void cSkinRepo::Action(void) {
    if (command.size() < 1)
        return;
    if (tempfile.size() > 0) {
        command = *cString::sprintf("%s > /tmp/%s 2>&1", command.c_str(), tempfile.c_str());
    }
    
    result = system (command.c_str());

    if (result == 0 && command2.size() > 0) {
        result = system (command2.c_str());            
    }

    if (result == 0) {
        if (action == eaInstall)
            CreateThemeFiles();
        dsyslog("skindesigner: %s successfully executed", command.c_str());
    } else {
        esyslog("skindesigner: ERROR executing %s", command.c_str());
    }
}

void cSkinRepo::CreateThemeFiles(void) {
    string availableThemesPath = skinPath + "/themes/";
    DIR *folder = NULL;
    struct dirent *dirEntry;
    folder = opendir(availableThemesPath.c_str());
    if (!folder) {
        return;
    }
    vector<string> skinThemes;
    while (dirEntry = readdir(folder)) {
        string dirEntryName = dirEntry->d_name;
        int dirEntryType = dirEntry->d_type;
        if (!dirEntryName.compare(".") || !dirEntryName.compare("..") || dirEntryType != DT_DIR)
            continue;
        skinThemes.push_back(dirEntryName);
    }
    for (vector<string>::iterator it = skinThemes.begin(); it != skinThemes.end(); it++) {
        string themeName = *it;
        string themeFileName = themesPath;
        themeFileName += name + "-" + themeName + ".theme";
        if (FileExists(themeFileName)) {
            continue;
        }
        ofstream themeFile (themeFileName.c_str());
        if (themeFile.is_open()) {
            themeFile << "Description = ";
            themeFile <<  themeName << "\n";
            themeFile.close();
        }
    }
}

bool cSkinRepo::SuccessfullyUpdated(void) {
    string logfilePath = "/tmp/" + tempfile;
    bool updated = true;
    string line;
    ifstream logfile(logfilePath.c_str());
    if (logfile.is_open()) {
        while ( getline (logfile, line) ) {
            if (line.find("up-to-date") != string::npos) {
                updated = false;
                break;
            }
        }
        logfile.close();
    }
    return updated;
}

void cSkinRepo::Debug() {
    string strRepoType = "Undefined";
    if (repoType == rtGit)
        strRepoType = "Git";
    else if (repoType == rtZipUrl)
        strRepoType = "ZipUrl";
    dsyslog("skindesigner: --- skinrepo %s, Type %s ---", name.c_str(), strRepoType.c_str());
    dsyslog("skindesigner: url %s", url.c_str());
    dsyslog("skindesigner: author %s", author.c_str());
    dsyslog("skindesigner: minimum Skindesigner Version required %s", minSDVersion.c_str());
    if (specialFonts.size() > 0) {
        for (vector<string>::iterator it = specialFonts.begin(); it != specialFonts.end(); it++) {
            dsyslog("skindesigner: special font %s", (*it).c_str());
        }
    }
    if (supportedPlugins.size() > 0) {
        for (vector<string>::iterator it = supportedPlugins.begin(); it != supportedPlugins.end(); it++) {
            dsyslog("skindesigner: supported plugin %s", (*it).c_str());
        }
    }
    if (screenshots.size() > 0) {
        for (vector<pair<string,string> >::iterator it = screenshots.begin(); it != screenshots.end(); it++) {
            string desc = (it->first).c_str();
            string url = (it->second).c_str();
            dsyslog("skindesigner: screenshot \"%s\", url %s", desc.c_str(), url.c_str());
        }
    }
}

// --- cSkinRepos -------------------------------------------------------------

cSkinRepos::cSkinRepos(void) {
    skinRepoUrl = "https://github.com/louisbraun/skinrepository.git";
    repoFolder = "skinrepositories/";
}

cSkinRepos::~cSkinRepos() {
    for (vector<cSkinRepo*>::iterator it = repos.begin(); it != repos.end(); it++) {
        delete (*it);
    }
}

void cSkinRepos::Init(string path) {
    string repoPath = path + repoFolder;
    if (FolderExists(repoPath)) {
        PullRepoGit(repoPath);
    } else {
        InitRepoGit(repoPath);
    }
}

void cSkinRepos::Read(string path) {
    string repoPath = path + repoFolder;
    DIR *folder = NULL;
    struct dirent *dirEntry;
    folder = opendir(repoPath.c_str());
    if (!folder) {
        esyslog("skindesigner: no skinrepository folder available in %s", repoPath.c_str());
        return;
    }
    while (dirEntry = readdir(folder)) {
        string fileName = dirEntry->d_name;
        if (!fileName.compare(".") || !fileName.compare("..") || !fileName.compare(".git"))
            continue;
        string filePath = repoPath + fileName;
        if (! ReadXMLFile(filePath.c_str(), false) ) {
            esyslog("skindesigner: error reading skinrepo %s", filePath.c_str());
            continue;
        }
        if (! SetDocument() )
            continue;
        if (!ParseRepository())
            esyslog("skindesigner: error parsing skinrepository %s", filePath.c_str());
        DeleteDocument();
    }

}

cSkinRepo *cSkinRepos::GetRepo(string name) {
    for (vector<cSkinRepo*>::iterator it = repos.begin(); it != repos.end(); it++) {
        cSkinRepo *repo = (*it);
        if (!name.compare(repo->Name()))
            return repo;
    }
    return NULL;
}

cSkinRepo *cSkinRepos::GetNextRepo(void) {
    if (repoIt == repos.end())
        return NULL;
    cSkinRepo *repo = *repoIt;
    repoIt++;
    return repo;
}


void cSkinRepos::Debug(void) {
    for (vector<cSkinRepo*>::iterator it = repos.begin(); it != repos.end(); it++) {
        (*it)->Debug();
    }
}

bool cSkinRepos::ParseRepository(void) {
    if (!LevelDown())
        return false;
    
    cSkinRepo *repo = new cSkinRepo();
    
    do {
        string value = "";
        if (CheckNodeName("name")) {
            if (GetNodeValue(value)) {
                repo->SetName(value);
            }
        } else if (CheckNodeName("type")) {
            if (GetNodeValue(value)) {
                eRepoType repoType = rtUndefined;
                if (!value.compare("git"))
                    repoType = rtGit;
                else if (!value.compare("zip"))
                    repoType = rtZipUrl;
                repo->SetRepoType(repoType);
            }
        } else if (CheckNodeName("url")) {
            if (GetNodeValue(value)) {
                repo->SetUrl(value);
            }
        } else if (CheckNodeName("author")) {
            if (GetNodeValue(value)) {
                repo->SetAuthor(value);
            }
        } else if (CheckNodeName("minimumskindesignerversion")) {
            if (GetNodeValue(value)) {
                repo->SetMinSDVersion(value);
            }
        } else if (CheckNodeName("specialfonts")) {
            if (!LevelDown())
                continue;
            do {
                if (CheckNodeName("font")) {
                    if (GetNodeValue(value)) {
                        repo->SetSpecialFont(value);
                    }
                }
            } while (NextNode());
            LevelUp();
        } else if (CheckNodeName("supportedplugins")) {
            if (!LevelDown())
                continue;
            do {
                if (CheckNodeName("plugin")) {
                    if (GetNodeValue(value)) {
                        repo->SetSupportedPlugin(value);
                    }
                }
            } while (NextNode());
            LevelUp();
        } else if (CheckNodeName("screenshots")) {
            if (!LevelDown()) {
                continue;
            }
            do {
                if (CheckNodeName("screenshot")) {
                    if (!LevelDown()) {
                        continue;
                    }
                    string desc = "";
                    string url = "";
                    do {
                        if (CheckNodeName("description")) {
                            GetNodeValue(desc);
                        } else if (CheckNodeName("url")) {
                            GetNodeValue(url);
                        }
                    } while (NextNode());
                    LevelUp();
                    if (desc.size() && url.size())
                        repo->SetScreenshot(desc, url);
                }
            } while (NextNode());
            LevelUp();
        }
    } while (NextNode());
    LevelUp();
    if (repo->Valid()) {
        repos.push_back(repo);
        return true;
    }
    return false;
}

void cSkinRepos::InitRepoGit(string path) {
    dsyslog("skindesigner: initiating skin repository %s", path.c_str());
    CreateFolder(path);

    cString command = cString::sprintf("git clone --depth=1 %s %s", skinRepoUrl.c_str(), path.c_str());
    int result = system (*command);
    
    if (result == 0) {
        dsyslog("skindesigner: skinrepository successfully initiated");
    } else {
        esyslog("skindesigner: ERROR initiating skinrepository. Command: %s", *command);
    }
}

void cSkinRepos::PullRepoGit(string path) {
    dsyslog("skindesigner: updating skin repository %s", path.c_str());
    cString command = *cString::sprintf("cd %s; git pull", path.c_str());
    int result = system (*command);
    if (result == 0) {
        dsyslog("skindesigner: skinrepository successfully updated");
    } else {
        esyslog("skindesigner: ERROR updating skinrepository. Command: %s", *command);
    }
}
