/**********************************************************\
* File: QwBeamMod.cc                                      *
*                                                         *
* Author: Joshua Hoskins                                  *
* Time-stamp: 052510                                      *
\**********************************************************/

#include "QwBeamMod.h"
#include "QwHistogramHelper.h"

#define MYSQLPP_SSQLS_NO_STATICS
#include "QwSSQLS.h"

#include <iostream>
#include <stdexcept>

#include "QwLog.h"

// Register this subsystem with the factory
QwSubsystemFactory<QwBeamMod> theBeamModFactory("QwBeamMod");


//*****************************************************************
void QwBeamMod::ProcessOptions(QwOptions &options){
      //Handle command line options
};

Int_t QwBeamMod::LoadChannelMap(TString mapfile)
{
  Bool_t ldebug=kFALSE;

  TString varname, varvalue;
  TString modtype, dettype, namech, keyword;
  TString combotype, comboname, dev_name;
  Int_t modnum, channum;
  Int_t currentrocread=0;
  Int_t currentbankread=0;
  Int_t wordsofar=0;
  Int_t currentsubbankindex=-1;
  Int_t fSample_size=0;
  Int_t index=0;
  Bool_t combolistdecoded;


  std::vector<TString> fDeviceName;
  std::vector<Double_t> fQWeight;
  std::vector<Double_t> fXWeight;
  std::vector<Double_t> fYWeight;


  QwParameterFile mapstr(mapfile.Data());  //Open the file

  while (mapstr.ReadNextLine())
 {
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;


   if (mapstr.HasVariablePair("=",varname,varvalue))
   { //  This is a declaration line.  Decode it.
      varname.ToLower();
      UInt_t value = QwParameterFile::GetUInt(varvalue);

      if (varname=="roc")
	{
	  currentrocread=value;
	  RegisterROCNumber(value,0);

	}
      else if (varname=="bank")
	{
	  currentbankread=value;
	  RegisterSubbank(value);
	std::cout<<"bank " <<  currentbankread <<std::endl;
	}
      else if (varname=="sample_size")
	{
	  fSample_size=value;
	}
   }
   else
     {
       Bool_t lineok=kTRUE;
       //  Break this line into tokens to process it.
       modtype   = mapstr.GetNextToken(", ").c_str();	// module type
       modnum    = (atol(mapstr.GetNextToken(", ").c_str()));	//slot number
       channum   = (atol(mapstr.GetNextToken(", ").c_str()));	//channel number
       //	dettype   = mapstr.GetNextToken(", ").c_str();	//type-purpose of the detector
       //	dettype.ToLower();
       namech    = mapstr.GetNextToken(", ").c_str();  //name of the detector
       namech.ToLower();
       keyword = mapstr.GetNextToken(", ").c_str();
       keyword.ToLower();


       if(currentsubbankindex!=GetSubbankIndex(currentrocread,currentbankread))
	  {
	    currentsubbankindex=GetSubbankIndex(currentrocread,currentbankread);
	    wordsofar=0;
	  }


       QwModChannelID localModChannelID(currentsubbankindex, wordsofar,namech, modtype, this);


       if(modtype=="VQWK")wordsofar+=6;
       else if(modtype=="SCALER")wordsofar+=1;
       else
	 {
	   std::cerr << "QwBeamMod::LoadChannelMap:  Unknown module type: "
		     << modtype <<", the detector "<<namech<<" will not be decoded "
		     << std::endl;
	   lineok=kFALSE;
	   continue;
	 }

       if (lineok){
	 QwVQWK_Channel localchan(localModChannelID.fmodulename);
	 fModChannel.push_back(localchan);
	 fModChannel[fModChannel.size()-1].SetDefaultSampleSize(fSample_size);
	 localModChannelID.fIndex=fModChannel.size()-1;
       }

       if(ldebug)
	 {
	   localModChannelID.Print();
	   std::cout<<"line ok=";
	   if(lineok) std::cout<<"TRUE"<<std::endl;
	   else
	     std::cout<<"FALSE"<<std::endl;
	 }

       if(lineok)
	 fModChannelID.push_back(localModChannelID);

     }
 }

  if(ldebug)
    {
      std::cout<<"Done with Load map channel \n";
      for(size_t i=0;i<fModChannelID.size();i++)
	fModChannelID[i].Print();
    }
  ldebug=kFALSE;

  return 0;
};



