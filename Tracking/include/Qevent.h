#ifndef QEVENT_H
#define QEVENT_H

/*!\brief Qevent is a generic data read-in class.

This class is not meant to be permanent.  I fully
expect to replace the method by which event data
is imported into this program.
*/

#include <cstdio>

#include "QwParameterFile.h"
#include "QwHitContainer.h"


#include "Hit.h"


#include "QwHitContainer.h"


class Qevent{

  public:

    Qevent();

    int GetEvent();
    int Open(const char *eventfile);


    Int_t ProcessHitContainer(QwHitContainer &);//this will turn HitContainer list into a rcDETRegion link-list 
    void UpdateHitVector(int region,int upplow, int detectId, int dir, int wire, double drift_dist, double res, double Zpos );//update QwHitContainer  with single hit
    void GetHitList(QwHitContainer & HitList){//return the QwHitContainer updated with hits form the processed event
	HitList.Append(fASCIIHits);
    };

  private:

    FILE *events;	//!- event stream
    Hit *hitlist;	//!- hit list

    int debug;		//!- debug level

    int ievent;		//!- event currently being read in
    int nevent;		//!- total number of events processed
    int thisevent;      // Stores the current event number


    QwHit * currentHit; 
    std::vector< QwHit > fASCIIHits;

};


#endif // QEVENT_H
