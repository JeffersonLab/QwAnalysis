
/**********************************************************\
* File: QwBeamLine.C                                      *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwBeamLine.h"
#include "QwHistogramHelper.h"

#define MYSQLPP_SSQLS_NO_STATICS
#include "QwSSQLS.h"

#include <iostream>
#include <stdexcept>

#include "QwLog.h"

// Register this subsystem with the factory
QwSubsystemFactory<QwBeamLine> theBeamLineFactory("QwBeamLine");

//*****************************************************************
void QwBeamLine::ProcessOptions(QwOptions &options){
      //Handle command line options
};

//*****************************************************************
Int_t QwBeamLine::LoadChannelMap(TString mapfile)
{
  Bool_t ldebug=kFALSE;

  TString varname, varvalue;
  TString modtype, dettype, namech, keyword;
  TString combotype, comboname, dev_name;
  Int_t   modnum, channum;
  Int_t   currentrocread  = 0;
  Int_t   currentbankread = 0;
  Int_t   offset = -1;
  Int_t   currentsubbankindex = -1;
  Int_t   fSample_size = 0;
  Int_t   index = 0;
  Bool_t  combolistdecoded;
  Bool_t  deviceok;
  Bool_t  unrotated(keyword=="unrotated");

  std::vector<TString>  fDeviceName;
  std::vector<TString>  fProperty;
  std::vector<TString>  fType;
  std::vector<Double_t> fQWeight;
  std::vector<Double_t> fXWeight;
  std::vector<Double_t> fYWeight;

  QwParameterFile mapstr(mapfile.Data());  //Open the file

  while (mapstr.ReadNextLine()){

    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;


    if (mapstr.HasVariablePair("=",varname,varvalue)){ //  This is a declaration line.  Decode it.
      varname.ToLower();
      UInt_t value = QwParameterFile::GetUInt(varvalue);

      if (varname=="roc"){
	currentrocread=value;
	RegisterROCNumber(value,0);
      }
      else if (varname=="bank"){
	currentbankread=value;
	RegisterSubbank(value);
      }
      else if (varname=="sample_size"){
	fSample_size=value;
      }
      else if (varname=="begin"){

	// Start to decode derived beamline devices combined+energy
	deviceok = kTRUE;
	combotype = varvalue;
	combolistdecoded = kFALSE;

	while(mapstr.ReadNextLine()&&!combolistdecoded){
	  mapstr.TrimComment('!');   // Remove everything after a '!' character.
	  mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.

	  if (mapstr.LineIsEmpty())  continue;

	  if (mapstr.HasVariablePair("=",varname,varvalue)) {
	    // at the end of the combination
	    if (varname=="end"){
	      // calculate the total weights of the charge
	      for(size_t i=0;i<fDeviceName.size();i++)
		fSumQweights+=fQWeight[i];
	      combolistdecoded = kTRUE;
	    }
	    else
	      comboname = varvalue;

	  }
	  else {
	    dev_name=mapstr.GetNextToken(", ").c_str();
	    dev_name.ToLower();
	    dettype   = mapstr.GetNextToken(", ").c_str();
	    dettype.ToLower();

	    // Check to see if the device being read is a valid physical device.
	    // If not, discard the combination.
	    index=GetDetectorIndex(GetQwBeamInstrumentType(dettype),dev_name);

	    if (index == -1) {
	      QwError << "QwBeamLine::LoadChannelMap:  Unknown device: "
		      <<  dev_name <<" used in "<< comboname
		      <<". This combination  will not be decoded!"
		      <<  QwLog::endl;
	      deviceok = kFALSE;
	      combolistdecoded = kTRUE;
	    }
	    else{
	      // Found the device
	      // Add to the array of names
	      fDeviceName.push_back(dev_name);

	      // Read in the weights.
	      // For combined bpms and combined bcms these are charge weights.
	      // For the energy calculator these are the ratios of the transport matrix elements.
	      fQWeight.push_back( atof(mapstr.GetNextToken(", ").c_str()));

	      // For combined BPMs,in addition, there are weights for the X & Y positions.
	      if(combotype == "combinedbpm"){
		fXWeight.push_back( atof(mapstr.GetNextToken(", ").c_str()));
		fYWeight.push_back( atof(mapstr.GetNextToken(", ").c_str()));
	      }

	      // For the enrgy calculator there are device type and the specified beam parameters.
	      if(combotype == "energycalculator"){
		fType.push_back(dettype);
		fProperty.push_back( mapstr.GetNextToken(", ").c_str());
	      }
	    }
	  }
	}
	// Now create the combined device
	QwBeamDetectorID localComboID(-1, -1, comboname, combotype, "none");

	localComboID.fdetectorname=comboname(0,comboname.Sizeof()-1);
	localComboID.fIndex = GetDetectorIndex(localComboID.fTypeID,localComboID.fdetectorname);

	if(localComboID.fTypeID==-1){
	  QwError << "QwBeamLine::LoadChannelMap:  Unknown detector type: "
		  << combotype <<", the detector "<<comboname<<" will not be decoded "
		  << QwLog::endl;
	  deviceok = kFALSE;
	  continue;
	}

	if((localComboID.fIndex==-1) && deviceok) {

	  // Decoding combined BCM array
	  if (localComboID.fTypeID == kQwCombinedBCM){

	    QwCombinedBCM localbcmcombo(GetSubsystemName(), localComboID.fdetectorname);
	    fBCMCombo.push_back(localbcmcombo);

	    for(size_t i=0;i<fDeviceName.size();i++){
	      index=GetDetectorIndex(GetQwBeamInstrumentType(dettype),fDeviceName[i]);
	      fBCMCombo[fBCMCombo.size()-1].Set(&fBCM[index],fQWeight[i],fSumQweights );
	    }
	    fDeviceName.clear();
	    fQWeight.clear();
	    localComboID.fIndex=fBCMCombo.size()-1;
	  }

	  // Decoding combined BPM array.
	  if(localComboID.fTypeID== kQwCombinedBPM){

	    QwCombinedBPM localbpmcombo(GetSubsystemName(), localComboID.fdetectorname );
	    fBPMCombo.push_back(localbpmcombo);

	    for(size_t i=0;i<fDeviceName.size();i++){
	      index=GetDetectorIndex(GetQwBeamInstrumentType(dettype),fDeviceName[i]);
	      fBPMCombo[fBPMCombo.size()-1].Set(&fStripline[index],
						fQWeight[i],fXWeight[i],
						fYWeight[i],fSumQweights  );

	    }
	    fDeviceName.clear();
	    fQWeight.clear();
	    fXWeight.clear();
	    fYWeight.clear();
	    localComboID.fIndex=fBPMCombo.size()-1;
	  }

	  // Decoding energy calculator.
	  if(localComboID.fTypeID== kQwEnergyCalculator){

	    QwEnergyCalculator localecalculator(GetSubsystemName(), localComboID.fdetectorname);
	    fECalculator.push_back(localecalculator);

	    for(size_t i=0;i<fDeviceName.size();i++){
	      index=GetDetectorIndex(GetQwBeamInstrumentType(fType[i]),fDeviceName[i]);

	      if ( GetQwBeamInstrumentType(fType[i]) == kQwBPMStripline)
		fECalculator[fECalculator.size()-1].Set(&fStripline[index],fType[i],fProperty[i],fQWeight[i]);

	      if ( GetQwBeamInstrumentType(fType[i]) == kQwCombinedBPM)
		fECalculator[fECalculator.size()-1].Set(&fBPMCombo[index],fType[i],fProperty[i],fQWeight[i]);

	    }

	    fDeviceName.clear();
	    fQWeight.clear();

	    fProperty.clear();
	    fType.clear();
	    localComboID.fIndex=fECalculator.size()-1;
	  }
	}
	// Use only the combinations that are of known type and has known physical devices.
	if(deviceok)
	  fBeamDetectorID.push_back(localComboID);
      }
     }
    else{

      // Start to decode the physical beamline devices
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

      if(currentsubbankindex!=GetSubbankIndex(currentrocread,currentbankread)){
	currentsubbankindex=GetSubbankIndex(currentrocread,currentbankread);
      }

      if(modtype=="VQWK"){
	offset = QwVQWK_Channel::GetBufferOffset(modnum, channum);
      }
      else if(modtype=="SCALER") {
	offset = QwSIS3801D24_Channel::GetBufferOffset(modnum, channum);
      }

      if(offset<0){
	QwError<< "QwBeamLine::LoadChannelMap:  Unknown module type: "
	       << modtype <<", the detector "<<namech<<" will not be decoded "
	       << QwLog::endl;
	lineok=kFALSE;
	continue;
      }

      QwBeamDetectorID localBeamDetectorID(currentsubbankindex, offset,
					   namech, dettype, modtype);

      if(localBeamDetectorID.fTypeID==-1){
	QwError << "QwBeamLine::LoadChannelMap:  Unknown detector type: "
		<< dettype <<", the detector "<<namech<<" will not be decoded "
		<< QwLog::endl;
	lineok=kFALSE;
	continue;
      }

      // Remove the subelement name from the bpm name
      if(localBeamDetectorID.fTypeID== kQwBPMStripline)
	localBeamDetectorID.fdetectorname=namech(0,namech.Sizeof()-3);

      // Remove the subelement name from the QPD name
     if(localBeamDetectorID.fTypeID== kQwQPD)
	localBeamDetectorID.fdetectorname=namech(0,namech.Sizeof()-3);

     // Remove the subelement name from the cavity name
      if(localBeamDetectorID.fTypeID==kQwBPMCavity)
	  localBeamDetectorID.fdetectorname=namech(0,namech.Sizeof()-2);

      localBeamDetectorID.fIndex=
	GetDetectorIndex(localBeamDetectorID.fTypeID,
			 localBeamDetectorID.fdetectorname);

      if(localBeamDetectorID.fIndex==-1){

	if(localBeamDetectorID.fTypeID == kQwHaloMonitor){
	  QwHaloMonitor localhalo(GetSubsystemName(), localBeamDetectorID.fdetectorname);
	  fHaloMonitor.push_back(localhalo);
	  localBeamDetectorID.fIndex=fHaloMonitor.size()-1;
	}

	if(localBeamDetectorID.fTypeID==kQwBPMCavity){
	    QwBPMCavity localcavity(GetSubsystemName(), localBeamDetectorID.fdetectorname,!unrotated);
	    fCavity.push_back(localcavity);
	    fCavity[fCavity.size()-1].SetDefaultSampleSize(fSample_size);
	    localBeamDetectorID.fIndex=fCavity.size()-1;
	}

	if(localBeamDetectorID.fTypeID== kQwBPMStripline){
	  QwBPMStripline localstripline(GetSubsystemName(), localBeamDetectorID.fdetectorname,!unrotated);
	  fStripline.push_back(localstripline);
	  fStripline[fStripline.size()-1].SetDefaultSampleSize(fSample_size);
	  localBeamDetectorID.fIndex=fStripline.size()-1;
	}

	if(localBeamDetectorID.fTypeID== kQwBCM){
	  QwBCM localbcm(GetSubsystemName(), localBeamDetectorID.fdetectorname);
	  fBCM.push_back(localbcm);
	  fBCM[fBCM.size()-1].SetDefaultSampleSize(fSample_size);
	  localBeamDetectorID.fIndex=fBCM.size()-1;
	 }

	if(localBeamDetectorID.fTypeID== kQwQPD){
	  QwQPD localqpd(GetSubsystemName(), localBeamDetectorID.fdetectorname);
	  fQPD.push_back(localqpd);
	  fQPD[fQPD.size()-1].SetDefaultSampleSize(fSample_size);
	  localBeamDetectorID.fIndex=fQPD.size()-1;
	 }
	if(localBeamDetectorID.fTypeID== kQwLinearArray){
	  QwLinearDiodeArray locallina(GetSubsystemName(), localBeamDetectorID.fdetectorname);
	  fLinearArray.push_back(locallina);
	  fLinearArray[fLinearArray.size()-1].SetDefaultSampleSize(fSample_size);
	  localBeamDetectorID.fIndex=fLinearArray.size()-1;
	 }

      }

	if(localBeamDetectorID.fTypeID == kQwBPMStripline){
	  TString subname=namech(namech.Sizeof()-3,2);
	  UInt_t localsubindex=
	    fStripline[localBeamDetectorID.fIndex].GetSubElementIndex(subname);
	  if(ldebug){
	    std::cout<<"=================================\n"
		     <<"Looking for subelement index \n"
		     <<" full name ="<<namech
		     <<" part passed to the function "<<subname
		     <<" sub element index="<<localsubindex
		     <<std::endl;
	  }

	  localBeamDetectorID.fSubelement=localsubindex;
	  if(localsubindex>4){
	    QwError << "QwBeamLine::LoadChannelMap: "<<subname
		    <<" was not recognized as a valid antenna for a stripline"
		    <<QwLog::endl;;
	    lineok=kFALSE;
	    continue;
	  }
	}

	if(localBeamDetectorID.fTypeID == kQwBPMCavity){
	  TString subname=namech(namech.Sizeof()-2,1);
	  UInt_t localsubindex=
	    fCavity[localBeamDetectorID.fIndex].GetSubElementIndex(subname);
	  if(ldebug){
	    std::cout<<"=================================\n"
		     <<"Looking for subelement index \n"
		     <<" full name ="<<namech
		     <<" part passed to the function "<<subname
		     <<" sub element index="<<localsubindex
		     <<std::endl;
	  }

	  localBeamDetectorID.fSubelement=localsubindex;
	  if(localsubindex>3){
	    QwError << "QwBeamLine::LoadChannelMap: "<<subname
		    <<" was not recognized as a valid antenna for a cavity"
		    <<QwLog::endl;;
	    lineok=kFALSE;
	    continue;
	  }
	}

	//  Set the subelement for the QPD
	if(localBeamDetectorID.fTypeID == kQwQPD){
	  TString subname=namech(namech.Sizeof()-3,2);
	  UInt_t localsubindex=
	    fQPD[localBeamDetectorID.fIndex].GetSubElementIndex(subname);
	  if(ldebug){
	    std::cout<<"=================================\n"
		     <<"Looking for subelement index \n"
		     <<" full name ="<<namech
		     <<" part passed to the function "<<subname
		     <<" sub element index="<<localsubindex
		     <<std::endl;
	  }

	  localBeamDetectorID.fSubelement=localsubindex;
	  if(localsubindex>4){
	    QwError << "QwBeamLine::LoadChannelMap: "<<subname
		    <<" was not recognized as a valid photodiode for the QPD"
		    <<QwLog::endl;;
	    lineok=kFALSE;
	    continue;
	  }
	}
	
	//  Set the subelement for the LinearArray
	if(localBeamDetectorID.fTypeID == kQwLinearArray){
	  TString subname=namech(namech.Sizeof()-3,2);
	  UInt_t localsubindex=
	    fLinearArray[localBeamDetectorID.fIndex].GetSubElementIndex(subname);
	  if(ldebug){
	    std::cout<<"=================================\n"
		     <<"Looking for subelement index \n"
		     <<" full name ="<<namech
		     <<" part passed to the function "<<subname
		     <<" sub element index="<<localsubindex
		     <<std::endl;
	  }

	  localBeamDetectorID.fSubelement=localsubindex;
	  if(localsubindex==999999){
	    QwError << "QwBeamLine::LoadChannelMap: "<<subname
		    <<" was not recognized as a valid photodiode for the LinearArray"
		    <<QwLog::endl;;
	    lineok=kFALSE;
	    continue;
	  }
	}

      if(ldebug){
	localBeamDetectorID.Print();
	std::cout<<"line ok=";
	if(lineok) std::cout<<"TRUE"<<std::endl;
	else
	  std::cout<<"FALSE"<<std::endl;
      }

      if(lineok)
	fBeamDetectorID.push_back(localBeamDetectorID);

    }
  }
  //now load the publish variables
  mapstr.RewindToFileStart();
  QwParameterFile *section;
  std::vector<TString> publishinfo;
  while ( (section=mapstr.ReadNextSection(varvalue)) ){
    if(varvalue=="PUBLISH"){
      fPublishList.clear();
      while(section->ReadNextLine()){
	section->TrimComment('!');   // Remove everything after a '!' character.
	section->TrimWhitespace();   // Get rid of leading and trailing spaces.
	for (Int_t ii=0;ii<4;ii++){
	  varvalue=section->GetNextToken(",").c_str();
	  if(varvalue.Length()){
	    publishinfo.push_back(varvalue);	  
	  }
	}
	if (publishinfo.size()==4)
	  fPublishList.push_back(publishinfo);
	publishinfo.clear();
      }
    }
  }
  QwMessage<<"To Publish"<<QwLog::endl;
  for (size_t jj=0;jj<fPublishList.size();jj++)
    QwMessage<<fPublishList.at(jj).at(0)<<" "<<fPublishList.at(jj).at(1)<<" "<<fPublishList.at(jj).at(2)<<" "<<" "<<fPublishList.at(jj).at(3)<<" "<<QwLog::endl;

  //exit(1);

  if(ldebug){
    std::cout<<"QwLumi::Done with Load map channel \n";
    for(size_t i=0;i<fBeamDetectorID.size();i++)
      fBeamDetectorID[i].Print();
  }
  ldebug=kFALSE;

  return 0;
};

  //*****************************************************************
QwBeamDetectorID::QwBeamDetectorID(Int_t subbankid, Int_t offset,
				   TString name, TString dettype,
				   TString modtype):
  fSubbankIndex(subbankid),fWordInSubbank(offset),
  fmoduletype(modtype),fdetectorname(name),fdetectortype(dettype)
{
  fTypeID = GetQwBeamInstrumentType(dettype);
};


//*****************************************************************
Int_t QwBeamLine::LoadEventCuts(TString  filename){
  Double_t ULX, LLX, ULY, LLY;
  Int_t samplesize;
  Int_t check_flag;
  Int_t eventcut_flag;
  TString varname, varvalue, vartypeID,varname2, varvalue2;
  TString device_type,device_name,channel_name;
  Int_t det_index = -1;

  QwParameterFile mapstr(filename.Data());  //Open the file

  samplesize = 0;
  check_flag = 0;

  eventcut_flag=1;

  while (mapstr.ReadNextLine()){
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;

    if (mapstr.HasVariablePair("=",varname2,varvalue2)){
      if (varname2=="EVENTCUTS"){
	eventcut_flag= QwParameterFile::GetUInt(varvalue2);
      }
    }
    else{
      device_type= mapstr.GetNextToken(", ").c_str();
      device_type.ToLower();
      device_name= mapstr.GetNextToken(", ").c_str();
      device_name.ToLower();

      det_index=GetDetectorIndex(GetQwBeamInstrumentType(device_type),device_name);


      //set limits to zero
      ULX=0;
      LLX=0;
      ULY=0;
      LLY=0;
      //std::cout << "SHOWME device_type = " << device_type << " and device name = " << device_name << "\n" << std::endl;
      //std::cout << "WHATIS Cavity = " << GetQwBeamInstrumentTypeName(kQwBPMCavity) << "\n" << std::endl;

      if (device_type == GetQwBeamInstrumentTypeName(kQwBCM)){
	LLX = (atof(mapstr.GetNextToken(", ").c_str()));	//lower limit for BCM value
	ULX = (atof(mapstr.GetNextToken(", ").c_str()));	//upper limit for BCM value
	fBCM[det_index].SetSingleEventCuts(LLX,ULX);//(fBCMEventCuts);
      }
      else if (device_type == GetQwBeamInstrumentTypeName(kQwHaloMonitor)){
	LLX = (atof(mapstr.GetNextToken(", ").c_str()));	//lower limit for HaloMonitor value
	ULX = (atof(mapstr.GetNextToken(", ").c_str()));	//upper limit for HaloMonitor value
	//fHaloMonitor[det_index].SetSingleEventCuts(LLX,ULX);//(fHaloMonitorEventCuts);
      }
	else if (device_type ==GetQwBeamInstrumentTypeName(kQwEnergyCalculator)){
	LLX = (atof(mapstr.GetNextToken(", ").c_str()));	//lower limit for energy
	ULX = (atof(mapstr.GetNextToken(", ").c_str()));	//upper limit for energy
	fECalculator[det_index].SetSingleEventCuts(LLX,ULX);//(fEnergyEventCuts);
      }
	else if (device_type == GetQwBeamInstrumentTypeName(kQwBPMStripline)){
	channel_name= mapstr.GetNextToken(", ").c_str();
	channel_name.ToLower();
	LLX = (atof(mapstr.GetNextToken(", ").c_str()));	//lower limit for BPMStripline X
	ULX = (atof(mapstr.GetNextToken(", ").c_str()));	//upper limit for BPMStripline X
	fStripline[det_index].SetSingleEventCuts(channel_name, LLX, ULX);
      }
	else if (device_type == GetQwBeamInstrumentTypeName(kQwQPD)){
	channel_name= mapstr.GetNextToken(", ").c_str();
	channel_name.ToLower();
	LLX = (atof(mapstr.GetNextToken(", ").c_str()));	//lower limit for QPD X
	ULX = (atof(mapstr.GetNextToken(", ").c_str()));	//upper limit for QPD X
	fQPD[det_index].SetSingleEventCuts(channel_name, LLX, ULX);
	}
	else if (device_type == GetQwBeamInstrumentTypeName(kQwLinearArray)){
	channel_name= mapstr.GetNextToken(", ").c_str();
	channel_name.ToLower();
	LLX = (atof(mapstr.GetNextToken(", ").c_str()));	//lower limit for LinearArray X
	ULX = (atof(mapstr.GetNextToken(", ").c_str()));	//upper limit for LinearArray X
	fLinearArray[det_index].SetSingleEventCuts(channel_name, LLX, ULX);
	}
	else if (device_type ==  GetQwBeamInstrumentTypeName(kQwBPMCavity)){
	channel_name= mapstr.GetNextToken(", ").c_str();
	channel_name.ToLower();
	  LLX = (atof(mapstr.GetNextToken(", ").c_str()));	//lower limit for cavity bpm X
	  ULX = (atof(mapstr.GetNextToken(", ").c_str()));	//upper limit for cavity bpm X
	  fCavity[det_index].SetSingleEventCuts(channel_name, LLX, ULX);
      }
      else if (device_type == GetQwBeamInstrumentTypeName(kQwCombinedBCM)){
	LLX = (atof(mapstr.GetNextToken(", ").c_str()));	//lower limit for BCM value
	ULX = (atof(mapstr.GetNextToken(", ").c_str()));	//upper limit for BCM value
	fBCMCombo[det_index].PrintInfo();
	fBCMCombo[det_index].SetSingleEventCuts(LLX,ULX);//(fBCMComboEventCuts);

      }
      else if (device_type == GetQwBeamInstrumentTypeName(kQwCombinedBPM)){
	channel_name= mapstr.GetNextToken(", ").c_str();
	channel_name.ToLower();

	LLX = (atof(mapstr.GetNextToken(", ").c_str()));	//lower limit for combined bpm X
	ULX = (atof(mapstr.GetNextToken(", ").c_str()));	//upper limit for combined bpm X

	fBPMCombo[det_index].SetSingleEventCuts(channel_name, LLX, ULX);
	fBPMCombo[det_index].SetSingleEventCuts(channel_name, LLX, ULX);
      }

    }

  }


  //update the event cut ON/OFF for all the devices
  //std::cout<<"EVENT CUT FLAG"<<eventcut_flag<<std::endl;
  for (size_t i=0;i<fStripline.size();i++)
    fStripline[i].SetEventCutMode(eventcut_flag);

  for (size_t i=0;i<fQPD.size();i++)
    fQPD[i].SetEventCutMode(eventcut_flag);

  for (size_t i=0;i<fLinearArray.size();i++)
    fLinearArray[i].SetEventCutMode(eventcut_flag);

  for (size_t i=0;i<fCavity.size();i++)
    fCavity[i].SetEventCutMode(eventcut_flag);

  for (size_t i=0;i<fBCM.size();i++)
    fBCM[i].SetEventCutMode(eventcut_flag);

  for (size_t i=0;i<fHaloMonitor.size();i++){
    //fHaloMonitor[i].SetEventCutMode(eventcut_flag);
  }
    
  for (size_t i=0;i<fBCMCombo.size();i++)
    fBCMCombo[i].SetEventCutMode(eventcut_flag);

  for (size_t i=0;i<fBPMCombo.size();i++)
    fBPMCombo[i].SetEventCutMode(eventcut_flag);

  for (size_t i=0;i<fECalculator.size();i++)
    fECalculator[i].SetEventCutMode(eventcut_flag);

  fQwBeamLineErrorCount=0; //set the error counter to zero

  return 0;
};


//*****************************************************************
Int_t QwBeamLine::LoadGeometryDefinition(TString mapfile){
  Bool_t ldebug=kFALSE;
  TString varname, varvalue;
  Int_t lineread=1;
  Int_t index = 0;
  TString  devname,devtype;
  TString  melement;
  Double_t devOffsetX = 0,devOffsetY = 0, devOffsetZ = 0;
  Double_t devSENfactor = 0, devAlphaX = 0, devAlphaY = 0;
  TString localname;


  if(ldebug)std::cout<<"QwBeamLine::LoadGeometryParameters("<< mapfile<<")\n";

  QwParameterFile mapstr(mapfile.Data());  //Open the file

  while (mapstr.ReadNextLine()){
    lineread+=1;
    if(ldebug)std::cout<<" line read so far ="<<lineread<<"\n";
    mapstr.TrimComment('!');
    mapstr.TrimWhitespace();

    if (mapstr.LineIsEmpty())  continue;

    Bool_t notfound=kTRUE;

    devtype = mapstr.GetNextToken(", \t").c_str();
    devtype.ToLower();
    devtype.Remove(TString::kBoth,' ');
    devname = mapstr.GetNextToken(", \t").c_str();
    devname.ToLower();
    devname.Remove(TString::kBoth,' ');

    devOffsetX   = (atof(mapstr.GetNextToken(", \t").c_str())); // X offset
    devOffsetY   = (atof(mapstr.GetNextToken(", \t").c_str())); // Y offset
    devOffsetZ   = (atof(mapstr.GetNextToken(", \t").c_str())); // Z offset
    devSENfactor = (atof(mapstr.GetNextToken(", \t").c_str())); // sensivity scaling factor
    devAlphaX    = (atof(mapstr.GetNextToken(", \t").c_str())); // alpha X
    devAlphaY    = (atof(mapstr.GetNextToken(", \t").c_str())); // alpha Y

    index=GetDetectorIndex(GetQwBeamInstrumentType(devtype),devname);

    if(ldebug){
      std::cout<<"####################\n";
      std::cout<<"! device type, device_name, Xoffset, Yoffset, Zoffset, BSEN scaling factor, AlpaX, AlpaY\n"<<std::endl;
      std::cout<<GetQwBeamInstrumentType(devtype)<<" / "
	       <<devOffsetX <<" / "
	       <<devOffsetY <<" / "
	       <<devOffsetZ <<" / "
	       <<devSENfactor <<" / "
	       <<devAlphaX <<" / "
	       <<devAlphaY <<" / "
	       <<std::endl;
    }

    while(notfound){
      if(GetQwBeamInstrumentType(devtype)==kQwBPMStripline){
	//Load bpm offsets
	if(index == -1){
	  QwWarning << "QwBeamLine::LoadGeometryDefinition:  Unknown bpm in qweak_beamline_geometry.map: "
		  <<devname
		  <<QwLog::endl;
	  notfound=kFALSE;
	  continue;
	}

	localname=fStripline[index].GetElementName();
	localname.ToLower();
	if(ldebug)  std::cout<<"element name =="<<localname
			     <<"== to be compared to =="<<devname<<"== \n";

	if(localname==devname){
	  if(ldebug) std::cout<<" I found the bpm !\n";
	  VQwBPM * bpm = &fStripline[index];
	  bpm->GetSurveyOffsets(devOffsetX,devOffsetY,devOffsetZ);
	  bpm->GetElectronicFactors(devSENfactor,devAlphaX, devAlphaY);
	  notfound=kFALSE;
	}
      }
      else if (GetQwBeamInstrumentType(devtype)==kQwCombinedBPM){
	//Load combined bpm offsets which are, ofcourse, target position in the beamline
	if(index == -1){
	  QwError << "QwBeamLine::LoadGeometryDefinition:  Unknown combined bpm in qweak_beamline_geometry.map: "
		  <<devname<<" Check the combined bpm names!\n "
		  << QwLog::endl;
	  notfound=kFALSE;
	  continue;
	}

	localname=fBPMCombo[index].GetElementName();
	localname.ToLower();
	if(ldebug)
	  std::cout<<"element name =="<<localname<<"== to be compared to =="<<devname<<"== \n";

	if(localname==devname){
	  if(ldebug) std::cout<<" I found the combinedbpm !\n";
	  fBPMCombo[index].GetSurveyOffsets(devOffsetX,devOffsetY,devOffsetZ);
	  notfound=kFALSE;
	}
      }

      else if(GetQwBeamInstrumentType(devtype)==kQwBPMCavity){
	//Load bpm offsets
	if(index == -1){
	  QwError << "QwBeamLine::LoadGeometryDefinition:  Unknown bpm : "
		  <<devname<<" will not be asigned with geometry parameters. \n"
		  <<QwLog::endl;
	  notfound=kFALSE;
	  continue;
	}
	localname=fCavity[index].GetElementName();
	localname.ToLower();
	if(ldebug)  std::cout<<"element name =="<<localname
			     <<"== to be compared to =="<<devname<<"== \n";

	if(localname==devname){
	  if(ldebug) std::cout<<" I found the cavity bpm !\n";
	  fCavity[index].GetSurveyOffsets(devOffsetX,devOffsetY,devOffsetZ);
	  notfound=kFALSE;
	}
      }

      else QwError<<" QwBeamLine::LoadGeometryDefinition: Unknown device type :"<<devtype<<". Are you sure we have this in the beamline? I am skipping this."<<QwLog::endl;
    }

  }
  
  if(ldebug) std::cout<<" line read in the geometry file ="<<lineread<<" \n";

  ldebug=kFALSE;
  return 0;

};


//*****************************************************************
Int_t QwBeamLine::LoadInputParameters(TString pedestalfile)
{
  Bool_t ldebug=kFALSE;
  TString varname;
  Double_t varped;
  Double_t varcal;
  Double_t varweight;
  TString localname;

  Int_t lineread=1;

  if(ldebug)std::cout<<"QwBeamLine::LoadInputParameters("<< pedestalfile<<")\n";

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

	  if(notfound) {

	    for(size_t i=0;i<fStripline.size();i++)
	      {
		for(int j=0;j<4;j++)
		  {
		    localname=fStripline[i].GetSubElementName(j);
		    localname.ToLower();
		    if(ldebug)  std::cout<<"Stripline element name =="<<localname
					 <<"== to be compared to =="<<varname<<"== \n";
		    if(localname==varname)
		      {
			if(ldebug) std::cout<<" I found it !\n";
			fStripline[i].SetSubElementPedestal(j,varped);
			fStripline[i].SetSubElementCalibrationFactor(j,varcal);
			notfound=kFALSE;
			j=5;
			i=fStripline.size()+1;
		      }
		  }
	      }

	    for(size_t i=0;i<fQPD.size();i++)
	      {
		for(int j=0;j<4;j++)
		  {
		    localname=fQPD[i].GetSubElementName(j);
		    localname.ToLower();
		    if(ldebug)  std::cout<<"QPD element name =="<<localname
					 <<"== to be compared to =="<<varname<<"== \n";
		    if(localname==varname)
		      {
			if(ldebug) std::cout<<" I found it !\n";
			fQPD[i].SetSubElementPedestal(j,varped);
			fQPD[i].SetSubElementCalibrationFactor(j,varcal);
			notfound=kFALSE;
			j=5;
			i=fQPD.size()+1;
		      }
		  }
	      }

	    for(size_t i=0;i<fLinearArray.size();i++)
	      {
		for(int j=0;j<4;j++)
		  {
		    localname=fLinearArray[i].GetSubElementName(j);
		    localname.ToLower();
		    if(ldebug)  std::cout<<"LinearArray element name =="<<localname
					 <<"== to be compared to =="<<varname<<"== \n";
		    if(localname==varname)
		      {
			if(ldebug) std::cout<<" I found it !\n";
			fLinearArray[i].SetSubElementPedestal(j,varped);
			fLinearArray[i].SetSubElementCalibrationFactor(j,varcal);
			notfound=kFALSE;
			j=5;
			i=fLinearArray.size()+1;
		      }
		  }
	      }
	    
	    for(size_t i=0;i<fCavity.size();i++)
	      {
		for(int j=0;j<2;j++)
		  {
		    localname=fCavity[i].GetSubElementName(j);
		    localname.ToLower();
		    if(ldebug)  std::cout<<"Cavity element name =="<<localname
					 <<"== to be compared to =="<<varname<<"== \n";
		    if(localname==varname)
		      {
			if(ldebug) std::cout<<" I found it !\n";
			fCavity[i].SetSubElementPedestal(j,varped);
			fCavity[i].SetSubElementCalibrationFactor(j,varcal);
			notfound=kFALSE;
			j=3;
			i=fCavity.size()+1;
		      }
		  }
	      }
	    
	    for(size_t i=0;i<fBCM.size();i++) {
	      if(fBCM[i].GetElementName()==varname)
		{
		  fBCM[i].SetPedestal(varped);
		  fBCM[i].SetCalibrationFactor(varcal);
		  i=fBCM.size()+1;
		  notfound=kFALSE;
		  i=fBCM.size()+1;
		}
            }

	    for(size_t i=0;i<fHaloMonitor.size();i++) {
	      if(fHaloMonitor[i].GetElementName()==varname)
		{
		  //fHaloMonitor[i].SetPedestal(varped);
		  //fHaloMonitor[i].SetCalibrationFactor(varcal);
		  i=fHaloMonitor.size()+1;
		  notfound=kFALSE;
		  i=fHaloMonitor.size()+1;
		}
            }



          }


	}

    }
  if(ldebug) std::cout<<" line read in the pedestal + cal file ="<<lineread<<" \n";

  ldebug=kFALSE;
  return 0;
};


//*****************************************************************
void QwBeamLine::RandomizeEventData(int helicity, double time)
{
  // Randomize all QwBPMStripline buffers
  for (size_t i = 0; i < fStripline.size(); i++)
    fStripline[i].RandomizeEventData(helicity, time);

  for (size_t i = 0; i < fCavity.size(); i++)
    fCavity[i].RandomizeEventData(helicity, time);

  // Randomize all QwBCM buffers
  for (size_t i = 0; i < fBCM.size(); i++)
    fBCM[i].RandomizeEventData(helicity, time);

  // Randomize all QwHaloMonitor buffers
  //for (size_t i = 0; i < fHaloMonitor.size(); i++)
    //fHaloMonitor[i].RandomizeEventData(helicity, time);
};


//*****************************************************************
void QwBeamLine::EncodeEventData(std::vector<UInt_t> &buffer)
{
  std::vector<UInt_t> elements;
  elements.clear();

  // Get all buffers in the order they are defined in the map file
  for (size_t i = 0; i < fBeamDetectorID.size(); i++) {
    // This is a QwBCM
    if (fBeamDetectorID.at(i).fTypeID == kQwBCM)
      fBCM[fBeamDetectorID.at(i).fIndex].EncodeEventData(elements);
    // This is a QwBPMStripline (which has 4 entries, only process the first one)
    if (fBeamDetectorID.at(i).fTypeID == kQwBPMStripline
     && fBeamDetectorID.at(i).fSubelement == 0)
      fStripline[fBeamDetectorID.at(i).fIndex].EncodeEventData(elements);
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
};


//*****************************************************************
Int_t QwBeamLine::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  Bool_t lkDEBUG=kFALSE;

  Int_t index = GetSubbankIndex(roc_id,bank_id);
  if (index>=0 && num_words>0){
    //  We want to process this ROC.  Begin looping through the data.
    if (lkDEBUG)
      std::cout << "QwBeamLine::ProcessEvBuffer:  "
		<< "Begin processing ROC" << roc_id
		<< " and subbank "<<bank_id
		<< " number of words="<<num_words<<std::endl;
    if (buffer[0]==0xf0f0f0f0 && num_words%2==1){
      buffer++;
      if (lkDEBUG)
	std::cout << "QwBeamLine::ProcessEvBuffer:  "
		  << "Skipped padding word 0xf0f0f0f0 at beginning of buffer."
		  << std::endl;
    }

    for(size_t i=0;i<fBeamDetectorID.size();i++)
      {
	if(fBeamDetectorID[i].fSubbankIndex==index)
	  {

	    if(fBeamDetectorID[i].fTypeID==kQwBPMStripline)
	      {
		if (lkDEBUG)
		  {
		    std::cout<<"found stripline data for "<<fBeamDetectorID[i].fdetectorname<<std::endl;
		    std::cout<<"word left to read in this buffer:"<<num_words-fBeamDetectorID[i].fWordInSubbank<<std::endl;
		  }
		fStripline[fBeamDetectorID[i].fIndex].
		  ProcessEvBuffer(&(buffer[fBeamDetectorID[i].fWordInSubbank]),
				  num_words-fBeamDetectorID[i].fWordInSubbank,
				  fBeamDetectorID[i].fSubelement);
	      }

	    if(fBeamDetectorID[i].fTypeID==kQwQPD)
	      {
		if (lkDEBUG)
		  {
		    std::cout<<"found qpd data for "<<fBeamDetectorID[i].fdetectorname<<std::endl;
		    std::cout<<"word left to read in this buffer:"<<num_words-fBeamDetectorID[i].fWordInSubbank<<std::endl;
		  }
		fQPD[fBeamDetectorID[i].fIndex].
		  ProcessEvBuffer(&(buffer[fBeamDetectorID[i].fWordInSubbank]),
				  num_words-fBeamDetectorID[i].fWordInSubbank,
				  fBeamDetectorID[i].fSubelement);
	      }

	    if(fBeamDetectorID[i].fTypeID==kQwLinearArray)
	      {
		if (lkDEBUG)
		  {
		    std::cout<<"found qpd data for "<<fBeamDetectorID[i].fdetectorname<<std::endl;
		    std::cout<<"word left to read in this buffer:"<<num_words-fBeamDetectorID[i].fWordInSubbank<<std::endl;
		  }
		fLinearArray[fBeamDetectorID[i].fIndex].
		  ProcessEvBuffer(&(buffer[fBeamDetectorID[i].fWordInSubbank]),
				  num_words-fBeamDetectorID[i].fWordInSubbank,
				  fBeamDetectorID[i].fSubelement);
	      }

	    if(fBeamDetectorID[i].fTypeID==kQwBPMCavity)
	      {
		if (lkDEBUG)
		  {
		    std::cout<<"found stripline data for "<<fBeamDetectorID[i].fdetectorname<<std::endl;
		    std::cout<<"word left to read in this buffer:"<<num_words-fBeamDetectorID[i].fWordInSubbank<<std::endl;
		  }
		fCavity[fBeamDetectorID[i].fIndex].
		  ProcessEvBuffer(&(buffer[fBeamDetectorID[i].fWordInSubbank]),
				  num_words-fBeamDetectorID[i].fWordInSubbank,
				  fBeamDetectorID[i].fSubelement);
	      }

	    if(fBeamDetectorID[i].fTypeID==kQwBCM)
	      {
		if (lkDEBUG)
		  {
		    std::cout<<"found bcm data for "<<fBeamDetectorID[i].fdetectorname<<std::endl;
		    std::cout<<"word left to read in this buffer:"<<num_words-fBeamDetectorID[i].fWordInSubbank<<std::endl;
		  }
		fBCM[fBeamDetectorID[i].fIndex].
		  ProcessEvBuffer(&(buffer[fBeamDetectorID[i].fWordInSubbank]),
				  num_words-fBeamDetectorID[i].fWordInSubbank);
	      }

	    if(fBeamDetectorID[i].fTypeID==kQwHaloMonitor)
	      {
		if (lkDEBUG)
		  {
		    std::cout<<"found halo monitor data for "<<fBeamDetectorID[i].fdetectorname<<std::endl;
		    std::cout<<"word left to read in this buffer:"<<num_words-fBeamDetectorID[i].fWordInSubbank<<std::endl;
		  }
		fHaloMonitor[fBeamDetectorID[i].fIndex].
		  ProcessEvBuffer(&(buffer[fBeamDetectorID[i].fWordInSubbank]),
				  num_words-fBeamDetectorID[i].fWordInSubbank);
	      }

	  }
      }
  }

  return 0;
};



//*****************************************************************
Bool_t QwBeamLine::ApplySingleEventCuts(){

  Bool_t status=kTRUE;

  for(size_t i=0;i<fBCM.size();i++){
    status &= fBCM[i].ApplySingleEventCuts();
    if(!status && bDEBUG) std::cout<<"******* QwBeamLine::SingleEventCuts()->BCM[ "<<i
				   <<" , "<<fBCM[i].GetElementName()<<" ] ******\n";
  }

  for(size_t i=0;i<fHaloMonitor.size();i++){
    status &= fHaloMonitor[i].ApplySingleEventCuts();
    if(!status && bDEBUG) std::cout<<"******* QwBeamLine::SingleEventCuts()->HaloMonitor[ "<<i
				   <<" , "<<fHaloMonitor[i].GetElementName()<<" ] ******\n";
  }

  for(size_t i=0;i<fStripline.size();i++){
    status &= fStripline[i].ApplySingleEventCuts();
    if(!status && bDEBUG) std::cout<<"******** QwBeamLine::SingleEventCuts()->BPMStripline[ "<<i
				   <<" , "<<fStripline[i].GetElementName()<<" ] *****\n";

    }

  for(size_t i=0;i<fQPD.size();i++){
    status &= fQPD[i].ApplySingleEventCuts();
    if(!status && bDEBUG) std::cout<<"******** QwBeamLine::SingleEventCuts()->QPD[ "<<i
				   <<" , "<<fQPD[i].GetElementName()<<" ] *****\n";
  }
  for(size_t i=0;i<fLinearArray.size();i++){
    status &= fLinearArray[i].ApplySingleEventCuts();
    if(!status && bDEBUG) std::cout<<"******** QwBeamLine::SingleEventCuts()->LinearArray[ "<<i
				   <<" , "<<fLinearArray[i].GetElementName()<<" ] *****\n";
  }

  for(size_t i=0;i<fCavity.size();i++){
    status &= fCavity[i].ApplySingleEventCuts();
    if(!status && bDEBUG) std::cout<<"******** QwBeamLine::SingleEventCuts()->BPMCavity[ "<<i
				   <<" , "<<fCavity[i].GetElementName()<<" ] *****\n";
  }

  for(size_t i=0;i<fBCMCombo.size();i++){
    status &= fBCMCombo[i].ApplySingleEventCuts();
    if(!status && bDEBUG) std::cout<<"******* QwBeamLine::SingleEventCuts()->CombinedBCM[ "<<i
				   <<" , "<<fBCMCombo[i].GetElementName()<<" ] ******\n";
  }

  for(size_t i=0;i<fBPMCombo.size();i++){
    status &= fBPMCombo[i].ApplySingleEventCuts();
    if(!status && bDEBUG) std::cout<<"******* QwBeamLine::SingleEventCuts()->CombinedBPM[ "<<i
				   <<" , "<<fBPMCombo[i].GetElementName()<<" ] ******\n";

  }
  for(size_t i=0;i<fECalculator.size();i++){
    status &= fECalculator[i].ApplySingleEventCuts();
    if(!status && bDEBUG) std::cout<<"******* QwBeamLine::SingleEventCuts()->EnergyCalculator[ "<<i
				   <<" , "<<fECalculator[i].GetElementName()<<" ] ******\n";

  }


  //If at least one of the devices falied  event cuts, increment error counter for QwBeamLine
  if (!status)
    fQwBeamLineErrorCount++;


  return status;

};


//*****************************************************************
Int_t QwBeamLine::GetEventcutErrorCounters(){//inherited from the VQwSubsystemParity; this will display the error summary

  std::cout<<"*********QwBeamLine Error Summary****************"<<std::endl;
  std::cout<<"Device name ||  Sample || SW_HW || Sequence || SameHW || ZeroHW || EventCut\n";
  for(size_t i=0;i<fBCM.size();i++){
    fBCM[i].GetEventcutErrorCounters();
  }

  for(size_t i=0;i<fHaloMonitor.size();i++){
    fHaloMonitor[i].GetEventcutErrorCounters();
  }

  for(size_t i=0;i<fStripline.size();i++){
    fStripline[i].GetEventcutErrorCounters();
  }

 for(size_t i=0;i<fQPD.size();i++){
    fQPD[i].GetEventcutErrorCounters();
  }

 for(size_t i=0;i<fLinearArray.size();i++){
    fLinearArray[i].GetEventcutErrorCounters();
  }

 for(size_t i=0;i<fCavity.size();i++){
    fCavity[i].GetEventcutErrorCounters();
  }

  for(size_t i=0;i<fBCMCombo.size();i++){
    fBCMCombo[i].GetEventcutErrorCounters();
  }

  for(size_t i=0;i<fBPMCombo.size();i++){
    fBPMCombo[i].GetEventcutErrorCounters();
  }

  for(size_t i=0;i<fECalculator.size();i++){
    fECalculator[i].GetEventcutErrorCounters();
  }
   std::cout<<"---------------------------------------------------"<<std::endl;
   std::cout<<std::endl;
  return 1;
};

//*****************************************************************
Int_t QwBeamLine::GetEventcutErrorFlag(){//return the error flag
  Int_t ErrorFlag;
  ErrorFlag=0;
  for(size_t i=0;i<fBCM.size();i++){
    ErrorFlag |= fBCM[i].GetEventcutErrorFlag();
  }
  for(size_t i=0;i<fHaloMonitor.size();i++){
    //ErrorFlag |= fHaloMonitor[i].GetEventcutErrorFlag();
  }
  for(size_t i=0;i<fStripline.size();i++){
    ErrorFlag |= fStripline[i].GetEventcutErrorFlag();
  }
  for(size_t i=0;i<fQPD.size();i++){
    ErrorFlag |= fQPD[i].GetEventcutErrorFlag();
  }
  for(size_t i=0;i<fLinearArray.size();i++){
    ErrorFlag |= fLinearArray[i].GetEventcutErrorFlag();
  }
  for(size_t i=0;i<fCavity.size();i++){
    ErrorFlag |= fCavity[i].GetEventcutErrorFlag();
  }
  for(size_t i=0;i<fBCMCombo.size();i++){
    ErrorFlag |= fBCMCombo[i].GetEventcutErrorFlag();
  }
  for(size_t i=0;i<fBPMCombo.size();i++){
    ErrorFlag |= fBPMCombo[i].GetEventcutErrorFlag();
  }
  for(size_t i=0;i<fECalculator.size();i++){
    ErrorFlag |= fECalculator[i].GetEventcutErrorFlag();
  }
  return ErrorFlag;

};


//*****************************************************************
void  QwBeamLine::ProcessEvent()
{
  for(size_t i=0;i<fStripline.size();i++)
    fStripline[i].ProcessEvent();

  for(size_t i=0;i<fCavity.size();i++)
    fCavity[i].ProcessEvent();

  for(size_t i=0;i<fBCM.size();i++)
    fBCM[i].ProcessEvent();

  for(size_t i=0;i<fQPD.size();i++)
    fQPD[i].ProcessEvent();

  for(size_t i=0;i<fLinearArray.size();i++)
    fLinearArray[i].ProcessEvent();

  for(size_t i=0;i<fHaloMonitor.size();i++)
    fHaloMonitor[i].ProcessEvent();

  for(size_t i=0;i<fBCMCombo.size();i++)
    fBCMCombo[i].ProcessEvent();

  for(size_t i=0;i<fBPMCombo.size();i++)
    fBPMCombo[i].ProcessEvent();

  for(size_t i=0;i<fECalculator.size();i++)
    fECalculator[i].ProcessEvent();

  return;
};


//*****************************************************************
Int_t QwBeamLine::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{

  return 0;
};

//*****************************************************************
const Bool_t QwBeamLine::PublishInternalValues() const 
{
  VQwDataElement* tmp_channel;
  TString publish_name,device_type,device_name, device_prop;
   ///  TODO:  The published variable list should be generated from
  ///         the channel map file.
  // Publish variables
  Bool_t status = kTRUE;
  
  //publish variables through map file
  //this should work with bcm,bpmstripline,bpmcavity, combo bpm and combo bcm
  for(size_t pp=0;pp<fPublishList.size();pp++){
    publish_name=fPublishList.at(pp).at(0);
    device_type=fPublishList.at(pp).at(1);
    device_name=fPublishList.at(pp).at(2);
    device_prop=fPublishList.at(pp).at(3);
    device_type.ToLower();
    device_prop.ToLower();
    //QwError<<publish_name << " "<<device_type<<" "<<device_name<<" "<<device_prop<< QwLog::endl;
    if (device_type=="bcm"){
      tmp_channel=const_cast<VQwDataElement*>((const_cast<QwBCM*>(GetBCM(device_name)))->GetCharge());
    } else if (device_type=="bpmstripline"){
      if (device_prop=="x")
	tmp_channel=const_cast<VQwDataElement*>((const_cast<QwBPMStripline*>(GetBPMStripline(device_name)))->GetPositionX());
      else if (device_prop=="y")
	tmp_channel=const_cast<VQwDataElement*>((const_cast<QwBPMStripline*>(GetBPMStripline(device_name)))->GetPositionY());
      else if (device_prop=="ef")
	tmp_channel=const_cast<VQwDataElement*>((const_cast<QwBPMStripline*>(GetBPMStripline(device_name)))->GetEffectiveCharge());
    }else if (device_type=="bpmcavity"){
      if (device_prop=="x")
	tmp_channel=const_cast<VQwDataElement*>((const_cast<QwBPMCavity*>(GetBPMCavity(device_name)))->GetPositionX());
      else if (device_prop=="y")
	tmp_channel=const_cast<VQwDataElement*>((const_cast<QwBPMCavity*>(GetBPMCavity(device_name)))->GetPositionY());
      else if (device_prop=="ef")
	tmp_channel=const_cast<VQwDataElement*>((const_cast<QwBPMCavity*>(GetBPMCavity(device_name)))->GetEffectiveCharge());
    } else if (device_type=="combobpm"){
      if (device_prop=="x")
	tmp_channel=const_cast<VQwDataElement*>((const_cast<QwCombinedBPM*>(GetCombinedBPM(device_name)))->GetPositionX());
      else if (device_prop=="y")
	tmp_channel=const_cast<VQwDataElement*>((const_cast<QwCombinedBPM*>(GetCombinedBPM(device_name)))->GetPositionY());
      else if (device_prop=="xp")
	tmp_channel=const_cast<VQwDataElement*>((const_cast<QwCombinedBPM*>(GetCombinedBPM(device_name)))->GetAngleX());
      else if (device_prop=="yp")
	tmp_channel=const_cast<VQwDataElement*>((const_cast<QwCombinedBPM*>(GetCombinedBPM(device_name)))->GetAngleY());
    } else if (device_type=="combobcm"){
      tmp_channel=const_cast<VQwDataElement*>((const_cast<QwCombinedBCM*>(GetCombinedBCM(device_name)))->GetCharge());
    } else if (device_type=="comboenergy"){
      tmp_channel=const_cast<VQwDataElement*>((const_cast<QwEnergyCalculator*>(GetEnergyCalculator(device_name)))->GetEnergy());
    } else
      QwError << "QwBeamLine::PublishInternalValues() error "<< QwLog::endl;
    
     if (tmp_channel==NULL){
       QwError << "QwBeamLine::PublishInternalValues():"<<publish_name <<"  not found "<< QwLog::endl;
       status |= kFALSE;
     }else{
       QwDebug << "QwBeamLine::PublishInternalValues():"<<publish_name <<" found "<< QwLog::endl;
       //QwError << "QwBeamLine::PublishInternalValues():"<<publish_name <<" found "<< QwLog::endl;
       //status = status && PublishInternalValue(publish_name, "published-value");
       //const_cast<QwBeamLine*>(this)->UpdatePublishValue(publish_name,tmp_channel);
     }
     status = status && PublishInternalValue(publish_name, "published-value");
     const_cast<QwBeamLine*>(this)->UpdatePublishValue(publish_name,tmp_channel);

  }
  
  /*
// *********************
//This was old way of publishing variables, only problem here is that after add/modify/delete published variables, need to recompile!

  //publish q_targ
  status = status && PublishInternalValue("q_targ", "Calculated charge on target");
  tmp_channel=const_cast<VQwDataElement*>((const_cast<QwCombinedBCM*>(GetCombinedBCM("qwk_charge")))->GetCharge());
  if (tmp_channel==NULL){
    QwError << "QwBeamLine::PublishInternalValues(): q_targ - qwk_charge not found "<< QwLog::endl;
    status |= kFALSE;
  }else
    QwDebug << "QwBeamLine::PublishInternalValues(): q_targ - qwk_charge found "<< QwLog::endl;
  const_cast<QwBeamLine*>(this)->UpdatePublishValue("q_targ",tmp_channel);

  //publish x_targ

  status = status && PublishInternalValue("x_targ", "Calculated beam X on the target");
  tmp_channel=const_cast<VQwDataElement*>((const_cast<QwCombinedBPM*>(GetCombinedBPM("qwk_target")))->GetPositionX());
  if (tmp_channel==NULL){
    QwError << "QwBeamLine::PublishInternalValues(): x_targ - qwk_target X not found "<< QwLog::endl;
    status |= kFALSE;
  }else
    QwDebug << "QwBeamLine::PublishInternalValues(): x_targ - qwk_target X found "<< QwLog::endl;
  const_cast<QwBeamLine*>(this)->UpdatePublishValue("x_targ",tmp_channel);

  //publish y_targ

  status = status && PublishInternalValue("y_targ", "Calculated beam Y on the target");
  tmp_channel=const_cast<VQwDataElement*>((const_cast<QwCombinedBPM*>(GetCombinedBPM("qwk_target")))->GetPositionY());
  if (tmp_channel==NULL){
    QwError << "QwBeamLine::PublishInternalValues(): y_targ - qwk_target Y not found "<< QwLog::endl;
    status |= kFALSE;
  }else
    QwDebug << "QwBeamLine::PublishInternalValues(): y_targ - qwk_target Y found "<< QwLog::endl;
  const_cast<QwBeamLine*>(this)->UpdatePublishValue("y_targ",tmp_channel);

  //publish xp_targ

  status = status && PublishInternalValue("xp_targ", "Calculated beam X_angle on the target");
  tmp_channel=const_cast<VQwDataElement*>((const_cast<QwCombinedBPM*>(GetCombinedBPM("qwk_target")))->GetAngleX());
  if (tmp_channel==NULL){
    QwError << "QwBeamLine::PublishInternalValues(): xp_targ - qwk_target X_angle not found "<< QwLog::endl;
    status |= kFALSE;
  }else
    QwDebug << "QwBeamLine::PublishInternalValues(): xp_targ - qwk_target X_angle found "<< QwLog::endl;
  const_cast<QwBeamLine*>(this)->UpdatePublishValue("xp_targ",tmp_channel);


  //publish yp_targ

  status = status && PublishInternalValue("yp_targ", "Calculated beam Y_angle on the target");
  
  tmp_channel=const_cast<VQwDataElement*>((const_cast<QwCombinedBPM*>(GetCombinedBPM("qwk_target")))->GetAngleY());
  if (tmp_channel==NULL){
    QwError << "QwBeamLine::PublishInternalValues(): yp_targ - qwk_target Y_angle not found "<< QwLog::endl;
    status |= kFALSE;
  }else
    QwDebug << "QwBeamLine::PublishInternalValues(): yp_targ - qwk_target Y_angle found "<< QwLog::endl;
  
  const_cast<QwBeamLine*>(this)->UpdatePublishValue("yp_targ",tmp_channel);

  
  //publish e_targ

  status = status && PublishInternalValue("e_targ", "Calculated beam energy on the target");
  
  tmp_channel=const_cast<VQwDataElement*>((const_cast<QwEnergyCalculator*>(GetEnergyCalculator("qwk_energy")))->GetEnergy());
  if (tmp_channel==NULL){
    QwError << "QwBeamLine::PublishInternalValues(): e_targ - qwk_energy energy  not found "<< QwLog::endl;
    status |= kFALSE;
  }else
    QwDebug << "QwBeamLine::PublishInternalValues(): e_targ - qwk_energy energy found "<< QwLog::endl;
  const_cast<QwBeamLine*>(this)->UpdatePublishValue("e_targ",tmp_channel);


  
   
  status = status && PublishInternalValue("qwk_3c16_effectivecharge", "Four wire sum of qwk_target");  
  tmp_channel=const_cast<VQwDataElement*>((const_cast<QwBPMStripline*>(GetBPMStripline("qwk_bpm3c16")))->GetEffectiveCharge());
  if (tmp_channel==NULL){
    QwError << "QwBeamLine::PublishInternalValues(): qwk_qwk_3c16_effectivecharge - qwk_qwk_3c16_EffectiveCharge not found "<< QwLog::endl;
    status |= kFALSE;
  }else
    QwDebug << "QwBeamLine::PublishInternalValues(): qwk_qwk_3c16_EffectiveCharge - qwk_qwk_3c16_EffectiveCharge found "<< QwLog::endl;
  const_cast<QwBeamLine*>(this)->UpdatePublishValue("qwk_3c16_effectivecharge",tmp_channel);
  // *********************
  */
  return status;

};


