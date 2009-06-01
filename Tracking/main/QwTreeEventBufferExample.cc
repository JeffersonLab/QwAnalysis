/*------------------------------------------------------------------------*//*!

 \file QwTreeEventBufferExample.cc \ingroup QwTrackingAnl

 \brief Example of the QwTreeEventBuffer class to read QweakSimG4 events

 This example illustrates the use of the QwTreeEventBuffer class.  It loads
 the file Tracking/prminput/QweakSim.root, which was produced with a 3-fold
 trigger in QweakSimG4 (hits in all of HDC, VDC-front, and VDC-back).  For
 every event a QwHitContainer is filled, which is then printed to std::cout.

*//*-------------------------------------------------------------------------*/

// Standard C and C++ headers
#include <iostream>
#include <cstdlib>

// Qweak Tracking headers
#include "Det.h"
#include "Qset.h"
#include "Qevent.h"
#include "Qoptions.h"
#include "options.h"
#include "tracking.h"
#include "treeregion.h"

// Qweak Tree event buffer header
#include "QwTreeEventBuffer.h"

//Temporary global variables for sub-programs
Det *rcDETRegion[kNumPackages][kNumRegions][kNumDirections];
treeregion *rcTreeRegion[kNumPackages][kNumRegions][kNumTypes][kNumDirections];
Det rcDET[NDetMax];
Options opt;


int main (int argc, char* argv[])
{
  QwTreeEventBuffer treebuffer(std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/QweakSim.root");
  treebuffer.SetDebugLevel (0);

  while (treebuffer.GetNextEvent() > 0) {
    // Get event number
    std::cout << "Event number: " << treebuffer.GetEventNumber() << std::endl;

    // Get hit list
    QwHitContainer* hitlist = treebuffer.GetHitList();

    // Print hit list
    QwHitContainer::iterator qwhit;
    for (qwhit  = hitlist->begin();
         qwhit != hitlist->end(); qwhit++) {
      std::cout << "0ud"[qwhit->GetDetectorID().fPackage] << " "
        << "r" << qwhit->GetDetectorID().fRegion << " "
        << "0xyuvrq"[qwhit->GetDetectorID().fDirection] << " "
        << qwhit->GetDetectorID().fPlane << " "
        << "w:" << qwhit->GetDetectorID().fElement << " "
        << std::endl;
    }
  }

  return 0;
}
