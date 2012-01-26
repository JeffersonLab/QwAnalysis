/**********************************************************\
* File: QwBeamMod.cc                                      *
*                                                         *
* Author: Joshua Hoskins                                  *
* Time-stamp: 052510                                      *
\**********************************************************/

#include "QwBeamMod.h"

// System headers
#include <iostream>
#include <stdexcept>

// Qweak headers
#include "QwLog.h"
#include "QwParameterFile.h"

// Register this subsystem with the factory
RegisterSubsystemFactory(QwBeamMod);


//*****************************************************************
void QwBeamMod::ProcessOptions(QwOptions &options){
      //Handle command line options
}

Int_t QwBeamMod::LoadChannelMap(TString mapfile)
{
  Bool_t ldebug=kFALSE;

  Int_t currentrocread=0;
  Int_t currentbankread=0;
  Int_t wordsofar=0;
  Int_t currentsubbankindex=-1;
  Int_t sample_size=0;
  //  Int_t index=0;
  //  Bool_t combolistdecoded;

  //added for QwWord

  // fPATTERNPHASEOFFSET=1;//Phase number offset is set to 1 by default and will be set to 0 if phase number starts from 0


  //Default value for random seed is 30 bits
  //BIT24=kFALSE;
  //BIT30=kTRUE;

  //end QwWord part one


  // Open the file
  QwParameterFile mapstr(mapfile.Data());
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());
  while (mapstr.ReadNextLine())
  {
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;

    TString varname, varvalue;
    if (mapstr.HasVariablePair("=",varname,varvalue))
    { //  This is a declaration line.  Decode it.
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
	  //	std::cout<<"bank " <<  currentbankread <<std::endl;
	}
      else if (varname=="sample_size")
	{
	  sample_size=value;
	}
   }
   else
     {
       Bool_t lineok=kTRUE;
       //  Break this line into tokens to process it.
       TString modtype = mapstr.GetTypedNextToken<TString>();	// module type
       /* Int_t modnum    = */ mapstr.GetTypedNextToken<Int_t>();	//slot number
       /* Int_t channum   = */ mapstr.GetTypedNextToken<Int_t>();	//channel number
       //	TString dettype   = mapstr.GetTypedNextToken<TString>();	//type-purpose of the detector
       //	dettype.ToLower();
       TString namech  = mapstr.GetTypedNextToken<TString>();  //name of the detector
       namech.ToLower();
       TString keyword = mapstr.GetTypedNextToken<TString>();
       keyword.ToLower();


       if (currentsubbankindex != GetSubbankIndex(currentrocread,currentbankread))
	  {
	    currentsubbankindex = GetSubbankIndex(currentrocread,currentbankread);
	    wordsofar = 0;
	  }


       QwModChannelID localModChannelID(currentsubbankindex, wordsofar,namech, modtype, this);


       if(modtype=="VQWK")
	 {
	   wordsofar+=6;
	   
	   if (lineok){
	     QwVQWK_Channel localchan;
	     localchan.InitializeChannel(GetSubsystemName(),"QwBeamMod",localModChannelID.fmodulename,"raw");
	     fModChannel.push_back(localchan);
	     fModChannel[fModChannel.size()-1].SetDefaultSampleSize(sample_size);
	     localModChannelID.fIndex=fModChannel.size()-1;
	     fModChannelID.push_back(localModChannelID);
	   }

	   if(ldebug)
	     {
	       localModChannelID.Print();
	       std::cout<<"line ok=";
	       if(lineok) std::cout<<"TRUE"<<std::endl;
	       else
		 std::cout<<"FALSE"<<std::endl;
	     }

          
	 }
     
       //       else if(modtype=="SCALER")wordsofar+=1;
      //  else
// 	 {
// 	   std::cerr << "QwBeamMod<VQWK>::LoadChannelMap:  Unknown module type: "
// 		     << modtype <<", the detector "<<namech<<" will not be decoded "
// 		     << std::endl;
// 	   lineok=kFALSE;
// 	   continue;
// 	 }

       //      if(modtype=="SKIP"){
       //	if (modnum<=0) wordsofar+=1;
       //	else           wordsofar+=modnum;
	//      }
//       else if(modtype =="WORD" && dettype!="modulationdata")
// 	{
// 	  QwError << "QwBeamMod::LoadChannelMap:  Unknown detector type: "
// 		  << dettype  << ", the detector " << namech << " will not be decoded "
// 		  << QwLog::endl;
// 	  lineok=kFALSE;
// 	  continue;
//	}

      if(modtype == "WORD")
	{
	  //  std::cout << "Decoding QwWord :: " << namech << std::endl;


	  QwWord localword;
	  localword.fSubbankIndex=currentsubbankindex;
	  localword.fWordInSubbank=wordsofar;
	  wordsofar+=1;
	  // I assume that one data = one word here. But it is not always the case, for
	  // example the triumf adc gives 6 words per channel
	  localword.fModuleType=modtype;
	  localword.fWordName=namech;
	  //localword.fWordType=dettype; // FIXME dettype is undefined so commented this out
	  fWord.push_back(localword);
	  fWordsPerSubbank[currentsubbankindex].second = fWord.size();
	  QwDebug << "--" << namech << "--" << fWord.size()-1 << QwLog::endl;

	  // Notice that "namech" is in lower-case, so these checks
	  // should all be in lower-case
	  //	  switch (fHelicityDecodingMode)
	  //{
	  // case kHelUserbitMode :
	  //  if(namech.Contains("userbit")) kUserbit=fWord.size()-1;
	  //  if(namech.Contains("scalercounter")) kScalerCounter=fWord.size()-1;
	  //  break;
	  // case kHelInputRegisterMode :
	  //    if(namech.Contains("input_register")) kInputRegister= fWord.size()-1;
	  //    if(namech.Contains("mps_counter")) kMpsCounter= fWord.size()-1;
	  //   if(namech.Contains("pat_counter")) kPatternCounter= fWord.size()-1;
	  //   if(namech.Contains("pat_phase")) kPatternPhase= fWord.size()-1;
	  //   break;
	  // case kHelInputMollerMode :
	  //   if(namech.Contains("mps_counter")) {
	  //		kMpsCounter= fWord.size()-1;
	  //  }
	  //   if(namech.Contains("pat_counter")) {
	  //	kPatternCounter = fWord.size()-1;
		//	      }
 //	      break;
	}
     }
  }

  if(ldebug)
    {
      std::cout<<"Done with Load map channel \n";
      for(size_t i=0;i<fModChannelID.size();i++)
	fModChannelID[i].Print();

      for(size_t i=0;i<fWord.size();i++)
	fWord[i].PrintID();
      //      std::cout << " kUserbit=" << kUserbit << "\n";
    }
  ldebug=kFALSE;

  return 0;
}



