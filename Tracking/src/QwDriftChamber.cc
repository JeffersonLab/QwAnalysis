/**********************************************************\
 * File: QwDriftChamber.C                                   *
 *                                                          *
 * Author: P. M. King, Rakitha Beminiwattha                 *
 *         J. H. Lee                                        *
 * Time-stamp: Wednesday, July 28 15:52:26 EDT 2010         *
\**********************************************************/

#include "QwDriftChamber.h"

#include "QwLog.h"
#include "QwColor.h"
#include "QwParameterFile.h"

const UInt_t QwDriftChamber::kMaxNumberOfTDCsPerROC = 21;
const Int_t QwDriftChamber::kReferenceChannelPlaneNumber = 99;


// OK, fDEBUG, fNumberOfTDCs
QwDriftChamber::QwDriftChamber(TString region_tmp,std::vector< QwHit > &fWireHits_TEMP)
  :VQwSubsystem(region_tmp),
   VQwSubsystemTracking(region_tmp),
   fWireHits(fWireHits_TEMP)
{
  OK            = 0;
  fDEBUG        = kFALSE;
  fNumberOfTDCs = 0;
  ClearAllBankRegistrations();
  InitHistogramPointers();
  //    kMaxNumberOfChannelsPerTDC = GetTDCMaxChannels(); 
  kMaxNumberOfChannelsPerTDC = fF1TDC.GetTDCMaxChannels();  
  //  fF1TDCs->clear();
  
  //   fF1DataIntegrityCount = 0;


  /*for (int i1 = 0; i1 < kNumPackages; i1++)
    for (int i2 = 0; i2 < 2; i2++)
    for (int i3 = 0; i3 < 279; i3++)
    fTimeWireOffsets[i1][i2][i3] = 0;*/

};

QwDriftChamber::QwDriftChamber(TString region_tmp)
  :VQwSubsystemTracking(region_tmp),fWireHits(fTDCHits)
{
  OK            = 0;
  fDEBUG        = kFALSE;
  fNumberOfTDCs = 0;
  ClearAllBankRegistrations();
  InitHistogramPointers();
  kMaxNumberOfChannelsPerTDC = fF1TDC.GetTDCMaxChannels(); 
  //    kMaxNumberOfChannelsPerTDC = GetTDCMaxChannels(); 
  kMaxNumberOfChannelsPerTDC = 0; 
  //    fF1DataIntegrityCount = 0;

  /*for (int i1 = 0; i1 < kNumPackages; i1++)
    for (int i2 = 0; i2 < 2; i2++)
    for (int i3 = 0; i3 < 279; i3++)
    fTimeWireOffsets[i1][i2][i3] = 0;*/

};



// Int_t QwDriftChamber::LoadChannelMap(TString mapfile)
// {
//     TString varname, varvalue;
//     UInt_t  chan, package, plane, wire, direction, DIRMODE;
//     wire = plane = package = 0;
//     DIRMODE=0;


//     fDirectionData.resize(2);//currently we have 2  package - Rakitha (10/23/2008)
//     fDirectionData.at(0).resize(12); //currently we have 12 wire planes in each package - Rakitha (10/23/2008)
//     fDirectionData.at(1).resize(12); //currently we have 12 wire planes in each package - Rakitha (10/23/2008)

//     QwParameterFile mapstr(mapfile.Data());  //Open the file

//     while (mapstr.ReadNextLine()) {
//         mapstr.TrimComment('!');   // Remove everything after a '!' character.
//         mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
//         if (mapstr.LineIsEmpty())  continue;

