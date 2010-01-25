/**
 * \file   QwTrackingTreeLine.h
 * \brief  Definition of the one-dimensional track stubs
 *
 * \author Wouter Deconinck <wdconinc@mit.edu>
 * \author Jie Pan <jpan@jlab.org>
 * \date   Sun May 24 11:05:29 CDT 2009
 */

#ifndef QWTRACKINGTREELINE_H
#define QWTRACKINGTREELINE_H

// System headers
#include <vector>

// ROOT headers
#include <TObject.h>
#include <TClonesArray.h>
#include <TObjArray.h>
#include <TRefArray.h>

// Qweak headers
#include "VQwTrackingElement.h"
#include "QwTypes.h"
#include "QwHit.h"
#include "globals.h"

// Maximum number of detectors combined for left-right ambiguity resolution
#define TREELINE_MAX_NUM_LAYERS 8

// Forward declarations
class QwHit;
class QwHitPattern;
class QwHitContainer;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/**
 *  \class QwTrackingTreeLine
 *  \ingroup QwTracking
 *  \brief One-dimensional (u, v, or x) track stubs and associated hits
 *
 * The QwTrackingTreeLine has a pointer to a set of hits.  It is passed to
 * various track fitting procedures and carries around the fit results.
 *
 * \todo This class needs a non-trivial copy constructor which ensures
 * that the hits are copied correctly.
 */
class QwTrackingTreeLine: public VQwTrackingElement {

  private:

    // Hits
    //#define QWTREELINE_MAX_NUM_HITS 1000
    //TClonesArray        *fQwHits; ///< Array of QwHits
    //static TClonesArray *gQwHits; ///< Static array of QwHits

    //! Number of hits in this tree line
    Int_t fNQwHits;
    //! List of hits in this tree line
    std::vector < QwHit* > fQwHits;

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

    //! \name Creating, adding, and getting hits and hit containers
    // @{
    //! \brief Create a new empty hit
    QwHit* CreateNewHit();
    //! \brief Add an existing hit
    void AddHit(QwHit* hit);
    //! \brief Add an existing hit container
    void AddHitContainer(QwHitContainer* hitlist);
    //! \brief Get the number of hits
    Int_t GetNumberOfHits() const;
    //! \brief Get a specific hit
    QwHit* GetHit(int i = 0);
    //! \brief Get the hits as a hit container
    QwHitContainer* GetHitContainer();
    //! \brief Clear the list of hits
    void ClearHits(Option_t *option = "");
    //! \brief Reset the list of hits
    void ResetHits(Option_t *option = "");
    // @}

    //! \brief Get the weighted chi^2
    double GetChiWeight ();

    //! \brief Get the hit with the smallest drift distance
    QwHit* GetBestWireHit (double offset = 0.0);

    void Print();
    void PrintValid();

    //! \brief Output stream operator
    friend ostream& operator<< (ostream& stream, const QwTrackingTreeLine& tl);

    //! \name Positions and resolutions in wire planes
    // @{
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
      return (a_end - a_beg) * binwidth;
    };
    //! Returns resolution at the last detector plane
    double GetResolutionLast (double binwidth) {
      return (b_end - b_beg) * binwidth;
    };
    // @}

    //! \name Calculate the residuals
    // @{
    //! Get the average residuals
    const double GetAverageResidual() const { return fAverageResidual; };
    //! Set the average residuals
    void SetAverageResidual(const double residual) { fAverageResidual = residual; };
    //! Calculate the average residuals
    const double CalculateAverageResidual();
    //! Calculate and set the average residuals
    void SetAverageResidual() { fAverageResidual = CalculateAverageResidual(); };
    // @}

    //! Set the offset
    void SetOffset(const double offset) { fOffset = offset; };
    //! Get the offset
    const double GetOffset() const { return fOffset; };
    //! Set the slope
    void SetSlope(const double slope) { fSlope = slope; };
    //! Get the slope
    const double GetSlope() const { return fSlope; };
    //! Set the chi^2
    void SetChi(const double chi) { fChi = chi; };
    //! Get the chi^2
    const double GetChi() const { return fChi; };
    //! Set the covariance
    void SetCov(const double* cov) { fCov[0] = cov[0]; fCov[1] = cov[1]; fCov[2] = cov[2]; };
    //! Get the covariance
    const double* GetCov() const { return fCov; };

  private:

    bool fIsVoid;			///< has been found void
    bool fIsUsed;			///< has been used (part of parttrack)

  public:

    QwHitPattern* fMatchingPattern; //!	///< matching hit pattern

    double fOffset;			///< track offset
    double fSlope;			///< track slope
    double fChi;			///< chi squared(?)
    double fCov[3];			///< covariance matrix of offset and slope

    int a_beg, a_end;			///< bin in first layer
    int b_beg, b_end;			///< bin in last layer

    int   fNumHits;			///< number of hits on this treeline
    int   fNumMiss;			///< number of planes without hits

    QwHit* hits[2*TLAYERS];	//!	///< all hits that satisfy road requirement
    QwHit* usedhits[TLAYERS];	//!	///< hits that correspond to optimal chi^2

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