QwModChannelID::QwModChannelID(Int_t subbankid, Int_t wordssofar,
				   TString name,  TString modtype, QwBeamMod * obj):
  fSubbankIndex(subbankid),fWordInSubbank(wordssofar),
  fmoduletype(modtype),fmodulename(name),kUnknownDeviceType(-1)
{
  fTypeID = kQwUnknownDeviceType;
  //  for(size_t i=0;i<obj->fgModTypeNames.size();i++){
 //   if(dettype == obj->fgModTypeNames[i]){
 //     fTypeID = EBeamInstrumentType(i);
  //   std::cout << "Detector type not recognized" << std::endl;
   //   break;
   // }
  //  }
//   if (fTypeID == kUnknownDeviceType) {
//     std::cerr << "QwModChannelID::QwModChannelID:  Unknown detector type: "
//   	      << dettype <<", the detector "<<name<<" will not be decoded "
//   	      << std::endl;
//   }
}


//*****************************************************************

Int_t QwBeamMod::LoadEventCuts(TString  filename)
{
  Int_t eventcut_flag = 1;

  // Open the file
  QwParameterFile mapstr(filename.Data());
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());
  while (mapstr.ReadNextLine()){
    //std::cout<<"********* In the loop  *************"<<std::endl;
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;

    TString varname, varvalue;
    if (mapstr.HasVariablePair("=",varname,varvalue)){
      if (varname == "EVENTCUTS"){
	eventcut_flag = QwParameterFile::GetUInt(varvalue);
	//std::cout<<"EVENT CUT FLAG "<<eventcut_flag<<std::endl;
      }
    }
    else{
      TString device_type = mapstr.GetTypedNextToken<TString>();
      device_type.ToLower();
      TString device_name = mapstr.GetTypedNextToken<TString>();
      device_name.ToLower();


      if (device_type == "vqwk"){

	Double_t LLX = mapstr.GetTypedNextToken<Double_t>();	//lower limit for BCM value
	Double_t ULX = mapstr.GetTypedNextToken<Double_t>();	//upper limit for BCM value
	varvalue = mapstr.GetTypedNextToken<TString>();//global/loacal
	varvalue.ToLower();
	Double_t stabilitycut = mapstr.GetTypedNextToken<Double_t>();
	QwMessage<<"QwBeamLine Error Code passing to QwBCM "<<GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut)<<QwLog::endl;
	Int_t det_index=GetDetectorIndex(GetDetectorTypeID(kQwUnknownDeviceType),device_name);
	std::cout<<"*****************************"<<std::endl;
	std::cout<<" Type "<<device_type<<" Name "<<device_name<<" Index ["<<det_index <<"] "<<" device flag "<<eventcut_flag<<std::endl;
	fModChannel[det_index].PrintInfo();
	fModChannel[det_index].SetSingleEventCuts((GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut)|kBModErrorFlag),LLX,ULX,stabilitycut);
	std::cout<<"*****************************"<<std::endl;

      }

    }

  }
  //update the event cut ON/OFF for all the devices
  //std::cout<<"EVENT CUT FLAG"<<eventcut_flag<<std::endl;
  for (size_t i=0;i<fModChannel.size();i++)
    fModChannel[i].SetEventCutMode(eventcut_flag);

  return 0;
}