//         if (mapstr.HasVariablePair("=",varname,varvalue)) {
//             //  This is a declaration line.  Decode it.
//             varname.ToLower();
//             UInt_t value = QwParameterFile::GetUInt(varvalue);
// 	    if (value ==0){
// 	      value = atol(varvalue.Data());
// 	    }
//             if (varname=="roc") {
// 	      RegisterROCNumber(value,0);
// 	      DIRMODE=0;
// 	    } 
// 	    else if (varname=="bank") {
//               RegisterSubbank(value);
// 	      DIRMODE=0;
// 	    } 
// 	    else if (varname=="slot") {
// 	      RegisterSlotNumber(value);
// 	      DIRMODE=0;
//             } 
// 	    else if (varname=="pkg") {
// 	      //this will identify the coming sequence is wire plane to direction mapping - Rakitha
// 	      DIRMODE=1;
// 	      package=value;
//             }
//         } 
// 	else if (DIRMODE==0) {
// 	  //  Break this line into tokens to process it.
// 	  chan    = (atol(mapstr.GetNextToken(", ").c_str()));
// 	  package = 1;
// 	  plane   = (atol(mapstr.GetNextToken(", ").c_str()));
// 	  wire    = (atol(mapstr.GetNextToken(", ").c_str()));
	  
// 	  // VDC and HDC
// 	  BuildWireDataStructure(chan, package, plane, wire);
	  
//         } 
// 	else if (DIRMODE==1) {
// 	  //this will decode the wire plane directions - Rakitha
// 	  plane     = (atol(mapstr.GetNextToken(", ").c_str()));
// 	  direction = (atol(mapstr.GetNextToken(", ").c_str()));
// 	  fDirectionData.at(package-1).at(plane-1)=direction;
// 	  BuildWireDataStructure(chan, package, plane, wire);
//         }
	
//     }
    
   


//     //  Construct the wire data structures.
//     AddChannelDefinition();

//     /*
//     for (size_t i=0; i<fDirectionData.at(0).size(); i++){
//     std::cout<<"Direction data Plane "<<i+1<<" "<<fDirectionData.at(0).at(i)<<std::endl;
//     }
//     */
//     //
//     ReportConfiguration();
//     return OK;
// };


void  QwDriftChamber::FillDriftDistanceToHits()
{ //Currently This routine is not in use the drift distance calculation is done at ProcessEvent() on each sub-class
  for (std::vector<QwHit>::iterator hit1=fWireHits.begin(); hit1!=fWireHits.end(); hit1++) {
    
    if (hit1->GetTime()<0) continue;
    hit1->SetDriftDistance(CalculateDriftDistance(hit1->GetTime(),hit1->GetDetectorID()));
    
  }
  return;
};












