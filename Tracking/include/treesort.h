#ifndef TREESORT_H
#define TREESORT_H

// Standard C and C++ headers
#include <iostream>
#include "tree.h"
#include "treesearch.h"

/*!
 \brief This module is used to identify good track segments versus ghost tracks/hits.

*/
///
/// \ingroup QwTrackingAnl
class treesort {

  public:

    treesort();
    ~treesort();

    double doubletrack;
    int good;

    int rcTreeConnSort (QwTrackingTreeLine *head, EQwRegionID region);
    double chiweight(QwTrackingTreeLine *tl );
    int connectiv( char *ca, int *array, int *isvoid, char size, int idx );
    int rcCommonWires(QwTrackingTreeLine *line1, QwTrackingTreeLine *line2 );
    int rcCommonWires_r3( QwTrackingTreeLine *line1, QwTrackingTreeLine *line2 );
    int connectarray( char *ca, int *array, int *isvoid, char size, int idx );
    void bestunconnected( char *ca, int *array, int *isvoid, double *chia,
		 int size, int idx);
    int bestconnected( char *ca, int *array, int *isvoid, double *chia,
	       int size, int idx);
    int globalconnectiv( char *ca, int *array, int *isvoid, int size, int idx);



    int rcPartConnSort (PartTrack *head);
    double ptchiweight (PartTrack *pt);
    int rcPTCommonWires (PartTrack *track1, PartTrack *track2);

  private:


};


#endif // TREESORT_H