/*
Int_t QwBeamMod::LoadGeometry(TString mapfile)
{
  Bool_t ldebug=kFALSE;

  Int_t lineread=1;
  Int_t index;
  TString  devname,devtype;
  Double_t devOffsetX,devOffsetY, devOffsetZ;


  if(ldebug)std::cout<<"QwBeamMod::LoadGeometry("<< mapfile<<")\n";

  QwParameterFile mapstr(mapfile.Data());  //Open the file

  while (mapstr.ReadNextLine()){
      lineread+=1;
      if(ldebug)std::cout<<" line read so far ="<<lineread<<"\n";
      mapstr.TrimComment('!');
      mapstr.TrimWhitespace();
      if (mapstr.LineIsEmpty())  continue;
      else
	{
	  devtype = mapstr.GetTypedNextToken<TString>();
	  devtype.ToLower();
	  devtype.Remove(TString::kBoth,' ');
	  devname = mapstr.GetTypedNextToken<TString>();
	  devname.ToLower();
	  devname.Remove(TString::kBoth,' ');

	  devOffsetX = mapstr.GetTypedNextToken<Double_t>(); // X offset
	  devOffsetY = mapstr.GetTypedNextToken<Double_t>(); // Y offset
	  devOffsetZ = mapstr.GetTypedNextToken<Double_t>(); // Z offset

	  Bool_t notfound=kTRUE;


	  while(notfound){
	    if(devtype=="bpmstripline")
	      {
		//Load bpm offsets
		index=GetDetectorIndex(GetDetectorTypeID("bpmstripline"),devname);
		if(index == -1)
		  {
		    std::cerr << "\nQwBeamMod::LoadGeometry:  Unknown bpm : "
			      <<devname<<" will not be asigned with geometry parameters. \n"
			      << std::endl;
		    notfound=kFALSE;
		    continue;
		  }
		TString localname = fStripline[index].GetElementName();
		localname.ToLower();
		if(ldebug)  std::cout<<"element name =="<<localname
				     <<"== to be compared to =="<<devname<<"== \n";

		if(localname==devname)
		  {
		    if(ldebug) std::cout<<" I found the bpm !\n";
		    fStripline[index].SetOffset(devOffsetX,devOffsetY,devOffsetZ);
		    notfound=kFALSE;
		  }
	      }
	    else if (devtype=="combinedbpm")
	      {
		//Load combined bpm offsets which are, ofcourse, target position in the beamline
		index=GetDetectorIndex(GetDetectorTypeID("combinedbpm"),devname);
		if(index == -1)
		  {
		    std::cerr << "\nQwBeamMod::LoadGeometry:  Unknown combinedbpm : "
			      <<devname<<" will not be asigned with geometry parameters.\n "
			      << std::endl;
		    notfound=kFALSE;
		    continue;
		  }

		TString localname = fBPMCombo[index].GetElementName();
		localname.ToLower();
		if(ldebug)
		  std::cout<<"element name =="<<localname<<"== to be compared to =="<<devname<<"== \n";

		if(localname==devname)
		  {
		    if(ldebug) std::cout<<" I found the combinedbpm !\n";
		    fBPMCombo[index].SetOffset(devOffsetX,devOffsetY,devOffsetZ);
		    notfound=kFALSE;
		  }
	      }
	    else std::cout<<" Unknown device type :"<<devtype<<". The geometry will not be assigned to this device."<<std::endl;

	    if(ldebug)  std::cout<<"QwBeamMod::LoadGeometry:Offsets for device "<<devname<<" of type "<<devtype<<" are "
				 <<": X offset ="<< devOffsetX
				 <<": Y offset ="<< devOffsetY
				 <<": Z offset ="<<devOffsetZ<<"\n";
	  }

	}
  }

  if(ldebug) std::cout<<" line read in the geometry file ="<<lineread<<" \n";

  ldebug=kFALSE;
  return 0;

}

*/


//*****************************************************************
Int_t QwBeamMod::LoadInputParameters(TString pedestalfile)
{
  Bool_t ldebug=kFALSE;
  TString varname;

  Int_t lineread=1;

  if(ldebug)std::cout<<"QwBeamMod::LoadInputParameters("<< pedestalfile<<")\n";

  QwParameterFile mapstr(pedestalfile.Data());  //Open the file
  while (mapstr.ReadNextLine())
    {
      lineread+=1;
      if(ldebug)std::cout<<" line read so far ="<<lineread<<"\n";
      mapstr.TrimComment('!');   // Remove everything after a '!' character.
      mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
      if (mapstr.LineIsEmpty())  continue;
      else
	{
	  TString varname = mapstr.GetTypedNextToken<TString>();	//name of the channel
	  varname.ToLower();
	  varname.Remove(TString::kBoth,' ');
	  Double_t varped = mapstr.GetTypedNextToken<Double_t>(); // value of the pedestal
	  Double_t varcal = mapstr.GetTypedNextToken<Double_t>(); // value of the calibration factor
	  /* Double_t varweight = */ mapstr.GetTypedNextToken<Double_t>(); // value of the statistical weight

	  //if(ldebug) std::cout<<"inputs for channel "<<varname
	  //	      <<": ped="<<varped<<": cal="<<varcal<<": weight="<<varweight<<"\n";
	  for(size_t i=0;i<fModChannel.size();i++)
	    if(fModChannel[i].GetElementName()==varname)
		{
		  fModChannel[i].SetPedestal(varped);
		  fModChannel[i].SetCalibrationFactor(varcal);
		  break;
		}
	}

    }
  if(ldebug) std::cout<<" line read in the pedestal + cal file ="<<lineread<<" \n";

  ldebug=kFALSE;
  return 0;
}



// //*****************************************************************
// void QwBeamMod::RandomizeEventData(int helicity)
// {
//   // Randomize all QwBPMStripline buffers
//   for (size_t i = 0; i < fStripline.size(); i++)
//     fStripline[i].RandomizeEventData(helicity);

//   // Randomize all QwBCM buffers
//   for (size_t i = 0; i < fModChannel.size(); i++)
//     fModChannel[i].RandomizeEventData(helicity);

