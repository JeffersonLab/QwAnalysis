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

// ROOT headers
#include "TObject.h"

// Qweak headers
#include "QwTypes.h"

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
class QwHit : public TObject {

 public:

  //! \name Constructors
  // (documented in QwHit.cc)
  // @{
  QwHit();
  QwHit(const QwHit& hit);
  QwHit(const QwHit* hit);
  QwHit(Int_t bank_index, Int_t slot_num, Int_t chan, Int_t hitcount,
	EQwRegionID region, EQwDetectorPackage package, Int_t plane,
	EQwDirectionID direction, Int_t wire, UInt_t rawdata = 0);
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
  Bool_t operator< (QwHit& hit);
  //! \brief Output stream operator
  friend ostream& operator<< (ostream& stream, const QwHit& hit);

  //! \brief Print debugging information
  void Print();

  //! \name Getter functions
  // @{
  const Int_t           GetSubbankID()    const { return fCrate; };
  const Int_t           GetModule()       const { return fModule; };
  const Int_t           GetChannel()      const { return fChannel; };
  const Int_t           GetHitNumber()    const { return fHitNumber; };
  const Int_t           GetHitNumberR()   const { return fHitNumber_R; };
  const EQwRegionID     GetRegion()       const { return fRegion; };
  const EQwDetectorPackage GetPackage()   const { return fPackage; };
  const EQwDirectionID  GetDirection()    const { return fDirection; };
  const Int_t           GetPlane()        const { return fPlane; };
  const Int_t           GetElement()      const { return fElement; };

  QwDetectorInfo*       GetDetectorInfo() const { return pDetectorInfo; };

  const Bool_t          AmbiguousElement()const { return fAmbiguousElement; };
  const Bool_t          LRAmbiguity()     const { return fLRAmbiguity; };

  const UInt_t&         GetRawTime()      const { return fRawTime; };
  const Double_t&       GetTime()         const { return fTime; };
  const Double_t        GetTimeRes()      const { return fTimeRes; };
  const Double_t&       GetDriftDistance()const { return fDistance; };
  const Double_t        GetPosition()     const { return fPosition; };
  const Double_t        GetResidual()     const { return fResidual; };
  const Double_t        GetZPos()         const { return fZPosition; };
  const Double_t        GetZPosition()    const { return fZPosition; };

  const Bool_t          IsUsed()          const { return fIsUsed; };

  const QwDetectorID    GetDetectorID()   const;      // QwHit.cc
  const QwElectronicsID GetElectronicsID()const;      // QwHit.cc
  // @}


  //! \name Setter functions
  // @{
  void SetSubbankID(const Int_t bank_index)         { fCrate = bank_index; };
  void SetModule(const Int_t slot_num)              { fModule = slot_num; };
  void SetChannel(const Int_t chan)                 { fChannel = chan; };
  void SetHitNumber(const Int_t hitcount)           { fHitNumber = hitcount; };
  void SetHitNumberR(const Int_t hitcountr)         { fHitNumber_R = hitcountr; };
  void SetRegion(const EQwRegionID region)          { fRegion = region; };
  void SetPackage(const EQwDetectorPackage package) { fPackage = package; };
  void SetDirection(const EQwDirectionID direction) { fDirection = direction; };
  void SetPlane(const Int_t plane)                  { fPlane = plane; };
  void SetElement(const Int_t element)              { fElement = element; };

  void SetDetectorInfo(const QwDetectorInfo *detectorinfo) { pDetectorInfo = const_cast<QwDetectorInfo*>(detectorinfo); };
  void SetAmbiguousElement(const Bool_t amelement)  { fAmbiguousElement = amelement; };
  void SetLRAmbiguity(const Bool_t amlr)            { fLRAmbiguity = amlr; };

  void SetAmbiguityID(const Bool_t amelement, const Bool_t amlr);  // QwHit.cc

  void SetRawTime(const UInt_t rawtime)             { fRawTime = rawtime; };
  void SetTime(const Double_t time)                 { fTime = time; };
  void SetTimeRes(const Double_t timeres)           { fTimeRes = timeres; };
  void SetDriftDistance(const Double_t distance)    { fDistance = distance; };
  void SetPosition(const Double_t position)         { fPosition = position; };
  void SetResidual(const Double_t residual)         { fResidual = residual; };
  void SetZPos(const Double_t zposition)            { fZPosition = zposition; };
  void SetZPosition(const Double_t zposition)       { fZPosition = zposition; };


