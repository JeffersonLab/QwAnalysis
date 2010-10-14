#ifndef TOY6DEVENT_H
#define TOY6DEVENT_
/*********************************************************************
 * $Id: $
 * \author Jan Balewski, MIT, 2010
 *********************************************************************
 * Descripion:
 * C-struct encapsulates  synthetic QW channel
 * Utility class CorrelatorEntity  dealing with I/O of list of iv & dv
 *********************************************************************/


enum {kheliP=0, kheliN=1, mxHeli=2};
// for now assume 6 iv and 4 dv, only 2 helicity states

typedef struct {
  Double_t block[2];// not used
  Double_t hw_sum;
} ToyQwChannel;  


#endif