//*****************************************************************
/**
 * Return the value of variable name
 * @param name Name of the desired variable
 * @param value Pointer to the value to be filled by the call
 * @return True if the variable was found, false if not found
 */
const Bool_t QwBeamLine::ReturnInternalValue(const TString& name,
				       VQwDataElement* value) const
{
  Bool_t ldebug=kFALSE;
  if (ldebug) std::cout << "QwBeamLine::ReturnInternalValue called for value name, "<< name.Data() <<std::endl;
  Bool_t foundit = kFALSE;
  /*
  QwVQWK_Channel* tmp = dynamic_cast<QwVQWK_Channel*>(value);
  if (tmp==NULL){
    QwWarning << "QwBeamLine::ReturnInternalValue requires that "
	      << "'value' be a pointer to QwVQWK_Channel"
	      << QwLog::endl;
  } else {
    if (name=="q_targ"  && (GetBCM("qwk_charge"))!=NULL){
      foundit = kTRUE;
      (*tmp) = GetBCM("qwk_charge")->GetCharge();
        if (ldebug) std::cout<<"QwBeamLine::ReturnInternalValue got element qwk_charge"<<std::endl;
    }

    //test for x_targ, y_targ, ...
    else if (name=="x_targ"){
      foundit = kTRUE;
      //(*tmp) = GetBPMStripline("ch_name")->GetSomething();
        if (ldebug) std::cout<<"QwBeamLine::ReturnInternalValue got element for x_targ"<<std::endl;
    }
    else if (name=="y_targ"){
      foundit = kTRUE;
      //(*tmp) = GetBPMStripline("ch_name")->GetSomething();
        if (ldebug) std::cout<<"QwBeamLine::ReturnInternalValue got element for y_targ"<<std::endl;
    }
    else if (name=="xp_targ"){
      foundit = kTRUE;
      //(*tmp) = GetBPMStripline("ch_name")->GetSomething();
        if (ldebug) std::cout<<"QwBeamLine::ReturnInternalValue got element for xp_targ"<<std::endl;
    }
    else if (name=="yp_targ"){
      foundit = kTRUE;
      //(*tmp) = GetBPMStripline("ch_name")->GetSomething();
        if (ldebug) std::cout<<"QwBeamLine::ReturnInternalValue got element for yp_targ"<<std::endl;
    }
    else if (name=="e_targ"){
      foundit = kTRUE;
      //(*tmp) = GetBCM("ch_name")->GetCharge();
        if (ldebug) std::cout<<"QwBeamLine::ReturnInternalValue got element for e_targ"<<std::endl;
    }
  }
  */
  return foundit;
};

