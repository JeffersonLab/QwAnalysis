#ifndef _GLOBALS_C_
#define _GLOBALS_C_

static const Int_t numtypes = 5;
Int_t fitStatus[numtypes];
TF1 *fitpol1[numtypes];
static const Int_t colors[5] = {kGreen, kBlue, kRed, kViolet, kCyan};
char titles[numtypes][20] = {"laser on", "laser off", "beam off", "laser flash","beam ramp"};

#endif