// }
// //*****************************************************************
// void QwBeamMod::EncodeEventData(std::vector<UInt_t> &buffer)
// {
//   std::vector<UInt_t> elements;
//   elements.clear();

//   // Get all buffers in the order they are defined in the map file
//   for (size_t i = 0; i < fModChannelID.size(); i++) {
//     // This is a QwBCM
//     if (fModChannelID.at(i).fTypeID == kBCM)
//       fModChannel[fModChannelID.at(i).fIndex].EncodeEventData(elements);
//     // This is a QwBPMStripline (which has 4 entries, only process the first one)
//     if (fModChannelID.at(i).fTypeID == kBPMStripline
//      && fModChannelID.at(i).fSubelement == 0)
//       fStripline[fModChannelID.at(i).fIndex].EncodeEventData(elements);
//   }

//   // If there is element data, generate the subbank header
//   std::vector<UInt_t> subbankheader;
//   std::vector<UInt_t> rocheader;
//   if (elements.size() > 0) {

//     // Form CODA subbank header
//     subbankheader.clear();
//     subbankheader.push_back(elements.size() + 1);	// subbank size
//     subbankheader.push_back((fCurrentBank_ID << 16) | (0x01 << 8) | (1 & 0xff));
// 		// subbank tag | subbank type | event number

//     // Form CODA bank/roc header
//     rocheader.clear();
//     rocheader.push_back(subbankheader.size() + elements.size() + 1);	// bank/roc size
//     rocheader.push_back((fCurrentROC_ID << 16) | (0x10 << 8) | (1 & 0xff));
// 		// bank tag == ROC | bank type | event number

//     // Add bank header, subbank header and element data to output buffer
//     buffer.insert(buffer.end(), rocheader.begin(), rocheader.end());
//     buffer.insert(buffer.end(), subbankheader.begin(), subbankheader.end());
//     buffer.insert(buffer.end(), elements.begin(), elements.end());
//   }
// }

// //*****************************************************************


Int_t QwBeamMod::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  Bool_t lkDEBUG=kFALSE;

  Int_t index = GetSubbankIndex(roc_id,bank_id);

  if (index>=0 && num_words>0){
    //  We want to process this ROC.  Begin looping through the data.
    if (lkDEBUG)
      std::cout << "QwBeamMod::ProcessEvBuffer:  "
		<< "Begin processing ROC" << roc_id
		<< " and subbank "<<bank_id
		<< " number of words="<<num_words<<std::endl;
    if (buffer[0]==0xf0f0f0f0 && num_words%2==1){
      buffer++;
      if (lkDEBUG)
	std::cout << "QwBeamMod::ProcessEvBuffer:  "
		  << "Skipped padding word 0xf0f0f0f0 at beginning of buffer."
		  << std::endl;
    }

    for(size_t i=0;i<fModChannelID.size();i++)
      {
	if(fModChannelID[i].fSubbankIndex==index)
	  {

		if (lkDEBUG)
		  {
		    std::cout<<"found modulation data for "<<fModChannelID[i].fmodulename<<std::endl;
		    std::cout<<"word left to read in this buffer:"<<num_words-fModChannelID[i].fWordInSubbank<<std::endl;
		  }
		fModChannel[fModChannelID[i].fIndex].  // Instead of BCM this will become our data word.If we use detectorID we need to add a specific ID
		  ProcessEvBuffer(&(buffer[fModChannelID[i].fWordInSubbank]),
				  num_words-fModChannelID[i].fWordInSubbank);

	  }
      }

     for(Int_t i=fWordsPerSubbank[index].first; i<fWordsPerSubbank[index].second; i++) {
      if(fWord[i].fWordInSubbank+1<= (Int_t) num_words) {
	fWord[i].fValue=buffer[fWord[i].fWordInSubbank];
      } else {
	QwWarning << "QwBeamMod::ProcessEvBuffer:  There is not enough word in the buffer to read data for "
		  << fWord[i].fWordName << QwLog::endl;
	QwWarning << "QwBeamMod::ProcessEvBuffer:  Words in this buffer:" << num_words
		  << " trying to read word number =" << fWord[i].fWordInSubbank << QwLog::endl;
      }
    }
    if(lkDEBUG) {
      QwDebug << "QwBeamMod::ProcessEvBuffer:  Done with Processing this event" << QwLog::endl;
      for(size_t i=0;i<fWord.size();i++) {
	std::cout << " word number = " << i << " ";
	fWord[i].Print();
      }
    }

  }
  lkDEBUG=kFALSE;
  return 0;
}

Bool_t QwBeamMod::ApplySingleEventCuts(){

  Bool_t test_Mod=kTRUE;
  Bool_t test_BCM1=kTRUE;


  for(size_t i=0;i<fModChannel.size();i++){
    //std::cout<<"  BCM ["<<i<<"] "<<std::endl;
    test_BCM1 = fModChannel[i].ApplySingleEventCuts();
    test_Mod &= test_BCM1;
    if(!test_BCM1 && bDEBUG) std::cout<<"******* QwBeamMod::SingleEventCuts()->BCM[ "<<i<<" , "<<fModChannel[i].GetElementName()<<" ] ******\n";
  }
  //if (!test_BCM)
  //fNumError_Evt_BCM++;//BCM falied  event counter for QwBeamMod

  return test_Mod;

}

