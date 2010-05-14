/**********************************************************\
* File: QwHelicity.C                                      *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwHelicity.h"
#include "QwHistogramHelper.h"
#include <stdexcept>

#include "QwLog.h"

extern QwHistogramHelper gQwHists;
//**************************************************//


//**************************************************//
void QwHelicity::ProcessOptions(QwOptions &options){
  //Read the cmd options and override channel map settings
  QwMessage<<"QwHelicity::ProcessOptions"<<QwLog::endl;
  if (gQwOptions.HasValue("helicity.patternoffset"))
    if (gQwOptions.GetValue<int>("helicity.patternoffset")==1 || gQwOptions.GetValue<int>("helicity.patternoffset")==0)
      fPATTERNPHASEOFFSET=gQwOptions.GetValue<int>("helicity.patternoffset");

  if (gQwOptions.HasValue("helicity.patternphase"))
    if (gQwOptions.GetValue<int>("helicity.patternphase")==4 || gQwOptions.GetValue<int>("helicity.patternphase")==8)
      fMaxPatternPhase=gQwOptions.GetValue<int>("helicity.patternphase");

  if (gQwOptions.HasValue("helicity.30bitseed")){
    BIT30=gQwOptions.GetValue<bool>("helicity.30bitseed");
    BIT24=kFALSE;
  }else if (gQwOptions.HasValue("helicity.24bitseed")){
    BIT24=gQwOptions.GetValue<bool>("helicity.24bitseed");
    BIT30=kFALSE;
  }
  if (gQwOptions.HasValue("helicity.delay")){
    std::cout<<" Helicity Delay ="<<gQwOptions.GetValue<int>("helicity.delay")<<"\n";
    SetHelicityDelay(gQwOptions.GetValue<int>("helicity.delay"));
  }
};
Bool_t QwHelicity::IsContinuous()
{
  Bool_t results=kFALSE;
  if(IsGoodPatternNumber()&&IsGoodEventNumber()&&IsGoodPhaseNumber())
    results=kTRUE;
  return results;
}


Bool_t QwHelicity::IsGoodPatternNumber()
{
  Bool_t results;

  if((fPatternNumber == fPatternNumberOld) && (fPatternPhaseNumber == fPatternPhaseNumberOld+1))//same pattern new phase
       results = kTRUE; //got same pattern
  else if((fPatternNumber == fPatternNumberOld + 1) && (fPatternPhaseNumber == fMinPatternPhase))
       results=kTRUE; //new pattern
  else results=kFALSE; //wrong pattern

  if(!results)
    {
      QwWarning << "QwHelicity::IsGoodPatternNumber: \n this is not s good pattern number indeed:"
		<< QwLog::endl;
      Print();
    }

  return results;
};


Bool_t QwHelicity::IsGoodEventNumber()
{
  Bool_t results;
  if(fEventNumber == fEventNumberOld + 1)
    results= kTRUE;
  else
    results= kFALSE;

 if(!results)
    {
      QwWarning << "QwHelicity::IsGoodEventNumber: \n this is not a good event number indeed:"
		<< QwLog::endl;
      Print();
    }
  return results;
};


Bool_t QwHelicity::IsGoodPhaseNumber()
{
  Bool_t results;

  if((fPatternPhaseNumber == fMaxPatternPhase)  && (fPatternNumber == fPatternNumberOld )) //maximum phase of old pattern
     results = kTRUE;
  else if((fPatternPhaseNumber == fPatternPhaseNumberOld+1) && (fPatternNumber == fPatternNumberOld))
    results = kTRUE;
  else if((fPatternPhaseNumber == fMinPatternPhase) && (fPatternNumber == fPatternNumberOld + 1))
    results= kTRUE;
  else
    results = kFALSE;

  if(fPatternPhaseNumber>fMaxPatternPhase)
    results=kFALSE;

 if(!results)
    {
      QwWarning << "QwHelicity::IsGoodPhaseNumber:  not a good phase number \t"
		<< "Phase: " << fPatternPhaseNumber << " out of "
		<<fMaxPatternPhase
		<< "(was " <<fPatternPhaseNumberOld<<")"
		<< "\tPattern #" << fPatternNumber << "(was "
		<< fPatternNumberOld <<")"
		<< QwLog::endl; //Paul's modifications
      Print();
    }

  return results;
};


Bool_t QwHelicity::IsGoodHelicity()
{

  fGoodHelicity = kTRUE;

  if (fHelicityReported!=fHelicityDelayed)
    {
      //helicities do not match. Check phase number to see if its a new pattern.
      fGoodHelicity=kFALSE;
      if(fPatternPhaseNumber == fMinPatternPhase)//first event in a new pattern
	{
	  std::cerr<<"QwHelicity::IsGoodHelicity - The helicity reported in event "<<fEventNumber
		   <<" is not what we expect from the randomseed. Not a good event nor pattern"<<"\n";
	}
      else
	{
	  std::cerr<<"QwHelicity::IsGoodHelicity - The helicity reported in event "<<fEventNumber
		   <<" is not what we expect according to pattern structure. Not a good event nor pattern"<<"\n";
	}
    }

  if(!fGoodHelicity)
    {
      fHelicityReported=kUndefinedHelicity;
      fHelicityActual=kUndefinedHelicity;
      fHelicityDelayed=kUndefinedHelicity;
      //Have to start over again
      ResetPredictor();
    }

  return fGoodHelicity;
}

//******************************************************** Buddhini 2/11/09

void QwHelicity::ClearEventData()
{

  for (size_t i=0;i<fWord.size();i++)
    fWord[i].ClearEventData();

  //reset data
  fEventNumberOld = fEventNumber;
  fPatternNumberOld = fPatternNumber;
  fPatternPhaseNumberOld = fPatternPhaseNumber;

  //clear data
  fHelicityReported = kUndefinedHelicity;
  fHelicityActual = kUndefinedHelicity;
  fHelicityDelayed= kUndefinedHelicity;
  fHelicityBitPlus = kFALSE;
  fHelicityBitMinus = kFALSE;
  // be careful: it is not that I forgot to reset fActualPatternPolarity
  // or fDelayedPatternPolarity. One doesn't want to do that here.
  fEventNumber = -1;
  if(!dolocalhelicity)fPatternNumber = -1;
  fPatternPhaseNumber = -1;

  return;
}

//*****************************************************************
Int_t QwHelicity::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  //stub function
  std::cerr<<" this function QwHelicity::ProcessConfigurationBuffer does nothing yet \n";
  return 0;
}

Int_t QwHelicity::LoadInputParameters(TString pedestalfile)
{
  //SetHelicityDelay(8); // that is the number of pattern delayed
  return 0;
}
//*****************************************************************

Bool_t QwHelicity::ApplySingleEventCuts(){//impose single event cuts //Paul's modifications


  return kTRUE;
};

//*****************************************************************
Int_t QwHelicity::GetEventcutErrorCounters(){// report number of events falied due to HW and event cut faliure

  return 1;
};