const VQwDataElement* QwBeamLine::ReturnInternalValue(const TString& name) const {
  Bool_t ldebug=kFALSE;
  if (ldebug) 
    std::cout << "QwBeamLine::ReturnInternalValue called for value name, "<< name.Data() <<std::endl;
  return const_cast<QwBeamLine*>(this)->GetPublishValue(name);
};


//*****************************************************************
void QwBeamLine::ClearEventData()
{
  for(size_t i=0;i<fStripline.size();i++)
    fStripline[i].ClearEventData();
  for(size_t i=0;i<fCavity.size();i++)
    fCavity[i].ClearEventData();
  for(size_t i=0;i<fBCM.size();i++)
    fBCM[i].ClearEventData();
  for(size_t i=0;i<fQPD.size();i++)
    fQPD[i].ClearEventData();
  for(size_t i=0;i<fLinearArray.size();i++)
    fLinearArray[i].ClearEventData();
  for(size_t i=0;i<fHaloMonitor.size();i++)
    fHaloMonitor[i].ClearEventData();

  for(size_t i=0;i<fBCMCombo.size();i++)
    fBCMCombo[i].ClearEventData();
  for(size_t i=0;i<fBPMCombo.size();i++)
    fBPMCombo[i].ClearEventData();
  for(size_t i=0;i<fECalculator.size();i++)
    fECalculator[i].ClearEventData();
  return;
};