Int_t QwDriftChamber::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{

  Int_t index           = 0;
  Int_t tdc_slot_number = 0;
  Int_t tdc_chan_number = 0;
  UInt_t tdc_data  = 0;
  Bool_t data_integrity_flag = false;

  Bool_t temp_print_flag     = false;

  index = GetSubbankIndex(roc_id, bank_id);

  if (index>=0 && num_words>0) {
    //  We want to process this ROC.  Begin looping through the data.
    SetDataLoaded(kTRUE);

    if (fDEBUG) std::cout << "QwDriftChamber::ProcessEvBuffer:  "
			  << "Begin processing ROC" << roc_id << std::endl;

    data_integrity_flag = fF1TDC.CheckDataIntegrity(roc_id, buffer, num_words);
    
    
    if (data_integrity_flag) {
      
      //   fF1DataIntegrityCount++;

      for (UInt_t i=0; i<num_words ; i++) {
	
	//  Decode this word as a F1TDC word.
	fF1TDC.DecodeTDCWord(buffer[i], roc_id); // MQwF1TDC or MQwV775TDC
	// For MQwF1TDC,   roc_id is needed to print out some warning messages.
	// For MQwV775TDC, roc_id isn't necessary, thus I set roc_id=0 in
	//                 MQwV775TDC.h  (Mon May  3 12:32:06 EDT 2010 jhlee)
	
	tdc_slot_number = fF1TDC.GetTDCSlotNumber();
	tdc_chan_number = fF1TDC.GetTDCChannelNumber();
	if ( tdc_slot_number == 31) {
	  //  This is a custom word which is not defined in
	  //  the F1TDC, so we can use it as a marker for
	  //  other data; it may be useful for something.
	}
	
	// Each subsystem has its own interesting slot(s), thus
	// here, if this slot isn't in its slot(s) (subsystem map file)
	// we skip this buffer to do the further process
	
	if (! IsSlotRegistered(index, tdc_slot_number) ) continue;
	
	if ( fF1TDC.IsValidDataword() ) {//;;
	  // if F1TDC has a valid slot, resolution locked, and data word
	  try {
	    tdc_data = fF1TDC.GetTDCData();
	    FillRawTDCWord(index, tdc_slot_number, tdc_chan_number,
			   tdc_data);
	    fF1TDC.PrintTDCData(temp_print_flag);
	  }
	  catch (std::exception& e) {
	    std::cerr << "Standard exception from QwDriftChamber::FillRawTDCWord: "
		      << e.what() << std::endl;
	    std::cerr << "   Parameters:  index=="<<index
		      << "; GetF1SlotNumber()=="<< tdc_slot_number
		      << "; GetF1ChannelNumber()=="<<tdc_chan_number
		      << "; GetF1Data()=="<<tdc_data
		      << std::endl;
	    Int_t tdcindex = GetTDCIndex(index, tdc_slot_number);
	    std::cerr << "   GetTDCIndex()=="<<tdcindex
		      << "; fTDCPtrs.at(tdcindex).size()=="
		      << fTDCPtrs.at(tdcindex).size()
		      << "; fTDCPtrs.at(tdcindex).at(chan).fPlane=="
		      << fTDCPtrs.at(tdcindex).at(tdc_chan_number).fPlane
		      << "; fTDCPtrs.at(tdcindex).at(chan).fElement=="
		      << fTDCPtrs.at(tdcindex).at(tdc_chan_number).fElement
		      << std::endl;
	  }
	}//;;

      } // for (UInt_t i=0; i<num_words ; i++) {
    }



  }

  return OK;
};