//*****************************************************************
Int_t QwHelicity::GetEventcutErrorFlag(){//return the error flag

  return 0;

}
//*****************************************************************
void QwHelicity::ProcessEventUserbitMode()
{
  //std::cout<<"In  QwHelicity::ProcessEventUserbitMode() \n";

  // In this version of the code, the helicty is extracted for a userbit configuration.
  // This is not what we plan to have for Qweak but what we used for the winter 2008-09
  // injector tests
  Bool_t ldebug=kFALSE;
  UInt_t userbits;
  static UInt_t lastuserbits  = 0xFF;
  UInt_t scaleroffset=fWord[kScalerCounter].fValue/32;

  if(scaleroffset==1)
    {
      userbits = (fWord[kUserbit].fValue & 0xE0000000)>>28;

      //  Now fake the input register, MPS coutner, QRT counter, and QRT phase.
      fEventNumber=fEventNumberOld+1;

      lastuserbits = userbits;

      if (lastuserbits==0xFF)
	{
	  fPatternPhaseNumber    = fMinPatternPhase;
	}
      else
	{
	  if ((lastuserbits & 0x8) == 0x8) //  Quartet bit is set.
	    {
	      fPatternPhaseNumber    = fMinPatternPhase;  // Reset the QRT phase
	      fPatternNumber=fPatternNumberOld+1;     // Increment the QRT counter
	    }
	  else
	    {
	      fPatternPhaseNumber=fPatternPhaseNumberOld+1;       // Increment the QRT phase
	    }

	  fHelicityReported=0;

	  if ((lastuserbits & 0x4) == 0x4){ //  Helicity bit is set.
	    fHelicityReported    |= 1; // Set the InputReg HEL+ bit.
	    fHelicityBitPlus=kTRUE;
	    fHelicityBitMinus=kFALSE;
	  } else {
	    fHelicityReported    |= 0; // Set the InputReg HEL- bit.
	    fHelicityBitPlus=kFALSE;
	    fHelicityBitMinus=kTRUE;
	  }
	}
    }
  else
    {
      std::cerr<<" QwHelicity::ProcessEvent finding a missed read event in the scaler\n";
      if(ldebug)
	{
	  std::cout<<" QwHelicity::ProcessEvent finding a missed read event in the scaler\n";
	  std::cout<<" QwHelicity::ProcessEvent :"<<scaleroffset<<" events were missed \n";
	  std::cout<<" before manipulation \n";
	  Print();
	}
      //there was more than one event since the last reading of the scalers
      //ie we should read only one event at the time,
      //if not something is wrong
      fEventNumber=fEventNumberOld+scaleroffset;
      Int_t localphase=fPatternPhaseNumberOld;
      Int_t localpatternnumber=fPatternNumberOld;
      for (UInt_t i=0;i<scaleroffset;i++)
	{
	  fPatternPhaseNumber=localphase+1;
	  if(fPatternPhaseNumber>fMaxPatternPhase)
	    {
	      fPatternNumber=localpatternnumber+fPatternPhaseNumber/fMaxPatternPhase;
	      fPatternPhaseNumber=fPatternPhaseNumber-fMaxPatternPhase;
	      localpatternnumber=fPatternNumber;
	    }
	  localphase=fPatternPhaseNumber;
	}
      //Reset helicity predictor because we are not sure of what we are doing
      fHelicityReported=-1;
      ResetPredictor();
      if(ldebug)
	{
	  std::cout<<" after manipulation \n";
	  Print();
	}
    }

  return;
}
//*****************************************************************
void QwHelicity::ProcessEventInputRegisterMode()
{

  static Bool_t firstpattern = kTRUE;
  Bool_t fake_the_counters=kFALSE;

  UInt_t thisinputregister=fWord[kInputRegister].fValue;

   fEventNumber=fWord[kMpsCounter].fValue;
   if (firstpattern && (thisinputregister & 0x4) == 0x4){
     firstpattern   = kFALSE;
   }

   if (firstpattern){
     fPatternNumber      = 0;
     fPatternPhaseNumber = 0;
   } else {
     fPatternNumber      = fWord[kPatternCounter].fValue;
     fPatternPhaseNumber = fWord[kPatternPhase].fValue - fPATTERNPHASEOFFSET + fMinPatternPhase;
   }

   if (fake_the_counters){
     //  Now fake the event counter, pattern counter, and phase counter.
     fEventNumber = fEventNumberOld+1;
     if ((thisinputregister & 0x4) == 0x4) {
       fPatternPhaseNumber = fMinPatternPhase;
       fPatternNumber      = fPatternNumberOld + 1;
     } else  {
       fPatternPhaseNumber = fPatternPhaseNumberOld + 1;
       fPatternNumber      = fPatternNumberOld;
     }
   }
   /*
   // folowing, a bunch of self consistancy checks
   if(fEventNumber!=fEventNumberOld+1)
   std::cerr<<"QwHelicity::ProcessEvent read event# is not old_event#+1 \n";

   if ((thisinputregister & 0x4) == 0x4) //  Quartet bit is set.
     {
       //std::cerr<<"QwHelicity::ProcessEvent:  The Multiplet Sync bit is set, but the Pattern Phase (" << fPatternPhaseNumber << ") is not 1!" << std::endl;
       fPatternPhaseNumber    = fPATTERNPHASEOFFSET;  // Reset the QRT phase
       fPatternNumber = fPatternNumberOld+1;     // Increment the QRT  counter
     }
   else
     {
       fPatternPhaseNumber=fPatternPhaseNumberOld+1; // Increment the QRT phase
     }
     // end of consistancy checks
   */

   if(fEventNumber!=fEventNumberOld+1)
     std::cerr<<"QwHelicity::ProcessEvent read event# is not  old_event#+1 \n";

   if ((thisinputregister & 0x4) == 0x4 && fPatternPhaseNumber != fMinPatternPhase){
     //  Quartet bit is set.
     std::cerr<<"QwHelicity::ProcessEvent:  The Multiplet Sync bit is  set, but  the Pattern Phase (" << fPatternPhaseNumber << ") is not 1!" << std::endl;
   }

   fHelicityReported=0;

   if ((thisinputregister & 0x1) == 0x1){ //  Helicity bit is set.
     fHelicityReported    |= 1; // Set the InputReg HEL+ bit.
     fHelicityBitPlus=kTRUE;
     fHelicityBitMinus=kFALSE;
   } else {
     fHelicityReported    |= 0; // Set the InputReg HEL- bit.
     fHelicityBitPlus=kFALSE;
     fHelicityBitMinus=kTRUE;
   }

  return;
}
//*****************************************************************
void  QwHelicity::ProcessEvent()
{
  switch (fHelicityDecodingMode)
    {
    case kHelUserbitMode :
      ProcessEventUserbitMode();
      break;
      ///////////////
    case kHelInputRegisterMode :
      ProcessEventInputRegisterMode();
      break;
    default:
      std::cerr<<"QwHelicity::ProcessEvent no instructions on how to decode the helicity !!!!\n";
      abort();
    }


  ///////////////


  if(fHelicityBitPlus==fHelicityBitMinus)
    fHelicityReported=-1;

  PredictHelicity();

  return;
  }
