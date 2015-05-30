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
    command = "";
    command2 = "";
    tempfile = "";
    result = -1;
    skinPath = "";
    themesPath = "";
}

cSkinRepo::~cSkinRepo() {
}

void cSkinRepo::Install(string path, string themesPath) {
    if (Running())
        return;
    action = eaInstall;
    this->skinPath = path + name;
    this->themesPath = themesPath;
    if (repoType == rtGit) {

        command = *cString::sprintf("git clone --progress %s %s", url.c_str(), skinPath.c_str());
        tempfile = *cString::sprintf("gitclone_%s_%ld.out", name.c_str(), time(0));

        Start();

    } else if (repoType == rtZipUrl) {

        size_t hit = url.find_last_of('/');
        if (hit == string::npos)
            return;
        string filename = url.substr(hit+1);
        
        command = *cString::sprintf("wget -P /tmp/ %s", url.c_str());
        command2 = *cString::sprintf("unzip /tmp/%s -d %s", filename.c_str(), path.c_str());
        
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
    repoFile = "skinrepositories.xml";
    doc = NULL;
}

cSkinRepos::~cSkinRepos() {
    for (vector<cSkinRepo*>::iterator it = repos.begin(); it != repos.end(); it++) {
        delete (*it);
    }
}

void cSkinRepos::Read(string path) {
    string filepath = path + repoFile;
    xmlParserCtxtPtr ctxt = xmlNewParserCtxt();
    xmlNodePtr root = NULL;

    doc = xmlCtxtReadFile(ctxt, filepath.c_str(), NULL, XML_PARSE_NOENT);
    if (doc == NULL) {
        esyslog("skindesigner: ERROR: skinrepository file %s not loaded successfully.", filepath.c_str());
        return;
    }

    root = xmlDocGetRootElement(doc);
    if (root == NULL) {
        return;
    }

    if (xmlStrcmp(root->name, (const xmlChar *) "skinrepositories")) {
        return;
    }

    xmlNodePtr node = root->xmlChildrenNode;
    while (node != NULL) {
        if (node->type != XML_ELEMENT_NODE) {
            node = node->next;
            continue;
        }
        if (xmlStrcmp(node->name, (const xmlChar *) "skinrepo")) {
            continue;
        }
        ReadRepository(node->xmlChildrenNode);
        node = node->next;
    }

    if (doc) xmlFreeDoc(doc);
    xmlFreeParserCtxt(ctxt);
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

void cSkinRepos::ReadRepository(xmlNodePtr node) {
    if (!node)
        return;
    cSkinRepo *repo = new cSkinRepo();
    while (node != NULL) {
        if (node->type != XML_ELEMENT_NODE) {
            node = node->next;
            continue;
        }
        
        xmlChar *value = NULL;
        //Repo Name
        if (!xmlStrcmp(node->name, (const xmlChar *) "name")) {
            value = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
            if (value)
                repo->SetName((const char *)value);
        //Repo Type
        } else if (!xmlStrcmp(node->name, (const xmlChar *) "type")) {
            value = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
            if (value) {
                eRepoType repoType = rtUndefined;
                if (!xmlStrcmp(value, (const xmlChar *) "git"))
                    repoType = rtGit;
                else if (!xmlStrcmp(value, (const xmlChar *) "zip"))
                    repoType = rtZipUrl;
                repo->SetRepoType(repoType);
            }
        //Repo URL
        } else if (!xmlStrcmp(node->name, (const xmlChar *) "url")) {
            value = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
            if (value)
                repo->SetUrl((const char *)value);
        //Skin Author
        } else if (!xmlStrcmp(node->name, (const xmlChar *) "author")) {
            value = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
            if (value)
                repo->SetAuthor((const char *)value);
        //Repo Specialfonts
        } else if (!xmlStrcmp(node->name, (const xmlChar *) "specialfonts")) {
            xmlNodePtr child = node->xmlChildrenNode;
            while (child != NULL) {
                if (child->type != XML_ELEMENT_NODE) {
                    child = child->next;
                    continue;
                }
                if (!xmlStrcmp(child->name, (const xmlChar *) "font")) {
                    xmlChar *fontvalue = NULL;
                    fontvalue = xmlNodeListGetString(doc, child->xmlChildrenNode, 1);
                    if (fontvalue) {
                        repo->SetSpecialFont((const char *)fontvalue);
                        xmlFree(fontvalue);
                    }
                }
                child = child->next;
            }
        //Repo supported Plugins
        } else if (!xmlStrcmp(node->name, (const xmlChar *) "supportedplugins")) {
            xmlNodePtr child = node->xmlChildrenNode;
            while (child != NULL) {
                if (child->type != XML_ELEMENT_NODE) {
                    child = child->next;
                    continue;
                }
                if (!xmlStrcmp(child->name, (const xmlChar *) "plugin")) {
                    xmlChar *plugvalue = NULL;
                    plugvalue = xmlNodeListGetString(doc, child->xmlChildrenNode, 1);
                    if (plugvalue) {
                        repo->SetSupportedPlugin((const char *)plugvalue);
                        xmlFree(plugvalue);
                    }
                }
                child = child->next;
            }
        //Repo Screenshots
        } else if (!xmlStrcmp(node->name, (const xmlChar *) "screenshots")) {
            xmlNodePtr child = node->xmlChildrenNode;
            while (child != NULL) {
                if (child->type != XML_ELEMENT_NODE) {
                    child = child->next;
                    continue;
                }
                if (!xmlStrcmp(child->name, (const xmlChar *) "screenshot")) {
                    xmlNodePtr subchild = child->xmlChildrenNode;
                    string desc = "";
                    string url = "";
                    while (subchild != NULL) {
                        if (subchild->type != XML_ELEMENT_NODE) {
                            subchild = subchild->next;
                            continue;
                        }
                        xmlChar *screenshotvalue = NULL;
                        if (!xmlStrcmp(subchild->name, (const xmlChar *) "description")) {
                            screenshotvalue = xmlNodeListGetString(doc, subchild->xmlChildrenNode, 1);
                            if (screenshotvalue) {
                                desc = (const char *)screenshotvalue;
                                xmlFree(screenshotvalue);
                            }
                        } else if (!xmlStrcmp(subchild->name, (const xmlChar *) "url")) {
                            screenshotvalue = xmlNodeListGetString(doc, subchild->xmlChildrenNode, 1);
                            if (screenshotvalue) {
                                url = (const char *)screenshotvalue;
                                xmlFree(screenshotvalue);
                            }
                        }
                        subchild = subchild->next;
                    }
                    repo->SetScreenshot(desc, url);
                }
                child = child->next;
            }
        }
        if (value)
            xmlFree(value);
        node = node->next;

    }
    repos.push_back(repo);
}

