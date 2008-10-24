/*File: QwDriftChamberHDC.C                                *
*                                                          *
* Author: P. M. King , Rakitha Beminiwattha                *
* Time-stamp: <2008-07-08 15:40>                           *
**********************************************************/

#include "QwDriftChamberHDC.h"

#include "QwParameterFile.h"

#include<boost/bind.hpp>





QwDriftChamberHDC::QwDriftChamberHDC(TString region_tmp): QwDriftChamber(region_tmp)
{
  OK=0;
};






void  QwDriftChamberHDC::ReportConfiguration(){
  for (size_t i = 0; i<fROC_IDs.size(); i++){
    for (size_t j=0; j<fBank_IDs.at(i).size(); j++){
      Int_t ind = GetSubbankIndex(fROC_IDs.at(i),fBank_IDs.at(i).at(j));
      std::cout << "ROC " << fROC_IDs.at(i) 
		<< ", subbank " << fBank_IDs.at(i).at(j) 
		<< ":  subbank index==" << ind
		<< std::endl;
      for (size_t k=0; k<kMaxNumberOfTDCsPerROC; k++){
	Int_t tdcindex = GetTDCIndex(ind,k);
	std::cout << "    Slot " << k;
	if (tdcindex == -1) 
	  std::cout << "  Empty" << std::endl;
	else
	  std::cout << "  TDC#" << tdcindex << std::endl;
      }
    }
  }
  for (size_t i=0; i<fWiresPerPlane.size(); i++){
    if (fWiresPerPlane.at(i) == 0) continue;
    std::cout << "Plane " << i << " has " << fWireData.at(i).size()
	      << " wires" 
	      <<std::endl;
  }
};



void  QwDriftChamberHDC::SubtractReferenceTimes()
{
  Bool_t refs_okay = kTRUE;
  std::vector<Double_t> reftimes;

  reftimes.resize(fReferenceData.size());
  for (size_t i=0; i<fReferenceData.size(); i++){
    if (fReferenceData.at(i).size()==0){
      //  There isn't a reference time!
      std::cerr << "QwDriftChamber::SubtractReferenceTimes:  Subbank ID "
		<< i << " is missing a reference time." << std::endl;
      refs_okay = kFALSE;
    } else {
      reftimes.at(i) = fReferenceData.at(i).at(0);
    }
  }
  if (refs_okay) {
    for (size_t i=0; i<fReferenceData.size(); i++){
      for (size_t j=0; j<fReferenceData.at(i).size(); j++){
	fReferenceData.at(i).at(j) -= reftimes.at(i);
      }
    }
    for(std::vector<QwHit>::iterator hit1=fTDCHits.begin(); hit1!=fTDCHits.end(); hit1++) {
      hit1->SubtractReference(reftimes.at(hit1->GetSubbankID()));
    }
  }
};


void  QwDriftChamberHDC::FillRawTDCWord(Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data)
{
  Int_t tdcindex = GetTDCIndex(bank_index,slot_num);
  if (tdcindex != -1){
    Int_t direction;
    Int_t hitCount=0;
    Int_t package = 1;    
    Int_t plane   = fTDCPtrs.at(tdcindex).at(chan).fPlane;
    Int_t wire    = fTDCPtrs.at(tdcindex).at(chan).fElement;
    
    //Int_t hitCount;
    if (plane == -1 || wire == -1){
      //  This channel is not connected to anything.
      //  Do nothing.
    } else if (plane == kReferenceChannelPlaneNumber){
      fReferenceData.at(wire).push_back(data);
    } else {
      direction = fDirectionData.at(package).at(plane); //Wire Direction is accessed from the vector -Rakitha (10/23/2008)

      //hitCount gives the total number of hits on a given wire -Rakitha (10/23/2008)
      hitCount=std::count_if(fTDCHits.begin(),fTDCHits.end(),boost::bind(&QwHit::WireMatches,_1,2,boost::ref(package),boost::ref(plane),boost::ref(wire)) );
      fTDCHits.push_back(QwHit(bank_index, slot_num, chan, hitCount,2, package, plane,direction, wire, data));//in order-> bank index, slot num, chan, hitcount, region=2, package, plane,,direction, wire,wire hit time
      
    }
  };
};




Int_t QwDriftChamberHDC::LinkReferenceChannel(const UInt_t chan, const UInt_t plane, const UInt_t wire){
  fReferenceChannels.at(fCurrentBankIndex).first  = fCurrentTDCIndex;
  fReferenceChannels.at(fCurrentBankIndex).second = chan;
  //  Register a reference channel with the wire equal to the bank index.
  fTDCPtrs.at(fCurrentTDCIndex).at(chan).fPackage = 0;
  fTDCPtrs.at(fCurrentTDCIndex).at(chan).fPlane   = plane;
  fTDCPtrs.at(fCurrentTDCIndex).at(chan).fElement = fCurrentBankIndex;
  return OK;
};

Int_t QwDriftChamberHDC::BuildWireDataStructure(const UInt_t chan, const UInt_t package, const UInt_t plane, const Int_t wire){
  if (plane == kReferenceChannelPlaneNumber){
    LinkReferenceChannel(chan, plane, wire);
  } else {
    fTDCPtrs.at(fCurrentTDCIndex).at(chan).fPackage = package;
    fTDCPtrs.at(fCurrentTDCIndex).at(chan).fPlane   = plane;
    fTDCPtrs.at(fCurrentTDCIndex).at(chan).fElement = wire;
    if (plane>=fWiresPerPlane.size()){
      fWiresPerPlane.resize(plane+1);
    }
    if (wire>=fWiresPerPlane.at(plane)){
      fWiresPerPlane.at(plane) =  wire+1;
    }
  }
  return OK;
};

Int_t QwDriftChamberHDC::AddChannelDefinition(const UInt_t plane, const UInt_t wire)
{
  fWireData.resize(fWiresPerPlane.size());
  for (size_t i=0; i<fWiresPerPlane.size(); i++){
    fWireData.at(i).resize(fWiresPerPlane.at(i));
  }
  for (size_t i=0; i<fTDC_Index.size(); i++){
    Int_t refchan = i;
    for (size_t j=0; j<fTDC_Index.at(i).size(); j++){
      Int_t mytdc = fTDC_Index.at(i).at(j);
      if (mytdc!=-1){
	for (size_t k=0; k<fTDCPtrs.at(mytdc).size(); k++){
	  //	  Int_t package = fTDCPtrs.at(mytdc).at(k).fPackage;
	  Int_t plane   = fTDCPtrs.at(mytdc).at(k).fPlane;
	  if (plane>0 && plane !=kReferenceChannelPlaneNumber){
	    Int_t wire  = fTDCPtrs.at(mytdc).at(k).fElement;
	    fWireData.at(plane).at(wire).SetElectronics(i,j,k);
	  }
	}
      }
    }
  }
  return OK;
}