Int_t QwBeamMod::GetEventcutErrorCounters(){//inherited from the VQwSubsystemParity; this will display the error summary

  std::cout<<"*********QwBeamMod Error Summary****************"<<std::endl;
  QwVQWK_Channel::PrintErrorCounterHead();
  for(size_t i=0;i<fModChannel.size();i++){
    fModChannel[i].GetEventcutErrorCounters();
  }
  QwVQWK_Channel::PrintErrorCounterTail();
  return 1;
}


UInt_t QwBeamMod::GetEventcutErrorFlag(){//return the error flag
  UInt_t ErrorFlag;
  ErrorFlag=0;
  for(size_t i=0;i<fModChannel.size();i++){
    ErrorFlag |= fModChannel[i].GetEventcutErrorFlag();
  }

  return ErrorFlag;

}



void  QwBeamMod::ProcessEvent()
{

  for(size_t i=0;i<fModChannel.size();i++){
    fModChannel[i].ApplyHWChecks();//first apply HW checks and update HW  error flags. 
    fModChannel[i].ProcessEvent();
  }
  
  return;
}


Int_t QwBeamMod::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{

  return 0;
}
/*

const Bool_t QwBeamMod::PublishInternalValues() const
{
  // Publish variables
  Bool_t status = kTRUE;
  status = status && PublishInternalValue("q_targ", "Calculated charge on target");
  return status;
}

*/

/**
 * Return the value of variable name
 * @param name Name of the desired variable
 * @param value Pointer to the value to be filled by the call
 * @return True if the variable was found, false if not found
 */

/*
const Bool_t QwBeamMod::ReturnInternalValue(TString name,
				       VQwDataElement* value) const
{
  Bool_t ldebug=kFALSE;
  if (ldebug) std::cout << "QwBeamMod::ReturnInternalValue called for value name, "
	    << name.Data() <<std::endl;
  Bool_t foundit = kFALSE;
  QwVQWK_Channel* tmp = dynamic_cast<QwVQWK_Channel*>(value);
  if (tmp==NULL){
    QwWarning << "QwBeamMod::ReturnInternalValue requires that "
	      << "'value' be a pointer to QwVQWK_Channel"
	      << QwLog::endl;
  } else {
    if (name=="q_targ"){
      foundit = kTRUE;
      (*tmp) = GetModChannel("qwk_bcm0l02")->GetCharge();
        if (ldebug) std::cout<<"QwBeamMod::ReturnInternalValue got element qwk_bcm0l02"<<std::endl;
    }

    //test for x_targ, y_targ, ...
    else if (name=="x_targ"){
      foundit = kTRUE;
      //(*tmp) = GetBPMStripline("ch_name")->GetSomething();
        if (ldebug) std::cout<<"QwBeamMod::ReturnInternalValue got element for x_targ"<<std::endl;
    }
    else if (name=="y_targ"){
      foundit = kTRUE;
      //(*tmp) = GetBPMStripline("ch_name")->GetSomething();
        if (ldebug) std::cout<<"QwBeamMod::ReturnInternalValue got element for y_targ"<<std::endl;
    }
    else if (name=="xp_targ"){
      foundit = kTRUE;
      //(*tmp) = GetBPMStripline("ch_name")->GetSomething();
        if (ldebug) std::cout<<"QwBeamMod::ReturnInternalValue got element for xp_targ"<<std::endl;
    }
    else if (name=="yp_targ"){
      foundit = kTRUE;
      //(*tmp) = GetBPMStripline("ch_name")->GetSomething();
        if (ldebug) std::cout<<"QwBeamMod::ReturnInternalValue got element for yp_targ"<<std::endl;
    }
    else if (name=="e_targ"){
      foundit = kTRUE;
      //(*tmp) = GetModChannel("ch_name")->GetCharge();
        if (ldebug) std::cout<<"QwBeamMod::ReturnInternalValue got element for e_targ"<<std::endl;
    }
  }
  return foundit;
}
*/
//*****************************************************************

void QwBeamMod::ClearEventData()
{
   for(size_t i=0;i<fModChannel.size();i++)
    fModChannel[i].ClearEventData();

   //The following is added in to handle the QwWord


   for (size_t i=0;i<fWord.size();i++)
    fWord[i].ClearEventData();

  /**Reset data by setting the old event number, pattern number and pattern phase 
     to the values of the previous event.*/
  //fEventNumberOld = fEventNumber;
  //fPatternNumberOld = fPatternNumber;
  //fPatternPhaseNumberOld = fPatternPhaseNumber;

  //fIgnoreHelicity = kFALSE;

  /**Clear out helicity variables */
  //  fHelicityReported = kUndefinedHelicity;
  //fHelicityActual = kUndefinedHelicity;
  //fHelicityDelayed= kUndefinedHelicity;
  //fHelicityBitPlus = kFALSE;
  //fHelicityBitMinus = kFALSE;
  // be careful: it is not that I forgot to reset fActualPatternPolarity
  // or fDelayedPatternPolarity. One doesn't want to do that here.
  /** Set the new event number and pattern number to -1. If we are not reading these correctly
      from the data stream, -1 will allow us to identify that.*/
  //fEventNumber = -1;
  //fPatternPhaseNumber = -1;


    return;
}

//*****************************************************************
Int_t QwBeamMod::GetDetectorTypeID(TString name)
{
  Int_t result=-1;
  for(size_t i=0;i<fgModTypeNames.size();i++)
    if(name==fgModTypeNames[i])
      {result=i;i=fgModTypeNames.size()+1;}
  return result;
}

