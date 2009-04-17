/**********************************************************\
* File: QwDriftChamberVDC.h                                *
*                                                          *
* Author: P. M. King, Rakitha Beminiwattha                 *
* Time-stamp: <2008-07-16 15:40>                           *
\**********************************************************/


#ifndef __QWDRIFTCHAMBERVDC__
#define __QWDRIFTCHAMBERVDC__






#include "QwDriftChamber.h"


#include "QwDelayLine.h"

#include <stdlib.h>
#include <stdio.h>
#include <utility>





class QwDriftChamberVDC: public QwDriftChamber {
  /******************************************************************
   *  Class: QwDriftChamberVDC
   *
   *
   ******************************************************************/
 public:
  QwDriftChamberVDC(TString region_tmp);










  /* Unique virtual member functions from QwDrifChamber base class */

 public:

 void  ReportConfiguration();

 void  SubtractReferenceTimes();

 void  LoadMap ( TString& );        //read the TDC convert DelayLine map
 void  ReadEvent ( TString& );     //read the events file
 Double_t  QwDriftChamberVDC::CalculateDriftDistance(Double_t drifttime, QwDetectorID detector);


 
 protected:
 void FillRawTDCWord(Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data);




 protected:

  Int_t LinkReferenceChannel(const UInt_t chan, const UInt_t plane, const UInt_t wire);
  Int_t BuildWireDataStructure(const UInt_t chan, const UInt_t package, const UInt_t plane, const Int_t wire);
  Int_t AddChannelDefinition(const UInt_t plane, const UInt_t wire);





  static const UInt_t BackPlanenum;
  static const UInt_t Linenum;
  std::vector<std::vector<DelayLine> > DelayLineArray;      //indexed by backplane and line number
  std::vector<std::vector<QwDelayLineID> > fDelayLinePtrs;  //indexed by slot and channel number
  std::vector< QwHit > fWireHitsVDC;


};

#endif
