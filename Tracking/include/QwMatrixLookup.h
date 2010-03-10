/*! \file   QwMatrixLookup.h
 *
 *  \author Wouter Deconinck <wdconinc@mit.edu>
 *
 *  \date   Mon Mar  1 11:48:48 EST 2010
 *  \brief  Definition of the matrix lookup bridging method
 *
 *  \ingroup QwTracking
 *
 */

// System headers
#include <vector>

// Qweak headers
#include "VQwBridgingMethod.h"
#include "QwTrajMatrix.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef __QWMATRIXLOOKUP_H__
#define __QWMATRIXLOOKUP_H__

class QwMatrixLookup: public VQwBridgingMethod {

  public:

    /// \brief Default constructor
    QwMatrixLookup() { };
    /// \brief Destructor
    virtual ~QwMatrixLookup() { };

    /// \brief Load the matrix lookup table from disk
    static bool LoadTrajMatrix(const std::string filename);

    /// \brief Bridge from the front to back partial track
    int Bridge(QwPartialTrack* front, QwPartialTrack* back);

    // TEMP
    void SetStartAndEndPoints(TVector3 startposition, TVector3 startdirection,
                              TVector3 endposition, TVector3 enddirection);

    // TEMP
    TVector3 fStartPosition, fStartDirection;
    double fStartPositionR, fStartPositionPhi;
    double fStartDirectionTheta, fStartDirectionPhi;
    //
    TVector3 fEndPosition, fEndDirection;
    double fEndPositionR, fEndPositionPhi;
    double fEndDirectionTheta, fEndDirectionPhi;

    // TEMP
    void GetBridgingResult(Double_t *buffer);

  private:

    /// Lookup table (static)
    static QwTrajMatrix* fMatrix;

    double fMomentum;  /// electron momentum

    TVector3 fHitLocation, fHitDirection;
    double fHitLocationR, fHitLocationPhi;
    double fHitDirectionTheta, fHitDirectionPhi;

    double fPositionROff;
    double fPositionPhiOff;
    double fDirectionThetaOff;
    double fDirectionPhiOff;

    double fPositionXOff;
    double fPositionYOff;

    double fDirectionXOff;
    double fDirectionYOff;
    double fDirectionZOff;

    Int_t fMatchFlag; // MatchFlag = -2 : cannot match
                      // MatchFlag = -1 : potential track cannot pass through the filter
                      // MatchFlag = 0; : matched with look-up table
                      // MatchFlag = 1; : matched by using shooting method
                      // MatchFlag = 2; : potential track is forced to match


}; // class QwMatrixLookup

#endif // __QWMATRIXLOOKUP_H__
