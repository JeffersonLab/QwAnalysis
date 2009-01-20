#ifndef QEVENT_H
#define QEVENT_H

/*!\brief Qevent is a generic data read-in class.

This class is not meant to be permanent.  I fully
expect to replace the method by which event data
is imported into this program.
*/

#include <cstdio>

#include "Hit.h"

class Qevent{

  public:

    Qevent();

    int GetEvent();
    int Open(const char *eventfile);

  private:

    FILE *events;	//!- event stream
    Hit *hitlist;	//!- hit list

    int debug;		//!- debug level

    int ievent;		//!- event currently being read in
    int nevent;		//!- total number of events processed

};


#endif // QEVENT_H