////////////////////////////////////////////
void QwHelicity::EncodeEventData(std::vector<UInt_t> &buffer)
{
  std::vector<UInt_t> localbuffer;
  localbuffer.clear();

  // Userbit mode
  switch (fHelicityDecodingMode) {
  case kHelUserbitMode: {
    UInt_t userbit = 0x0;
    if (fPatternPhaseNumber == fMinPatternPhase) userbit |= 0x80000000;
    if (fHelicityDelayed == 1)    userbit |= 0x40000000;

    // Write the words to the buffer
    localbuffer.push_back(0x1); // cleandata
    localbuffer.push_back(0xa); // scandata1
    localbuffer.push_back(0xa); // scandata2
    localbuffer.push_back(0x0); // scalerheader
    localbuffer.push_back(0x20); // scalercounter (32)
    localbuffer.push_back(userbit); // userbit

    for (int i = 0; i < 64; i++) localbuffer.push_back(0x0); // (not used)
    break;
  }
  case kHelInputRegisterMode: {
    UInt_t input_register = 0x0;
    if (fHelicityDelayed == 1) input_register |= 0x1;
    if (fHelicityDelayed == 0) input_register |= 0x2; // even the mock data has balanced inputs!
    if (fPatternPhaseNumber == fMinPatternPhase) input_register |= 0x4;

    // Write the words to the buffer
    localbuffer.push_back(input_register); // input_register
    localbuffer.push_back(0x0); // output_register
    localbuffer.push_back(fEventNumber); // mps_counter
    localbuffer.push_back(fPatternNumber); // pat_counter
    localbuffer.push_back(fPatternPhaseNumber - fMinPatternPhase + fPATTERNPHASEOFFSET); // pat_phase

    for (int i = 0; i < 17; i++) localbuffer.push_back(0x0); // (not used)
    break;
  }
  default:
    QwWarning << "QwHelicity::EncodeEventData: Unsupported helicity encoding!" << QwLog::endl;
  }

  // If there is element data, generate the subbank header
  std::vector<UInt_t> subbankheader;
  std::vector<UInt_t> rocheader;
  if (localbuffer.size() > 0) {

    // Form CODA subbank header
    subbankheader.clear();
    subbankheader.push_back(localbuffer.size() + 1);	// subbank size
    subbankheader.push_back((fCurrentBank_ID << 16) | (0x01 << 8) | (1 & 0xff));
		// subbank tag | subbank type | event number

    // Form CODA bank/roc header
    rocheader.clear();
    rocheader.push_back(subbankheader.size() + localbuffer.size() + 1);	// bank/roc size
    rocheader.push_back((fCurrentROC_ID << 16) | (0x10 << 8) | (1 & 0xff));
		// bank tag == ROC | bank type | event number

    // Add bank header, subbank header and element data to output buffer
    buffer.insert(buffer.end(), rocheader.begin(), rocheader.end());
    buffer.insert(buffer.end(), subbankheader.begin(), subbankheader.end());
    buffer.insert(buffer.end(), localbuffer.begin(), localbuffer.end());
  }
}

////////////////////////////////////////////
void QwHelicity::Print()
{

//   for(size_t i=0;i<fWord.size();i++)
//     fWord[i].Print();

   std::cout<<"this event: Event#, Pattern#, PatternPhase#="
 	   << fEventNumber<<", "
 	   << fPatternNumber<<", "
	    << fPatternPhaseNumber<<"\n";
  std::cout<<"Previous event: Event#, Pattern#, PatternPhase#="
 	   << fEventNumberOld<<", "
 	   << fPatternNumberOld<<", "
 	   << fPatternPhaseNumberOld<<"\n";

  std::cout<<"delta = \n(fEventNumberOld)-(fMaxPatternPhase)x(fPatternNumberOld)-(fPatternPhaseNumberOld)= "
	   <<((fEventNumberOld)-(fMaxPatternPhase)*(fPatternNumberOld)-(fPatternPhaseNumberOld))<<"\n";

  std::cout<<"Helicity Reported, Delayed, Actual ="
	   << fHelicityReported<<","
	   << fHelicityDelayed<<","
	   << fHelicityActual<<"\n";

//   std::cout<<"Helicity from previous patterns: the latest helicity in this list\n is the actual (predicted) helicity for this pattern\n";
//   for(int i=0;i<fHelicityDelay+1;i++)
//     std::cout<<fCheckHelicityDelay[i]<<":";
//   std::cout<<"\n";

//   std::cout<<"Helicity Actual ("<<fHelicityDelay<<"events before) - Helicity reported (this event)="<<fCheckHelicityDelay[fHelicityDelay]-fHelicityReported<<"\n";
// this is obsolete

  std::cout<<"===\n";
  return;
}

////////////////////////////////////////////
Int_t QwHelicity::LoadChannelMap(TString mapfile)
{

  Bool_t ldebug=kFALSE;


  TString varname, varvalue;
  TString modtype, dettype, namech, keyword;
  Int_t modnum, channum;
  Int_t currentrocread=0;
  Int_t currentbankread=0;
  Int_t wordsofar=0;
  Int_t currentsubbankindex=-1;

  fPATTERNPHASEOFFSET=1;//Phase number offset is set to 1 by default and will be set to 0 if phase number starts from 0


  //Default value for random seed is 24 bits
  BIT24=kTRUE;
  BIT30=kFALSE;


  QwParameterFile mapstr(mapfile.Data());  //Open the file
  while (mapstr.ReadNextLine()){
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;

    if (mapstr.HasVariablePair("=",varname,varvalue)){
      //  This is a declaration line.  Decode it.
      varname.ToLower();
      UInt_t value = QwParameterFile::GetUInt(varvalue);

      if (varname=="roc")
	{
	  currentrocread=value;
	  RegisterROCNumber(value,0);
	  fWordsPerSubbank.push_back( std::pair<Int_t, Int_t>(fWord.size(),fWord.size()));
	}
      else if (varname=="bank")
	{
	  currentbankread=value;
	  RegisterSubbank(value);
	  fWordsPerSubbank.push_back( std::pair<Int_t, Int_t>(fWord.size(),fWord.size()));
	}
      else if (varname=="patternphase")
	{
	  fMaxPatternPhase=value;
	  //std::cout<<" fMaxPatternPhase "<<fMaxPatternPhase<<std::endl;
	}
      else if (varname=="numberpatternsdelayed")
	{
	  SetHelicityDelay(value);
	}
      else if (varname=="randseedbits")
	{
	  if (value==30){
	    BIT24=kFALSE;
	    BIT30=kTRUE;
	  }
	}
      else if (varname=="patternphaseoffset")
	{
	  fPATTERNPHASEOFFSET=value;
	  //std::cout<<" PATTERNPHASEOFFSET "<<fPATTERNPHASEOFFSET<<std::endl;

	}
      else if (varname=="helicitydecodingmode")
	{
	  if (varvalue=="InputRegisterMode") {
	    std::cout<<" **** Input Register Mode **** "<<std::endl;
	    fHelicityDecodingMode=kHelInputRegisterMode;
	  }
	  else if (varvalue=="UserbitMode"){
	    std::cout<<" **** Userbit Mode **** "<<std::endl;
	    fHelicityDecodingMode=kHelUserbitMode;
	  }
	  else if (varvalue=="HelLocalyMadeUp"){
	    std::cout<<"**** Helicity Locally Made Up ****"<<std::endl;
	    fHelicityDecodingMode=kHelLocalyMadeUp;
	  }
	  else
	    {
	      std::cerr<<"The helicity decoding mode read in file "<<mapfile<<"\n";
	      std::cerr<<" is not recognized in function QwHelicity::LoadChannelMap \n";
	      std::cerr<<" Quiting this execution \n";
	    }
	}
    } else
      {
	Bool_t lineok=kTRUE;
	//  Break this line into tokens to process it.
	modtype   = mapstr.GetNextToken(", ").c_str();	// module type
	modnum    = (atol(mapstr.GetNextToken(", ").c_str()));	//slot number
	channum   = (atol(mapstr.GetNextToken(", ").c_str()));	//channel number
	dettype   = mapstr.GetNextToken(", ").c_str();	//type-purpose of the detector
	dettype.ToLower();
	namech    = mapstr.GetNextToken(", ").c_str();  //name of the detector
	namech.ToLower();
	keyword = mapstr.GetNextToken(", ").c_str();
	keyword.ToLower();
	// Notice that "namech" and "keyword" are now forced to lower-case.

	if(currentsubbankindex!=GetSubbankIndex(currentrocread,currentbankread))
	  {
	    currentsubbankindex=GetSubbankIndex(currentrocread,currentbankread);
	    wordsofar=0;
	  }

	if(modtype=="SKIP"){
	  if (modnum<=0) wordsofar+=1;
	  else           wordsofar+=modnum;
	}
	else if(modtype!="WORD"|| dettype!="helicitydata")
	  {
	    std::cerr << "QwHelicity::LoadChannelMap:  Unknown detector type: "
		      << dettype <<", the detector "<<namech<<" will not be decoded "
		      << std::endl;
	    lineok=kFALSE;
	    continue;
	  }
	else
	  {
	    QwWord localword;
	    localword.fSubbankIndex=currentsubbankindex;
	    localword.fWordInSubbank=wordsofar;
	    wordsofar+=1;
	    // I assume that one data = one word here. But it is not always the case, for
	    // example the triumf adc gives 6 words per channel
	    localword.fModuleType=modtype;
	    localword.fWordName=namech;
	    localword.fWordType=dettype;
	    fWord.push_back(localword);
	    fWordsPerSubbank[currentsubbankindex].second = fWord.size();
	    if(ldebug) std::cout<<"--"<<namech<<"--"<<fWord.size()-1<<"\n";

	    // Notice that "namech" is in lower-case, so these checks
	    // should all be in lower-case
	    switch (fHelicityDecodingMode)
	      {
	      case kHelUserbitMode :
		if(namech.Contains("userbit")) kUserbit=fWord.size()-1;
		if(namech.Contains("scalercounter")) kScalerCounter=fWord.size()-1;
		break;
	      case kHelInputRegisterMode :
		if(namech.Contains("input_register")) kInputRegister= fWord.size()-1;
		if(namech.Contains("mps_counter")) kMpsCounter= fWord.size()-1;
		if(namech.Contains("pat_counter")) kPatternCounter= fWord.size()-1;
		if(namech.Contains("pat_phase")) kPatternPhase= fWord.size()-1;
		break;
	      }
	  }
      }
  }

  if(ldebug)
    {
      std::cout<<"Done with Load map channel \n";
      for(size_t i=0;i<fWord.size();i++)
	fWord[i].PrintID();
      std::cout<<" kUserbit="<<kUserbit<<"\n";

    }
  ldebug=kFALSE;

  //Read the cmd options and override channel map settings
  if (gQwOptions.HasValue("helicity.patternoffset"))
    if (gQwOptions.GetValue<int>("helicity.patternoffset")==1 || gQwOptions.GetValue<int>("helicity.patternoffset")==0)
      fPATTERNPHASEOFFSET=gQwOptions.GetValue<int>("helicity.patternoffset");

  if (gQwOptions.HasValue("helicity.patternphase"))
    if (gQwOptions.GetValue<int>("helicity.patternphase")==4 || gQwOptions.GetValue<int>("helicity.patternphase")==8)
      fMaxPatternPhase=gQwOptions.GetValue<int>("helicity.patternphase");

  if (gQwOptions.HasValue("helicity.30bitseed")){
    BIT30=gQwOptions.GetValue<bool>("helicity.30bitseed");
    BIT24=kFALSE;
  }else if (gQwOptions.HasValue("helicity.24bitseed")){
    BIT24=gQwOptions.GetValue<bool>("helicity.24bitseed");
    BIT30=kFALSE;
  }
  if (gQwOptions.HasValue("helicity.delay")){
    std::cout<<" Helicity Delay ="<<gQwOptions.GetValue<int>("helicity.delay")<<"\n";
    SetHelicityDelay(gQwOptions.GetValue<int>("helicity.delay"));
  }

  return 0;
};
//////////////////////////////////////////////////////

