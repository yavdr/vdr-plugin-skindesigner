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
#endif //__SKINDESIGNERSERVICES_H