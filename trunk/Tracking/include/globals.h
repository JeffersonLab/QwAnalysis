#ifndef GLOBALS_H
#define GLOBALS_H

// Maximum number of detector layers of same dir in either the front or the back
#define DLAYERS 4

// Maximum number of detector layers in the tree search database
#define MAX_LAYERS 8

// Maximum number of hits in one detector plane which will be considered
// to belong to the track, if several hits are near the line
#define MAXHITPERLINE 4

// Number of wires in region 3
#define NUMWIRESR3 279
// TODO This is only used in QwTrackingWorker and should be taken from the
// geometry definition file.

#endif // GLOBALS_H
