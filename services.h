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
// in
	string name;				 //name of plugin
	map< int, string > menus;    //menus as key -> templatename hashmap 
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

// Data structure for service "RegisterStandalonePlugin"
/*
class RegisterStandalonePlugin {
public:
	RegisterStandalonePlugin(void) {
		name = "";
		rootView = "";
	};
	void SetMenu(int key, string templateName) {
		menus.insert(pair<int, string>(key, templateName));
	}
// in
	string name;				 //name of plugin
	string rootView;		     //name of plugin
	map< int, string > menus;    //menus as key -> templatename hashmap 
//out
};
*/
// Data structure for service "GetDisplayPlugin"
class GetDisplayPlugin {
public:
	GetDisplayPlugin(void) {
		displayPlugin = NULL;
	};
// in
//out
	cDisplayPlugin *displayPlugin;
};
#endif //__SKINDESIGNERSERVICES_H