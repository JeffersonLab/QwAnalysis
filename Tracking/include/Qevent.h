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

    int debug;
    int numevents;
    int nevent;
    int newevent;
    int firstevent;
    FILE *events;
    Hit *hitlist;

  private:

};


#endif // QEVENT_H
