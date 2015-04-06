#include "viewgrid.h"

using namespace std;

cViewGrid::cViewGrid(cTemplateViewGrid *tmplGrid) {
    this->tmplGrid = tmplGrid;
}

cViewGrid::~cViewGrid() {
    Clear();
}

void cViewGrid::SetGrid(long gridID, 
                        double x, double y, double width, double height, 
                        map<string,int> *intTokens, map<string,string> *stringTokens) {
    map < long, cGrid* >::iterator hit = grids.find(gridID);
    cGrid *grid;
    if (hit == grids.end()) {
        grid = new cGrid(tmplGrid);
        grid->Set(x, y, width, height, intTokens, stringTokens);
        grids.insert(pair<long,cGrid*>(gridID, grid));
    } else {
        (hit->second)->Set(x, y, width, height, intTokens, stringTokens);
    }
}

void cViewGrid::SetCurrent(long gridID, bool current) {
    map<long,cGrid*>::iterator hit = grids.find(gridID);
    if (hit != grids.end())
        (hit->second)->SetCurrent(current);
}

void cViewGrid::Delete(long gridID) {
    map<long,cGrid*>::iterator hit = grids.find(gridID);
    if (hit == grids.end())
        return;
    delete (hit->second);
    grids.erase(gridID);
}

void cViewGrid::Clear(void) {
    for (map < long, cGrid* >::iterator it = grids.begin(); it != grids.end(); it++)
        delete it->second;
    grids.clear();
}

void cViewGrid::Render(void) {
    for (map < long, cGrid* >::iterator it = grids.begin(); it != grids.end(); it++) {
        cGrid *grid = it->second;
        if (grid->Dirty()) {
            if (grid->Moved()) {
                grid->Move();
            }
            grid->Clear();
            //esyslog("skindesigner: rendering grid %ld", it->first);
            grid->Draw();
        } else if (grid->Resized()) {
            //esyslog("skindesigner: resizing grid %ld", it->first);
            grid->DeletePixmaps();
            grid->Draw();
        } else if (grid->Moved()) {
            //esyslog("skindesigner: moving grid %ld", it->first);
            grid->Move();
        } else {
            //esyslog("skindesigner: skipping grid %ld", it->first);
        }
    }
}

void cViewGrid::Hide(void) {
    for (map < long, cGrid* >::iterator it = grids.begin(); it != grids.end(); it++) {
        cGrid *grid = it->second;
        grid->HidePixmaps();
    }
}

void cViewGrid::Show(void) {
    for (map < long, cGrid* >::iterator it = grids.begin(); it != grids.end(); it++) {
        cGrid *grid = it->second;
        grid->ShowPixmaps();
    }
}


void cViewGrid::Debug(void) {
    
}