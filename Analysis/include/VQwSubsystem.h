/**********************************************************\
* File: VQwSubsystem.h                                     *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/


#ifndef __VQWSUBSYSTEM__
#define __VQWSUBSYSTEM__

#include <iostream>
#include <vector>
#include <Rtypes.h>
#include <TString.h>
#include <TDirectory.h>

#include "QwParameterFile.h"

class VQwSubsystem{
  /******************************************************************
   *  Class: VQwSubsystem
   *         Virtual base class for the classes containing the
   *         event-based information from each tracking subsystem.
   *         This will define the interfaces used in communicating
   *         with the CODA routines.
   *
   ******************************************************************/
 public:
  VQwSubsystem(TString region_tmp):fSystemName(region_tmp),fIsDataLoaded(kFALSE){};

  virtual ~VQwSubsystem(){};

  TString GetSubsystemName() const {return fSystemName;};
  Bool_t  HasDataLoaded() const  {return fIsDataLoaded;}
  


  virtual Int_t LoadChannelMap(TString mapfile) = 0;
  virtual void  ClearEventData() = 0;

  virtual Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words) = 0;

  virtual Int_t ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words) = 0;

  virtual void  ProcessEvent() = 0;

  virtual void  ConstructHistograms(){ConstructHistograms((TDirectory*)NULL);};
  virtual void  ConstructHistograms(TDirectory *folder) = 0;
  virtual void  FillHistograms() = 0;
  virtual void  DeleteHistograms() = 0;
  
  
  

 protected:
  void  ClearAllBankRegistrations();
  Int_t RegisterROCNumber(const UInt_t roc_id, const UInt_t bank_id);   // Tells this object that it will decode data from this ROC and sub-bank
  Int_t RegisterSubbank(const UInt_t bank_id);  // Tells this object that it will decode data from this sub-bank in the ROC currently open for registration
  Int_t GetSubbankIndex(const UInt_t roc_id, const UInt_t bank_id) const;

  void  SetDataLoaded(Bool_t flag){fIsDataLoaded = flag;};

 protected:
  Int_t FindIndex(const std::vector<UInt_t> &myvec, const UInt_t value) const ;

 protected:
  TString  fSystemName;  ///  Name of this subsystem (the region).

  Bool_t   fIsDataLoaded; /// Has this subsystem gotten data to be processed?

 protected:
  Int_t fCurrentROC_ID;
  Int_t fCurrentBank_ID;

  std::vector<UInt_t> fROC_IDs;
  std::vector< std::vector<UInt_t> > fBank_IDs;

 private:
  VQwSubsystem(){};  //  Private constructor.


};


/*
 *  Define the functions.
 */



#endif
