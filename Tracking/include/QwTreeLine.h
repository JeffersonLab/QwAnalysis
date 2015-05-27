/**
 * \file   QwTreeLine.h
 * \brief  Definition of the one-dimensional track stubs
 *
 * \author Wouter Deconinck <wdconinc@mit.edu>
 * \author Jie Pan <jpan@jlab.org>
 * \date   Sun May 24 11:05:29 CDT 2009
 */

#ifndef QWTREELINE_H
#define QWTREELINE_H

// System headers
#include <vector>
#include <utility>

// ROOT headers
#include <TClonesArray.h>
#include <TObjArray.h>
#include <TRefArray.h>

// Qweak headers
#include "VQwTrackingElement.h"
#include "QwTypes.h"
#include "QwObjectCounter.h"
#include "QwHit.h"
#include "globals.h"
#include "QwHitPattern.h"

// Maximum number of detectors combined for left-right ambiguity resolution
#define TREELINE_MAX_NUM_LAYERS 8

// Forward declarations
class QwHit;
// class QwHitPattern;
class QwHitContainer;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/**
 *  \class QwTreeLine
 *  \ingroup QwTracking
 *  \brief One-dimensional (u, v, or x) track stubs and associated hits
 *
 * The QwTreeLine has a pointer to a set of hits.  It is passed to
 * various track fitting procedures and carries around the fit results.
 *
 * \todo This class needs a non-trivial copy constructor which ensures
 * that the hits are copied correctly.
 */
class QwTreeLine: public VQwTrackingElement, public QwObjectCounter<QwTreeLine> {

  private:

    //! Number of hits in this tree line
    Int_t fNQwHits;
    //! List of hits in this tree line
    std::vector<QwHit*> fQwHits;

  public:


    /// Default constructor
    QwTreeLine();
    /// Constructor with tree search results
    QwTreeLine(int _a_beg, int _a_end , int _b_beg, int _b_end);
    /// Constructor with offset and slope
    QwTreeLine(double offset, double slope);
    /// Copy constructor
    QwTreeLine(const QwTreeLine& that);
    /// Copy constructor
    QwTreeLine(const QwTreeLine* that);
    /// Destructor
    virtual ~QwTreeLine();

    /// Assignment operator
    QwTreeLine& operator=(const QwTreeLine& treeline);

  private:

    /// Initialization
    void Initialize();

  public:

    //! Is this tree line void?
    bool IsVoid() const { return fIsVoid; }
    void SetVoid(const bool isvoid = true) { fIsVoid = isvoid; }
    //! Is this tree line valid?
    bool IsValid() const { return ! fIsVoid; }
    void SetValid(const bool isvoid = false) { fIsVoid = isvoid; }
    //! Is this tree line not void?
    bool IsNotVoid() const { return ! fIsVoid; }
    void SetNotVoid(const bool isvoid = false) { fIsVoid = isvoid; }
    //! Is this tree line used?
    bool IsUsed() const { return fIsUsed; }
    void SetUsed(const bool isused = true) { fIsUsed = isused; }
    //! Is this tree line not used?
    bool IsNotUsed() const { return ! fIsUsed; }
    void SetNotUsed(const bool isused = false) { fIsUsed = isused; }

    //! \name Creating, adding, and getting hits and hit containers
    // @{
    //! \brief Add a single hit
    void AddHit(const QwHit* hit);
    //! \brief Add a list of hits
    void AddHitList(const std::vector<QwHit*> &fQwHits);
    //! \brief Get the number of hits
    Int_t GetNumberOfHits() const { return fNQwHits; }
    //! \brief Get a specific hit
    const QwHit* GetHit(int i = 0) const;
    //! \brief Get the list of hits
    const std::vector<QwHit*>& GetListOfHits() const { return fQwHits; };
    //! \brief Clear the list of hits without deleting
    void ClearHits();
    //! \brief Delete the hits in the list
    void DeleteHits();
    // @}

    //! \brief Get the weighted chi^2
    double GetChiWeight ();

    //! \brief Get the hit with the smallest drift distance
    QwHit* GetBestWireHit (double offset = 0.0) const;

    void Print(const Option_t* options = 0) const;
    void PrintValid();

