/**********************************************************\
* File: QwDriftChamberVDC.h                                *
*                                                          *
* Author: P. M. King, Rakitha Beminiwattha                 *
* Time-stamp: <2008-07-16 15:40>                           *
\**********************************************************/


#ifndef __QWDRIFTCHAMBERVDC__
#define __QWDRIFTCHAMBERVDC__






#include "QwDriftChamber.h"








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



 protected:
 void FillRawTDCWord(Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data);




 protected:

  Int_t LinkReferenceChannel(const UInt_t chan, const UInt_t plane, const UInt_t wire);
  Int_t BuildWireDataStructure(const UInt_t chan, const UInt_t package, const UInt_t plane, const Int_t wire);
  Int_t AddChannelDefinition(const UInt_t plane, const UInt_t wire);
 




  std::vector< QwHit > fWireHitsVDC;

  
};

#endif