//*****************************************************************
Int_t QwBeamLine::GetDetectorIndex( EQwBeamInstrumentType type_id, TString name)
{
  Bool_t ldebug=kFALSE;
  if(ldebug)
    {
      std::cout<<"QwBeamLine::GetDetectorIndex\n";
      std::cout<<"type_id=="<<type_id<<" name="<<name<<"\n";
      std::cout<<fBeamDetectorID.size()<<" already registered detector\n";
    }

  Int_t result=-1;
  for(size_t i=0;i<fBeamDetectorID.size();i++)
    {
      if(fBeamDetectorID[i].fTypeID==type_id)
	if(fBeamDetectorID[i].fdetectorname==name)
	  result=fBeamDetectorID[i].fIndex;
      if(ldebug)
	{
	  std::cout<<"testing against ("<<fBeamDetectorID[i].fTypeID
		   <<","<<fBeamDetectorID[i].fdetectorname<<")=>"<<result<<"\n";
	}
    }

  return result;
};

//*****************************************************************
QwBPMStripline* QwBeamLine::GetBPMStripline(const TString name)
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

//*****************************************************************

QwBPMCavity* QwBeamLine::GetBPMCavity(const TString name)
{
  if (! fCavity.empty()) {
    for (std::vector<QwBPMCavity>::iterator cavity = fCavity.begin(); cavity != fCavity.end(); ++cavity) {
      if (cavity->GetElementName() == name) {
	return &(*cavity);
      }
    }
  }
  return 0;
};