// void  QwDriftChamber::DeleteHistograms()
// {

  //   ===========================================================
  // There was a crash (#7 0x00a9789d in SigHandler(ESignals) () from /opt/root/lib/libCore.so).
  // This is the entire stack trace of all threads:
  // ===========================================================
  // #0  0x00148410 in __kernel_vsyscall ()
  // #1  0x013135b3 in __waitpid_nocancel () from /lib/libc.so.6
  // #2  0x012b807b in do_system () from /lib/libc.so.6
  // #3  0x0228aead in system () from /lib/libpthread.so.0
  // #4  0x00a9316d in TUnixSystem::Exec(char const*) () from /opt/root/lib/libCore.so
  // #5  0x00a9a26d in TUnixSystem::StackTrace() () from /opt/root/lib/libCore.so
  // #6  0x00a977ce in TUnixSystem::DispatchSignals(ESignals) () from /opt/root/lib/libCore.so
  // #7  0x00a9789d in SigHandler(ESignals) () from /opt/root/lib/libCore.so
  // #8  0x00a90a64 in sighandler(int) () from /opt/root/lib/libCore.so
  // #9  <signal handler called>
  // #10 0x0030c798 in QwDriftChamber::DeleteHistograms() () from ./lib/libQw.so
  // #11 0x0034565e in QwDriftChamberHDC::~QwDriftChamberHDC() () from ./lib/libQw.so
  // #12 0x00336b62 in void boost::checked_delete<VQwSubsystem>(VQwSubsystem*) () from ./lib/libQw.so
  // #13 0x00337876 in boost::detail::sp_counted_impl_p<VQwSubsystem>::dispose() () from ./lib/libQw.so
  // #14 0x08053666 in boost::detail::sp_counted_base::release (this=0x8750498) at /usr/include/boost/detail/sp_counted_base_gcc_x86.hpp:145
  // #15 0x080536a0 in boost::detail::shared_count::~shared_count (this=0x877e6d4, __in_chrg=<value optimized out>)
  //     at /usr/include/boost/detail/shared_count.hpp:159
  // #16 0x08054e46 in boost::shared_ptr<VQwSubsystem>::~shared_ptr (this=0x877e6d0, __in_chrg=<value optimized out>) at /usr/include/boost/shared_ptr.hpp:106
  // #17 0x08054e69 in std::_Destroy<boost::shared_ptr<VQwSubsystem> > (__pointer=0x877e6d0)
  //     at /usr/lib/gcc/i386-redhat-linux/4.1.2/../../../../include/c++/4.1.2/bits/stl_construct.h:107
  // #18 0x08054e8f in std::__destroy_aux<boost::shared_ptr<VQwSubsystem>*> (__first=0x877e6d0, __last=0x877e700)
  //     at /usr/lib/gcc/i386-redhat-linux/4.1.2/../../../../include/c++/4.1.2/bits/stl_construct.h:122
  // #19 0x08054ece in std::_Destroy<boost::shared_ptr<VQwSubsystem>*> (__first=0x877e6c8, __last=0x877e700)
  //     at /usr/lib/gcc/i386-redhat-linux/4.1.2/../../../../include/c++/4.1.2/bits/stl_construct.h:155
  // #20 0x08054ef8 in std::_Destroy<boost::shared_ptr<VQwSubsystem>*, boost::shared_ptr<VQwSubsystem> > (__first=0x877e6c8, __last=0x877e700)
  //     at /usr/lib/gcc/i386-redhat-linux/4.1.2/../../../../include/c++/4.1.2/bits/stl_construct.h:182
  // #21 0x08056317 in std::vector<boost::shared_ptr<VQwSubsystem>, std::allocator<boost::shared_ptr<VQwSubsystem> > >::~vector (this=0xbf8788a0, 
  //     __in_chrg=<value optimized out>) at /usr/lib/gcc/i386-redhat-linux/4.1.2/../../../../include/c++/4.1.2/bits/stl_vector.h:272
  // #22 0x08056496 in QwSubsystemArray::~QwSubsystemArray (this=0xbf87889c, __in_chrg=<value optimized out>) at ./Analysis/include/QwSubsystemArray.h:52
  // #23 0x080569a7 in QwSubsystemArrayTracking::~QwSubsystemArrayTracking (this=0xbf87889c, __in_chrg=<value optimized out>)
  //     at ./Tracking/include/QwSubsystemArrayTracking.h:30
  // #24 0x08053461 in main (argc=5, argv=0xbf878b64) at Tracking/main/QwTracking.cc:342

  // When I explictly try to delete objects in DeleteHistograms(), I met the above error messages..
  // Wednesday, July 28 15:23:57 EDT 2010, jhlee
  //

  // for( std::vector<TH1F* >::iterator i = TotHits.begin();  i != TotHits.end();  ++i ) delete *i;
  // for( std::vector<TH1F* >::iterator i = TOFP.begin();     i != TOFP.end();     ++i ) delete *i;
  // for( std::vector<TH1F* >::iterator i = TOFP_raw.begin(); i != TOFP_raw.end(); ++i ) delete *i;
  // for( std::vector<TH1F* >::iterator i = WiresHit.begin(); i != WiresHit.end(); ++i ) delete *i;

  // for( std::vector<TH2F* >::iterator i = TOFW.begin();     i != TOFW.end();     ++i ) delete *i;
  // for( std::vector<TH2F* >::iterator i = TOFW_raw.begin(); i != TOFW_raw.end(); ++i ) delete *i;
  // for( std::vector<TH2F* >::iterator i = HitsWire.begin(); i != HitsWire.end(); ++i ) delete *i;

  

//   return;

// };