Int_t QwHelicity::LoadEventCuts(TString filename){
  return 0;
};


//////////////////////////////////////////////////////
Int_t QwHelicity::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  Bool_t lkDEBUG=kFALSE ;

  Int_t index = GetSubbankIndex(roc_id,bank_id);

  if (index>=0 && num_words>0){
    //  We want to process this ROC.  Begin loopilooping through the data.
    if (lkDEBUG)
      {
	std::cout << "QwHelicity::ProcessEvBuffer:  "
		  << "Begin processing ROC" << roc_id
		  << " and subbank "<<bank_id
		  << " number of words="<<num_words<<std::endl;
      }

    for(Int_t i=fWordsPerSubbank[index].first; i<fWordsPerSubbank[index].second; i++)
      {
	if(fWord[i].fWordInSubbank+1<= (Int_t) num_words)
	  {
	    fWord[i].fValue=buffer[fWord[i].fWordInSubbank];
	  }
	else
	  {
	    std::cout<<"There is not enough word in the buffer to read data for "
		     <<fWord[i].fWordName<<"\n";
	    std::cout<<"words in this buffer:"<<num_words<<" tyring to read woord number ="
		     <<fWord[i].fWordInSubbank<<"\n";
	  }
      }
    if(lkDEBUG)
      {
	std::cout<<"Done with Processing this event \n";
	for(size_t i=0;i<fWord.size();i++) {
	  std::cout<<" word number = "<<i<<" ";
	  fWord[i].Print();
	}
      }
  }
  lkDEBUG=kFALSE;
  return 0;
}


//**************************************************//
Int_t QwHelicity::GetHelicityReported()
{
  return fHelicityReported;
};

Int_t QwHelicity::GetHelicityActual()
{
  return fHelicityActual;
};

Int_t QwHelicity::GetHelicityDelayed()
{
  return fHelicityDelayed;
};

Long_t QwHelicity::GetPatternNumber()
{
  return  fPatternNumber;
};

Long_t QwHelicity::GetEventNumber()
{
  return fEventNumber;
};

Int_t QwHelicity::GetPhaseNumber()
{
  return fPatternPhaseNumber;
};

void QwHelicity::SetEventPatternPhase(Int_t event, Int_t pattern, Int_t phase)
{
  fEventNumber = event;
  fPatternNumber = pattern;
  fPatternPhaseNumber = phase;
};

//**************************************************//
void QwHelicity::SetHistoTreeSave(TString &prefix)
{
  if(prefix=="asym_")
    fHistoType=kHelNoSave;
  else if (prefix=="yield_")
    fHistoType=kHelSavePattern;
  else
    fHistoType=kHelSaveMPS;

  return;
}

//**************************************************//
void  QwHelicity::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  SetHistoTreeSave(prefix);
  std::cout<<"QwHelicity::ConstructHistograms with prefix "<<prefix<<" and fHistoType="<<fHistoType<<"\n";
  if (folder != NULL) folder->cd();
  TString basename;
  size_t index=0;

  if(fHistoType==kHelNoSave)
    {
      //do nothing
    }
  else if(fHistoType==kHelSavePattern)
    {
      fHistograms.resize(1+fWord.size(), NULL);
      basename="pattern_polarity";
      fHistograms[index]   = gQwHists.Construct1DHist(basename);
      index+=1;
      for (size_t i=0; i<fWord.size(); i++){
	basename="hel_"+fWord[i].fWordName;
	fHistograms[index]   = gQwHists.Construct1DHist(basename);
	index+=1;
      }
    }
  else if(fHistoType==kHelSaveMPS)
    {
      fHistograms.resize(4+fWord.size(), NULL);
      //eventnumber, patternnumber, helicity, patternphase + fWord.size
      basename=prefix+"delta_event_number";
      fHistograms[index]   = gQwHists.Construct1DHist(basename);
      index+=1;
      basename=prefix+"delta_pattern_number";
      fHistograms[index]   = gQwHists.Construct1DHist(basename);
      index+=1;
      basename=prefix+"pattern_phase";
      fHistograms[index]   = gQwHists.Construct1DHist(basename);
      index+=1;
      basename=prefix+"helicity";
      fHistograms[index]   = gQwHists.Construct1DHist(basename);
      index+=1;
      for (size_t i=0; i<fWord.size(); i++){
	basename=prefix+fWord[i].fWordName;
	fHistograms[index]   = gQwHists.Construct1DHist(basename);
	index+=1;
      }
    }
  else
    std::cerr<<"QwHelicity::ConstructHistograms this prefix--"<<prefix<<"-- is not unknown:: no histo created";

  return;
}

