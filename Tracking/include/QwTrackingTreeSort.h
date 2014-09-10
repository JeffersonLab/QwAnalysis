/**
 * \class	QwTrackingTreeSort	QwTrackingTreeSort.h
 *
 * \brief This module is used to identify good track segments versus ghost tracks/hits.
 *
 * \date Sun May 24 14:29:42 CDT 2009
 *
 */
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef QWTRACKINGTREESORT_H
#define QWTRACKINGTREESORT_H

// Standard C and C++ headers
#include <iostream>
#include "QwTrackingTree.h"
#include "QwPartialTrack.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
///
/// \ingroup QwTracking
class QwTrackingTreeSort {

  public:

    QwTrackingTreeSort();
    virtual ~QwTrackingTreeSort();

    void SetDebugLevel (const int debuglevel) { fDebug = debuglevel; };

    double doubletrack;
    int good;

    int rcTreeConnSort (QwTreeLine *head, EQwRegionID region);
    int connectiv( char *ca, int *array, int *isvoid, char size, int idx );
    int rcCommonWires(const QwTreeLine *line1, const QwTreeLine *line2 );
    int rcCommonWires_r3(const QwTreeLine *line1, const QwTreeLine *line2 );
    int connectarray( char *ca, int *array, int *isvoid, char size, int idx );
    void bestunconnected( char *ca, int *array, int *isvoid, double *chia,
		 int size, int idx);
    int bestconnected( char *ca, int *array, int *isvoid, double *chia,
	       int size, int idx);
    int globalconnectiv( char *ca, int *array, int *isvoid, int size, int idx);



    int rcPartConnSort (std::vector<QwPartialTrack*> head);
    int rcPTCommonWires (const QwPartialTrack *track1, const QwPartialTrack *track2);

  private:

    int fDebug;


}; // class QwTrackingTreeSort

#endif // QWTRACKINGTREESORT_H

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