//*****************************************************************
QwBCM* QwBeamLine::GetBCM(const TString name)
{
  //QwWarning << "QwBeamLine::GetBCM" << QwLog::endl;
  if (! fBCM.empty()) {
    
    for (std::vector<QwBCM>::iterator bcm = fBCM.begin(); bcm != fBCM.end(); ++bcm) {
      if (bcm->GetElementName() == name) {
	return &(*bcm);
      }
    }
    
    //QwWarning << "BCM Found" << QwLog::endl;
    return 0;
  }
  return 0;
};


//*****************************************************************
QwCombinedBCM* QwBeamLine::GetCombinedBCM(const TString name)
{
  //QwWarning << "QwBeamLine::GetCombinedBCM" << QwLog::endl;
  if (! fBCMCombo.empty()) {
    
    for (std::vector<QwCombinedBCM>::iterator cbcm = fBCMCombo.begin(); cbcm != fBCMCombo.end(); ++cbcm) {
      if (cbcm->GetElementName() == name) {
	return &(*cbcm);
      }
    }
    

  }
  return 0;
};

//*****************************************************************
QwCombinedBPM* QwBeamLine::GetCombinedBPM(const TString name)
{
  //QwWarning << "QwBeamLine::GetCombinedBCM" << QwLog::endl;
  if (! fBPMCombo.empty()) {
    
    for (std::vector<QwCombinedBPM>::iterator cbpm = fBPMCombo.begin(); cbpm != fBPMCombo.end(); ++cbpm) {
      if (cbpm->GetElementName() == name) {
	return &(*cbpm);
      }
    }
    

  }
  return 0;
};