QwModChannelID::QwModChannelID(Int_t subbankid, Int_t wordssofar,
				   TString name,  TString modtype, QwBeamMod * obj):
  fSubbankIndex(subbankid),fWordInSubbank(wordssofar),
  fmoduletype(modtype),fmodulename(name),kUnknownDeviceType(-1)
{
  fTypeID = kUnknownDeviceType;
  for(size_t i=0;i<obj->fgModTypeNames.size();i++){
 //   if(dettype == obj->fgModTypeNames[i]){
 //     fTypeID = EBeamInstrumentType(i);
  //   std::cout << "Detector type not recognized" << std::endl;
   //   break;
   // }
  }
//   if (fTypeID == kUnknownDeviceType) {
//     std::cerr << "QwModChannelID::QwModChannelID:  Unknown detector type: "
//   	      << dettype <<", the detector "<<name<<" will not be decoded "
//   	      << std::endl;
//   }
};


//*****************************************************************

Int_t QwBeamMod::LoadEventCuts(TString  filename){/*
  Double_t ULX, LLX, ULY, LLY;
  Int_t samplesize;
  Int_t check_flag;
  Int_t eventcut_flag;
  TString varname, varvalue, vartypeID,varname2, varvalue2;
  TString device_type,device_name,channel_name;
  std::cout<<" QwBeamMod::LoadEventCuts  "<<filename<<std::endl;
  QwParameterFile mapstr(filename.Data());  //Open the file

  samplesize = 0;
  check_flag = 0;

  eventcut_flag=1;

  while (mapstr.ReadNextLine()){
    //std::cout<<"********* In the loop  *************"<<std::endl;
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;

    if (mapstr.HasVariablePair("=",varname2,varvalue2)){
      if (varname2=="EVENTCUTS"){
	//varname="";
	eventcut_flag= QwParameterFile::GetUInt(varvalue2);
	//std::cout<<"EVENT CUT FLAG "<<eventcut_flag<<std::endl;
      }
    }
    else{
      device_type= mapstr.GetNextToken(", ").c_str();
      device_type.ToLower();
      device_name= mapstr.GetNextToken(", ").c_str();
      device_name.ToLower();


      //set limits to zero
      ULX=0;
      LLX=0;
      ULY=0;
      LLY=0;

      if (device_type == "bcm"){

	//std::cout<<" device name "<<device_name<<" device flag "<<check_flag<<std::endl;
	LLX = (atof(mapstr.GetNextToken(", ").c_str()));	//lower limit for BCM value
	ULX = (atof(mapstr.GetNextToken(", ").c_str()));	//upper limit for BCM value

	//samplesize = (atoi(mapstr.GetNextToken(", ").c_str()));	//sample size
	//	std::cout<<" sample size "<<samplesize<<std::endl;
	//retrieve the detector from the vector.
	//device_name="empty2";
	Int_t det_index=GetDetectorIndex(GetDetectorTypeID(device_type),device_name);
	//std::cout<<"*****************************"<<std::endl;
	//std::cout<<" Type "<<device_type<<" Name "<<device_name<<" Index ["<<det_index <<"] "<<" device flag "<<eventcut_flag<<std::endl;

	//fModChannel[det_index].Print();
	fModChannel[det_index].SetSingleEventCuts(LLX,ULX);//(fModChannelEventCuts);
	//std::cout<<"*****************************"<<std::endl;

      }
      else if (device_type == "bpmstripline"){
	channel_name= mapstr.GetNextToken(", ").c_str();
	channel_name.ToLower();


	LLX = (atof(mapstr.GetNextToken(", ").c_str()));	//lower limit for BPMStripline X
	ULX = (atof(mapstr.GetNextToken(", ").c_str()));	//upper limit for BPMStripline X
	//LLY = (atof(mapstr.GetNextToken(", ").c_str()));	//lower limit for BPMStripline Y
	//ULY = (atof(mapstr.GetNextToken(", ").c_str()));	//upper limit for BPMStripline Y

	Int_t det_index=GetDetectorIndex(GetDetectorTypeID(device_type),device_name);

	//std::cout<<"*****************************"<<std::endl;
	//std::cout<<" Name "<<device_name<<" Index ["<<det_index <<"] "<<" device flag "<<eventcut_flag<<std::endl;
	//fStripline[det_index].SetSingleEventCuts(LLX, ULX, LLY,ULY);
	fStripline[det_index].SetSingleEventCuts(channel_name, LLX, ULX);
	//fStripline[det_index].Print();
	//std::cout<<"*****************************"<<std::endl;
      }
    }

  }
  //update the event cut ON/OFF for all the devices
  //std::cout<<"EVENT CUT FLAG"<<eventcut_flag<<std::endl;
  for (size_t i=0;i<fStripline.size();i++){
    fStripline[i].SetEventCutMode(eventcut_flag);
  }

  for (size_t i=0;i<fModChannel.size();i++)
    fModChannel[i].SetEventCutMode(eventcut_flag);


  fQwBeamModErrorCount=0; //set the error counter to zero
*/
  return 0;
};

