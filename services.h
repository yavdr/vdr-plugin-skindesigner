#ifndef __SKINDESIGNERSERVICES_H
#define __SKINDESIGNERSERVICES_H

using namespace std;

#include <string>
#include <map>

/*********************************************************************
* Data Structures for Service Calls
*********************************************************************/

// Data structure for service "RegisterPlugin"
class RegisterPlugin {
public:
    RegisterPlugin(void) {
        name = "";
    };
    void SetMenu(int key, string templateName) {
        menus.insert(pair<int, string>(key, templateName));
    }
    void SetView(int key, string templateName) {
		views.insert(pair<int, string>(key, templateName));    	
    }
    void SetViewElement(int view, int viewElement, string name) {
        map< int, map<int, string> >::iterator hit = viewElements.find(view);
        if (hit == viewElements.end()) {
        	map<int, string> vE;
        	vE.insert(pair<int, string >(viewElement, name));
        	viewElements.insert(pair<int, map < int, string > >(view, vE));
        } else {
        	(hit->second).insert(pair<int, string >(viewElement, name));
        }
    }
    void SetViewGrid(int view, int viewGrid, string name) {
        map< int, map<int, string> >::iterator hit = viewGrids.find(view);
        if (hit == viewGrids.end()) {
            map<int, string> vG;
            vG.insert(pair<int, string >(viewGrid, name));
            viewGrids.insert(pair<int, map < int, string > >(view, vG));
        } else {
            (hit->second).insert(pair<int, string >(viewGrid, name));
        }
    }
// in
    string name;                          //name of plugin
    map< int, string > menus;             //menus as key -> templatename hashmap 
    map< int, string>  views;             //standalone views as key -> templatename hashmap 
    map< int, map <int, string> > viewElements;  //viewelements as key -> (viewelement, viewelementname) hashmap 
    map< int, map <int, string> > viewGrids;     //viewgrids as key -> (viewgrid, viewgridname) hashmap
//out
};

// Data structure for service "GetDisplayMenu"
class GetDisplayMenu {
public:
    GetDisplayMenu(void) {
        displayMenu = NULL;
    };
// in
//out	
    cSDDisplayMenu *displayMenu;
};

// Data structure for service "GetDisplayPlugin"
class GetDisplayPlugin {
public:
    GetDisplayPlugin(void) {
        pluginName = "";
        viewID = -1;
        displayPlugin = NULL;
    };
// in
    string pluginName;
    int viewID;
//out
    cSkinDisplayPlugin *displayPlugin;
};
#endif //__SKINDESIGNERSERVICES_H