//*****************************************************************
Int_t QwBeamMod::GetDetectorIndex(Int_t type_id, TString name)
{
  Bool_t ldebug=kFALSE;
  if(ldebug)
    {
      std::cout<<"QwBeamMod::GetDetectorIndex\n";
      std::cout<<"type_id=="<<type_id<<" name="<<name<<"\n";
      std::cout<<fModChannelID.size()<<" already registered detector\n";
    }

  Int_t result=-1;
  for(size_t i=0;i<fModChannelID.size();i++)
    {

	if(fModChannelID[i].fmodulename==name)
	  result=fModChannelID[i].fIndex;
      if(ldebug)
	{
	  std::cout<<"testing against ("<<fModChannelID[i].fTypeID
		   <<","<<fModChannelID[i].fmodulename<<")=>"<<result<<"\n";
	}
    }

  return result;
}
//*****************************************************************
/*
QwBPMStripline* QwBeamMod::GetBPMStripline(const TString name)
{
  if (! fStripline.empty()) {
    for (std::vector<QwBPMStripline>::iterator stripline = fStripline.begin(); stripline != fStripline.end(); ++stripline) {
      if (stripline->GetElementName() == name) {
	return &(*stripline);
      }
    }
  }
  return 0;
}

QwBCM* QwBeamMod::GetModChannel(const TString name)
{
  if (! fModChannel.empty()) {
    for (std::vector<QwBCM>::iterator bcm = fModChannel.begin(); bcm != fModChannel.end(); ++bcm) {
      if (bcm->GetElementName() == name) {
	return &(*bcm);
      }
    }
  }
  return 0;
}

const QwBPMStripline* QwBeamMod::GetBPMStripline(const TString name) const
{
  return const_cast<QwBeamMod*>(this)->GetBPMStripline(name);
}
const QwBCM* QwBeamMod::GetModChannel(const TString name) const
{
  return const_cast<QwBeamMod*>(this)->GetModChannel(name);
}
*/
//*****************************************************************
VQwSubsystem&  QwBeamMod::operator=  (VQwSubsystem *value)
{
  //  std::cout<<" here in QwBeamMod::operator= \n";
  if(Compare(value))
    {

      QwBeamMod* input = dynamic_cast<QwBeamMod*>(value);
      
      for(size_t i=0;i<input->fModChannel.size();i++)
	this->fModChannel[i]=input->fModChannel[i];
        for(size_t i=0;i<input->fWord.size();i++)
  	this->fWord[i].fValue=input->fWord[i].fValue;
    }
  return *this;
}

VQwSubsystem&  QwBeamMod::operator+=  (VQwSubsystem *value)
{
  if(Compare(value))
    {
      //QwBeamMod* input= (QwBeamMod*)value ;
      QwBeamMod* input = dynamic_cast<QwBeamMod*>(value);

      for(size_t i=0;i<input->fModChannel.size();i++)
	this->fModChannel[i]+=input->fModChannel[i];
//         for(size_t i=0;i<input->fWord.size();i++)
//    	this->fWord[i]+=input->fWord[i];

    }
  return *this;
}

VQwSubsystem&  QwBeamMod::operator-=  (VQwSubsystem *value)
{

  if(Compare(value))
    {
      //QwBeamMod* input= (QwBeamMod*)value;
      QwBeamMod* input = dynamic_cast<QwBeamMod*>(value);

      for(size_t i=0;i<input->fModChannel.size();i++)
	this->fModChannel[i]-=input->fModChannel[i];
//       for(size_t i=0;i<input->fWord.size();i++)
//         this->fWord[i]-=input->fWord[i];

    }
  return *this;
}

void  QwBeamMod::Sum(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if(Compare(value1)&&Compare(value2))
    {
      *this =  value1;
      *this += value2;
    }
}

void  QwBeamMod::Difference(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if(Compare(value1)&&Compare(value2))
    {
      *this =  value1;
      *this -= value2;
    }
}

void QwBeamMod::Ratio(VQwSubsystem  *numer, VQwSubsystem  *denom)
{
  if(Compare(numer)&&Compare(denom))
    {
      //QwBeamMod* innumer= (QwBeamMod*)numer ;
      QwBeamMod* innumer = dynamic_cast<QwBeamMod*>(numer);
      //QwBeamMod* indenom= (QwBeamMod*)denom ;
      QwBeamMod* indenom = dynamic_cast<QwBeamMod*>(denom);

      for(size_t i=0;i<innumer->fModChannel.size();i++)
	this->fModChannel[i].Ratio(innumer->fModChannel[i],indenom->fModChannel[i]);
      for(size_t i=0;i<innumer->fWord.size();i++)
	this->fWord[i].fValue=innumer->fWord[i].fValue;

    }
  return;
}


void QwBeamMod::Scale(Double_t factor)
{

  for(size_t i=0;i<fModChannel.size();i++)
    fModChannel[i].Scale(factor);
  return;
}

void QwBeamMod::CalculateRunningAverage()
{/*
  UInt_t i = 0;
  std::cout<<"*********QwBeamMod device Averages****************"<<std::endl;
  std::cout<<"Device \t    ||  Average\t || error\t || events"<<std::endl;
  printf("BPM\n");
  for(i=0; i<fStripline.size(); i++) fStripline[i].Calculate_Running_Average();
  printf("BCM\n");
  for(i=0; i<fModChannel.size();       i++) fModChannel[i].Calculate_Running_Average();
  for(i=0; i<fModChannelCombo.size();  i++) fModChannelCombo[i].Calculate_Running_Average();
  for(i=0; i<fBPMCombo.size();  i++) fBPMCombo[i].Calculate_Running_Average();
  std::cout<<"---------------------------------------------------"<<std::endl;
  std::cout<<std::endl;*/
  return;
}