//*****************************************************************
QwEnergyCalculator* QwBeamLine::GetEnergyCalculator(const TString name){
   if (! fECalculator.empty()) {
    
    for (std::vector<QwEnergyCalculator>::iterator ecal = fECalculator.begin(); ecal != fECalculator.end(); ++ecal) {
      if (ecal->GetElementName() == name) {
	return &(*ecal);
      }
    }
    

  }
  return 0;
};

//*****************************************************************
const QwBPMStripline* QwBeamLine::GetBPMStripline(const TString name) const
{
  return const_cast<QwBeamLine*>(this)->GetBPMStripline(name);
};

//*****************************************************************
const QwBPMCavity* QwBeamLine::GetBPMCavity(const TString name) const
{
  return const_cast<QwBeamLine*>(this)->GetBPMCavity(name);
};

//*****************************************************************
const QwBCM* QwBeamLine::GetBCM(const TString name) const
{
  return const_cast<QwBeamLine*>(this)->GetBCM(name);
};

//*****************************************************************
const QwCombinedBCM* QwBeamLine::GetCombinedBCM(const TString name) const{
  return const_cast<QwBeamLine*>(this)->GetCombinedBCM(name);
};

//*****************************************************************
const QwCombinedBPM* QwBeamLine::GetCombinedBPM(const TString name) const{
  return const_cast<QwBeamLine*>(this)->GetCombinedBPM(name);
};

//*****************************************************************
const QwEnergyCalculator* QwBeamLine::GetEnergyCalculator(const TString name) const{
  return const_cast<QwBeamLine*>(this)->GetEnergyCalculator(name);
};

//*****************************************************************
VQwSubsystem&  QwBeamLine::operator=  (VQwSubsystem *value)
{
  //  std::cout<<" here in QwBeamLine::operator= \n";
  if(Compare(value))
    {

      QwBeamLine* input = dynamic_cast<QwBeamLine*>(value);

      for(size_t i=0;i<input->fStripline.size();i++)
	this->fStripline[i]=input->fStripline[i];
      for(size_t i=0;i<input->fQPD.size();i++)
	this->fQPD[i]=input->fQPD[i];
      for(size_t i=0;i<input->fLinearArray.size();i++)
	this->fLinearArray[i]=input->fLinearArray[i];
      for(size_t i=0;i<input->fCavity.size();i++)
	this->fCavity[i]=input->fCavity[i];
      for(size_t i=0;i<input->fBCM.size();i++)
	this->fBCM[i]=input->fBCM[i];
      for(size_t i=0;i<input->fHaloMonitor.size();i++)
	this->fHaloMonitor[i]=input->fHaloMonitor[i];
      for(size_t i=0;i<input->fBCMCombo.size();i++)
	this->fBCMCombo[i]=input->fBCMCombo[i];
      for(size_t i=0;i<input->fBPMCombo.size();i++)
	this->fBPMCombo[i]=input->fBPMCombo[i];
      for(size_t i=0;i<input->fECalculator.size();i++)
	this->fECalculator[i]=input->fECalculator[i];

      if (input->fPublishList.size()>0){
	this->fPublishList.resize(input->fPublishList.size());
	for(size_t i=0;i<input->fPublishList.size();i++){
	  this->fPublishList.at(i).resize(input->fPublishList.at(i).size());
	  for(size_t j=0;j<input->fPublishList.at(i).size();j++){
	    this->fPublishList.at(i).at(j)=input->fPublishList.at(i).at(j);
	  }
	}
      }

    }
  return *this;
};


//*****************************************************************
VQwSubsystem&  QwBeamLine::operator+=  (VQwSubsystem *value)
{
  if(Compare(value))
    {
      //QwBeamLine* input= (QwBeamLine*)value ;
      QwBeamLine* input = dynamic_cast<QwBeamLine*>(value);

      for(size_t i=0;i<input->fStripline.size();i++)
	this->fStripline[i]+=input->fStripline[i];
      for(size_t i=0;i<input->fCavity.size();i++)
	this->fCavity[i]+=input->fCavity[i];
      for(size_t i=0;i<input->fQPD.size();i++)
	this->fQPD[i]+=input->fQPD[i];
      for(size_t i=0;i<input->fLinearArray.size();i++)
	this->fLinearArray[i]+=input->fLinearArray[i];
      for(size_t i=0;i<input->fBCM.size();i++)
	this->fBCM[i]+=input->fBCM[i];
      for(size_t i=0;i<input->fHaloMonitor.size();i++)
	this->fHaloMonitor[i]+=input->fHaloMonitor[i];
      for(size_t i=0;i<input->fBCMCombo.size();i++)
	this->fBCMCombo[i]+=input->fBCMCombo[i];
      for(size_t i=0;i<input->fBPMCombo.size();i++)
	this->fBPMCombo[i]+=input->fBPMCombo[i];
      for(size_t i=0;i<input->fECalculator.size();i++)
	this->fECalculator[i]+=input->fECalculator[i];

      if (input->fPublishList.size()>0){
	this->fPublishList.resize(input->fPublishList.size());
	for(size_t i=0;i<input->fPublishList.size();i++){
	  this->fPublishList.at(i).resize(input->fPublishList.at(i).size());
	  for(size_t j=0;j<input->fPublishList.at(i).size();j++){
	    this->fPublishList.at(i).at(j)=input->fPublishList.at(i).at(j);
	  }
	}
      }

    }
  return *this;
};

//*****************************************************************
VQwSubsystem&  QwBeamLine::operator-=  (VQwSubsystem *value)
{

  if(Compare(value))
    {
      QwBeamLine* input = dynamic_cast<QwBeamLine*>(value);

      for(size_t i=0;i<input->fStripline.size();i++)
	this->fStripline[i]-=input->fStripline[i];
      for(size_t i=0;i<input->fCavity.size();i++)
	this->fCavity[i]-=input->fCavity[i];
      for(size_t i=0;i<input->fQPD.size();i++)
	this->fQPD[i]-=input->fQPD[i];
      for(size_t i=0;i<input->fLinearArray.size();i++)
	this->fLinearArray[i]-=input->fLinearArray[i];
      for(size_t i=0;i<input->fBCM.size();i++)
	this->fBCM[i]-=input->fBCM[i];
      for(size_t i=0;i<input->fHaloMonitor.size();i++)
	this->fHaloMonitor[i]-=input->fHaloMonitor[i];
      for(size_t i=0;i<input->fBCMCombo.size();i++)
	this->fBCMCombo[i]-=input->fBCMCombo[i];
      for(size_t i=0;i<input->fBPMCombo.size();i++)
	this->fBPMCombo[i]-=input->fBPMCombo[i];
      for(size_t i=0;i<input->fECalculator.size();i++)
	this->fECalculator[i]-=input->fECalculator[i];

      if (input->fPublishList.size()>0){
	this->fPublishList.resize(input->fPublishList.size());
	for(size_t i=0;i<input->fPublishList.size();i++){
	  this->fPublishList.at(i).resize(input->fPublishList.at(i).size());
	  for(size_t j=0;j<input->fPublishList.at(i).size();j++){
	    this->fPublishList.at(i).at(j)=input->fPublishList.at(i).at(j);
	  }
	}
      } 

    }
  return *this;
};


//*****************************************************************
void  QwBeamLine::Sum(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if(Compare(value1)&&Compare(value2))
    {
      *this =  value1;
      *this += value2;
    }
};


//*****************************************************************
void  QwBeamLine::Difference(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if(Compare(value1)&&Compare(value2))
    {
      *this =  value1;
      *this -= value2;
    }
};


//*****************************************************************
void QwBeamLine::Ratio(VQwSubsystem  *numer, VQwSubsystem  *denom)
{
  if(Compare(numer)&&Compare(denom))
    {
      QwBeamLine* innumer = dynamic_cast<QwBeamLine*>(numer);
      QwBeamLine* indenom = dynamic_cast<QwBeamLine*>(denom);

      for(size_t i=0;i<innumer->fStripline.size();i++)
	this->fStripline[i].Ratio(innumer->fStripline[i],indenom->fStripline[i]);
      for(size_t i=0;i<innumer->fCavity.size();i++)
	this->fCavity[i].Ratio(innumer->fCavity[i],indenom->fCavity[i]);
      for(size_t i=0;i<innumer->fQPD.size();i++)
	this->fQPD[i].Ratio(innumer->fQPD[i],indenom->fQPD[i]);
      for(size_t i=0;i<innumer->fLinearArray.size();i++)
	this->fLinearArray[i].Ratio(innumer->fLinearArray[i],indenom->fLinearArray[i]);
      for(size_t i=0;i<innumer->fBCM.size();i++)
	this->fBCM[i].Ratio(innumer->fBCM[i],indenom->fBCM[i]);
     for(size_t i=0;i<innumer->fHaloMonitor.size();i++)
	this->fHaloMonitor[i].Ratio(innumer->fHaloMonitor[i],indenom->fHaloMonitor[i]);
      for(size_t i=0;i<innumer->fBCMCombo.size();i++)
	this->fBCMCombo[i].Ratio(innumer->fBCMCombo[i],indenom->fBCMCombo[i]);
      for(size_t i=0;i<innumer->fBPMCombo.size();i++)
	this->fBPMCombo[i].Ratio(innumer->fBPMCombo[i],indenom->fBPMCombo[i]);
     for(size_t i=0;i<innumer->fECalculator.size();i++)
       this->fECalculator[i].Ratio(innumer->fECalculator[i],indenom->fECalculator[i]);

      // For the combined bcm, maybe we might want to think about getting
      // the asymmetry using the asymmetries of the individual bcms with a
      // weight. But right now it is unclear if really need to have that
      // option.
    }
  return;
};

//*****************************************************************
void QwBeamLine::Scale(Double_t factor)
{
  for(size_t i=0;i<fStripline.size();i++)   fStripline[i].Scale(factor);
  for(size_t i=0;i<fCavity.size();i++)      fCavity[i].Scale(factor);
  for(size_t i=0;i<fQPD.size();i++)         fQPD[i].Scale(factor);
  for(size_t i=0;i<fLinearArray.size();i++) fLinearArray[i].Scale(factor);
  for(size_t i=0;i<fBCM.size();i++)         fBCM[i].Scale(factor);
  for(size_t i=0;i<fHaloMonitor.size();i++) fHaloMonitor[i].Scale(factor);
  for(size_t i=0;i<fBCMCombo.size();i++)    fBCMCombo[i].Scale(factor);
  for(size_t i=0;i<fBPMCombo.size();i++)    fBPMCombo[i].Scale(factor);
  for(size_t i=0;i<fECalculator.size();i++) fECalculator[i].Scale(factor);
  return;
};

//*****************************************************************
void QwBeamLine::CalculateRunningAverage()
{
  for (size_t i = 0; i < fStripline.size();    i++) fStripline[i].CalculateRunningAverage();
  for (size_t i = 0; i < fCavity.size();    i++) fCavity[i].CalculateRunningAverage();
  for(size_t i=0;i<fQPD.size();i++)         fQPD[i].CalculateRunningAverage();
  for(size_t i=0;i<fLinearArray.size();i++) fLinearArray[i].CalculateRunningAverage();

  for (size_t i = 0; i < fBCM.size();          i++) fBCM[i].CalculateRunningAverage();
  for(size_t i=0;i<fHaloMonitor.size();i++) fHaloMonitor[i].CalculateRunningAverage();
  for (size_t i = 0; i < fBCMCombo.size();     i++) fBCMCombo[i].CalculateRunningAverage();
  for (size_t i = 0; i < fBPMCombo.size();     i++) fBPMCombo[i].CalculateRunningAverage();
  for (size_t i = 0; i < fECalculator.size();  i++) fECalculator[i].CalculateRunningAverage();
};