/*
Int_t QwBeamMod::LoadGeometry(TString mapfile)
{
  Bool_t ldebug=kFALSE;

  Int_t lineread=1;
  Int_t index;
  TString  devname,devtype;
  Double_t devOffsetX,devOffsetY, devOffsetZ;
  TString localname;


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
	  devtype = mapstr.GetNextToken(", \t").c_str();
	  devtype.ToLower();
	  devtype.Remove(TString::kBoth,' ');
	  devname = mapstr.GetNextToken(", \t").c_str();
	  devname.ToLower();
	  devname.Remove(TString::kBoth,' ');

	  devOffsetX = (atof(mapstr.GetNextToken(", \t").c_str())); // X offset
	  devOffsetY = (atof(mapstr.GetNextToken(", \t").c_str())); // Y offset
	  devOffsetZ = (atof(mapstr.GetNextToken(", \t").c_str())); // Z offset

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
		localname=fStripline[index].GetElementName();
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

		localname=fBPMCombo[index].GetElementName();
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
  Double_t varped;
  Double_t varcal;
  Double_t varweight;
  TString localname;

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
	  varname = mapstr.GetNextToken(", \t").c_str();	//name of the channel
	  varname.ToLower();
	  varname.Remove(TString::kBoth,' ');
	  varped= (atof(mapstr.GetNextToken(", \t").c_str())); // value of the pedestal
	  varcal= (atof(mapstr.GetNextToken(", \t").c_str())); // value of the calibration factor
	  varweight= (atof(mapstr.GetNextToken(", \t").c_str())); // value of the statistical weight

	  //if(ldebug) std::cout<<"inputs for channel "<<varname
	  //	      <<": ped="<<varped<<": cal="<<varcal<<": weight="<<varweight<<"\n";
	  Bool_t notfound=kTRUE;
	  for(size_t i=0;i<fModChannel.size();i++)
	    if(fModChannel[i].GetElementName()==varname)
		{
		  fModChannel[i].SetPedestal(varped);
		  fModChannel[i].SetCalibrationFactor(varcal);
		  notfound=kFALSE;
		  break;
		}
	}

    }
  if(ldebug) std::cout<<" line read in the pedestal + cal file ="<<lineread<<" \n";

  ldebug=kFALSE;
  return 0;
}


/*
//*****************************************************************
void QwBeamMod::RandomizeEventData(int helicity)
{
  // Randomize all QwBPMStripline buffers
  for (size_t i = 0; i < fStripline.size(); i++)
    fStripline[i].RandomizeEventData(helicity);

  // Randomize all QwBCM buffers
  for (size_t i = 0; i < fModChannel.size(); i++)
    fModChannel[i].RandomizeEventData(helicity);

}
//*****************************************************************
void QwBeamMod::EncodeEventData(std::vector<UInt_t> &buffer)
{
  std::vector<UInt_t> elements;
  elements.clear();

  // Get all buffers in the order they are defined in the map file
  for (size_t i = 0; i < fModChannelID.size(); i++) {
    // This is a QwBCM
    if (fModChannelID.at(i).fTypeID == kBCM)
      fModChannel[fModChannelID.at(i).fIndex].EncodeEventData(elements);
    // This is a QwBPMStripline (which has 4 entries, only process the first one)
    if (fModChannelID.at(i).fTypeID == kBPMStripline
     && fModChannelID.at(i).fSubelement == 0)
      fStripline[fModChannelID.at(i).fIndex].EncodeEventData(elements);
  }

  // If there is element data, generate the subbank header
  std::vector<UInt_t> subbankheader;
  std::vector<UInt_t> rocheader;
  if (elements.size() > 0) {

    // Form CODA subbank header
    subbankheader.clear();
    subbankheader.push_back(elements.size() + 1);	// subbank size
    subbankheader.push_back((fCurrentBank_ID << 16) | (0x01 << 8) | (1 & 0xff));
		// subbank tag | subbank type | event number

    // Form CODA bank/roc header
    rocheader.clear();
    rocheader.push_back(subbankheader.size() + elements.size() + 1);	// bank/roc size
    rocheader.push_back((fCurrentROC_ID << 16) | (0x10 << 8) | (1 & 0xff));
		// bank tag == ROC | bank type | event number

    // Add bank header, subbank header and element data to output buffer
    buffer.insert(buffer.end(), rocheader.begin(), rocheader.end());
    buffer.insert(buffer.end(), subbankheader.begin(), subbankheader.end());
    buffer.insert(buffer.end(), elements.begin(), elements.end());
  }
}

//*****************************************************************

*/
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
  }

  return 0;
};

