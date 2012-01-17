/**********************************************************\
 * File: QwSciFiDetector.cc                                *
 *                                                         *
 * Author: Jeong Han Lee                                   *
 * Date:  Sunday, January 15 17:22:07 EST 2012             *
\**********************************************************/

#include "QwSciFiDetector.h"
#include "QwParameterFile.h"

// Register this subsystem with the factory
RegisterSubsystemFactory(QwSciFiDetector);




const UInt_t QwSciFiDetector::kMaxNumberOfSlotsPerROC = 21;
const Int_t  QwSciFiDetector::kF1ReferenceChannelNumber = 99;



QwSciFiDetector::QwSciFiDetector(const TString& name)
: VQwSubsystem(name),
  VQwSubsystemTracking(name)
{
  ClearAllBankRegistrations();
  kRegion             = "";
  fCurrentBankIndex   = 0;
  fCurrentSlot        = 0;
  fCurrentModuleIndex = 0;
  kNumberOfVMEModules = 0;
  kNumberOfF1TDCs     = 0;

  fF1TDContainer = new QwF1TDContainer();
  fF1TDCDecoder  = fF1TDContainer->GetF1TDCDecoder();
  kMaxNumberOfChannelsPerF1TDC = fF1TDCDecoder.GetTDCMaxChannels(); 
  
}

QwSciFiDetector::~QwSciFiDetector()
{
  fSCAs.clear();
};


Int_t 
QwSciFiDetector::LoadChannelMap( TString mapfile )
{
  
  Bool_t local_debug = false;

  if(local_debug) printf("\n------------- R1 LoadChannelMap %s\n\n", mapfile.Data());

  TString varname   = "";
  TString varvalue  = "";
  UInt_t  value     = 0;

  TString modtype  = "";
  Int_t   moduleidx = 0; // for SIS3801 scaler. In case of F1TDC, it is a dummy slot number.
  Int_t   channum  = 0; 
  Int_t   fibernum = 0;
  TString name     = "";
 

  EQwDetectorPackage package = kPackageNull;
  //  EQwDirectionID   direction = kDirectionNull;


  QwParameterFile mapstr(mapfile.Data());  //Open the file
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());

  while (mapstr.ReadNextLine()){
    
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.

    if (mapstr.LineIsEmpty())  continue;

    if (mapstr.HasVariablePair("=",varname,varvalue)){
       //  This is a declaration line.  Decode it.
      varname.ToLower();
      value = QwParameterFile::GetUInt(varvalue);
      if (value ==0){
	value = atol(varvalue.Data());
      }
      if (varname=="roc") {
	RegisterROCNumber(value);
      } 
      else if (varname=="bank"){
       	RegisterSubbank(value);
      }
      else if (varname=="pkg") {
       	package=(EQwDetectorPackage)value;
      }
      else if (varname=="slot") {
      	RegisterSlotNumber(value);
      } 
    }
    else {

      modtype   = mapstr.GetTypedNextToken<TString>();
      moduleidx = mapstr.GetTypedNextToken<Int_t>();
      channum   = mapstr.GetTypedNextToken<Int_t>();
      fibernum  = mapstr.GetTypedNextToken<Int_t>();
      name      = mapstr.GetTypedNextToken<TString>();

      if(local_debug) {
	printf("Modtype  %8s Moduleidx %d  ChanN %4d, FiberN %4d, FiberName %s\n", modtype.Data(), moduleidx, channum, fibernum, name.Data());
      }
      if (modtype=="F1TDC") {
	Int_t temp_element = 0;
	if (name=="ab") {
	  temp_element = 0;
	} else if (name=="a") {
	  temp_element  = 1;
	} else if (name=="b") {
	  temp_element  = 2;
	} else if (name=="ref") {
	  temp_element = fCurrentModuleIndex;  
	  fReferenceChannels.at ( fCurrentBankIndex ).first  = fCurrentModuleIndex;
	  fReferenceChannels.at ( fCurrentBankIndex ).second = channum;
	}
	
	fDetectorIDs.at(fCurrentModuleIndex).at(channum).fRegion    = kRegionID1;
	fDetectorIDs.at(fCurrentModuleIndex).at(channum).fPackage   = package;
	fDetectorIDs.at(fCurrentModuleIndex).at(channum).fPlane     = fibernum;
	fDetectorIDs.at(fCurrentModuleIndex).at(channum).fDirection = kDirectionNull;
	fDetectorIDs.at(fCurrentModuleIndex).at(channum).fElement   = temp_element;
      }
      else if (modtype == "SIS3801") {
	QwSIS3801D24_Channel localchannel(name);
	localchannel.SetNeedsExternalClock(kFALSE);
	fSCAs.push_back(localchannel);
	fSCAs_map[name] = fSCAs.size()-1;
	fSCAs_offset.push_back( QwSIS3801D24_Channel::GetBufferOffset(moduleidx,channum) );

	fDetectorIDs.at(fCurrentModuleIndex).at(channum).fRegion    = kRegionID1;
	fDetectorIDs.at(fCurrentModuleIndex).at(channum).fPackage   = package;
	fDetectorIDs.at(fCurrentModuleIndex).at(channum).fPlane     = fibernum;
	fDetectorIDs.at(fCurrentModuleIndex).at(channum).fDirection = kDirectionNull;
	fDetectorIDs.at(fCurrentModuleIndex).at(channum).fElement   = 0;

      }
      
   
      
    } 

  }
  
  if(local_debug) {
  
  
    Int_t unused_size_counter = 0;
    for(size_t bank_size = 0; bank_size < fModuleIndex.size(); bank_size++) 
      {
	for(size_t slot_size =0; slot_size < fModuleIndex.at(bank_size).size(); slot_size++)
	  {
	    Int_t m_idx = 0;
	    m_idx = fModuleIndex.at(bank_size).at(slot_size);
	    if(m_idx != -1 ) {
	      std::cout << "[" << bank_size <<","<< slot_size << "] "
			<< " module index " << fModuleIndex.at(bank_size).at(slot_size)
			<< std::endl;
	    }
	    else {
	      unused_size_counter++;
	    }
	  }
      }
    // why the bank index is always odd number?
    // 
    printf("Total unused size of fModuleIndex vector %6d\n", unused_size_counter);
    printf("\n------------- R1 LoadChannelMap End%s\n\n", mapfile.Data());

  }
  ReportConfiguration(local_debug);
  return 0;
};

