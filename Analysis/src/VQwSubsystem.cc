/**********************************************************\
* File: VQwSubsystem.C                                     *
*                                                          *
* Author: P. M. King, Rakitha Beminiwattha                 *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

/*------------------------------------------------------------------------*//*!

 \defgroup QwAnalysis QwAnalysis

 \section myoverview Overview

   Qweak Analysis Framework

   Each subsystem will have a class derived from "VQwSubsystem", and
   will be responsible for decoding of it's own data stream and any
   special event processing required. QwSubsystemArray will handle
   mutiple "VQwSubsystem" objects and one call on the QwSubsystemArray
   will handle all the calls to that method in each subsystem.  Each
   susbsytem will also own the histograms and ntupling functions used
   for its data.

*//*-------------------------------------------------------------------------*/

#include "VQwSubsystem.h"

Int_t ERROR = -1;


void VQwSubsystem::ClearAllBankRegistrations()
{
  fBank_IDs.clear();
  fROC_IDs.clear();
  fCurrentROC_ID    = -1;
  fCurrentBank_ID   = -1;
};

Int_t VQwSubsystem::FindIndex(const std::vector<UInt_t> &myvec, const UInt_t value) const
{
  Int_t index = -1;
  for (size_t i=0 ; i < myvec.size(); i++ ){
    if (myvec[i]==value){
      index=i;
      break;
    }
  }
  return index;
};

Int_t VQwSubsystem::GetSubbankIndex(const UInt_t roc_id, const UInt_t bank_id) const
{
  //  Bool_t lDEBUG=kTRUE;
  Int_t index = -1;
  Int_t roc_index = FindIndex(fROC_IDs, roc_id);//will return the vector index for the Roc from the vector fROC_IDs.
  if (roc_index>=0){
    index = FindIndex(fBank_IDs[roc_index],bank_id);
    if (index>=0){
      for (Int_t i=0; i<roc_index; i++){
	index += (fBank_IDs[i].size());
      }
    }
  }
  return index;
};

Int_t VQwSubsystem::RegisterROCNumber(const UInt_t roc_id, const UInt_t bank_id = 0)
{
  Int_t stat = 0;
  Int_t roc_index = FindIndex(fROC_IDs, roc_id);//will return the vector index for this roc_id on the vector fROC_IDs
  if (roc_index==-1){
    fROC_IDs.push_back(roc_id);//new ROC number is added.
    roc_index = (fROC_IDs.size() - 1);
    std::vector<UInt_t> tmpvec(1,bank_id);
    fBank_IDs.push_back(tmpvec);
  } else {
    Int_t bank_index = FindIndex(fBank_IDs[roc_index],bank_id);
    if (bank_index==-1){//if the bank_id is not registered then register it.
      fBank_IDs[roc_index].push_back(bank_id);
    } else {
      //  This subbank in this ROC has already been registered!
      std::cerr << "VQwSubsystem::RegisterROCNumber:  This subbank ("
		<< bank_id << ") in this ROC (" << roc_id
		<< ") has already been registered!\n";
      stat = ERROR;
    }
  }
  if (stat!=-1){
    fCurrentROC_ID    = roc_id;
    fCurrentBank_ID   = bank_id;
  } else {
    fCurrentROC_ID    = -1;
    fCurrentBank_ID   = -1;
  }
  return stat;
};

Int_t VQwSubsystem::RegisterSubbank(const UInt_t bank_id)
{
  Int_t stat = 0;
  if (fCurrentROC_ID != -1){
    stat = RegisterROCNumber(fCurrentROC_ID, bank_id);
    fCurrentBank_ID   = bank_id;
  } else {
    //  There is not a ROC registered yet!
    std::cerr << "VQwSubsystem::RegisterSubbank:  This subbank ("
	      << bank_id << ") does not have an associated ROC!  "
	      << "Add a 'ROC=#' line to the map file.\n";
    stat = ERROR;
    fCurrentROC_ID  = -1;
    fCurrentBank_ID = -1;
  }
  return stat;
};

void VQwSubsystem::Print()
{
  std::cout << "Name of this subsystem: " << fSystemName << std::endl;
  for (size_t roc_index = 0; roc_index < fROC_IDs.size(); roc_index++) {
    std::cout << "ROC" << std::dec << fROC_IDs[roc_index] << ": ";
    for (size_t bank_index = 0; bank_index < fBank_IDs[roc_index].size(); bank_index++)
      std::cout << std::hex << "0x" << fBank_IDs[roc_index][bank_index] << " ";
    std::cout << std::dec << std::endl;
  }
};


void VQwSubsystem::Copy(VQwSubsystem *source){
  try
    {
     
      this->fSystemName=source->fSystemName;
      this->fIsDataLoaded= source->fIsDataLoaded;
      this->fCurrentROC_ID= source->fCurrentROC_ID;
      this->fCurrentBank_ID= source->fCurrentBank_ID;

      this->fROC_IDs.resize(source->fROC_IDs.size());
      for(size_t i=0;i<this->fROC_IDs.size();i++)
	this->fROC_IDs[i]=source->fROC_IDs[i];

      this->fBank_IDs.resize(source->fBank_IDs.size());
      

      for(size_t i=0;i<this->fBank_IDs.size();i++)
	this->fBank_IDs[i].resize(source->fBank_IDs[i].size());

      for(size_t i=0;i<this->fBank_IDs.size();i++)
	for(size_t j=0;j<this->fBank_IDs.size();j++)
	  this->fBank_IDs[i][j]=source->fBank_IDs[i][j];
      

    }
  catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
    }
  return;
};

VQwSubsystem&  VQwSubsystem::operator=  (VQwSubsystem *value){
  this->fIsDataLoaded= value->fIsDataLoaded;
  return *this;
};


