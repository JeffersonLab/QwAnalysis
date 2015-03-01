/**
 *  \file   QwHit.h
 *  \brief  Draft skeleton for the decoding-to-QTR interface class.
 *
 *  \author P. M. King
 *  \date   2008feb13
 */

#ifndef QWHIT_H
#define QWHIT_H

// System headers
#include <iostream>

// Qweak headers
#include "QwTypes.h"
#include "QwObjectCounter.h"
#include "VQwTrackingElement.h"

// Forward declarations
class QwDetectorInfo;

/**
 *  \class QwHit
 *  \ingroup QwTracking
 *
 *  \brief Hit structure uniquely defining each hit
 *
 *  This would either be used to generate the list of "Hit" elements
 *  used by QTR, or QTR (and this class) would be modified to use this
 *  as the base  class instead of the "Hit" class (which is in tree.h).
 *
 *  The decoding routines would create a vector of these objects which
 *  would contain all hits from all subsystems in each CODA event.
 *
 *  Subsystem classes (derived from the VQwSubsystem class) would need
 *  to have routines to process the raw TDC data into this form.
 *
 *  A rough example which ought to work for Region 2 exists as the
 *  QwDriftChamber class (although the decoding uses a previous algorithm
 *  and is in the process of being rewritten to support this form).
 */
class QwHit : public VQwTrackingElement, public QwObjectCounter<QwHit> {

 public:

  //! \name Constructors
  // (documented in QwHit.cc)
  // @{
  QwHit();
  QwHit(const QwHit& that);
  QwHit(const QwHit* that);
  QwHit(Int_t bank_index, Int_t slot_num, Int_t chan, Int_t hitcount,
        EQwRegionID region, EQwDetectorPackage package, Int_t octant,
        Int_t plane, EQwDirectionID direction, Int_t wire, UInt_t rawdata = 0);
  // @}

  //! \brief Destructor
  virtual ~QwHit();

 private:
  //! \brief Initialize the hit
  void Initialize();

 public:
  //! \brief Assignment operator
  QwHit& operator= (const QwHit& hit);
  //! \brief Ordering operator
  Bool_t operator< (const QwHit& hit);
  //! \brief Output stream operator
  friend std::ostream& operator<< (std::ostream& stream, const QwHit& hit);

  //! \brief Print debugging information
  void Print(const Option_t* options = 0) const;

  //! \name Getter functions
  // @{
  const Int_t&          GetSubbankID()     const { return fCrate; };
  const Int_t&          GetModule()        const { return fModule; };
  const Int_t&          GetChannel()       const { return fChannel; };
  const Int_t&          GetHitNumber()     const { return fHitNumber; };
  const Int_t&          GetHitNumberR()    const { return fHitNumber_R; };

  const Bool_t&         AmbiguousElement() const { return fAmbiguousElement; };
  const Bool_t&         LRAmbiguity()      const { return fLRAmbiguity; };

  const UInt_t&         GetRawTime()       const { return fRawTime; };
  const UInt_t&         GetRawRefTime()    const { return fRawRefTime; };
  const Double_t&       GetTime()          const { return fTime; };
  const Double_t&       GetTimeNs()        const { return fTimeNs; };
  const Double_t&       GetTimeRes()       const { return fTimeRes; };
  const Double_t&       GetDriftDistance() const { return fDistance; };
  const Double_t&       GetDriftPosition() const { return fDriftPosition; };
  const Double_t&       GetWirePosition()  const { return fWirePosition; };

  const Double_t&       GetTreeLinePosition() const { return fTreeLinePosition; };
  const Double_t&       GetTreeLineResidual() const { return fTreeLineResidual; };
  const Double_t&       GetPartialTrackPosition() const { return fPartialTrackPosition; };
  const Double_t&       GetPartialTrackResidual() const { return fPartialTrackResidual; };

  const Bool_t&         IsUsed()           const { return fIsUsed; };

  const QwDetectorID    GetDetectorID()    const;      // QwHit.cc
  const QwElectronicsID GetElectronicsID() const;      // QwHit.cc
  // @}


  //! \name Setter functions
  // @{
  void SetSubbankID(const Int_t bank_index)         { fCrate = bank_index; };
  void SetModule(const Int_t slot_num)              { fModule = slot_num; };
  void SetChannel(const Int_t chan)                 { fChannel = chan; };
  void SetHitNumber(const Int_t hitcount)           { fHitNumber = hitcount; };
  void SetHitNumberR(const Int_t hitcountr)         { fHitNumber_R = hitcountr; };

  void SetAmbiguousElement(const Bool_t amelement)  { fAmbiguousElement = amelement; };
  void SetLRAmbiguity(const Bool_t amlr)            { fLRAmbiguity = amlr; };

  void SetAmbiguityID(const Bool_t amelement, const Bool_t amlr);  // QwHit.cc