void  QwHelicity::DeleteHistograms()
{
    std::cout<<"QwHelicity::DeleteHistograms for system="<<fSystemName<<"; fHistograms.size="<<fHistograms.size()<<"\n";
  if((fHistoType==kHelSaveMPS)||(fHistoType==kHelSavePattern))
    {
      for (size_t i=0; i<fHistograms.size(); i++){
	if (fHistograms.at(i) != NULL){
	  fHistograms.at(i)->Delete();
	  fHistograms.at(i) =  NULL;
	}
      }
      fHistograms.clear();
    }
  return;
}


void  QwHelicity::FillHistograms()
{
  Bool_t localdebug=kFALSE;
  size_t index=0;
  if(fHistoType==kHelNoSave)
    {
      //do nothing
    }
  else if(fHistoType==kHelSavePattern)
    {
      if(localdebug) std::cout<<"QwHelicity::FillHistograms helicity info \n";
      if(localdebug) std::cout<<"QwHelicity::FillHistograms  pattern polarity="<<fActualPatternPolarity<<"\n";
      fHistograms[index]->Fill(fActualPatternPolarity);
      index+=1;
      for (size_t i=0; i<fWord.size(); i++){
	fHistograms[index]->Fill(fWord[i].fValue);
	index+=1;
	if(localdebug) std::cout<<"QwHelicity::FillHistograms "<<fWord[i].fWordName<<"="<<fWord[i].fValue<<"\n";
      }
    }
  else if(fHistoType==kHelSaveMPS)
    {
      if(localdebug) std::cout<<"QwHelicity::FillHistograms mps info \n";
      fHistograms[index]->Fill(fEventNumber-fEventNumberOld);
      index+=1;
      fHistograms[index]->Fill(fPatternNumber-fPatternNumberOld);
      index+=1;
      fHistograms[index]->Fill(fPatternPhaseNumber);
      index+=1;
      fHistograms[index]->Fill(fHelicityActual);
      index+=1;
      for (size_t i=0; i<fWord.size(); i++){
	fHistograms[index]->Fill(fWord[i].fValue);
	index+=1;
	if(localdebug) std::cout<<"QwHelicity::FillHistograms "<<fWord[i].fWordName<<"="<<fWord[i].fValue<<"\n";
      }
    }

  return;
}

/////////////////////////////////////////////////////////////////

void  QwHelicity::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  SetHistoTreeSave(prefix);
  //  std::cout<<"QwHelicity::ConstructBranchAndVector  prefix="<<prefix<<" fHistoType="<<fHistoType<<"\n";


  fTreeArrayIndex  = values.size();
  size_t localindex=fTreeArrayIndex;
  TString basename;
  if(fHistoType==kHelNoSave)
    {
      //do nothing
    }
  else if(fHistoType==kHelSaveMPS)
    {
      /*
      basename = "helicity";
      values.push_back(0.0);
      tree->Branch(basename, &(values[localindex]),basename+"/D");
      //
      */
      basename = "actual_helicity";    //predicted actual helicity before being delayed.
      values.push_back(0.0);
      tree->Branch(basename, &(values[localindex]),basename+"/D");
      //
      basename = "delayed_helicity";   //predicted delayed helicity
      values.push_back(0.0);
      tree->Branch(basename, &(values[localindex]),basename+"/D");
      //
      basename = "reported_helicity";  //delayed helicity reported by the input register.
      values.push_back(0.0);
      tree->Branch(basename, &(values[localindex]),basename+"/D");
      localindex  = values.size();
      //
      basename = "pattern_phase";
      values.push_back(0.0);
      tree->Branch(basename, &(values[localindex]),basename+"/D");
     //
      localindex  = values.size();
      basename = "pattern_number";
      values.push_back(0.0);
      tree->Branch(basename, &(values[localindex]),basename+"/D");
      //
      localindex  = values.size();
      basename = "event_number";
      values.push_back(0.0);
      tree->Branch(basename, &(values[localindex]),basename+"/D");
      //
      for (size_t i=0; i<fWord.size(); i++)
	{
	  localindex  = values.size();
	  basename = fWord[i].fWordName;
	  values.push_back(0.0);
	  tree->Branch(basename, &(values[localindex]),basename+"/D");
	}
    }
  else if(fHistoType==kHelSavePattern)
    {
      /*
      basename = "pattern_polarity";
      values.push_back(0.0);
      tree->Branch(basename, &(values[localindex]),basename+"/D");
      //
      */
      basename = "actual_pattern_polarity";
      values.push_back(0.0);
      tree->Branch(basename, &(values[localindex]),basename+"/D");
      //
      basename = "delayed_pattern_polarity";
      values.push_back(0.0);
      tree->Branch(basename, &(values[localindex]),basename+"/D");
      //

      localindex  = values.size();
      basename = "pattern_number";
      values.push_back(0.0);
      tree->Branch(basename, &(values[localindex]),basename+"/D");
      //
      for (size_t i=0; i<fWord.size(); i++)
	{
	  localindex  = values.size();
	  basename = fWord[i].fWordName;
	  values.push_back(0.0);
	  tree->Branch(basename, &(values[localindex]),basename+"/D");

	}
    }

  return;
}

/////////////////////////////////////////////////////////////////
void  QwHelicity::FillTreeVector(std::vector<Double_t> &values)
{
  //std::cout<<"QwHelicity::FillTreeVector fPatternNumber="<<fPatternNumber<<"\n";
  //if(ldebug)

  size_t index=fTreeArrayIndex;
  if(fHistoType==kHelSaveMPS)
    {
      values[index++] = fHelicityActual;
      values[index++] = fHelicityDelayed;
      values[index++] = fHelicityReported;
      values[index++] = fPatternPhaseNumber;
      values[index++] = fPatternNumber;
      values[index++] = fEventNumber;
      for (size_t i=0; i<fWord.size(); i++)
	values[index++] = fWord[i].fValue;
    }
  else if(fHistoType==kHelSavePattern)
    {
      values[index++] = fActualPatternPolarity;
      values[index++] = fDelayedPatternPolarity;
      values[index++] = fPatternNumber;
      for (size_t i=0; i<fWord.size(); i++)
	values[index++] = fWord[i].fValue;
    }

  return;
}