    //! \brief Output stream operator
    friend std::ostream& operator<< (std::ostream& stream, const QwTreeLine& tl);

    //! \name Positions and resolutions in wire planes
    // @{
    //! Returns position at the first detector plane
    // binwidth=0.2921 here, might be changed later
    double GetPositionFirst (const double binwidth) {
      //return 0.5 * (a_beg + a_end) * binwidth + 0.5*binwidth;
        return 0.5*(a_beg + a_end)*binwidth;
    }
    //! Returns position at the last detector plane
    double GetPositionLast (const double binwidth) {
      return 0.5*(b_beg+b_end)*binwidth;
      //return 0.5 * (b_beg + b_end) * binwidth + 0.5*binwidth;
      
    }
    //! Returns resolution at the first detector plane
    double GetResolutionFirst (const double binwidth) {
      return (a_end - a_beg) * binwidth;
    }
    //! Returns resolution at the last detector plane
    double GetResolutionLast (const double binwidth) {
      return (b_end - b_beg) * binwidth;
    }
    // @}

    //! \name Calculate the residuals
    // @{
    //! Get the average residuals
    double GetAverageResidual() const { return fAverageResidual; }
    //! Set the average residuals
    void SetAverageResidual(const double residual) { fAverageResidual = residual; }
    //! Calculate the average residuals
    double CalculateAverageResidual();
    //! Calculate and set the average residuals
    void SetAverageResidual() { fAverageResidual = CalculateAverageResidual(); }
    // @}

    //! Set the offset
    void SetOffset(const double offset) { fOffset = offset; }
    //! Get the offset
    double GetOffset() const { return fOffset; }
    //! Set the slope
    void SetSlope(const double slope) { fSlope = slope; }
    //! Get the slope
    double GetSlope() const { return fSlope; }
    //! Set the chi^2
    void SetChi(const double chi) { fChi = chi; }
    //! Get the chi^2
    double GetChi() const { return fChi; }
    //! Set the covariance
    void SetCov(const double* cov) { fCov[0] = cov[0]; fCov[1] = cov[1]; fCov[2] = cov[2]; }
    //! Get the covariance
    const double* GetCov() const { return fCov; }
    /// Set the matching pattern
    void SetMatchingPattern(std::vector<int>& box);
    /// calculate the upper and lower bound of the drift distance give the row number
    std::pair<double,double> CalculateDistance(int row,double width,unsigned int bins,double error);

    /// Get vector for linked list of treelines
    std::vector<QwTreeLine*> GetListAsVector() const {
      std::vector<QwTreeLine*> list;
      QwTreeLine* treeline = const_cast<QwTreeLine*>(this);
      while (treeline) {
        list.push_back(treeline);
        treeline = treeline->next;
      }
      return list;
    }

  private:

    bool fIsVoid;			///< has been found void
    bool fIsUsed;			///< has been used (part of parttrack)

  public:

//   QwHitPattern* fMatchingPattern; //!	///< matching hit pattern
    std::vector<int> fMatchingPattern;
    double fOffset;			///< track offset
    double fSlope;			///< track slope
    double fChi;			///< chi squared(?)
    double fCov[3];			///< covariance matrix of offset and slope

    int a_beg, a_end;			///< bin in first layer
    int b_beg, b_end;			///< bin in last layer

    int   fNumHits;			///< number of hits on this treeline
    int   fNumMiss;			///< number of planes without hits

    QwHit* fHits[2*MAX_LAYERS];	//!     ///< all hits that satisfy road requirement

    int fHashArray[2*MAX_LAYERS]; //!

    int fR3Offset;			///< offset of demultiplexed group of 8
    int fR3FirstWire, fR3LastWire;	///< first and last wire in group of 8

    QwTreeLine *next; //!               ///< link to next list element

  private:

    double fAverageResidual;	        ///< average residual over all used hits

  #if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
    ClassDef(QwTreeLine,1);
  #endif

}; // class QwTreeLine

typedef QwTreeLine QwTrackingTreeLine;

typedef VQwTrackingElementContainer<QwTreeLine> QwTreeLineContainer;

#endif // QWTREELINE_H
