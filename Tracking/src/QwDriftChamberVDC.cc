/**********************************************************\
* File: QwDriftChamberVDC.C                                *
*                                                          *
* Author: P. M. King , Rakitha Beminiwattha                *
* Time-stamp: <2008-07-16 15:40>                           *
\**********************************************************/

#include "QwDriftChamberVDC.h"

#include "QwParameterFile.h"





QwDriftChamberVDC::QwDriftChamberVDC(TString region_tmp): QwDriftChamber(region_tmp,fWireHitsVDC)
{
  OK=0;
};






void  QwDriftChamberVDC::ReportConfiguration(){
  
};



void  QwDriftChamberVDC::SubtractReferenceTimes()
{
  
};



void  QwDriftChamberVDC::FillRawTDCWord(Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data)
{
  
};




Int_t QwDriftChamberVDC::LinkReferenceChannel(const UInt_t chan, const UInt_t plane, const UInt_t wire){
  
  return OK;
};

Int_t QwDriftChamberVDC::BuildWireDataStructure(const UInt_t chan, const UInt_t package, const UInt_t plane, const Int_t wire){
  
  return OK;
};

Int_t QwDriftChamberVDC::AddChannelDefinition(const UInt_t plane, const UInt_t wire)
{
  
  return OK;
}
