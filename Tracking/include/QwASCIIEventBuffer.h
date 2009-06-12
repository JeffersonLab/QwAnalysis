/**********************************************************\
* File: QwASCIIEventBuffer.h                                    *
*                                                          *
* Author: Rakitha Beminiwattha                             *
* Time-stamp: <2009-02-22 15:40>                           *
\**********************************************************/

#ifndef __QWEVENTASCIIBUFFER__
#define __QWEVENTASCIIBUFFER__

#include "QwTypes.h"
#include "QwEventBuffer.h"
#include "QwParameterFile.h"

#include "Det.h"
#include "QwHit.h"


#include "QwHitContainer.h"
#include "QwDetectorInfo.h"




#define DEBUG1 1


///
/// \ingroup QwTrackingAnl
class QwASCIIEventBuffer : public QwEventBuffer
{



 public:
  QwASCIIEventBuffer(){};

  ~QwASCIIEventBuffer(){
  };

  Int_t OpenDataFile(const TString filename,const TString rw);

  Int_t LoadChannelMap(const char *geomname);

  Int_t GetEvent();
  Int_t GetEventNumber(){return fEvtNumber;}

  Int_t ProcessHitContainer(QwHitContainer &);


  //In the QwASCIIEventBuffer, Only InitrcDETRegion and GetrcDETRegion are used to merge Decoding and QTR software


  //InitrcDETRegion will load rcDET and rcDETRegion by reading QwDetectorInfo vector. This vector is indexed by package and plane.
  //Currently we have only region = 2 working.
  Int_t InitrcDETRegion(std::vector< std::vector< QwDetectorInfo > > &);
  //GetrcDETRegion will update rcDETRegion by reading  QwHitContainer list.
  void  GetrcDETRegion(QwHitContainer &,Int_t );
  //internal function to update the rcDET
  void AddDetector(QwDetectorInfo, Int_t c);





  void  GetHitList(QwHitContainer & grandHitContainer){
    grandHitContainer.Append(fASCIIHits);
  }

 public:





  enum EQwDetectorPackage package;//from QwTypes.h
  enum EQwRegionID region;//from QwTypes.h
  enum EQwDirectionID dir;//from QwTypes.h
  enum EQwDetectorType type;//from QwTypes.h




  QwParameterFile *eventf ;
  Int_t CurrentEvent;

  int DetectId;





  QwHit * currentHit;
  std::vector< QwHit > fASCIIHits;

  std::vector< std::vector< QwDetectorInfo > > fDetectorInfo;//detector geometires





};

#endif
