#include "skinrepo.h"
#include "../libcore/helpers.h"

using namespace std;

// --- cSkinRepo -------------------------------------------------------------

cSkinRepo::cSkinRepo(void) {
    name = "";
    repoType = rtUndefined;
    url = "";
    command = "";
    tempfile = "";
    result = -1;
}

cSkinRepo::~cSkinRepo() {
}

void cSkinRepo::Install(string path) {
    if (Running())
        return;
    if (repoType == rtGit) {

        command = *cString::sprintf("git clone --progress %s %s%s", url.c_str(), path.c_str(), name.c_str());
        tempfile = *cString::sprintf("gitclone_%s_%ld.out", name.c_str(), time(0));

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
    dsyslog("skindesigner: executing %s", command.c_str());
    result = system (command.c_str());
    dsyslog("skindesigner: execution done, result: %d", result);
}

void cSkinRepo::Debug() {
    string strRepoType = "Undefined";
    if (repoType == rtGit)
        strRepoType = "Git";
    else if (repoType == rtZipUrl)
        strRepoType = "ZipUrl";
    dsyslog("skindesigner: --- skinrepo %s, Type %s ---", name.c_str(), strRepoType.c_str());
    dsyslog("skindesigner: url %s", url.c_str());
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
    esyslog("skindesigner: reading skinrepos from %s", filepath.c_str());
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