Int_t 
QwSciFiDetector::LoadInputParameters(TString mapfile)
{
  return 0;
};

Int_t 
QwSciFiDetector::LoadGeometryDefinition(TString mapfile)
{
  return 0;
}

void 
QwSciFiDetector::ClearEventData()
{ 
  SetDataLoaded(kFALSE);

  for (size_t i=0; i<fSCAs.size(); i++) {
    fSCAs.at(i).ClearEventData();
  }
  return;
};

Int_t 
QwSciFiDetector::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
 
  return 0;
};

Int_t 
QwSciFiDetector::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{

  return 0;
}

void
QwSciFiDetector::ProcessEvent()
{

  for (size_t i=0; i<fSCAs.size(); i++)
    {
      fSCAs.at(i).ProcessEvent();
    }
  return;
};



void  
QwSciFiDetector::FillListOfHits(QwHitContainer& hitlist)
{
  return;
};

void  
QwSciFiDetector::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  
  for (size_t i=0; i<fSCAs.size(); i++) {
    fSCAs.at(i).ConstructHistograms(folder, prefix);
  }
  return;
};

void 
QwSciFiDetector::FillHistograms()
{
  if (! HasDataLoaded()) return;
  for (size_t i=0; i<fSCAs.size(); i++) {
    fSCAs.at(i).FillHistograms();
  }

  return;
};

void  
QwSciFiDetector::DeleteHistograms()
{

  return;
};





void 
QwSciFiDetector::ClearAllBankRegistrations()
{
  VQwSubsystemTracking::ClearAllBankRegistrations();

  std::size_t i = 0;

  for ( i=0; i<fModuleIndex.size(); i++)
    {
      fModuleIndex.at(i).clear();
    }
  
  fModuleIndex.clear();

  fDetectorIDs.clear();
  fHits.clear();
  kNumberOfVMEModules = 0;
  return;
}

Int_t 
QwSciFiDetector::RegisterROCNumber(const UInt_t roc_id)
{
  Int_t status = 0;
  status = VQwSubsystemTracking::RegisterROCNumber(roc_id, 0);
  std::vector<Int_t> tmpvec(kMaxNumberOfSlotsPerROC,-1);
  fModuleIndex.push_back(tmpvec);
  // std::cout<<"Registering ROC "<<roc_id<<std::endl;

  return status;
}


Int_t 
QwSciFiDetector::RegisterSubbank(const UInt_t bank_id)
{

  // In VQwSubsystem, register subbank to where for what?
  //
  Int_t stat = VQwSubsystem::RegisterSubbank(bank_id);

 
  // from here, just expand some vectors in QwSciFiDetector...
  //
  fCurrentBankIndex = GetSubbankIndex(VQwSubsystem::fCurrentROC_ID, bank_id);//subbank id is directly related to the ROC
 if (fReferenceChannels.size()<=fCurrentBankIndex) {
    fReferenceChannels.resize(fCurrentBankIndex+1);
    fReferenceData.resize(fCurrentBankIndex+1);
  }


  // Why do we need to expand unused and empty vector...
  // 
  std::vector<Int_t> tmpvec(kMaxNumberOfSlotsPerROC,-1);
  fModuleIndex.push_back(tmpvec);
  std::cout<< "RegisterSubbank()" 
	   <<" ROC " << (VQwSubsystem::fCurrentROC_ID)
	   <<" Subbank "<<bank_id
	   <<" with BankIndex "<<fCurrentBankIndex<<std::endl;

  return stat;
}