  void SetRawTime(const UInt_t rawtime)             { fRawTime = rawtime; };
  void SetRawRefTime(const UInt_t rawreftime)       { fRawRefTime = rawreftime; };
  void SetTimens(const Double_t timens)             { fTimeNs = timens; };
  void SetTime(const Double_t time)                 { fTime = time; };
  void SetTimeRes(const Double_t timeres)           { fTimeRes = timeres; };
  void SetDriftDistance(const Double_t distance)    { fDistance = distance; };
  void SetDriftPosition(const Double_t position)    { fDriftPosition = position; };
  void SetWirePosition(const Double_t position)     { fWirePosition = position; };

  void SetTreeLinePosition(const Double_t position)     { fTreeLinePosition = position; };
  void SetPartialTrackPosition(const Double_t position) { fPartialTrackPosition = position; };
  void SetTreeLineResidual(const Double_t residual)     { fTreeLineResidual = residual; };
  void SetPartialTrackResidual(const Double_t residual) { fPartialTrackResidual = residual; };

  void SetSpatialResolution(const Double_t sresolution) { fSpatialResolution = sresolution; };
  void SetTrackResolution(const Double_t tresolution)   { fTrackResolution = tresolution; };

  void SetUsed(const Bool_t isused = true)          { fIsUsed = isused; };
  // @}

  Bool_t IsFirstDetectorHit()                  {  return (fHitNumber == 0); };

  void SubtractTimeAuOffset(Double_t time_au_offset) { fTime   -= time_au_offset; };
  void SubtractTimeNsOffset(Double_t time_ns_offset) { fTimeNs -= time_ns_offset; };

  void ApplyTimeCalibration(Double_t f1tdc_resolution_ns) { 
    fTimeRes = f1tdc_resolution_ns;
    fTimeNs  = fTime*fTimeRes;
  };

  // three functions and their comments can be found in QwHit.cc,
  Bool_t PlaneMatches(EQwRegionID region, EQwDetectorPackage package, Int_t plane);
  Bool_t DirMatches(EQwRegionID region, EQwDetectorPackage package, EQwDirectionID dir);
  Bool_t WireMatches(EQwRegionID region, EQwDetectorPackage package, Int_t plane, Int_t wire);

  void CalculateTreeLineResidual()     {
    fTreeLineResidual = fabs(fDriftPosition - fTreeLinePosition);
  };
  void CalculatePartialTrackResidual() {
    fPartialTrackResidual = fabs(fDriftPosition - fPartialTrackPosition);
  };

 public:

  //  Identification information for readout channels
  Int_t fCrate;                      ///< ROC number
  Int_t fModule;                     ///< F1TDC slot number, or module index
  Int_t fChannel;                    ///< Channel number
  Int_t fHitNumber;                  ///< Index for multiple hits in a single channel on the left
  Int_t fHitNumber_R;                ///< Index for multiple hits in a single channel on the right

  Bool_t fAmbiguousElement;          ///< TRUE if this hit could come from two different elements
                                     ///  (used by Region 3 tracking only)
  Bool_t fLRAmbiguity;               ///< TRUE  if the other element is 8 wires "right";
                                     ///  FALSE if the other element is 8 wires "left" (used by Region 3 tracking only)


  //  Data specific to the hit
  UInt_t   fRawTime;                 ///< Time as reported by TDC; it is a raw data word, and is UNSUBTRACTED
  UInt_t   fRawRefTime;              ///< Time as reported by TDC as a reference time 
  Double_t fTimeNs;                  ///< Reference Corrected and TimeCalibration time (unit ns)
  Double_t fTime;                    ///< Start corrected time, may also be further modified
  Double_t fTimeRes;                 ///< Resolution of time (if appropriate)
  Double_t fDistance;                ///< Perpendicular distance from the wire to the track,
                                     ///  as reconstructed from the drift time
  Double_t fDriftPosition;           ///< Position of the decoded hit in the drift cell
  Double_t fWirePosition;            ///< Longitudinal position of the hit (this is mainly
                                     ///  used in region 3 where the z coordinate is taken
                                     ///  in the wire plane instead of perpendicular to it)

  Double_t fTreeLinePosition;        ///< Position of the fitted treeline through the drift cell
  Double_t fTreeLineResidual;        ///< Treeline residual of this hit (difference between
                                     ///  the drift distance and the distance to the fitted track)

  Double_t fPartialTrackPosition;    ///< Position of the fitted treeline through the drift cell
  Double_t fPartialTrackResidual;    ///< Partial track residual of this hit (difference between
                                     ///  the drift distance and the distance to the fitted track)

  Double_t fSpatialResolution;       ///< Spatial resolution
  Double_t fTrackResolution;         ///< Track resolution

  Bool_t fIsUsed; //!                ///< Is this hit used in a tree line?


  #if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
    ClassDef(QwHit,1);
  #endif

}; // class QwHit

#endif // QWHIT_H