void QwBeamMod::AccumulateRunningSum(VQwSubsystem*){}

Bool_t QwBeamMod::Compare(VQwSubsystem *value)
{
  //  std::cout<<" Here in QwBeamMod::Compare \n";

  Bool_t res=kTRUE;
  if(typeid(*value)!=typeid(*this))
    {
      res=kFALSE;
      //      std::cout<<" types are not ok \n";
      //      std::cout<<" this is bypassed just for now but should be fixed eventually \n";
    }
  else
    {
      //QwBeamMod* input= (QwBeamMod*)value;
      QwBeamMod* input = dynamic_cast<QwBeamMod*>(value);

	if(input->fModChannel.size()!=fModChannel.size())
	  {
	    res=kFALSE;
	  //	  std::cout<<" not the same number of bcms \n";
	  }
    }
  return res;
}


//*****************************************************************

void  QwBeamMod::ConstructHistograms(TDirectory *folder, TString &prefix)
{

  //SetHistoTreeSave(prefix);
  if (folder != NULL) folder->cd();
  TString basename;
  //size_t index=0;

//   //  std::cout<<" here is QwBeamMod::ConstructHistogram with prefix ="<<prefix<<"\n";
//   for(size_t i=0;i<fStripline.size();i++)
//       fStripline[i].ConstructHistograms(folder,prefix);

//   for(size_t i=0;i<fModChannel.size();i++)
//       fModChannel[i].ConstructHistograms(folder,prefix);

//   for(size_t i=0;i<fModChannelCombo.size();i++)
//       fModChannelCombo[i].ConstructHistograms(folder,prefix);

//   for(size_t i=0;i<fBPMCombo.size();i++)
//       fBPMCombo[i].ConstructHistograms(folder,prefix);

//Following is added from QwBeamMod to handle QwWord


  // if(fHistoType==kHelNoSave)
  //   {
  //     //do nothing
  //   }
  // else if(fHistoType==kHelSavePattern)
  //   {
  //     fHistograms.resize(1+fWord.size(), NULL);
  //     basename="pattern_polarity";
  //     fHistograms[index]   = gQwHists.Construct1DHist(basename);
  //     index+=1;
  //     for (size_t i=0; i<fWord.size(); i++){
  // 	basename="hel_"+fWord[i].fWordName;
  // 	fHistograms[index]   = gQwHists.Construct1DHist(basename);
  // 	index+=1;
  //     }
  //   }
  // else if(fHistoType==kHelSaveMPS)
  //   {
  //     fHistograms.resize(4+fWord.size(), NULL);
  //     //eventnumber, patternnumber, helicity, patternphase + fWord.size
  //     basename=prefix+"delta_event_number";
  //     fHistograms[index]   = gQwHists.Construct1DHist(basename);
  //     index+=1;
  //     basename=prefix+"delta_pattern_number";
  //     fHistograms[index]   = gQwHists.Construct1DHist(basename);
  //     index+=1;
  //     basename=prefix+"pattern_phase";
  //     fHistograms[index]   = gQwHists.Construct1DHist(basename);
  //     index+=1;
  //     basename=prefix+"helicity";
  //     fHistograms[index]   = gQwHists.Construct1DHist(basename);
  //     index+=1;
  //     for (size_t i=0; i<fWord.size(); i++){
  // 	basename=prefix+fWord[i].fWordName;
  // 	fHistograms[index]   = gQwHists.Construct1DHist(basename);
  // 	index+=1;
  //     }
  //   }
  // else
  //   QwError << "QwBeamMod::ConstructHistograms this prefix--" << prefix << "-- is not unknown:: no histo created" << QwLog::endl;


  return;
}

void  QwBeamMod::FillHistograms()
{

  //size_t index=0;

//   for(size_t i=0;i<fStripline.size();i++)
//     fStripline[i].FillHistograms();
//   for(size_t i=0;i<fModChannel.size();i++)
//     fModChannel[i].FillHistograms();
//   for(size_t i=0;i<fModChannelCombo.size();i++)
//     fModChannelCombo[i].FillHistograms();
//   for(size_t i=0;i<fBPMCombo.size();i++)
//     fBPMCombo[i].FillHistograms();

//Added to handle QwWord's


  // if(fHistoType==kHelNoSave)
  //   {
  //     //do nothing
  //   }
  // else if(fHistoType==kHelSavePattern)
  //   {
  //     QwDebug << "QwBeamMod::FillHistograms helicity info " << QwLog::endl;
  //     QwDebug << "QwBeamMod::FillHistograms  pattern polarity=" << fActualPatternPolarity << QwLog::endl;
  //     if (fHistograms[index]!=NULL)
  // 	fHistograms[index]->Fill(fActualPatternPolarity);
  //     index+=1;
      
  //     for (size_t i=0; i<fWord.size(); i++){
  // 	if (fHistograms[index]!=NULL)
  // 	  fHistograms[index]->Fill(fWord[i].fValue);
  // 	index+=1;	
  // 	QwDebug << "QwBeamMod::FillHistograms " << fWord[i].fWordName << "=" << fWord[i].fValue << QwLog::endl;
  //     }
  //   }
  // else if(fHistoType==kHelSaveMPS)
  //   {
  //     QwDebug << "QwBeamMod::FillHistograms mps info " << QwLog::endl;
  //     if (fHistograms[index]!=NULL)
  // 	fHistograms[index]->Fill(fEventNumber-fEventNumberOld);
  //     index+=1;
  //     if (fHistograms[index]!=NULL)
  // 	fHistograms[index]->Fill(fPatternNumber-fPatternNumberOld);
  //     index+=1;
  //     if (fHistograms[index]!=NULL)
  // 	fHistograms[index]->Fill(fPatternPhaseNumber);
  //     index+=1;
  //     if (fHistograms[index]!=NULL)
  // 	fHistograms[index]->Fill(fHelicityActual);
  //     index+=1;
  //     for (size_t i=0; i<fWord.size(); i++){
  // 	if (fHistograms[index]!=NULL)
  // 	  fHistograms[index]->Fill(fWord[i].fValue);
  // 	index+=1;
  // 	QwDebug << "QwBeamMod::FillHistograms " << fWord[i].fWordName << "=" << fWord[i].fValue << QwLog::endl;
  //     }
  //   }



  return;
}



