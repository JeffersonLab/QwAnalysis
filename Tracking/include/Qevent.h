#include "tree.h"

/*!\brief Qevent is a generic data read-in class.

This class is not meant to be permanent.  I fully
expect to replace the method by which event data
is imported into this program.
*/
class Qevent{
public:
Qevent();
int FillHits(char *eventfile);
int numevents;
FILE *events;
Hit *hitlist;


private:

};
