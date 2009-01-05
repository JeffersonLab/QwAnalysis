
/*! \file QwTracking.cc
    \brief This is the main program which simply executes a series of commands
 */

#include "treedo.h"
#include "mathstd.h"
#include "mathdefs.h"
#include "Qset.h"
#include "Qevent.h"
#include "Qoptions.h"
#include <iostream>
using namespace std;

#define NDetMax 1010
#define NEventMax 10

//Temporary global variables for sub-programs
bool bWriteGlobal = false;
int iEvent = 0;
TreeLine  *trelin;
int trelinanz;
Det *rcDETRegion[2][3][4];
treeregion *rcTreeRegion[2][3][4][4];
Det rcDET[NDetMax];
Options opt;


int main (int argc, char* argv[])
{
        Qset qset;
        qset.FillDetec("qweak.geo");
        cout << "QwTracking: Geometry loaded" << endl; // R3,R2

        Qoptions qoptions;
        qoptions.Get("qweak.options");
        cout << "QwTracking: Options loaded" << endl; // R3,R2

        tree thetree;
        cout << "QwTracking: Initializing pattern database" << endl;
        thetree.rcInitTree();
        cout << "QwTracking: Pattern database loaded" << endl; // R3,R2


/// Event loop goes here

        Qevent qevent;
        qevent.Open("qweak.event");
        cout << "QwTracking: Sample events loaded" << endl;

        treedo Treedo; // R3 needs debugging in the 3-D fit

        // RANT (wdconinc) what is this here all about?  A single event should be an
        // object, not a whole event file (that should be another object that returns
        // an event object with some getEvent method).  Then you pass this event object
        // to the tracking routine (which probably would be a method of the event class)
        // This rcTreeDo terminology is anachronistic and a literal translation from the
        // Hermes fortran code.

        // The event loop should end when iEvent is unphysical,
        // or: GetEvent returns bool and GetEventNumber returns int
        while ((iEvent = qevent.GetEvent()) < NEventMax) {
                cout << "QwTracking: iEvent = " << iEvent << endl;
                Treedo.rcTreeDo(iEvent);
        }
        cout << "Good :" << Treedo.ngood << endl;
        cout << "Bad  :" << Treedo.nbad  << endl;

        return 0;
}
