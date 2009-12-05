
/*! \file   QwHit.h
 *  \author P. M. King
 *  \date   2008feb13
 *  \brief  Draft skeleton for the decoding-to-QTR interface class.
 *
 *  \ingroup QwTrackingAnl
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
 *
 */

#ifndef QWHIT_H
#define QWHIT_H

#include <iostream>

#include "TObject.h"

#include "QwTypes.h"

class QwDetectorInfo;

class QwHit : public TObject {

 public:

  QwHit();

  QwHit(Int_t bank_index, Int_t slot_num, Int_t chan, Int_t hitcount,
	Int_t region, Int_t package, Int_t plane, Int_t direction, Int_t wire,
	UInt_t data);

  virtual ~QwHit();

  void Print();

  friend ostream& operator<< (ostream& stream, const QwHit& hit);

  Int_t GetSubbankID(){return fCrate;};

  QwDetectorID GetDetectorID(){
    return QwDetectorID(fRegion,fPackage,fPlane,fDirection,fElement);
  };


  QwElectronicsID GetElectronicsID(){
    return QwElectronicsID ( fModule,fChannel );
  };



  void AmbiguityID ( const Bool_t amelement,const Bool_t amlr )       //this function might be modified later
  {
    fAmbiguousElement=amelement;
    fLRAmbiguity=amlr;
  };

  void SetHitNumberR ( const Int_t hitcountR ){
    fHitNumber_R=hitcountR;
  };


  const Double_t& GetRawTime() const {return fRawTime;};
  const Double_t& GetTime()    const {return fTime;};

  const Double_t& GetDriftDistance()     const {return fDistance;};

  void SetHitNumber(Int_t count) {
    fHitNumber=count;
  }

  void SetDriftDistance(Double_t distance) {
    fDistance=distance;
  }

  void SetZPos(double zp) { fZPos = zp; };
  double GetZPos() { return fZPos; };

  const int GetElement() const { return fElement; };
  void SetElement(int element) { fElement = element; };

  QwDetectorInfo* GetDetectorInfo () const { return pDetectorInfo; };
  void SetDetectorInfo(QwDetectorInfo *detectorinfo) { pDetectorInfo = detectorinfo; };

  const int GetPlane() const { return fPlane; };

  const Double_t GetSpatialResolution() const;
  void SetSpatialResolution(const double resolution) { fSpatialResolution = resolution; };

  const Double_t GetTrackResolution() const;
  void SetTrackResolution(const double resolution) { fTrackResolution = resolution; };

  const double GetPosition() const { return fPosition; };
  void SetPosition(const double position) { fPosition = position; };

  const double GetZPosition() const;
  void SetZPosition(const double zpos) { fZPos = zpos; };

  const double GetRPosition() const { return fRPos; };
  void SetRPosition(const double rpos) { fRPos = rpos; };

  const double GetPhiPosition() const { return fPhiPos; };
  void SetPhiPosition(const double phipos) { fPhiPos = phipos; };

  Bool_t IsFirstDetectorHit() { return (fHitNumber==0); };
  Bool_t IsUsed() { return fIsUsed; };

  void SubtractTimeOffset(Double_t timeoffset){
    fTime = fTime - timeoffset;
  };

  void SetTime(Double_t time){
    fTime=time;
  }

  const Int_t GetModule()     const {return fModule;};
  const Int_t GetChannel()    const {return fChannel;};
  const Int_t GetHitNumber()  const {return fHitNumber;};
  const Int_t GetHitNumberR() const {return fHitNumber_R;};
  const Int_t GetRegion()     const {return fRegion;};
  const Int_t GetPackage()    const {return fPackage;};
  const Int_t GetDirection()  const {return fDirection;};

//below two metods retrieve subsets of QwHitContainer vector - rakitha (08/2008)