void QwBeamMod::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values)
{

  TString basename;
  
  // std::cout << "ConstructBranchAndVector" << std::endl;

  for(size_t i = 0; i < fModChannel.size(); i++)
    fModChannel[i].ConstructBranchAndVector(tree, prefix, values);
//   for (size_t i=0;i<fWord.size();i++)
//     fWord[i].ConstructBranchAndVector(tree, prefix, values);
  fTreeArrayIndex  = values.size();
  for (size_t i=0; i<fWord.size(); i++)
	{
// 	  basename = fWord[i].fWordName;
 	  basename = prefix+fWord[i].fWordName;
	  values.push_back(0.0);
	  tree->Branch(basename, &(values.back()), basename+"/D");
	}


  return;
}

void QwBeamMod::FillTreeVector(std::vector<Double_t> &values) const
{

  size_t index = fTreeArrayIndex;
//   std::cout << "FillTreeVector" << std::endl;

  for(size_t i = 0; i < fModChannel.size(); i++)
    fModChannel[i].FillTreeVector(values);
  for (size_t i=0; i<fWord.size(); i++){
	values[index++] = fWord[i].fValue;
  }
  return;
}


//*****************************************************************
void  QwBeamMod::Print()
{
  std::cout<<"Name of the subsystem ="<<fSystemName<<"\n";

  std::cout<<"there are "<<fModChannel.size()<<" mods \n";

  std::cout<<" Printing Running AVG and other channel info for fModChannel"<<std::endl;
  for(size_t i=0;i<fModChannel.size();i++)
    fModChannel[i].PrintValue();
  for(size_t i=0;i<fWord.size();i++)
    fWord[i].Print();

  return;
}

void  QwBeamMod::PrintModChannelID()
{
  for (size_t i=0;i<fModChannelID.size();i++)
    {
      std::cout<<"============================="<<std::endl;
      std::cout<<" Detector ID="<<i<<std::endl;
      fModChannelID[i].Print();
    }
  return;
}


void  QwModChannelID::Print()
{

  std::cout<<std::endl<<"Detector name= "<<fmodulename<<std::endl;
  std::cout<<"SubbankkIndex= "<<fSubbankIndex<<std::endl;
  std::cout<<"word index in subbank= "<<fWordInSubbank<<std::endl;
  std::cout<<"module type= "<<fmoduletype<<std::endl;
// std::cout<<"detector type=  "<<fdetectortype<<" that is index="<<fTypeID<<std::endl;
  std::cout<<"Index of this detector in the vector of similar detector= "<<
    fIndex<<std::endl;
//std::cout<<"Subelement index= "<< fSubelement<<std::endl;





  std::cout<<"---------------------------------------------------"<<std::endl;
  std::cout<<std::endl;


  return;
}

//*****************************************************************


void  QwBeamMod::Copy(const VQwSubsystem *source)
{

  try
    {
     if(typeid(*source)==typeid(*this))
	 {
	  VQwSubsystem::Copy(source);
	  const QwBeamMod* input = dynamic_cast<const QwBeamMod*>(source);

	  this->fModChannel.resize(input->fModChannel.size());
	  for(size_t i=0;i<this->fModChannel.size();i++)
	    this->fModChannel[i].Copy(&(input->fModChannel[i]));

//  	  this->fWord.resize(input->fWord.size());
//  	  for(size_t i=0;i<this->fWord.size();i++)
//  	    this->fWord[i].Copy(&(input->fWord[i]));

	  this->fWord.resize(input->fWord.size());
          for(size_t i=0;i<this->fWord.size();i++)
            {
              this->fWord[i].fWordName=input->fWord[i].fWordName;
              this->fWord[i].fModuleType=input->fWord[i].fModuleType;
              this->fWord[i].fWordType=input->fWord[i].fWordType;
            } 

         }




  else
    {
	  TString loc="Standard exception from QwBeamMod::Copy = "
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



void QwBeamMod::FillDB(QwParityDB *db, TString datatype)
{
 
  return;
}



void QwBeamMod::FillErrDB(QwParityDB *db, TString datatype)
{
  return;
}




void QwBeamMod::WritePromptSummary(QwPromptSummary *ps, TString datatype)
{
  return;
};