Int_t 
QwSciFiDetector::RegisterSlotNumber(UInt_t slot_id)
{
  if (slot_id<kMaxNumberOfSlotsPerROC) {
    
    if (fCurrentBankIndex <= fModuleIndex.size()) {
 
      fDetectorIDs.resize(kNumberOfVMEModules+1);
      fDetectorIDs.at(kNumberOfVMEModules).resize(kMaxNumberOfChannelsPerF1TDC);
      kNumberOfVMEModules = (Int_t) fDetectorIDs.size();
      fModuleIndex.at(fCurrentBankIndex).at(slot_id) = kNumberOfVMEModules-1;

      fCurrentSlot     = slot_id;
      fCurrentModuleIndex = kNumberOfVMEModules-1;
    }
  } 
  else {
    std::cerr << "QwSciFiDetector::RegisterSlotNumber:  Slot number "
	      << slot_id << " is larger than the number of slots per ROC, "
	      << kMaxNumberOfSlotsPerROC << std::endl;
  }
  return fCurrentModuleIndex;
    
}

Int_t 
QwSciFiDetector::GetModuleIndex(size_t bank_index, size_t slot_num) const 
{
  Int_t module_index = -1;
  // bank_index and slot_num are unsigned int and always positive
  if (/* bank_index >= 0 && */ bank_index < fModuleIndex.size()) {
    if (/* slot_num >= 0 && */ slot_num < fModuleIndex.at(bank_index).size()) {
      module_index = fModuleIndex.at(bank_index).at(slot_num);
    }
  }
  return module_index;
}





void  
QwSciFiDetector::ReportConfiguration(Bool_t verbose)
{
  if(verbose) {
  std::size_t i    = 0;
  std::size_t j    = 0;

  Int_t roc_num    = 0;
  Int_t bank_flag  = 0;
  Int_t bank_index = 0;
  Int_t module_index  = 0;

  UInt_t slot_id   = 0;
  UInt_t vme_slot_num = 0;
    
  std::cout << "QwSciFiDetector Region : " 
	    << this->GetSubsystemName()
	    << "::ReportConfiguration fDetectorIDs.size() " 
	    << fDetectorIDs.size() << std::endl;


  for ( i=0; i<fROC_IDs.size(); i++ ) 
    {

      roc_num = fROC_IDs.at(i);

      for ( j=0; j<fBank_IDs.at(i).size(); j++ ) 
	{
	
	  bank_flag    = fBank_IDs.at(i).at(j);
	  if(bank_flag == 0) continue; 
	  // must be uncommented if one doesn't define "bank_flag" in a CRL file
	  // but, now we use "bank_flag" in our crl files, thus skip to print
	  // unnecessary things on a screen
	  // Monday, August 30 14:45:34 EDT 2010, jhlee

	  bank_index = GetSubbankIndex(roc_num, bank_flag);
	
	  std::cout << "ROC [index, Num][" 
		    << i
		    << ","
		    << std::setw(2) << roc_num
		    << "]"
		    << " Bank [index,id]["
		    <<  bank_index
		    << ","
		    << bank_flag
		    << "]"
		    << std::endl;
	
	  for ( slot_id=2; slot_id<kMaxNumberOfSlotsPerROC; slot_id++ ) 
	    { 
	      // slot id starts from 2, because 0 and 1 are used for CPU and TI.
	      // Tuesday, August 31 10:57:07 EDT 2010, jhlee

	      module_index = GetModuleIndex(bank_index, slot_id);
	  
	      vme_slot_num = slot_id;
	  
	      std::cout << "    "
			<< "Slot [id, VME num] [" 
			<< std::setw(2) << slot_id
			<< ","
			<< std::setw(2) << vme_slot_num
			<< "]";
	      if ( module_index == -1 ) {
		std::cout << "    "
			  << "Unused in R1 SciFiDetector" 
			  << std::endl;
	      }
	      else {
		std::cout << "    "
			  << "Module index " 
			  << module_index << std::endl;
	      }
	    }
	}
    }
    
  for( size_t midx = 0; midx < fDetectorIDs.size(); midx++ )
    {
      for (size_t chan = 0 ; chan< fDetectorIDs.at(midx).size(); chan++)
	{
	  std::cout << "[" << midx <<","<< chan << "] "
		    << " detectorID " << fDetectorIDs.at(midx).at(chan)
		    << std::endl;
	}
    }
  }
  return;
}