//*****************************************************************
void QwBeamLine::PrintValue() const
{
  QwMessage << "=== QwBeamLine: " << GetSubsystemName() << " ===" << QwLog::endl;
  QwMessage << "BPM stripline" << QwLog::endl;
  for (size_t i = 0; i < fStripline.size(); i++) fStripline[i].PrintValue();
  QwMessage << "QPD" << QwLog::endl;
  for (size_t i = 0; i < fQPD.size(); i++) fQPD[i].PrintValue();
  QwMessage << "LinearArray" << QwLog::endl;
  for (size_t i = 0; i < fLinearArray.size(); i++) fLinearArray[i].PrintValue();
  QwMessage << "BPM cavity" << QwLog::endl;
  for (size_t i = 0; i < fCavity.size(); i++) fCavity[i].PrintValue();
  QwMessage << "BCM" << QwLog::endl;
  for (size_t i = 0; i < fBCM.size();       i++) fBCM[i].PrintValue();
  QwMessage << "HaloMonitor" << QwLog::endl;
  for (size_t i = 0; i < fHaloMonitor.size();       i++) fHaloMonitor[i].PrintValue();
  QwMessage << "BPM combo" << QwLog::endl;
  for (size_t i = 0; i < fBCMCombo.size();  i++) fBCMCombo[i].PrintValue();
  QwMessage << "BPM combo" << QwLog::endl;
  for (size_t i = 0; i < fBPMCombo.size();  i++) fBPMCombo[i].PrintValue();
  QwMessage << "Energy " << QwLog::endl;
  for (size_t i = 0; i < fECalculator.size();  i++) fECalculator[i].PrintValue();

};

//*****************************************************************
void QwBeamLine::AccumulateRunningSum(VQwSubsystem* value1)
{
  if (Compare(value1)) {
    QwBeamLine* value = dynamic_cast<QwBeamLine*>(value1);

    for (size_t i = 0; i < fStripline.size(); i++)
      fStripline[i].AccumulateRunningSum(value->fStripline[i]);
    for (size_t i = 0; i < fCavity.size(); i++)
      fCavity[i].AccumulateRunningSum(value->fCavity[i]);
    for (size_t i = 0; i < fBCM.size();       i++)
      fBCM[i].AccumulateRunningSum(value->fBCM[i]);
    for (size_t i = 0; i < fBCMCombo.size();  i++)
      fBCMCombo[i].AccumulateRunningSum(value->fBCMCombo[i]);
    for (size_t i = 0; i < fBPMCombo.size();  i++)
      fBPMCombo[i].AccumulateRunningSum(value->fBPMCombo[i]);
    for (size_t i = 0; i < fECalculator.size();  i++)
      fECalculator[i].AccumulateRunningSum(value->fECalculator[i]);
  }
};

//*****************************************************************
Bool_t QwBeamLine::Compare(VQwSubsystem *value)
{
  //  std::cout<<" Here in QwBeamLine::Compare \n";

  Bool_t res=kTRUE;
  if(typeid(*value)!=typeid(*this))
    {
      res=kFALSE;
      //      std::cout<<" types are not ok \n";
      //      std::cout<<" this is bypassed just for now but should be fixed eventually \n";
    }
  else
    {
      QwBeamLine* input = dynamic_cast<QwBeamLine*>(value);
      if(input->fStripline.size()!=fStripline.size())
	{
	  //	  std::cout<<" not the same number of striplines \n";
	  res=kFALSE;
	}
      else
	if(input->fBCM.size()!=fBCM.size())
	  {
	    res=kFALSE;
	  //	  std::cout<<" not the same number of bcms \n";
	  }
	else if(input->fHaloMonitor.size()!=fHaloMonitor.size())
	  {
	    res=kFALSE;
	  //	  std::cout<<" not the same number of halomonitors \n";
	  }
    }
  return res;
};


//*****************************************************************
void  QwBeamLine::ConstructHistograms(TDirectory *folder, TString &prefix)
{

  //  std::cout<<" here is QwBeamLine::ConstructHistogram with prefix ="<<prefix<<"\n";
  for(size_t i=0;i<fStripline.size();i++)
      fStripline[i].ConstructHistograms(folder,prefix);

  for(size_t i=0;i<fQPD.size();i++)
      fQPD[i].ConstructHistograms(folder,prefix);

  for(size_t i=0;i<fLinearArray.size();i++)
      fLinearArray[i].ConstructHistograms(folder,prefix);

  for(size_t i=0;i<fCavity.size();i++)
      fCavity[i].ConstructHistograms(folder,prefix);

  for(size_t i=0;i<fBCM.size();i++)
      fBCM[i].ConstructHistograms(folder,prefix);

  for(size_t i=0;i<fHaloMonitor.size();i++)
      fHaloMonitor[i].ConstructHistograms(folder,prefix);

  for(size_t i=0;i<fBCMCombo.size();i++)
      fBCMCombo[i].ConstructHistograms(folder,prefix);

  for(size_t i=0;i<fBPMCombo.size();i++)
      fBPMCombo[i].ConstructHistograms(folder,prefix);

  for(size_t i=0;i<fECalculator.size();i++)
      fECalculator[i].ConstructHistograms(folder,prefix);
  return;
};

//*****************************************************************
void  QwBeamLine::DeleteHistograms()
{
  for(size_t i=0;i<fStripline.size();i++)
    fStripline[i].DeleteHistograms();

  for(size_t i=0;i<fQPD.size();i++)
    fQPD[i].DeleteHistograms();

  for(size_t i=0;i<fLinearArray.size();i++)
    fLinearArray[i].DeleteHistograms();

  for(size_t i=0;i<fCavity.size();i++)
    fCavity[i].DeleteHistograms();

  for(size_t i=0;i<fBCM.size();i++)
    fBCM[i].DeleteHistograms();

  for(size_t i=0;i<fHaloMonitor.size();i++)
    fHaloMonitor[i].DeleteHistograms();

  for(size_t i=0;i<fBCMCombo.size();i++)
    fBCMCombo[i].DeleteHistograms();

  for(size_t i=0;i<fBPMCombo.size();i++)
    fBPMCombo[i].DeleteHistograms();

  for(size_t i=0;i<fECalculator.size();i++)
    fECalculator[i].DeleteHistograms();
  return;
};

//*****************************************************************
void  QwBeamLine::FillHistograms()
{
  for(size_t i=0;i<fStripline.size();i++)
    fStripline[i].FillHistograms();
  for(size_t i=0;i<fQPD.size();i++)
    fQPD[i].FillHistograms();
  for(size_t i=0;i<fLinearArray.size();i++)
    fLinearArray[i].FillHistograms();
  for(size_t i=0;i<fCavity.size();i++)
    fCavity[i].FillHistograms();
  for(size_t i=0;i<fBCM.size();i++)
    fBCM[i].FillHistograms();
  for(size_t i=0;i<fHaloMonitor.size();i++)
    fHaloMonitor[i].FillHistograms();
  for(size_t i=0;i<fBCMCombo.size();i++)
    fBCMCombo[i].FillHistograms();
  for(size_t i=0;i<fBPMCombo.size();i++)
    fBPMCombo[i].FillHistograms();
  for(size_t i=0;i<fECalculator.size();i++)
    fECalculator[i].FillHistograms();

  return;
};


//*****************************************************************
void QwBeamLine::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values)
{

  for(size_t i = 0; i < fStripline.size(); i++)
    fStripline[i].ConstructBranchAndVector(tree, prefix, values);
  for(size_t i = 0; i < fQPD.size(); i++)
    fQPD[i].ConstructBranchAndVector(tree, prefix, values);
  for(size_t i = 0; i < fLinearArray.size(); i++)
    fLinearArray[i].ConstructBranchAndVector(tree, prefix, values);
  for(size_t i = 0; i < fCavity.size(); i++)
    fCavity[i].ConstructBranchAndVector(tree, prefix, values);
  for(size_t i = 0; i < fBCM.size(); i++)
    fBCM[i].ConstructBranchAndVector(tree, prefix, values);
  for(size_t i = 0; i < fHaloMonitor.size(); i++)
    fHaloMonitor[i].ConstructBranchAndVector(tree, prefix, values);
  for(size_t i = 0; i <fBCMCombo.size();i++)
    fBCMCombo[i].ConstructBranchAndVector(tree, prefix, values);
  for(size_t i = 0; i <fBPMCombo.size();i++)
    fBPMCombo[i].ConstructBranchAndVector(tree, prefix, values);
  for(size_t i = 0; i <fECalculator.size();i++)
    fECalculator[i].ConstructBranchAndVector(tree, prefix, values);

  return;
};

//*****************************************************************
void QwBeamLine::ConstructBranch(TTree *tree, TString & prefix)
{
  for(size_t i = 0; i < fStripline.size(); i++)
    fStripline[i].ConstructBranch(tree, prefix);
  for(size_t i = 0; i < fQPD.size(); i++)
    fQPD[i].ConstructBranch(tree, prefix);
  for(size_t i = 0; i < fLinearArray.size(); i++)
    fLinearArray[i].ConstructBranch(tree, prefix);
  for(size_t i = 0; i < fBCM.size(); i++)
    fBCM[i].ConstructBranch(tree, prefix);
  for(size_t i = 0; i <fCavity.size(); i++)
    fStripline[i].ConstructBranch(tree, prefix);
  for(size_t i = 0; i < fHaloMonitor.size(); i++)
    fHaloMonitor[i].ConstructBranch(tree, prefix);
  for(size_t i = 0; i <fBCMCombo.size();i++)
    fBCMCombo[i].ConstructBranch(tree, prefix);
  for(size_t i = 0; i <fBPMCombo.size();i++)
    fBPMCombo[i].ConstructBranch(tree, prefix);
  for(size_t i = 0; i <fECalculator.size();i++)
    fECalculator[i].ConstructBranch(tree, prefix);


  return;
};

//*****************************************************************
void QwBeamLine::ConstructBranch(TTree *tree, TString & prefix, QwParameterFile& trim_file)
{
  TString tmp,varname,varvalue;
  tmp="QwBCM";
  QwParameterFile* nextmodule;
  trim_file.RewindToFileStart();


  tmp="QwBPMStripline";
  trim_file.RewindToFileStart();
  if (trim_file.FileHasModuleHeader(tmp)){
    nextmodule=trim_file.ReadUntilNextModule();//This section contains sub modules and or channels to be included in the tree
    for(size_t i = 0; i < fStripline.size(); i++)
      fStripline[i].ConstructBranch(tree, prefix,*nextmodule);

  }

  tmp="QwQPD";
  trim_file.RewindToFileStart();
  if (trim_file.FileHasModuleHeader(tmp)){
    nextmodule=trim_file.ReadUntilNextModule();//This section contains sub modules and or channels to be included in the tree
    for(size_t i = 0; i < fQPD.size(); i++)
      fQPD[i].ConstructBranch(tree, prefix,*nextmodule);

  }
  tmp="QwLinearArray";
  trim_file.RewindToFileStart();
  if (trim_file.FileHasModuleHeader(tmp)){
    nextmodule=trim_file.ReadUntilNextModule();//This section contains sub modules and or channels to be included in the tree
    for(size_t i = 0; i < fLinearArray.size(); i++)
      fLinearArray[i].ConstructBranch(tree, prefix,*nextmodule);

  }

  tmp="QwBPMCavity";
  trim_file.RewindToFileStart();
  if (trim_file.FileHasModuleHeader(tmp)){
    nextmodule=trim_file.ReadUntilNextModule();//This section contains sub modules and or channels to be included in the tree
    for(size_t i = 0; i < fCavity.size(); i++)
      fCavity[i].ConstructBranch(tree, prefix,*nextmodule);

  }

  tmp="QwBCM";
  trim_file.RewindToFileStart();
  if (trim_file.FileHasModuleHeader(tmp)){
    nextmodule=trim_file.ReadUntilNextModule();//This section contains sub modules and or channels to be included in the tree
    for(size_t i = 0; i < fBCM.size(); i++)
      fBCM[i].ConstructBranch(tree, prefix,*nextmodule);
  }

  tmp="QwHaloMonitor";
  trim_file.RewindToFileStart();
  if (trim_file.FileHasModuleHeader(tmp)){
    nextmodule=trim_file.ReadUntilNextModule();//This section contains sub modules and or channels to be included in the tree
    for(size_t i = 0; i < fHaloMonitor.size(); i++)
      fHaloMonitor[i].ConstructBranch(tree, prefix,*nextmodule);
  }


  tmp="QwCombinedBCM";
  trim_file.RewindToFileStart();
  if (trim_file.FileHasModuleHeader(tmp)){
    nextmodule=trim_file.ReadUntilNextModule();//This section contains sub modules and or channels to be included in the tree
    for(size_t i = 0; i <fBCMCombo.size();i++)
      fBCMCombo[i].ConstructBranch(tree, prefix,*nextmodule);
  }


  tmp="QwCombinedBPM";
  trim_file.RewindToFileStart();
  if (trim_file.FileHasModuleHeader(tmp)){
    nextmodule=trim_file.ReadUntilNextModule();//This section contains sub modules and or channels to be included in the tree
    for(size_t i = 0; i <fBPMCombo.size();i++)
      fBPMCombo[i].ConstructBranch(tree, prefix,*nextmodule);
  }

  tmp="QwEnergyCalculator";
  trim_file.RewindToFileStart();
  if (trim_file.FileHasModuleHeader(tmp)){
    nextmodule=trim_file.ReadUntilNextModule();//This section contains sub modules and or channels to be included in the tree
    for(size_t i = 0; i <fECalculator.size();i++)
      fECalculator[i].ConstructBranch(tree, prefix,*nextmodule);
  }

  return;
};