void QwDriftChamber::ClearAllBankRegistrations()
{
  VQwSubsystemTracking::ClearAllBankRegistrations();
  std::size_t i = 0;
  for ( i=0; i<fTDC_Index.size(); i++){
    fTDC_Index.at(i).clear();
  }
  fTDC_Index.clear();

  fTDCPtrs.clear();
  fWireData.clear();
  fNumberOfTDCs = 0;
  return;
}

Int_t QwDriftChamber::RegisterROCNumber(const UInt_t roc_id, const UInt_t bank_id)
{
  Int_t status = 0;
  status = VQwSubsystemTracking::RegisterROCNumber(roc_id, bank_id);
  std::vector<Int_t> tmpvec(kMaxNumberOfTDCsPerROC,-1);
  fTDC_Index.push_back(tmpvec);
  std::cout<<"Registering ROC "<<roc_id<<std::endl;

  return status;
};


Int_t QwDriftChamber::RegisterSubbank(const UInt_t bank_id)
{
  Int_t stat = VQwSubsystem::RegisterSubbank(bank_id);
  fCurrentBankIndex = GetSubbankIndex(VQwSubsystem::fCurrentROC_ID, bank_id);//subbank id is directly related to the ROC
  
  if (fReferenceChannels.size()<=fCurrentBankIndex) {
    fReferenceChannels.resize(fCurrentBankIndex+1);
    fReferenceData.resize(fCurrentBankIndex+1);
  }
  std::vector<Int_t> tmpvec(kMaxNumberOfTDCsPerROC,-1);
  fTDC_Index.push_back(tmpvec);
  std::cout<< "RegisterSubbank()" 
	   <<" ROC " << (VQwSubsystem::fCurrentROC_ID)
	   <<" Subbank "<<bank_id
	   <<" with BankIndex "<<fCurrentBankIndex<<std::endl;
  return stat;
};


Int_t QwDriftChamber::RegisterSlotNumber(UInt_t slot_id)
{
  if (slot_id<kMaxNumberOfTDCsPerROC) {
    if (fCurrentBankIndex>=0 && fCurrentBankIndex<=fTDC_Index.size()) {
      fTDCPtrs.resize(fNumberOfTDCs+1);
      fTDCPtrs.at(fNumberOfTDCs).resize(kMaxNumberOfChannelsPerTDC);
      fNumberOfTDCs = fTDCPtrs.size();
      fTDC_Index.at(fCurrentBankIndex).at(slot_id) = fNumberOfTDCs-1;
      fCurrentSlot     = slot_id;
      fCurrentTDCIndex = fNumberOfTDCs-1;
    }
  } else {
    std::cerr << "QwDriftChamber::RegisterSlotNumber:  Slot number "
	      << slot_id << " is larger than the number of slots per ROC, "
	      << kMaxNumberOfTDCsPerROC << std::endl;
  }
  return fCurrentTDCIndex;
    
};

Int_t QwDriftChamber::GetTDCIndex(size_t bank_index, size_t slot_num) const 
{
  Int_t tdcindex = -1;
  if (bank_index>=0 && bank_index<fTDC_Index.size()) {
    if (slot_num>=0 && slot_num<fTDC_Index.at(bank_index).size()) {
      tdcindex = fTDC_Index.at(bank_index).at(slot_num);
    }
  }
  return tdcindex;
};


Int_t QwDriftChamber::LinkReferenceChannel (const UInt_t chan,const Int_t plane, const Int_t wire )
{
  fReferenceChannels.at ( fCurrentBankIndex ).first  = fCurrentTDCIndex;
  fReferenceChannels.at ( fCurrentBankIndex ).second = chan;
  //  Register a reference channel with the wire equal to the bank index.
  fTDCPtrs.at ( fCurrentTDCIndex ).at ( chan ).fPackage = kPackageNull;
  fTDCPtrs.at ( fCurrentTDCIndex ).at ( chan ).fPlane   = plane;
  fTDCPtrs.at ( fCurrentTDCIndex ).at ( chan ).fElement = fCurrentBankIndex;
  return OK;
};
