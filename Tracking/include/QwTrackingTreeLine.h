/**
 * \class	QwTrackingTreeLine	QwTrackingTreeLine.h
 *
 * \brief	A container for track information
 *
 * \author	Wouter Deconinck <wdconinc@mit.edu>
 * \author	Jie Pan <jpan@jlab.org>
 * \date	Sun May 24 11:05:29 CDT 2009
 * \ingroup	QwTracking
 *
 * The QwTrackingTreeLine has a pointer to a set of hits.  It is passed to
 * various track fitting procedures and carries around the fit results.
 *
 */

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef QWTRACKINGTREELINE_H
#define QWTRACKINGTREELINE_H

#include <vector>

// ROOT headers
#include "TObject.h"
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TRefArray.h"

// Qweak headers
#include "QwTypes.h"
#include "globals.h"

// Maximum number of detectors combined for left-right ambiguity resolution
#define TREELINE_MAX_NUM_LAYERS 8


#include "QwHit.h"


// Forward declarations
class QwHit;
class QwHitContainer;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

///
/// \ingroup QwTracking

class QwTrackingTreeLine: public TObject {

  private:

    // Hits
    #define QWTREELINE_MAX_NUM_HITS 1000
    Int_t fNQwHits; ///< Number of QwHits in the array
    TClonesArray        *fQwHits; ///< Array of QwHits
    static TClonesArray *gQwHits; ///< Static array of QwHits

    //TRefArray *fQwHits2;

    //std::vector<QwHit> fQwHits3;

  public:

    QwTrackingTreeLine(int _a_beg = 0, int _a_end = 0 , int _b_beg = 0, int _b_end = 0);
    virtual ~QwTrackingTreeLine();

    //! Is this tree line void?
    const bool IsVoid() const { return fIsVoid; };
    void SetVoid(const bool isvoid = true) { fIsVoid = isvoid; };
    //! Is this tree line valid?
    const bool IsValid() const { return ! fIsVoid; };
    void SetValid(const bool isvoid = false) { fIsVoid = isvoid; };
    //! Is this tree line not void?
    const bool IsNotVoid() const { return ! fIsVoid; };
    void SetNotVoid(const bool isvoid = false) { fIsVoid = isvoid; };
    //! Is this tree line used?
    const bool IsUsed() const { return fIsUsed; };
    void SetUsed(const bool isused = true) { fIsUsed = isused; };
    //! Is this tree line not used?
    const bool IsNotUsed() const { return ! fIsUsed; };
    void SetNotUsed(const bool isused = false) { fIsUsed = isused; };

    // Housekeeping methods
    void ClearHits(Option_t *option = "");
    void ResetHits(Option_t *option = "");

    // Creating, adding, and getting hits and hit containers
    QwHit* CreateNewHit();
    void AddHit(QwHit* hit);
    void AddHitContainer(QwHitContainer* hitlist);
    QwHitContainer* GetHitContainer();
    // Get the number of hits
    Int_t GetNumberOfHits() const { return fNQwHits; };

    //! Returns the weighted chi^2
    double GetChiWeight ();

    //! Returns the hit with the best
    QwHit* bestWireHit (double offset = 0.0);

    void Print();
    void PrintValid();

    friend ostream& operator<< (ostream& stream, const QwTrackingTreeLine& tl);

    //! Returns position at the first detector plane
    double GetPositionFirst (double binwidth) {
      return 0.5 * (a_beg + a_end) * binwidth;
    };

    //! Returns position at the last detector plane
    double GetPositionLast (double binwidth) {
      return 0.5 * (b_beg + b_end) * binwidth;
    };

    //! Returns resolution at the first detector plane
    double GetResolutionFirst (double binwidth) {
      return (a_beg - a_end) * binwidth;
    };

    //! Returns resolution at the last detector plane
    double GetResolutionLast (double binwidth) {
      return (b_beg - b_end) * binwidth;
    };

    //! Get the average residuals
    const double GetAverageResidual() const { return fAverageResidual; };
    //! Set the average residuals
    void SetAverageResidual(const double residual) { fAverageResidual = residual; };
    //! Calculate the average residuals
    const double CalculateAverageResidual();
    //! Calculate and set the average residuals
    void SetAverageResidual() { fAverageResidual = CalculateAverageResidual(); };

    //! \brief Get the detector info pointer
    QwDetectorInfo* GetDetectorInfo () const { return pDetectorInfo; };
    //! \brief Set the detector info pointer
    void SetDetectorInfo(QwDetectorInfo *detectorinfo) { pDetectorInfo = detectorinfo; };

  private:

    QwDetectorInfo* pDetectorInfo; //!	///< Pointer to the detector info object (not saved)

    EQwRegionID fRegion;		///< Region
    EQwDetectorPackage fPackage;	///< Package
    EQwDirectionID fDirection;		///< Direction
    Int_t fPlane;			///< Detector plane

    bool fIsVoid;			///< has been found void
    bool fIsUsed;			///< has been used (part of parttrack)

  public:

    double fOffset, fSlope;		///< track offset and slope
    double fChi;			///< chi squared(?)
    double fCov[3];			///< covariance matrix of offset and slope

    int a_beg, a_end;			///< bin in first layer
    int b_beg, b_end;			///< bin in last layer

    int   numhits;			///< number of hits on this treeline
    int   nummiss;			///< number of planes without hits

    QwHit *hits[2*TLAYERS];	//!	///< all hits that satisfy road requirement
    QwHit *usedhits[TLAYERS];	//!	///< hits that correspond to optimal chi^2

    int   hasharray[2*TLAYERS];	//!
    int   ID;				///< adamo ID

    int r3offset;			///< offset of demultiplexed group of 8
    int firstwire, lastwire;		///< first and last wire in group of 8

    QwTrackingTreeLine *next;	//!	///< link to next list element

  private:

    double fAverageResidual;	/// average residual over all used hits

  ClassDef(QwTrackingTreeLine,1);

}; // class QwTrackingTreeLine

#endif // QWTRACKINGTREELINE_H