  const Bool_t PlaneMatches(EQwRegionID region, EQwDetectorPackage package, Int_t plane)
    {
      return (fRegion == region && fPackage == package && fPlane == plane);
    };
  const Bool_t DirMatches(EQwRegionID region, EQwDetectorPackage package, EQwDirectionID dir)
    {
      return (fRegion == region && fPackage == package && fDirection == dir);
    };

  //main use of this method is to count no.of hits for a given wire and update the fHitNumber - rakitha (08/2008)

  Bool_t WireMatches(Int_t region, Int_t package, Int_t plane, Int_t wire)
    {
      return (fRegion == region && fPackage == package && fElement == fElement);
    };

  Bool_t operator<(QwHit & obj);

 protected:
  // public:
  //  Identification information for readout channels

  Int_t fCrate;     ///< ROC number
  Int_t fModule;    ///< F1TDC slot number, or module index
  Int_t fChannel;   ///< Channel number
  Int_t fHitNumber; ///< Index for multiple hits in a single channel on the left
  Int_t fHitNumber_R;    ///< Index for multiple hits in a single channel on the right

  //  Identification information for the detector
  Int_t fRegion;    ///< Region 1, 2, 3, trigger scint., or cerenkov
  Int_t fPackage;   ///< Which arm of the rotator, or octant number
  Int_t fDirection; ///< Direction of the plane:  X, Y, U, V; R, theta; etc.
  Int_t fPlane;     ///< R or theta index for R1; plane index for R2 & R3
  Int_t fElement;   ///< Trace # for R1; wire # for R2 & R3; PMT # for others
  QwDetectorInfo* pDetectorInfo; //! ///< Pointer to the detector info object (not saved)

  Bool_t fAmbiguousElement;  ///< TRUE if this hit could come from two different elements (used by Region 3 tracking only)
  Bool_t fLRAmbiguity;       ///< TRUE if the other element is 8 wires "right"; FALSE if the other element is 8 wires "left" (used by Region 3 tracking only)


  //  Data specific to the hit
  Double_t fRawTime;    ///< Time as reported by TDC; it is UNSUBTRACTED
  Double_t fTime;       ///< Start corrected time, may also be further modified
  Double_t fTimeRes;    ///< Resolution of time (if appropriate)

  Double_t fDistance;   ///< Perpendicular distance from the wire to the track,
                        ///  as reconstructed from the drift time
  Double_t fPosition;   ///< Reconstructed position of the hit in real x, u, v
                        ///  coordinates perpendicular to the wires
  Double_t fResidual;   ///< Residual of this hit (difference between the drift
                        ///  distance and the distance to the fitted track)
  Double_t fZPos;       ///< Lontigudinal position of the hit (this is mainly
                        ///  used in region 3 where the z coordinate is taken
                        ///  in the wire plane instead of perpendicular to it)
  Double_t fRPos;       ///< fRPos and fPhiPos are specificed for region 1
  Double_t fPhiPos;   ///  hit location

  Double_t fSpatialResolution;	/// Spatial resolution
  Double_t fTrackResolution;	/// Track resolution

  public: // Sorry, will write accessors later (wdconinc)

    Bool_t fIsUsed;	/// Is this hit used in a tree line?


  // The following public section are taken from the original Hit class
  // for merging the Hit class into the QwHit class.

  public:

    //  Hits for individual detector elements are strung together.  They also
    //  have a pointer back to the detector in case more information about the
    //  hit is needed.  The various position values are used in multiple ways,
    //  and therefore are not strictly defined.

    QwHit *next;	//!	///< next hit
    QwHit *nextdet;	//!	///< next hit in same detector
    Double_t rResultPos;	/// Resulting hit position
    Double_t rPos;		/// Position from level I track finding
    Double_t rPos2;		/// Position from level II decoding

    // NOTE (wdc) Probably it makes sense to rename rPos and rPos2 to fPosition,
    // which would be the first level signed track position with respect to the
    // ideal wire position, and fPosition2, which would be the second level signed
    // track position.

  ClassDef(QwHit,1);

};

#endif