void  QwHelicity::FillDB(QwDatabase *db, TString type)
{
  if (type=="yield" || type=="asymmetry")
    return;

  db->Connect();
  mysqlpp::Query query = db->Query();

  Char_t s_number[20];
  string s_sql = "INSERT INTO seeds (seed, comment) VALUES (";
  sprintf(s_number, "%d,", this->GetRandomSeedActual());
  s_sql += string(s_number);
  s_sql += " \'actual random seed\')";
  //std::cout<<s_sql<<std::endl;
  query <<s_sql;
  query.execute();
  db->Disconnect();
};
/////////////////////////////////////////////////////////////////
void QwHelicity::SetFirst24Bits(UInt_t seed)
{
  // This gives the predictor a quick start
  n_ranbits = 24;
  UShort_t first24bits[n_ranbits];
  for (unsigned int i = 0; i < n_ranbits; i++) first24bits[i] = (seed >> i) & 0x1;
  // Set delayed seed
  iseed_Delayed = GetRandomSeed(first24bits);
  // Progress actual seed by the helicity delay
  iseed_Actual = iseed_Delayed;
  for (int i = 0; i < fHelicityDelay; i++) GetRandbit(iseed_Actual);
}
/////////////////////////////////////////////////////////////////
UInt_t QwHelicity::GetRandbit(UInt_t& ranseed){
  Bool_t status = false;

  if (BIT24)
    status=GetRandbit24(ranseed);
  if (BIT30)
    status=GetRandbit30(ranseed);

  return status;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
UInt_t QwHelicity::GetRandbit24(UInt_t& ranseed)
{
  /* This is a 24 bit random bit generator according to the "new" algorithm
     described in "G0 Helicity Digital Controls" by E. Stangland, R. Flood, H. Dong.
   */

  /* The helicity board uses a maximum-length linear feedback shift registers
     for the generation of a pseudo-random sequence of bits.  The length of the
     register (24 bits or 30 bits) defines the length before a sequence is
     repeated: 2^n - 1.

     For a mathematical introduction to the generation of pseudo-random numbers
     with maximum-length linear feedback shift registers (LFSR), see the
     following web references:
       http://en.wikipedia.org/wiki/Linear_feedback_shift_register
       http://www.newwaveinstruments.com/resources/articles/m_sequence_linear_feedback_shift_register_lfsr.htm

     In particular, the used solutions are to place XNOR taps at the bits
      24 stages, 4 taps:  (47 sets)
       [24, 23, 21, 20]
      30 stages, 4 taps:  (104 sets)
       [30, 29, 28, 7]

     The 24 stage solution we use has been mirrored by transforming [m, A, B, C]
     into [m, m-C, m-B, m-A].  This goes through the sequence in the opposite
     direction.
   */

  const UInt_t IB1 = 1;                     //Bit 1 of shift register 000000000000000000000001
  const UInt_t IB3 = 4;                     //Bit 3 of shift register 000000000000000000000100
  const UInt_t IB4 = 8;                     //Bit 4 of shift register 000000000000000000001000
  const UInt_t IB24 = 8388608;               //Bit 24 of shift register 100000000000000000000000
  const UInt_t MASK = IB1+IB3+IB4+IB24;     //Sum of the four feedback bits is 100000000000000000001101

  UInt_t result; //The generated pattern

  if(ranseed<=0)
    {
      std::cerr<<"ranseed must be greater than zero!"<<"\n";
      result = 0;
    }

  if(ranseed & IB24) // if bit 24 of ranseed = 1, then output 1
    {
      ranseed = ((ranseed^MASK)<<1|IB1);
      result = 1;
    }
  else
    {
      ranseed<<=1;
      result = 0;
    }
  return(result);

};
///////////////////////////////////////////////////////////////////////////////////////////////////
UInt_t QwHelicity::GetRandbit30(UInt_t& ranseed)
{
  /* For an explanation of the algorithm, see above in GetRandbit24() */

  UInt_t bit7    = (ranseed & 0x00000040) != 0;
  UInt_t bit28   = (ranseed & 0x08000000) != 0;
  UInt_t bit29   = (ranseed & 0x10000000) != 0;
  UInt_t bit30   = (ranseed & 0x20000000) != 0;

  UInt_t result = (bit30 ^ bit29 ^ bit28 ^ bit7) & 0x1;

  if(ranseed<=0) {
    std::cerr<<"ranseed must be greater than zero!"<<"\n";
    result = 0;
  }
  ranseed =  ( (ranseed<<1) | result ) & 0x3FFFFFFF;

  return(result);
};
///////////////////////////////////////////////////////////////////////////////////////////////////

UInt_t QwHelicity::GetRandomSeed(UShort_t* first24randbits)
{
  Bool_t ldebug=0;
  if(ldebug)std::cout<<" Entering QwHelicity::GetRandomSeed \n";

  /*  This the random seed generator used in G0 (L.Jianglai)
      Here we get the 24 random bits and derive the randome seed from that.
      randome seed                      : b24 b23 b22.....b2 b1
      first 24 random bit from this seed: h1 h2 h3 ....h23 h24
      we have,
      b23 = h1, b22 = h2,... b5 = h20,
      h21^b24 = b4 , h3^b24^b23 = b3 ,h23^b23^b22 = b2, h24^b22^b24 = b1.
      Thus by using h1,...h24, we can derive the b24,..b1 of the randseed.
  */

  UShort_t b[25];
  UInt_t ranseed = 0;

  if(ldebug)
    {
     for(size_t i=0;i<25;i++)
       std::cout<<i<<" : "<<first24randbits[i]<<"\n";
    }

  for(size_t i=24;i>=5;i--)   b[i]= first24randbits[24-i+1]; //fill h24..h5

  // fill b[4] to b[1]
  b[4] = first24randbits[21]^b[24]; //h21^b24 = b4
  b[3] = first24randbits[22]^b[24]^b[23]; //h22^b24^b23 = b3
  b[2] = first24randbits[23]^b[23]^b[22];// h23^b23^b22 = b2
  b[1] = first24randbits[24]^b[21]^b[22]^b[24];// h24^b22^b24 = b1

  ///assign the values in the h aray and into the sead
  for(size_t i=24;i>=1;i--)  ranseed=ranseed<<1|(b[i]&1);

  ranseed = ranseed&0xFFFFFF; //put a mask

  if(ldebug)std::cout<<" seed ="<<ranseed<<"\n";
  if(ldebug)std::cout<<" Exiting QwHelicity::GetRandomSeed \n";


  return ranseed;

}

///////////////////////////////////////////////////////////////////////////////////////////////////2-20-2009

// Helicity Prediction Functions

///////////////////////////////////////////////////////////////////////////////////////////////////

void QwHelicity::RunPredictor()
{
  Int_t ldebug = 0;

  //if(ldebug)  std::cerr<<"Entering QwHelicity::RunPredictor \n";
  if(ldebug)  std::cerr<<"Entering QwHelicity::RunPredictor for fEventNumber, "<<fEventNumber
		       <<", fPatternNumber, "<<fPatternNumber
		       << ", and fPatternPhaseNumber, "<<fPatternPhaseNumber<<std::endl;//Paul's modifications

  //Update the random seed if the new event is from a different pattern.
  //Have to write the for loop!

  for( size_t i=0;i<size_t(fPatternNumber- fPatternNumberOld);i++) //got a new pattern
    {
      fActualPatternPolarity = GetRandbit(iseed_Actual);
      fDelayedPatternPolarity= GetRandbit(iseed_Delayed);
      if(ldebug)std::cout<<"Predicting : seed actual, delayed: "<< iseed_Actual<<":"<<iseed_Delayed<<"\n";
    }

  //Predict the helicity according to pattern
  //  Defined patterns:
  //    Pair:    +-       or -+
  //    Quartet: +--+     or -++-
  //    Octet:   +--+-++- or -++-+--+
  //    Symmetric octet:  +-+--+-+ or -+-++-+-
  Int_t localphase = fPatternPhaseNumber-fMinPatternPhase;//Paul's modifications
   if ((fMaxPatternPhase<=4) || (1==1)){//Paul's modifications
    switch(localphase){
    case 0: //phase 0 -> new pattern, fHelicityActual has been predicteed in the for loop.
    case 3:
    case 5:
    case 6:
      fHelicityActual  = fActualPatternPolarity;  //DON'T flip helicity
      fHelicityDelayed = fDelayedPatternPolarity; //DON'T flip helicity
      break;
    case 1:
    case 2: //phase 2 helicity  = phase 1 helicity
    case 4: //phase 4 helicity  = phase 1 helicity
    case 7: //phase 7 helicity  = phase 4 helicity
      fHelicityActual  = fActualPatternPolarity  ^ 0x1; //flip helicity
      fHelicityDelayed = fDelayedPatternPolarity ^ 0x1; //flip helicity
      break;

    default: //for values other than 1,2,3,4. //This would be discovered in error checking.
      ResetPredictor();
      break;
    }
  } else {
    //  This is the Symmetric octet:  +-+--+-+ or -+-++-+-
    switch(localphase){
    case 0: //phase 1 -> new pattern, fHelicityActual has been predicteed in the for loop.
    case 2:
    case 5:
    case 7:
      fHelicityActual  = fActualPatternPolarity;  //DON'T flip helicity
      fHelicityDelayed = fDelayedPatternPolarity; //DON'T flip helicity
      break;
    case 1:
    case 3:
    case 4:
    case 6:
      fHelicityActual  = fActualPatternPolarity  ^ 0x1; //flip helicity
      fHelicityDelayed = fDelayedPatternPolarity ^ 0x1; //flip helicity
      break;
    default: //for values other than 1,2,3,4. //This would be discovered in error checking.
      ResetPredictor();
      break;
    }
   }
  if(ldebug)
    {
      std::cout<<"Predicted Polarity ::: Delayed ="
	       <<fDelayedPatternPolarity<<" Actual ="
	       <<fActualPatternPolarity<<"\n";
      std::cout<<"Predicted Helicity ::: Delayed Helicity="<<fHelicityDelayed
	       <<" Actual Helicity="<<fHelicityActual<<" Reported Helicity="<<fHelicityReported<<"\n";
      std::cerr<<"Exiting QwHelicity::RunPredictor \n";

    }

  return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Bool_t QwHelicity::CollectRandBits()
{
  Bool_t status = false;

  if (BIT24)
    status=CollectRandBits24();
  if (BIT30)
    status=CollectRandBits30();
  return status;
}


///////////////////////////////////////////////////////////////////////////////////////////////////

Bool_t QwHelicity::CollectRandBits24()
{
    //routine to collect 24 random bits before getting the randseed for prediction
    Bool_t  ldebug = kFALSE;
    const UInt_t ranbit_goal = 24;

  if(ldebug) std::cout<<"QwHelicity::Entering CollectRandBits24...."<<"\n";


  if (n_ranbits==ranbit_goal)    return kTRUE;

  if(n_ranbits<ranbit_goal&&fPatternPhaseNumber==fMinPatternPhase)
    {
      std::cerr<<"Collecting information from event #"<<fEventNumber<<" to generate helicity seed ";
      std::cerr<<"(need 24 bit, so far got "<<n_ranbits<<" bits )\n";
    }


  static UShort_t first24bits[25]; //array to store the first 24 bits

  fGoodHelicity = kFALSE; //reset before prediction begins

  if(IsContinuous())
    {
      if((fPatternPhaseNumber==fMinPatternPhase)&& (fPatternNumber>=0))
	{
	  first24bits[n_ranbits+1] = fHelicityReported;
	  n_ranbits ++;
	  if(ldebug)
	    {
	      std::cout<<" event number"<<fEventNumber<<", fPatternNumber"
		       <<fPatternNumber<<", n_ranbit"<<n_ranbits
		       <<", fHelicityReported"<<fHelicityReported<<"\n";
	    }

	  if(n_ranbits == ranbit_goal ) //If its the 24th consecative random bit,
	    {
	       if(ldebug)
		 {
		   std::cout<<"Collected 24 random bits. Get the random seed for the predictor."<<"\n";
		   for(UInt_t i=0;i<ranbit_goal;i++) std::cout<<" i:bit ="<<i<<":"<<first24bits[i]<<"\n";
		 }
	      iseed_Delayed = GetRandomSeed(first24bits);
	      //This random seed will predict the helicity of the event (24+fHelicityDelay) patterns  before;
	      // run GetRandBit 24 times to get the delayed helicity for this event
	       if(ldebug)std::cout<<"The reported seed 24 patterns ago = "<<iseed_Delayed<<"\n";

	      for(UInt_t i=0;i<ranbit_goal;i++) fDelayedPatternPolarity =GetRandbit(iseed_Delayed);
	      fHelicityDelayed = fDelayedPatternPolarity;
	      //The helicity of the first phase in a pattern is
	      //equal to the polarity of the pattern

	      //Check whether the reported helicity is the same as the helicity predicted by the random seed

	      if(fHelicityDelay >=0){
		iseed_Actual = iseed_Delayed;
		for(Int_t i=0; i<fHelicityDelay; i++)
		  {
		    if(ldebug) std::cout<<"Delaying helicity \n";
		    fActualPatternPolarity = GetRandbit(iseed_Actual);
		  }
	      }
	      else
		{
		  std::cerr<<"QwHelicity::CollectRandBits  We cannot handle negative delay(prediction) in the reported helicity. Exiting."<<"\n";
		  ResetPredictor();
		}

	      fHelicityActual =  fActualPatternPolarity;
	    }
	}
    }
  else // while collecting the seed, we encounter non continuous events.
    {
      ResetPredictor();
      std::cerr<<"QwHelicity::CollectRandBits, while collecting the seed, we encountered non continuous events: need to reset the seed collecting \n";
      std::cerr<<" event number="<<fEventNumber<<", fPatternNumber="
	       <<fPatternNumber<<",  fPatternPhaseNumber="<<fPatternPhaseNumber<<"\n";
    }

      //else n randbits have been set to zero in the error checking routine
      //start over from the next pattern
  if(ldebug) std::cout<<"QwHelicity::CollectRandBits24 => Done collecting ...\n";

  return kFALSE;

};
///////////////////////////////////////////////////////////////////////////////////////////////////

Bool_t QwHelicity::CollectRandBits30()
{
    Bool_t  ldebug = kFALSE;

  const UInt_t ranbit_goal = 30;
  if (n_ranbits==ranbit_goal)    return kTRUE;

  if(n_ranbits<ranbit_goal&&fPatternPhaseNumber==fMinPatternPhase) {
    std::cerr<<"Collecting information (";
    if (fHelicityReported==1) std::cerr<<"+";
    else                      std::cerr<<"-";
    std::cerr<<") from event #"<<fEventNumber<<" to generate helicity seed ";
    std::cerr<<"(need " << ranbit_goal << " bit, so far got "<<n_ranbits<<" bits )\n";
  }

  fGoodHelicity = kFALSE; //reset before prediction begins

  if(IsContinuous()) {
    if((fPatternPhaseNumber==fMinPatternPhase)&& (fPatternNumber>=0)) {
      iseed_Delayed = ((iseed_Delayed<<1)&0x3FFFFFFF)|fHelicityReported;
      if(ldebug) std::cout<<"QwHelicity:: CollectRandBits30, Collecting randbit "<<n_ranbits<<"..\n";
      n_ranbits++;

      if(n_ranbits == ranbit_goal){
	if(ldebug) std::cout<<"QwHelicity:: CollectRandBits30, done Collecting 30 randbits\n";
	fDelayedPatternPolarity = fHelicityReported;
	if(ldebug) std::cout<<"QwHelicity:: CollectRandBits30, delayedpatternpolarity ="<<fDelayedPatternPolarity<<"\n";
	fHelicityDelayed = fDelayedPatternPolarity;
	if(fHelicityDelay >=0){
	  iseed_Actual = iseed_Delayed;
	  for(Int_t i=0; i<fHelicityDelay; i++) {
	    fActualPatternPolarity = GetRandbit(iseed_Actual);
	  }
	} else {
	  std::cerr<<"QwHelicity::CollectRandBits30,  We cannot handle negative delay(prediction) in the reported helicity. Exiting."<<"\n";
	  ResetPredictor();
	}
	fHelicityActual =  fActualPatternPolarity;
      }
    }
  } else {
    // while collecting the seed, we encounter non continuous events.
    ResetPredictor();
    std::cerr<<"QwHelicity::CollectRandBits30, while collecting the seed, we encountered non continuous events: need to reset the seed collecting \n";
    std::cerr <<" event number="<<fEventNumber<<", fPatternNumber="
	      <<fPatternNumber<<",  fPatternPhaseNumber="<<fPatternPhaseNumber<<"\n";
  }
  return kFALSE;
};
///////////////////////////////////////////////////////////////////////////////////////////////////

void QwHelicity::PredictHelicity()
{
   Bool_t ldebug=kFALSE;

  if(ldebug)  std::cout<<"Entering QwHelicity::PredictHelicity \n";
  //Routine to predict the true helicity from the delayed helicity.
  //Helicities are delayed by 8 events or 2 quartets
  if(CollectRandBits()) {
    //After accumulating 24/30 helicity bits, iseed id up-to-date
    //If nothing goes wrong, n-ranbits will stay as 24/30
    //Reset it to zero if something goes wrong
    if(ldebug)  std::cout<<"QwHelicity::PredictHelicity=>Predicting the  helicity \n";
    RunPredictor();
    if(!IsGoodHelicity())
      ResetPredictor();
  }

  if(ldebug)  std::cout<<"n_ranbit exiting the function = "<<n_ranbits<<"\n";

  return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void QwHelicity::SetHelicityDelay(Int_t delay)
{
  //  Int_t locali=kUndefinedHelicity;
  //Sets the number of bits the helicity reported gets delayed with
  if(delay>=0)
    {
      fHelicityDelay = delay;

      //       for(int i=0;i<delay+1;i++)
      // 	fCheckHelicityDelay.push_back(locali);
      // this is obsolete
      //basically we are creating a ring to store the ActualHelicity.
      //The Actualhelcity for this event goes in position 0, the actualhelicity for the previous event
      //is in position 1, and the actual helicity from fHelicityDelay events is in event delay and
      //it should match the reported helicity
    }
  else
    std::cerr<<"QwHelicity::SetHelicityDelay We cannot handle negative delay in the prediction of delayed helicity. Exiting.. \n";

  return;
}

  //start a new helicity prediction sequence

void QwHelicity::ResetPredictor()
{
  n_ranbits = 0;
  fGoodHelicity = kFALSE;
  fGoodPattern = kFALSE;
  return;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
void QwHelicity::Copy(VQwSubsystem *source)
{
 try
    {
     if(typeid(*source)==typeid(*this))
	{
	  VQwSubsystem::Copy(source);
	  //QwHelicity* input=((QwHelicity*)source);
	  VQwSubsystem::Copy(source);
          QwHelicity* input = dynamic_cast<QwHelicity*>(source);
	  this->fWord.resize(input->fWord.size());
	  for(size_t i=0;i<this->fWord.size();i++)
	    {
	      this->fWord[i].fWordName=input->fWord[i].fWordName;
	      this->fWord[i].fModuleType=input->fWord[i].fModuleType;
	      this->fWord[i].fWordType=input->fWord[i].fWordType;
	    }
	  this->kUserbit = input->kUserbit;
	}
      else
	{
	  TString loc="Standard exception from QwHelicity::Copy = "
	    +source->GetSubsystemName()+" "
	    +this->GetSubsystemName()+" are not of the same type";
	  throw std::invalid_argument(loc.Data());
	}
    }
  catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
    }

  return;
}

VQwSubsystem*  QwHelicity::Copy()
{

  QwHelicity* TheCopy=new QwHelicity("Helicity Copy");
  TheCopy->Copy(this);
  return TheCopy;
}

VQwSubsystem&  QwHelicity::operator=  (VQwSubsystem *value)
{

  if(Compare(value))
    {
      //QwHelicity* input= (QwHelicity*)value;
      VQwSubsystem::operator=(value);
      QwHelicity* input= dynamic_cast<QwHelicity*>(value);
      for(size_t i=0;i<input->fWord.size();i++)
	this->fWord[i].fValue=input->fWord[i].fValue;
      this->fHelicityActual=input->fHelicityActual;
      this->fPatternNumber =input->fPatternNumber ;
      this->fPatternPhaseNumber=input->fPatternPhaseNumber;
      this->fEventNumber=input->fEventNumber;
      this->fActualPatternPolarity=input->fActualPatternPolarity;
      this->fDelayedPatternPolarity=input->fDelayedPatternPolarity;
      this->fHelicityReported=input->fHelicityReported;
      this->fHelicityActual=input->fHelicityActual;
      this->fHelicityDelayed=input->fHelicityDelayed;
      this->fHelicityBitPlus=input->fHelicityBitPlus;
      this->fHelicityBitMinus=input->fHelicityBitMinus;
      this->fGoodHelicity=input->fGoodHelicity;
      this->fGoodPattern=input->fGoodPattern;
      /*
      std::cerr << "QwHelicity::operator=:  "
		<< " input->fEventNumber=="<< input->fEventNumber
		<< " input->fPatternNumber=="<< input->fPatternNumber
		<< " input->fPatternPhaseNumber=="<< input->fPatternPhaseNumber
		<< " this->fEventNumber=="<< this->fEventNumber
		<< " this->fPatternNumber=="<< this->fPatternNumber
		<< " this->fPatternPhaseNumber=="<< this->fPatternPhaseNumber
		<< std::endl;
      */
    }
  return *this;
};

VQwSubsystem&  QwHelicity::operator+=  (VQwSubsystem *value)
{
  Bool_t localdebug=kFALSE;
  if(localdebug) std::cout<<"Entering QwHelicity::operator+= adding "<<value->GetSubsystemName()<<" to "<<this->GetSubsystemName()<<" \n";

  //this routine is most likely to be called during the computatin of assymetry
  //this call doesn't make too much sense for this class so the following lines
  //are only use to put safe gards testing for example if the two instantiation indeed
  // refers to elements in the same pattern
  if(Compare(value))
    {
      //QwHelicity* input= (QwHelicity*)value;
      QwHelicity* input= dynamic_cast<QwHelicity*>(value);
      for(size_t i=0;i<input->fWord.size();i++)
	{
	  if(this->fWord[i].fValue!=input->fWord[i].fValue)
	    this->fWord[i].fValue=-999999;
	}
      if(localdebug) std::cout<<"QwHelicity::operator+= this->fPatternNumber="<<this->fPatternNumber;
      if(localdebug) std::cout<<"input->fPatternNumber="<<input->fPatternNumber<<"\n";

      if(this->fPatternNumber!=input->fPatternNumber)
	this->fPatternNumber=-999999;
      if(this->fActualPatternPolarity!=input->fActualPatternPolarity)
	this->fPatternNumber=-999999;
    }
  return *this;
};

void QwHelicity::Sum(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  //this routine is most likely to be called during the computatin of assymetry
  //this call doesn't make too much sense for this class so the followign lines
  //are only use to put safe gards testing for example if the two instantiation indeed
  // refers to elements in the same pattern
  if(Compare(value1)&&Compare(value2))
    {
      *this =  value1;
      *this += value2;
    }
};

void QwHelicity::Difference(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  // this is stub function defined here out of completion and uniformity between each subsystem
      *this =  value1;
};

void QwHelicity::Ratio(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  // this is stub function defined here out of completion and uniformity between each subsystem
      *this =  value1;
};


Bool_t QwHelicity::Compare(VQwSubsystem *value)
{
  Bool_t res=kTRUE;
  if(typeid(*value)!=typeid(*this))
    {
      res=kFALSE;
    }
  else
    {
      QwHelicity* input= dynamic_cast<QwHelicity*>(value);
      if(input->fWord.size()!=fWord.size())
	{
	res=kFALSE;
	}
    }
  return res;
}

