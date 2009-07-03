//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//
// C++ Interface: QwTrackingTreeSort
//
// Description:
//
//
// Modified by: Wouter Deconinck <wdconinc@mit.edu>, (C) 2008
//
//              Jie Pan <jpan@jlab.org>, Sun May 24 14:29:42 CDT 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/*! \class QwTrackingTreeSort

    \file QwTrackingTreeSort.h

    $date: Sun May 24 14:29:42 CDT 2009 $

    \brief This module is used to identify good track segments versus ghost tracks/hits.
 */
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef QWTRACKINGTREESORT_H
#define QWTRACKINGTREESORT_H

// Standard C and C++ headers
#include <iostream>
#include "QwTrackingTree.h"
#include "QwTrackingTreeSearch.h"
#include "QwPartialTrack.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
///
/// \ingroup QwTrackingAnl
class QwTrackingTreeSort {

  public:

    QwTrackingTreeSort();
    ~QwTrackingTreeSort();

    void SetDebugLevel (int debuglevel) { debug = debuglevel; };

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



    int rcPartConnSort (QwPartialTrack *head);
    double ptchiweight (QwPartialTrack *pt);
    int rcPTCommonWires (QwPartialTrack *track1, QwPartialTrack *track2);

  private:

    int debug;


};//class QwTrackingTreeSort

#endif // QWTRACKINGTREESORT_H

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