Bool_t QwBeamMod::ApplySingleEventCuts(){
  //currently this will check the IsGoodEvent() only!
  //std::cout<<" QwBeamMod::SingleEventCuts() ";

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

};

Int_t QwBeamMod::GetEventcutErrorCounters(){//inherited from the VQwSubsystemParity; this will display the error summary

  std::cout<<"*********QwBeamMod Error Summary****************"<<std::endl;
  std::cout<<"Device name ||  Sample || SW_HW || Sequence || SameHW || ZeroHW || EventCut\n";
  for(size_t i=0;i<fModChannel.size();i++){
    fModChannel[i].GetEventcutErrorCounters();
  }

   std::cout<<"---------------------------------------------------"<<std::endl;
   std::cout<<std::endl;
  return 1;
}


Int_t QwBeamMod::GetEventcutErrorFlag(){//return the error flag
  Int_t ErrorFlag;
  ErrorFlag=0;
  for(size_t i=0;i<fModChannel.size();i++){
    ErrorFlag |= fModChannel[i].GetEventcutErrorFlag();
  }

  return ErrorFlag;

}



void  QwBeamMod::ProcessEvent()
{

  for(size_t i=0;i<fModChannel.size();i++)
    fModChannel[i].ProcessEvent();

  return;
};


Int_t QwBeamMod::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{

  return 0;
};
/*

const Bool_t QwBeamMod::PublishInternalValues() const
{
  // Publish variables
  Bool_t status = kTRUE;
  status = status && PublishInternalValue("q_targ", "Calculated charge on target");
  return status;
};

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
};
*/
//*****************************************************************

void QwBeamMod::ClearEventData()
{
   for(size_t i=0;i<fModChannel.size();i++)
    fModChannel[i].ClearEventData();
    return;
};

//*****************************************************************
Int_t QwBeamMod::GetDetectorTypeID(TString name)
{
  Int_t result=-1;
  for(size_t i=0;i<fgModTypeNames.size();i++)
    if(name==fgModTypeNames[i])
      {result=i;i=fgModTypeNames.size()+1;}
  return result;
};

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
};
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
};

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
};

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

    }
  return *this;
};

VQwSubsystem&  QwBeamMod::operator+=  (VQwSubsystem *value)
{
  if(Compare(value))
    {
      //QwBeamMod* input= (QwBeamMod*)value ;
      QwBeamMod* input = dynamic_cast<QwBeamMod*>(value);

      for(size_t i=0;i<input->fModChannel.size();i++)
	this->fModChannel[i]+=input->fModChannel[i];

    }
  return *this;
};

VQwSubsystem&  QwBeamMod::operator-=  (VQwSubsystem *value)
{

  if(Compare(value))
    {
      //QwBeamMod* input= (QwBeamMod*)value;
      QwBeamMod* input = dynamic_cast<QwBeamMod*>(value);

      for(size_t i=0;i<input->fModChannel.size();i++)
	this->fModChannel[i]-=input->fModChannel[i];

    }
  return *this;
};

void  QwBeamMod::Sum(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if(Compare(value1)&&Compare(value2))
    {
      *this =  value1;
      *this += value2;
    }
};

void  QwBeamMod::Difference(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if(Compare(value1)&&Compare(value2))
    {
      *this =  value1;
      *this -= value2;
    }
};

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

    }
  return;
};


void QwBeamMod::Scale(Double_t factor)
{

  for(size_t i=0;i<fModChannel.size();i++)
    fModChannel[i].Scale(factor);
  return;
};

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
};

void QwBeamMod::AccumulateRunningSum(VQwSubsystem*){};

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

//   //  std::cout<<" here is QwBeamMod::ConstructHistogram with prefix ="<<prefix<<"\n";
//   for(size_t i=0;i<fStripline.size();i++)
//       fStripline[i].ConstructHistograms(folder,prefix);

//   for(size_t i=0;i<fModChannel.size();i++)
//       fModChannel[i].ConstructHistograms(folder,prefix);

//   for(size_t i=0;i<fModChannelCombo.size();i++)
//       fModChannelCombo[i].ConstructHistograms(folder,prefix);