  void SetSpatialResolution(const Double_t sresolution) { fSpatialResolution = sresolution; };
  void SetTrackResolution(const Double_t tresolution)   { fTrackResolution = tresolution; };

  void SetUsed(const Bool_t isused = true)          { fIsUsed = isused; };
  // @}

  Bool_t IsFirstDetectorHit()                  {  return (fHitNumber == 0); };
  void SubtractTimeOffset(Double_t timeoffset) { fTime = fTime - timeoffset; };

  // three functions and their comments can be found in QwHit.cc,
  const Bool_t PlaneMatches(EQwRegionID region, EQwDetectorPackage package, Int_t plane);
  const Bool_t DirMatches(EQwRegionID region, EQwDetectorPackage package, EQwDirectionID dir);
  const Bool_t WireMatches(EQwRegionID region, EQwDetectorPackage package, Int_t plane, Int_t wire);

 public:

  //  Identification information for readout channels
  Int_t fCrate;                      ///< ROC number
  Int_t fModule;                     ///< F1TDC slot number, or module index
  Int_t fChannel;                    ///< Channel number
  Int_t fHitNumber;                  ///< Index for multiple hits in a single channel on the left
  Int_t fHitNumber_R;                ///< Index for multiple hits in a single channel on the right

  //  Identification information for the detector
  EQwRegionID        fRegion;        ///< Region 1, 2, 3, trigger scint., or cerenkov
  EQwDetectorPackage fPackage;       ///< Which arm of the rotator, or octant number
  EQwDirectionID     fDirection;     ///< Direction of the plane:  X, Y, U, V; R, theta; etc.
  Int_t              fPlane;         ///< R or theta index for R1; plane index for R2 & R3
  Int_t              fElement;       ///< Trace # for R1; wire # for R2 & R3; PMT # for others
  QwDetectorInfo* pDetectorInfo; //! ///< Pointer to the detector info object (not saved)

  Bool_t fAmbiguousElement;          ///< TRUE if this hit could come from two different elements
                                     ///  (used by Region 3 tracking only)
  Bool_t fLRAmbiguity;               ///< TRUE  if the other element is 8 wires "right";
                                     ///  FALSE if the other element is 8 wires "left" (used by Region 3 tracking only)


  //  Data specific to the hit
  UInt_t   fRawTime;                 ///< Time as reported by TDC; it is a raw data word, and is UNSUBTRACTED
  Double_t fTime;                    ///< Start corrected time, may also be further modified
  Double_t fTimeRes;                 ///< Resolution of time (if appropriate)
  Double_t fDistance;                ///< Perpendicular distance from the wire to the track,
                                     ///  as reconstructed from the drift time
  Double_t fPosition;                ///< Reconstructed position of the hit in real x, u, v
                                     ///  coordinates perpendicular to the wires
  Double_t fResidual;                ///< Residual of this hit (difference between the drift
                                     ///  distance and the distance to the fitted track)
  Double_t fZPosition;               ///< Lontigudinal position of the hit (this is mainly
                                     ///  used in region 3 where the z coordinate is taken
                                     ///  in the wire plane instead of perpendicular to it)
  Double_t fSpatialResolution;       ///< Spatial resolution
  Double_t fTrackResolution;         ///< Track resolution

  Bool_t fIsUsed; //!                ///< Is this hit used in a tree line?


 public:

  // The following public section are taken from the original Hit class
  // for merging the Hit class into the QwHit class.

  //  Hits for individual detector elements are strung together.  They also
  //  have a pointer back to the detector in case more information about the
  //  hit is needed.  The various position values are used in multiple ways,
  //  and therefore are not strictly defined.

  QwHit *next;	        //!	///< next hit
  QwHit *nextdet;	//!	///< next hit in same detector
  Double_t rResultPos;  	///< Resulting hit position
  Double_t rPos;		///< Position from level I track finding
  Double_t rPos2;		///< Position from level II decoding

  // NOTE (wdc) Probably it makes sense to rename rPos and rPos2 to fPosition,
  // which would be the first level signed track position with respect to the
  // ideal wire position, and fPosition2, which would be the second level signed
  // track position.

  ClassDef(QwHit,1);

}; // class QwHit

#endif // QWHIT_H
