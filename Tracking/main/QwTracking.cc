#include "treedo.h"
#include "mathstd.h"
#include "mathdefs.h"
#include "Qset.h"
#include "Qevent.h"
#include "Qoptions.h"
#include <iostream>
using namespace std;

#define NDetMax 1010
//Temporary global variables for sub-programs
bool bWriteGlobal = false;
int iEvent = 0;
TreeLine  *trelin;
int trelinanz;
Det *rcDETRegion[2][3][4];
treeregion *rcTreeRegion[2][3][4][4];
Det rcDET[NDetMax];
Options opt;


int main(void){

	Qset qset;
	qset.FillDetec("qweak.geo");
	cerr << "Detector Geometry Loaded" << endl;

	Qoptions qoptions;
	qoptions.Get("qweak.options");
	cerr << "Options Loaded" << endl;

	tree thetree;
	thetree.rcInitTree();
	cerr << "Track Pattern Database Loaded" << endl;


 
	Qevent qevent;
	qevent.FillHits("qweak.event");
	cerr << "Event Loaded" << endl;
	


	treedo Treedo;
	Treedo.rcTreeDo(iEvent);
	

	return 0;
}

/*! \file main.cc -------------------------
\brief
This is the main program which simply executes a series of commands
*/
