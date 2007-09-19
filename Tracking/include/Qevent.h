#include "tree.h"


class Qevent{
public:
Qevent();
int FillHits(char *eventfile);
int numevents;
FILE *events;
Hit *hitlist;


private:

};