//*****************************************************************
void QwBeamLine::FillTreeVector(std::vector<Double_t> &values) const
{
  for(size_t i = 0; i < fStripline.size(); i++)
    fStripline[i].FillTreeVector(values);
  for(size_t i = 0; i < fQPD.size(); i++)
    fQPD[i].FillTreeVector(values);
  for(size_t i = 0; i < fLinearArray.size(); i++)
    fLinearArray[i].FillTreeVector(values);
  for(size_t i = 0; i < fCavity.size(); i++)
    fCavity[i].FillTreeVector(values);
  for(size_t i = 0; i < fBCM.size(); i++)
    fBCM[i].FillTreeVector(values);
  for(size_t i = 0; i < fHaloMonitor.size(); i++)
    fHaloMonitor[i].FillTreeVector(values);
  for(size_t i = 0; i < fBCMCombo.size(); i++)
    fBCMCombo[i].FillTreeVector(values);
  for(size_t i = 0; i < fBPMCombo.size(); i++)
    fBPMCombo[i].FillTreeVector(values);
  for(size_t i = 0; i < fECalculator.size(); i++){
    fECalculator[i].FillTreeVector(values);}
  return;
};


//*****************************************************************
void  QwBeamLine::PrintInfo() const
{
  std::cout<<"Name of the subsystem ="<<fSystemName<<"\n";
  std::cout<<"there are "<<fStripline.size()<<" striplines \n";
  std::cout<<"there are "<<fQPD.size()<<" QPDs \n";
  std::cout<<"there are "<<fLinearArray.size()<<" LinearArrays \n";
  std::cout<<"there are "<<fCavity.size()<<" cavities \n";
  std::cout<<"there are "<<fBCM.size()<<" bcm \n";
  std::cout<<"there are "<<fHaloMonitor.size()<<" halomonitors \n";
  std::cout<<"there are "<<fBCMCombo.size()<<" combined bcms \n";
  std::cout<<"there are "<<fBPMCombo.size()<<" combined bpms \n";
  std::cout<<"there are "<<fECalculator.size()<<" energy calculators \n";
  std::cout<<" Printing Running AVG and other channel info for BCMs"<<std::endl;
  for(size_t i=0;i<fBCM.size();i++)
    fBCM[i].PrintInfo();
  for(size_t i=0;i<fHaloMonitor.size();i++)
    fHaloMonitor[i].PrintInfo();
  return;
};


//*****************************************************************
void  QwBeamLine::PrintDetectorID() const
{
  for (size_t i=0;i<fBeamDetectorID.size();i++)
    {
      std::cout<<"============================="<<std::endl;
      std::cout<<" Detector ID="<<i<<std::endl;
      fBeamDetectorID[i].Print();
    }
  return;
};

//*****************************************************************
void  QwBeamDetectorID::Print() const
{

  std::cout<<std::endl<<"Detector name= "<<fdetectorname<<std::endl;
  std::cout<<"SubbankkIndex= "<<fSubbankIndex<<std::endl;
  std::cout<<"word index in subbank= "<<fWordInSubbank<<std::endl;
  std::cout<<"module type= "<<fmoduletype<<std::endl;
  std::cout<<"detector type=  "<<fdetectortype<<" that is index="<<fTypeID<<std::endl;
  std::cout<<"Index of this detector in the vector of similar detector= "<<
    fIndex<<std::endl;
  std::cout<<"Subelement index= "<<
    fSubelement<<std::endl;
  std::cout<<"---------------------------------------------------"<<std::endl;
  std::cout<<std::endl;


  return;
};


//*****************************************************************
void  QwBeamLine::Copy(VQwSubsystem *source)
{

  try
    {
     if(typeid(*source)==typeid(*this))
	{
	  VQwSubsystem::Copy(source);

          QwBeamLine* input = dynamic_cast<QwBeamLine*>(source);

	  this->fStripline.resize(input->fStripline.size());
	  for(size_t i=0;i<this->fStripline.size();i++)
	    this->fStripline[i].Copy(&(input->fStripline[i]));

	  this->fQPD.resize(input->fQPD.size());
	  for(size_t i=0;i<this->fQPD.size();i++)
	    this->fQPD[i].Copy(&(input->fQPD[i]));

	  this->fLinearArray.resize(input->fLinearArray.size());
	  for(size_t i=0;i<this->fLinearArray.size();i++)
	    this->fLinearArray[i].Copy(&(input->fLinearArray[i]));

	  this->fCavity.resize(input->fCavity.size());
	  for(size_t i=0;i<this->fCavity.size();i++)
	    this->fCavity[i].Copy(&(input->fCavity[i]));

	  this->fBCM.resize(input->fBCM.size());
	  for(size_t i=0;i<this->fBCM.size();i++)
	    this->fBCM[i].Copy(&(input->fBCM[i]));

	  this->fHaloMonitor.resize(input->fHaloMonitor.size());
	  for(size_t i=0;i<this->fHaloMonitor.size();i++)
	    this->fHaloMonitor[i].Copy(&(input->fHaloMonitor[i]));

	  this->fBCMCombo.resize(input->fBCMCombo.size());
	  for(size_t i=0;i<this->fBCMCombo.size();i++){
	    this->fBCMCombo[i].Copy(&(input->fBCMCombo[i]));
	  }

	  this->fBPMCombo.resize(input->fBPMCombo.size());
	  for(size_t i=0;i<this->fBPMCombo.size();i++){
	    this->fBPMCombo[i].Copy(&(input->fBPMCombo[i]));
	  }

	  this->fECalculator.resize(input->fECalculator.size());
	  for(size_t i=0;i<this->fECalculator.size();i++){
	    this->fECalculator[i].Copy(&(input->fECalculator[i]));
	  }

	  if (input->fPublishList.size()>0){
	    this->fPublishList.resize(input->fPublishList.size());
	    for(size_t i=0;i<input->fPublishList.size();i++){
	      this->fPublishList.at(i).resize(input->fPublishList.at(i).size());
	      for(size_t j=0;j<input->fPublishList.at(i).size();j++){
		this->fPublishList.at(i).at(j)=input->fPublishList.at(i).at(j);
	      }
	    }
	  }
	    

	}
     else
       {
	  TString loc="Standard exception from QwBeamLine::Copy = "
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
};

//*****************************************************************
VQwSubsystem*  QwBeamLine::Copy()
{

  QwBeamLine* TheCopy=new QwBeamLine("Injector Beamline Copy");
  TheCopy->Copy(this);
  return TheCopy;
};



//*****************************************************************
void QwBeamLine::FillDB(QwDatabase *db, TString datatype)
{

  Bool_t local_print_flag = true;

  if(local_print_flag) {
    QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
    QwMessage << "                         QwBeamLine::FillDB                      " << QwLog::endl;
    QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
  }

  std::vector<QwDBInterface> interface;
  std::vector<QwParityDB::beam> entrylist;

  UInt_t analysis_id = db->GetAnalysisID();

  Char_t measurement_type_bcm[4];
  Char_t measurement_type_bpm[4];

  if(datatype.Contains("yield")) {
    sprintf(measurement_type_bcm, "%s", "yq");
    sprintf(measurement_type_bpm, "%s", "yp");
  }
  else if (datatype.Contains("asymmetry")) {
    sprintf(measurement_type_bcm, "%s", "a");
    sprintf(measurement_type_bpm, "%s", "d");
  }
  else {
    sprintf(measurement_type_bcm, "%s", "");
    sprintf(measurement_type_bpm, "%s", "");
  }


  UInt_t i,j;
  i = j = 0;
  // try to access BCM mean and its error
  // there are 2 different types BCM data we have at the moment
  // Yield and Asymmetry
  if(local_print_flag)  QwMessage <<  QwColor(Qw::kGreen) << "Beam Current Monitors" <<QwLog::endl;

  for(i=0; i< fBCM.size(); i++) {
    interface.clear();
    interface = fBCM[i].GetDBEntry();
    for (j=0; j<interface.size(); j++){
      interface.at(j).SetAnalysisID( analysis_id );
      interface.at(j).SetMonitorID( db );
      interface.at(j).SetMeasurementTypeID( measurement_type_bcm );
      interface.at(j).PrintStatus( local_print_flag );
      interface.at(j).AddThisEntryToList( entrylist );
    }
  }

/*
  if(local_print_flag)  QwMessage <<  QwColor(Qw::kGreen) << "Halo Monitors" <<QwLog::endl;

  for(i=0; i< fHaloMonitor.size(); i++) {
    interface.clear();
    interface = fHaloMonitor[i].GetDBEntry();
    for (j=0; j<interface.size(); j++){
      interface.at(j).SetAnalysisID( analysis_id );
      interface.at(j).SetMonitorID( db );
      interface.at(j).SetMeasurementTypeID( measurement_type_bcm );
      interface.at(j).PrintStatus( local_print_flag );
      interface.at(j).AddThisEntryToList( entrylist );
    }
  }
*/

  ///   try to access BPM mean and its error
  if(local_print_flag) QwMessage <<  QwColor(Qw::kGreen) << "Beam Position Monitors" <<QwLog::endl;
  for(i=0; i< fStripline.size(); i++) {
    fStripline[i].MakeBPMList();
    interface.clear();
    interface = fStripline[i].GetDBEntry();
    for (j=0; j<interface.size(); j++){
      interface.at(j).SetAnalysisID( analysis_id ) ;
      interface.at(j).SetMonitorID( db );
      interface.at(j).SetMeasurementTypeID( measurement_type_bpm );
      interface.at(j).PrintStatus( local_print_flag);
      interface.at(j).AddThisEntryToList( entrylist );
    }
  }

  ///   try to access QPD mean and its error
  if(local_print_flag) QwMessage <<  QwColor(Qw::kGreen) << "Quadrant PhotoDiodes" <<QwLog::endl;
  for(i=0; i< fQPD.size(); i++) {
    fQPD[i].MakeQPDList();
    interface.clear();
    interface = fQPD[i].GetDBEntry();
    for (j=0; j<interface.size(); j++){
      interface.at(j).SetAnalysisID( analysis_id ) ;
      interface.at(j).SetMonitorID( db );
      interface.at(j).SetMeasurementTypeID( measurement_type_bpm );
      interface.at(j).PrintStatus( local_print_flag);
      interface.at(j).AddThisEntryToList( entrylist );
    }
  }

  ///   try to access LinearArray mean and its error
  if(local_print_flag) QwMessage <<  QwColor(Qw::kGreen) << "Linear PhotoDiode Array" <<QwLog::endl;
  for(i=0; i< fLinearArray.size(); i++) {
    fLinearArray[i].MakeLinearArrayList();
    interface.clear();
    interface = fLinearArray[i].GetDBEntry();
    for (j=0; j<interface.size(); j++){
      interface.at(j).SetAnalysisID( analysis_id ) ;
      interface.at(j).SetMonitorID( db );
      interface.at(j).SetMeasurementTypeID( measurement_type_bpm );
      interface.at(j).PrintStatus( local_print_flag);
      interface.at(j).AddThisEntryToList( entrylist );
    }
  }

  if(local_print_flag) QwMessage <<  QwColor(Qw::kGreen) << "Beam Position Monitors" <<QwLog::endl;
  for(i=0; i< fCavity.size(); i++) {
    fCavity[i].MakeBPMCavityList();
    interface.clear();
    interface = fCavity[i].GetDBEntry();
    for (j=0; j<interface.size(); j++){
      interface.at(j).SetAnalysisID( analysis_id ) ;
      interface.at(j).SetMonitorID( db );
      interface.at(j).SetMeasurementTypeID( measurement_type_bpm );
      interface.at(j).PrintStatus( local_print_flag);
      interface.at(j).AddThisEntryToList( entrylist );
    }
  }

  if(local_print_flag){
    QwMessage << QwColor(Qw::kGreen)   << "Entrylist Size : "
	      << QwColor(Qw::kBoldRed) << entrylist.size()
              << QwColor(Qw::kNormal)  << QwLog::endl;
  }

  db->Connect();
  // Check the entrylist size, if it isn't zero, start to query..
  if( entrylist.size() ) {
    mysqlpp::Query query= db->Query();
    query.insert(entrylist.begin(), entrylist.end());
    query.execute();
  }
  else {
    QwMessage << "QwBeamLine::FillDB :: This is the case when the entrlylist contains nothing in "<< datatype.Data() << QwLog::endl;
  }
  db->Disconnect();

  return;
};