//   for(size_t i=0;i<fBPMCombo.size();i++)
//       fBPMCombo[i].ConstructHistograms(folder,prefix);
  return;
};

void  QwBeamMod::DeleteHistograms()
{
//   for(size_t i=0;i<fStripline.size();i++)
//     fStripline[i].DeleteHistograms();

//   for(size_t i=0;i<fModChannel.size();i++)
//     fModChannel[i].DeleteHistograms();

//   for(size_t i=0;i<fModChannelCombo.size();i++)
//     fModChannelCombo[i].DeleteHistograms();

//   for(size_t i=0;i<fBPMCombo.size();i++)
//     fBPMCombo[i].DeleteHistograms();

  return;
};

void  QwBeamMod::FillHistograms()
{
//   for(size_t i=0;i<fStripline.size();i++)
//     fStripline[i].FillHistograms();
//   for(size_t i=0;i<fModChannel.size();i++)
//     fModChannel[i].FillHistograms();
//   for(size_t i=0;i<fModChannelCombo.size();i++)
//     fModChannelCombo[i].FillHistograms();
//   for(size_t i=0;i<fBPMCombo.size();i++)
//     fBPMCombo[i].FillHistograms();

  return;
};



void QwBeamMod::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values)
{
  for(size_t i = 0; i < fModChannel.size(); i++)
    fModChannel[i].ConstructBranchAndVector(tree, prefix, values);

  return;
};

void QwBeamMod::FillTreeVector(std::vector<Double_t> &values) const
{
  for(size_t i = 0; i < fModChannel.size(); i++)
    fModChannel[i].FillTreeVector(values);
  return;
};


//*****************************************************************
void  QwBeamMod::Print()
{
  std::cout<<"Name of the subsystem ="<<fSystemName<<"\n";

  std::cout<<"there are "<<fModChannel.size()<<" mods \n";

  std::cout<<" Printing Running AVG and other channel info for fModChannel"<<std::endl;
  for(size_t i=0;i<fModChannel.size();i++)
    fModChannel[i].PrintValue();

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


void  QwBeamMod::Copy(VQwSubsystem *source)
{

  try
    {
     if(typeid(*source)==typeid(*this))
	 {
	  VQwSubsystem::Copy(source);
	  //QwBeamMod* input=((QwBeamMod*)source);
          QwBeamMod* input = dynamic_cast<QwBeamMod*>(source);

	  this->fModChannel.resize(input->fModChannel.size());
	  for(size_t i=0;i<this->fModChannel.size();i++)
	    this->fModChannel[i].Copy(&(input->fModChannel[i]));

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


VQwSubsystem*  QwBeamMod::Copy()
{

  QwBeamMod* TheCopy=new QwBeamMod("BeamMod");
  TheCopy->Copy(this);
  return TheCopy;
}



void QwBeamMod::FillDB(QwDatabase *db, TString datatype)
{
  /*
  vector<QwParityDB::beam> entrylist;

  //      QwParityDB::beam row;
  // Without (0), I see the following error message:
  //terminate called after throwing an instance of 'mysqlpp::BadQuery'
  //  what():  Duplicate entry '11399104' for key 1
  //Abort
  //

  QwParityDB::beam row(0);

  // try to access BCM mean and its error
  // there are 2 different types BCM data we have at the moment
  // Yield and Asymmetry
  printf("%s  ************** BCM **************\n", datatype.Data());
  for(UInt_t i=0; i< fModChannel.size(); i++)
    {
      entrylist.push_back(fModChannel[i].GetDBEntry(db, datatype, "" )) ;
    }

  ///   try to access BPM mean and its error
  printf("%s  ************** BPM **************\n", datatype.Data());
  for(UInt_t i=0; i< fStripline.size(); i++)
    {
      entrylist.push_back(fStripline[i].GetDBEntry(db, datatype, "RelX"));
      entrylist.push_back(fStripline[i].GetDBEntry(db, datatype, "RelY"));
    }

  printf("BeamLine Entrylist Vector Size %d\n", (Int_t) entrylist.size());

  db->Connect();
  // Check the entrylist size, if it isn't zero, start to query..
  if( entrylist.size() )
    {
      mysqlpp::Query query= db->Query();
      //    if(query)
      //	{
	  query.insert(entrylist.begin(), entrylist.end());
	  query.execute();
	  //	  query.reset(); // do we need?
	  //	}
	  //      else
	  //	{
	  //	  printf("Query is empty\n");
	  //	}
    }
  else
    {
      printf("This is the case when the entrlylist contains nothing in %s \n", datatype.Data());
    }

  db->Disconnect();
  */
  return;
};


