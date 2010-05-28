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
Int_t QwBeamLine::LoadChannelMap(TString mapfile)
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

  while (mapstr.ReadNextLine()){
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;


    if (mapstr.HasVariablePair("=",varname,varvalue)){ //  This is a declaration line.  Decode it.
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
	}
      else if (varname=="sample_size")
	{
	  fSample_size=value;
	}
      else if (varname=="begincombo")
	{
	  combolistdecoded = kFALSE;
	  combotype = varvalue;
	  while(mapstr.ReadNextLine()&&!combolistdecoded){
	    mapstr.TrimComment('!');   // Remove everything after a '!' character.
	    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.

	    if (mapstr.LineIsEmpty())  continue;

	    if (mapstr.HasVariablePair("=",varname,varvalue)) {
	      if (varname=="endCOMBO")
		{
		  //At the end of the combined device calculate the total weights of the charge
		  for(size_t i=0;i<fDeviceName.size();i++)
		    fSumQweights+=fQWeight[i];
		  combolistdecoded = kTRUE;
		  //std::cout<<"End decoding combo list:"<<comboname<<"\n";
		}
	      else
		{
		  comboname = varvalue;
		  comboname.ToLower();
		}
	    }
	    else
	      {
		dev_name=mapstr.GetNextToken(", ").c_str();
		dev_name.ToLower();
		fDeviceName.push_back(dev_name);
		fQWeight.push_back( atof(mapstr.GetNextToken(", ").c_str())); //read in the weights for the charge

		if(combotype == "combinedbpm") //for combined BPMs,in addition,
		  {
		    fXWeight.push_back( atof(mapstr.GetNextToken(", ").c_str())); //read in the weights for the X position
		    fYWeight.push_back( atof(mapstr.GetNextToken(", ").c_str())); //read in the weights for the Y position
		  }
	      }
	  }

	  QwBeamDetectorID localComboID(-1, -1, comboname, combotype, "none", this);

	  if(localComboID.fTypeID==-1)
	    {
	      std::cerr << "QwBeamLine::LoadChannelMap:  Unknown detector type: "
			<< combotype <<", the detector "<<comboname<<" will not be decoded "
			<< std::endl;
	      combolistdecoded=kTRUE;
	      continue;
	    }

	  if(fgDetectorTypeNames[localComboID.fTypeID]=="combinedbcm")
	    localComboID.fdetectorname=comboname(0,comboname.Sizeof()-1);


	  localComboID.fIndex=
	    GetDetectorIndex(localComboID.fTypeID,
			     localComboID.fdetectorname);



	  if(localComboID.fIndex==-1)
	    {
	      if(fgDetectorTypeNames[localComboID.fTypeID]=="combinedbcm")
		{
		  QwCombinedBCM localbcmcombo(localComboID.fdetectorname); //create a new combo with combo name
		  fBCMCombo.push_back(localbcmcombo); //add to the array of combos
		  for(size_t i=0;i<fDeviceName.size();i++)
		    {
		      index=GetDetectorIndex(GetDetectorTypeID("bcm"),fDeviceName[i]);
		      fBCMCombo[fBCMCombo.size()-1].Set(&fBCM[index],fQWeight[i],fSumQweights );

		    }
		  fDeviceName.clear();   //reset the device vector for the next combo
		  fQWeight.clear(); //reset the device weights for the next combo

		  localComboID.fIndex=fBCMCombo.size()-1;

		}
	      if(fgDetectorTypeNames[localComboID.fTypeID]=="combinedbpm")
		{
		  Bool_t unrotated(keyword=="unrotated");
		  QwCombinedBPM localbpmcombo(localComboID.fdetectorname,!unrotated);
		  fBPMCombo.push_back(localbpmcombo);

		  for(size_t i=0;i<fDeviceName.size();i++)
		    {
		      index=GetDetectorIndex(GetDetectorTypeID("bpmstripline"),fDeviceName[i]);
		      fBPMCombo[fBPMCombo.size()-1].Set(&fStripline[index],fQWeight[i],fXWeight[i],fYWeight[i],fSumQweights);

		    }
		  fDeviceName.clear();
		  fQWeight.clear();
		  fXWeight.clear();
		  fYWeight.clear();
		  localComboID.fIndex=fBPMCombo.size()-1;

		}
	    }

	  if(combolistdecoded)
	    fBeamDetectorID.push_back(localComboID);
	}
    }
    else
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

	if(currentsubbankindex!=GetSubbankIndex(currentrocread,currentbankread))
	  {
	    currentsubbankindex=GetSubbankIndex(currentrocread,currentbankread);
	    wordsofar=0;
	  }


        QwBeamDetectorID localBeamDetectorID(currentsubbankindex, wordsofar,namech, dettype, modtype, this);


	if(modtype=="VQWK")wordsofar+=6;
	else if(modtype=="SCALER")wordsofar+=1;
	else
	  {
	    std::cerr << "QwBeamLine::LoadChannelMap:  Unknown module type: "
		      << modtype <<", the detector "<<namech<<" will not be decoded "
		      << std::endl;
	    lineok=kFALSE;
	    continue;
	  }

	if(localBeamDetectorID.fTypeID==-1)
	  {
	    std::cerr << "QwBeamLine::LoadChannelMap:  Unknown detector type: "
		      << dettype <<", the detector "<<namech<<" will not be decoded "
		      << std::endl;
	    lineok=kFALSE;
	    continue;
	  }

	if(fgDetectorTypeNames[localBeamDetectorID.fTypeID]=="bpmstripline")
	  localBeamDetectorID.fdetectorname=namech(0,namech.Sizeof()-3);


	localBeamDetectorID.fIndex=
	  GetDetectorIndex(localBeamDetectorID.fTypeID,
			   localBeamDetectorID.fdetectorname);

	if(localBeamDetectorID.fIndex==-1)
	  {
	    if(fgDetectorTypeNames[localBeamDetectorID.fTypeID]=="bpmstripline")
	      {
		Bool_t unrotated(keyword=="unrotated");

		QwBPMStripline localstripline(localBeamDetectorID.fdetectorname,!unrotated);
		fStripline.push_back(localstripline);
		fStripline[fStripline.size()-1].SetDefaultSampleSize(fSample_size);
		localBeamDetectorID.fIndex=fStripline.size()-1;
	      }
	    if(fgDetectorTypeNames[localBeamDetectorID.fTypeID]=="bcm")
	      {
		QwBCM localbcm(localBeamDetectorID.fdetectorname);
		fBCM.push_back(localbcm);
		fBCM[fBCM.size()-1].SetDefaultSampleSize(fSample_size);
		localBeamDetectorID.fIndex=fBCM.size()-1;
	      }
	  }

	if(fgDetectorTypeNames[localBeamDetectorID.fTypeID]=="bpmstripline")
	  {
	    TString subname=namech(namech.Sizeof()-3,2);
	    UInt_t localsubindex=
	      fStripline[localBeamDetectorID.fIndex].GetSubElementIndex(subname);
	      if(ldebug)
		{
		  std::cout<<"=================================\n"
			   <<"Looking for subelement index \n"
			   <<" full name ="<<namech
			   <<" part passed to the function "<<subname
			   <<" sub element index="<<localsubindex
			   <<std::endl;
		}
	      localBeamDetectorID.fSubelement=localsubindex;
	      if(localsubindex>4)
		{
		  std::cerr << "QwBeamLine::LoadChannelMap: "<<subname
			    <<" was not recognized as a valid antenna for a stripline\n";
		  lineok=kFALSE;
		  continue;
		}
	  }

	  if(ldebug)
	    {
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


  if(ldebug)
    {
      std::cout<<"Done with Load map channel \n";
      for(size_t i=0;i<fBeamDetectorID.size();i++)
	fBeamDetectorID[i].Print();
    }
  ldebug=kFALSE;

  return 0;
};

QwBeamDetectorID::QwBeamDetectorID(Int_t subbankid, Int_t wordssofar,
				   TString name, TString dettype,
				   TString modtype, QwBeamLine * obj):
  fSubbankIndex(subbankid),fWordInSubbank(wordssofar),
  fmoduletype(modtype),fdetectorname(name),fdetectortype(dettype),kUnknownDeviceType(-1)
{
  fTypeID = kUnknownDeviceType;
  for(size_t i=0;i<obj->fgDetectorTypeNames.size();i++){
    if(dettype == obj->fgDetectorTypeNames[i]){
      fTypeID = EBeamInstrumentType(i);
      break;
    }
  }
}


//*****************************************************************

Int_t QwBeamLine::LoadEventCuts(TString  filename){
  Double_t ULX, LLX, ULY, LLY;
  Int_t samplesize;
  Int_t check_flag;
  Int_t eventcut_flag;
  TString varname, varvalue, vartypeID,varname2, varvalue2;
  TString device_type,device_name,channel_name;
  std::cout<<" QwBeamLine::LoadEventCuts  "<<filename<<std::endl;
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


      //set limits to zero
      ULX=0;
      LLX=0;
      ULY=0;
      LLY=0;

      if (device_type == "bcm"){
	LLX = (atof(mapstr.GetNextToken(", ").c_str()));	//lower limit for BCM value
	ULX = (atof(mapstr.GetNextToken(", ").c_str()));	//upper limit for BCM value
	Int_t det_index=GetDetectorIndex(GetDetectorTypeID(device_type),device_name);
	fBCM[det_index].SetSingleEventCuts(LLX,ULX);//(fBCMEventCuts);
      }
      else if (device_type == "bpmstripline"){
	channel_name= mapstr.GetNextToken(", ").c_str();
	channel_name.ToLower();
	LLX = (atof(mapstr.GetNextToken(", ").c_str()));	//lower limit for BPMStripline X
	ULX = (atof(mapstr.GetNextToken(", ").c_str()));	//upper limit for BPMStripline X

	Int_t det_index=GetDetectorIndex(GetDetectorTypeID(device_type),device_name);

	fStripline[det_index].SetSingleEventCuts(channel_name, LLX, ULX);
      }
      else if (device_type == "combinedbcm"){

	LLX = (atof(mapstr.GetNextToken(", ").c_str()));	//lower limit for BCM value
	ULX = (atof(mapstr.GetNextToken(", ").c_str()));	//upper limit for BCM value

	Int_t det_index=GetDetectorIndex(GetDetectorTypeID(device_type),device_name);
	fBCMCombo[det_index].Print();
	fBCMCombo[det_index].SetSingleEventCuts(LLX,ULX);//(fBCMComboEventCuts);

      }
      else if (device_type == "combinedbpm"){
	channel_name= mapstr.GetNextToken(", ").c_str();
	channel_name.ToLower();

	LLX = (atof(mapstr.GetNextToken(", ").c_str()));	//lower limit for BPMStripline X
	ULX = (atof(mapstr.GetNextToken(", ").c_str()));	//upper limit for BPMStripline X

	Int_t det_index=GetDetectorIndex(GetDetectorTypeID(device_type),device_name);
	fBPMCombo[det_index].SetSingleEventCuts(channel_name, LLX, ULX);
	fBPMCombo[det_index].SetSingleEventCuts(channel_name, LLX, ULX);
      }

    }

  }


  //update the event cut ON/OFF for all the devices
  //std::cout<<"EVENT CUT FLAG"<<eventcut_flag<<std::endl;
  for (size_t i=0;i<fStripline.size();i++){
    fStripline[i].SetEventCutMode(eventcut_flag);
  }

  for (size_t i=0;i<fBCM.size();i++)
    fBCM[i].SetEventCutMode(eventcut_flag);


  for (size_t i=0;i<fBCMCombo.size();i++)
    fBCMCombo[i].SetEventCutMode(eventcut_flag);

  for (size_t i=0;i<fBPMCombo.size();i++)
    fBPMCombo[i].SetEventCutMode(eventcut_flag);

  fQwBeamLineErrorCount=0; //set the error counter to zero

  return 0;
};


Int_t QwBeamLine::LoadGeometry(TString mapfile)
{
  Bool_t ldebug=kFALSE;

  Int_t lineread=1;
  Int_t index;
  TString  devname,devtype;
  Double_t devOffsetX,devOffsetY, devOffsetZ;
  TString localname;


  if(ldebug)std::cout<<"QwBeamLine::LoadGeometry("<< mapfile<<")\n";

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
		    std::cerr << "\nQwBeamLine::LoadGeometry:  Unknown bpm : "
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
		    std::cerr << "\nQwBeamLine::LoadGeometry:  Unknown combinedbpm : "
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

	    if(ldebug)  std::cout<<"QwBeamLine::LoadGeometry:Offsets for device "<<devname<<" of type "<<devtype<<" are "
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
	  for(size_t i=0;i<fStripline.size();i++)
	    {
	      if(notfound)
		for(int j=0;j<4;j++)
		  {
		    localname=fStripline[i].GetSubElementName(j);
		    localname.ToLower();
		    if(ldebug)  std::cout<<"element name =="<<localname
					 <<"== to be compared to =="<<varname<<"== \n";
		    if(notfound)
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
	  if(notfound)
	    for(size_t i=0;i<fBCM.size();i++)
	      if(fBCM[i].GetElementName()==varname)
		{
		  fBCM[i].SetPedestal(varped);
		  fBCM[i].SetCalibrationFactor(varcal);
		  i=fBCM.size()+1;
		  notfound=kFALSE;
		  i=fBCM.size()+1;
		}
	}

    }
  if(ldebug) std::cout<<" line read in the pedestal + cal file ="<<lineread<<" \n";

  ldebug=kFALSE;
  return 0;
}

//*****************************************************************
void QwBeamLine::RandomizeEventData(int helicity, double time)
{
  // Randomize all QwBPMStripline buffers
  for (size_t i = 0; i < fStripline.size(); i++)
    fStripline[i].RandomizeEventData(helicity, time);

  // Randomize all QwBCM buffers
  for (size_t i = 0; i < fBCM.size(); i++)
    fBCM[i].RandomizeEventData(helicity, time);

}
//*****************************************************************
void QwBeamLine::EncodeEventData(std::vector<UInt_t> &buffer)
{
  std::vector<UInt_t> elements;
  elements.clear();

  // Get all buffers in the order they are defined in the map file
  for (size_t i = 0; i < fBeamDetectorID.size(); i++) {
    // This is a QwBCM
    if (fBeamDetectorID.at(i).fTypeID == kBCM)
      fBCM[fBeamDetectorID.at(i).fIndex].EncodeEventData(elements);
    // This is a QwBPMStripline (which has 4 entries, only process the first one)
    if (fBeamDetectorID.at(i).fTypeID == kBPMStripline
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
}

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

	    if(fBeamDetectorID[i].fTypeID==kBPMStripline)
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
	    if(fBeamDetectorID[i].fTypeID==kBCM)
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
	  }
      }
  }

  return 0;
};

Bool_t QwBeamLine::ApplySingleEventCuts(){

  Bool_t status=kTRUE;

  for(size_t i=0;i<fBCM.size();i++){
    status &= fBCM[i].ApplySingleEventCuts();
    if(!status && bDEBUG) std::cout<<"******* QwBeamLine::SingleEventCuts()->BCM[ "<<i
				   <<" , "<<fBCM[i].GetElementName()<<" ] ******\n";
  }

  for(size_t i=0;i<fStripline.size();i++){
    status &= fStripline[i].ApplySingleEventCuts();
    if(!status && bDEBUG) std::cout<<"******** QwBeamLine::SingleEventCuts()->BPMStripline[ "<<i
				   <<" , "<<fStripline[i].GetElementName()<<" ] *****\n";

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

  //If at least one of the devices falied  event cuts, increment error counter for QwBeamLine
  if (!status)
    fQwBeamLineErrorCount++;


  return status;

};

Int_t QwBeamLine::GetEventcutErrorCounters(){//inherited from the VQwSubsystemParity; this will display the error summary

  std::cout<<"*********QwBeamLine Error Summary****************"<<std::endl;
  std::cout<<"Device name ||  Sample || SW_HW || Sequence || SameHW || ZeroHW || EventCut\n";
  for(size_t i=0;i<fBCM.size();i++){
    fBCM[i].GetEventcutErrorCounters();
  }

  for(size_t i=0;i<fStripline.size();i++){
    fStripline[i].GetEventcutErrorCounters();
  }

  for(size_t i=0;i<fBCMCombo.size();i++){
    fBCMCombo[i].GetEventcutErrorCounters();
  }

  for(size_t i=0;i<fBPMCombo.size();i++){
    fBPMCombo[i].GetEventcutErrorCounters();
  }

   std::cout<<"---------------------------------------------------"<<std::endl;
   std::cout<<std::endl;
  return 1;
}


Int_t QwBeamLine::GetEventcutErrorFlag(){//return the error flag
  Int_t ErrorFlag;
  ErrorFlag=0;
  for(size_t i=0;i<fBCM.size();i++){
    ErrorFlag |= fBCM[i].GetEventcutErrorFlag();
  }
  for(size_t i=0;i<fStripline.size();i++){
    ErrorFlag |= fStripline[i].GetEventcutErrorFlag();
  }
  for(size_t i=0;i<fBCMCombo.size();i++){
    ErrorFlag |= fBCMCombo[i].GetEventcutErrorFlag();
  }
  for(size_t i=0;i<fBPMCombo.size();i++){
    ErrorFlag |= fBPMCombo[i].GetEventcutErrorFlag();
  }

  return ErrorFlag;

}



void  QwBeamLine::ProcessEvent()
{
  for(size_t i=0;i<fStripline.size();i++)
    fStripline[i].ProcessEvent();

  for(size_t i=0;i<fBCM.size();i++)
    fBCM[i].ProcessEvent();

  for(size_t i=0;i<fBCMCombo.size();i++)
    fBCMCombo[i].ProcessEvent();

  for(size_t i=0;i<fBPMCombo.size();i++)
    fBPMCombo[i].ProcessEvent();

  return;
};

Int_t QwBeamLine::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{

  return 0;
};

const Bool_t QwBeamLine::PublishInternalValues() const
{
  // Publish variables
  Bool_t status = kTRUE;
  status = status && PublishInternalValue("q_targ", "Calculated charge on target");
  return status;
};


/**
 * Return the value of variable name
 * @param name Name of the desired variable
 * @param value Pointer to the value to be filled by the call
 * @return True if the variable was found, false if not found
 */
const Bool_t QwBeamLine::ReturnInternalValue(TString name,
				       VQwDataElement* value) const
{
  Bool_t ldebug=kFALSE;
  if (ldebug) std::cout << "QwBeamLine::ReturnInternalValue called for value name, "
	    << name.Data() <<std::endl;
  Bool_t foundit = kFALSE;
  QwVQWK_Channel* tmp = dynamic_cast<QwVQWK_Channel*>(value);
  if (tmp==NULL){
    QwWarning << "QwBeamLine::ReturnInternalValue requires that "
	      << "'value' be a pointer to QwVQWK_Channel"
	      << QwLog::endl;
  } else {
    if (name=="q_targ"){
      foundit = kTRUE;
      (*tmp) = GetBCM("qwk_bcm0l02")->GetCharge();
        if (ldebug) std::cout<<"QwBeamLine::ReturnInternalValue got element qwk_bcm0l02"<<std::endl;
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
  return foundit;
};

//*****************************************************************
void QwBeamLine::ClearEventData()
{
  for(size_t i=0;i<fStripline.size();i++)
    fStripline[i].ClearEventData();
  for(size_t i=0;i<fBCM.size();i++)
    fBCM[i].ClearEventData();
  for(size_t i=0;i<fBCMCombo.size();i++)
    fBCMCombo[i].ClearEventData();
  for(size_t i=0;i<fBPMCombo.size();i++)
    fBPMCombo[i].ClearEventData();
  return;
};
//*****************************************************************
Int_t QwBeamLine::GetDetectorTypeID(TString name)
{
  Int_t result=-1;
  for(size_t i=0;i<fgDetectorTypeNames.size();i++)
    if(name==fgDetectorTypeNames[i])
      {result=i;i=fgDetectorTypeNames.size()+1;}
  return result;
};

//*****************************************************************
Int_t QwBeamLine::GetDetectorIndex(Int_t type_id, TString name)
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

QwBCM* QwBeamLine::GetBCM(const TString name)
{
  if (! fBCM.empty()) {
    for (std::vector<QwBCM>::iterator bcm = fBCM.begin(); bcm != fBCM.end(); ++bcm) {
      if (bcm->GetElementName() == name) {
	return &(*bcm);
      }
    }
  }
  return 0;
};

const QwBPMStripline* QwBeamLine::GetBPMStripline(const TString name) const
{
  return const_cast<QwBeamLine*>(this)->GetBPMStripline(name);
}
const QwBCM* QwBeamLine::GetBCM(const TString name) const
{
  return const_cast<QwBeamLine*>(this)->GetBCM(name);
}

//*****************************************************************
VQwSubsystem&  QwBeamLine::operator=  (VQwSubsystem *value)
{
  //  std::cout<<" here in QwBeamLine::operator= \n";
  if(Compare(value))
    {

      QwBeamLine* input = dynamic_cast<QwBeamLine*>(value);

      for(size_t i=0;i<input->fStripline.size();i++)
	this->fStripline[i]=input->fStripline[i];
      for(size_t i=0;i<input->fBCM.size();i++)
	this->fBCM[i]=input->fBCM[i];
      for(size_t i=0;i<input->fBCMCombo.size();i++)
	this->fBCMCombo[i]=input->fBCMCombo[i];
      for(size_t i=0;i<input->fBPMCombo.size();i++)
	this->fBPMCombo[i]=input->fBPMCombo[i];
    }
  return *this;
};

VQwSubsystem&  QwBeamLine::operator+=  (VQwSubsystem *value)
{
  if(Compare(value))
    {
      //QwBeamLine* input= (QwBeamLine*)value ;
      QwBeamLine* input = dynamic_cast<QwBeamLine*>(value);
      for(size_t i=0;i<input->fStripline.size();i++)
	this->fStripline[i]+=input->fStripline[i];
      for(size_t i=0;i<input->fBCM.size();i++)
	this->fBCM[i]+=input->fBCM[i];
      for(size_t i=0;i<input->fBCMCombo.size();i++)
	this->fBCMCombo[i]+=input->fBCMCombo[i];
      for(size_t i=0;i<input->fBPMCombo.size();i++)
	this->fBPMCombo[i]+=input->fBPMCombo[i];
    }
  return *this;
};

VQwSubsystem&  QwBeamLine::operator-=  (VQwSubsystem *value)
{

  if(Compare(value))
    {
      //QwBeamLine* input= (QwBeamLine*)value;
      QwBeamLine* input = dynamic_cast<QwBeamLine*>(value);
      for(size_t i=0;i<input->fStripline.size();i++)
	this->fStripline[i]-=input->fStripline[i];
      for(size_t i=0;i<input->fBCM.size();i++)
	this->fBCM[i]-=input->fBCM[i];
      for(size_t i=0;i<input->fBCMCombo.size();i++)
	this->fBCMCombo[i]-=input->fBCMCombo[i];
      for(size_t i=0;i<input->fBPMCombo.size();i++)
	this->fBPMCombo[i]-=input->fBPMCombo[i];
    }
  return *this;
};

void  QwBeamLine::Sum(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if(Compare(value1)&&Compare(value2))
    {
      *this =  value1;
      *this += value2;
    }
};

void  QwBeamLine::Difference(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if(Compare(value1)&&Compare(value2))
    {
      *this =  value1;
      *this -= value2;
    }
};

void QwBeamLine::Ratio(VQwSubsystem  *numer, VQwSubsystem  *denom)
{
  if(Compare(numer)&&Compare(denom))
    {
      //QwBeamLine* innumer= (QwBeamLine*)numer ;
      QwBeamLine* innumer = dynamic_cast<QwBeamLine*>(numer);
      //QwBeamLine* indenom= (QwBeamLine*)denom ;
      QwBeamLine* indenom = dynamic_cast<QwBeamLine*>(denom);
      for(size_t i=0;i<innumer->fStripline.size();i++)
	this->fStripline[i].Ratio(innumer->fStripline[i],indenom->fStripline[i]);
      for(size_t i=0;i<innumer->fBCM.size();i++)
	this->fBCM[i].Ratio(innumer->fBCM[i],indenom->fBCM[i]);
      for(size_t i=0;i<innumer->fBCMCombo.size();i++)
	this->fBCMCombo[i].Ratio(innumer->fBCMCombo[i],indenom->fBCMCombo[i]);
      for(size_t i=0;i<innumer->fBPMCombo.size();i++)
	this->fBPMCombo[i].Ratio(innumer->fBPMCombo[i],indenom->fBPMCombo[i]);

      // For the combined bcm, maybe we might want to think about getting
      // the asymmetry using the asymmetries of the individual bcms with a
      // weight. But right now it is unclear if wer really need to have that
      // option.
    }
  return;
};


void QwBeamLine::Scale(Double_t factor)
{
  for(size_t i=0;i<fStripline.size();i++)
    fStripline[i].Scale(factor);
  for(size_t i=0;i<fBCM.size();i++)
    fBCM[i].Scale(factor);
  for(size_t i=0;i<fBCMCombo.size();i++)
    fBCMCombo[i].Scale(factor);
  for(size_t i=0;i<fBPMCombo.size();i++)
    fBPMCombo[i].Scale(factor);
  return;
};

void QwBeamLine::CalculateRunningAverage()
{
  UInt_t i = 0;
  std::cout<<"*********QwBeamLine device Averages****************"<<std::endl;
  std::cout<<"Device \t    ||  Average\t || error\t || events"<<std::endl;
  printf("BPM\n");
  for(i=0; i<fStripline.size(); i++) fStripline[i].CalculateRunningAverage();
  printf("BCM\n");
  for(i=0; i<fBCM.size();       i++) fBCM[i].CalculateRunningAverage();
  for(i=0; i<fBCMCombo.size();  i++) fBCMCombo[i].CalculateRunningAverage();
  for(i=0; i<fBPMCombo.size();  i++) fBPMCombo[i].CalculateRunningAverage();
  std::cout<<"---------------------------------------------------"<<std::endl;
  std::cout<<std::endl;
  return;
};

void QwBeamLine::AccumulateRunningSum(VQwSubsystem* value1)
{
  if (Compare(value1)) {
    QwBeamLine* value = dynamic_cast<QwBeamLine*>(value1);

    for (size_t i = 0; i < fStripline.size(); i++)
      fStripline[i].AccumulateRunningSum(value->fStripline[i]);
    for (size_t i = 0; i < fBCM.size();       i++)
      fBCM[i].AccumulateRunningSum(value->fBCM[i]);
    for (size_t i = 0; i < fBCMCombo.size();  i++)
      fBCMCombo[i].AccumulateRunningSum(value->fBCMCombo[i]);
    for (size_t i = 0; i < fBPMCombo.size();  i++)
      fBPMCombo[i].AccumulateRunningSum(value->fBPMCombo[i]);
  }
};


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
      //QwBeamLine* input= (QwBeamLine*)value;
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
    }
  return res;
}


//*****************************************************************

void  QwBeamLine::ConstructHistograms(TDirectory *folder, TString &prefix)
{

  //  std::cout<<" here is QwBeamLine::ConstructHistogram with prefix ="<<prefix<<"\n";
  for(size_t i=0;i<fStripline.size();i++)
      fStripline[i].ConstructHistograms(folder,prefix);

  for(size_t i=0;i<fBCM.size();i++)
      fBCM[i].ConstructHistograms(folder,prefix);

  for(size_t i=0;i<fBCMCombo.size();i++)
      fBCMCombo[i].ConstructHistograms(folder,prefix);

  for(size_t i=0;i<fBPMCombo.size();i++)
      fBPMCombo[i].ConstructHistograms(folder,prefix);
  return;
};

void  QwBeamLine::DeleteHistograms()
{
  for(size_t i=0;i<fStripline.size();i++)
    fStripline[i].DeleteHistograms();

  for(size_t i=0;i<fBCM.size();i++)
    fBCM[i].DeleteHistograms();

  for(size_t i=0;i<fBCMCombo.size();i++)
    fBCMCombo[i].DeleteHistograms();

  for(size_t i=0;i<fBPMCombo.size();i++)
    fBPMCombo[i].DeleteHistograms();

  return;
};

void  QwBeamLine::FillHistograms()
{
  for(size_t i=0;i<fStripline.size();i++)
    fStripline[i].FillHistograms();
  for(size_t i=0;i<fBCM.size();i++)
    fBCM[i].FillHistograms();
  for(size_t i=0;i<fBCMCombo.size();i++)
    fBCMCombo[i].FillHistograms();
  for(size_t i=0;i<fBPMCombo.size();i++)
    fBPMCombo[i].FillHistograms();

  return;
};



void QwBeamLine::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values)
{
  for(size_t i = 0; i < fStripline.size(); i++)
    fStripline[i].ConstructBranchAndVector(tree, prefix, values);
  for(size_t i = 0; i < fBCM.size(); i++)
    fBCM[i].ConstructBranchAndVector(tree, prefix, values);
  for(size_t i=0;i<fBCMCombo.size();i++)
    fBCMCombo[i].ConstructBranchAndVector(tree, prefix, values);
  for(size_t i=0;i<fBPMCombo.size();i++)
    fBPMCombo[i].ConstructBranchAndVector(tree, prefix, values);

  return;
};

void QwBeamLine::FillTreeVector(std::vector<Double_t> &values)
{
  for(size_t i = 0; i < fStripline.size(); i++)
    fStripline[i].FillTreeVector(values);
  for(size_t i = 0; i < fBCM.size(); i++)
    fBCM[i].FillTreeVector(values);
  for(size_t i = 0; i < fBCMCombo.size(); i++)
    fBCMCombo[i].FillTreeVector(values);
  for(size_t i = 0; i < fBPMCombo.size(); i++)
    fBPMCombo[i].FillTreeVector(values);
  return;
};


//*****************************************************************
void  QwBeamLine::Print()
{
  std::cout<<"Name of the subsystem ="<<fSystemName<<"\n";
  std::cout<<"there are "<<fStripline.size()<<" striplines \n";
  std::cout<<"there are "<<fBCM.size()<<" bcm \n";
  std::cout<<"there are "<<fBCMCombo.size()<<" combined bcms \n";
  std::cout<<"there are "<<fBPMCombo.size()<<" combined bpms \n";
  std::cout<<" Printing Running AVG and other channel info for BCMs"<<std::endl;
  for(size_t i=0;i<fBCM.size();i++)
    fBCM[i].Print();

  return;
}

void  QwBeamLine::PrintDetectorID()
{
  for (size_t i=0;i<fBeamDetectorID.size();i++)
    {
      std::cout<<"============================="<<std::endl;
      std::cout<<" Detector ID="<<i<<std::endl;
      fBeamDetectorID[i].Print();
    }
  return;
}


void  QwBeamDetectorID::Print()
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
}

//*****************************************************************
void  QwBeamLine::Copy(VQwSubsystem *source)
{

  try
    {
     if(typeid(*source)==typeid(*this))
	{
	  VQwSubsystem::Copy(source);
	  //QwBeamLine* input=((QwBeamLine*)source);
          QwBeamLine* input = dynamic_cast<QwBeamLine*>(source);
	  this->fStripline.resize(input->fStripline.size());

	  for(size_t i=0;i<this->fStripline.size();i++)
	    this->fStripline[i].Copy(&(input->fStripline[i]));

	  this->fBCM.resize(input->fBCM.size());
	  for(size_t i=0;i<this->fBCM.size();i++)
	    this->fBCM[i].Copy(&(input->fBCM[i]));

	  this->fBCMCombo.resize(input->fBCMCombo.size());
	  for(size_t i=0;i<this->fBCMCombo.size();i++){
	    this->fBCMCombo[i].Copy(&(input->fBCMCombo[i]));
	  }

	  this->fBPMCombo.resize(input->fBPMCombo.size());
	  for(size_t i=0;i<this->fBPMCombo.size();i++){
	    this->fBPMCombo[i].Copy(&(input->fBPMCombo[i]));
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
}


VQwSubsystem*  QwBeamLine::Copy()
{

  QwBeamLine* TheCopy=new QwBeamLine("Injector Beamline Copy");
  TheCopy->Copy(this);
  return TheCopy;
}


void QwBeamLine::FillDB(QwDatabase *db, TString datatype)
{

  QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
  QwMessage << "                         QwBeamLine::FillDB                      " << QwLog::endl;
  QwMessage << " --------------------------------------------------------------- " << QwLog::endl;

  Bool_t local_print_flag = true;
  QwDBInterface interface;
  std::vector<QwParityDB::beam> entrylist;

  UInt_t analysis_id = db->GetAnalysisID();

  TString yield_type(db->GetMeasurementID(12)); // yp
  TString asymm_type(db->GetMeasurementID(0));//a

  Char_t measurement_type[4];

  if(datatype.Contains("yield")) {
    sprintf(measurement_type, yield_type.Data());
  }
  else if (datatype.Contains("asymmetry")) {
    sprintf(measurement_type, asymm_type.Data());
  }
  else {
    sprintf(measurement_type, " ");
  }



  // try to access BCM mean and its error
  // there are 2 different types BCM data we have at the moment
  // Yield and Asymmetry
  QwMessage <<  QwColor(Qw::kGreen) << "Beam Current Monitors" <<QwLog::endl;
  for(UInt_t i=0; i< fBCM.size(); i++)
    {
      interface.Reset();
      interface = fBCM[i].GetDBEntry(""); // BCM has only one element, thus noname "" on it.
      interface.SetAnalysisID( analysis_id );
      interface.SetDeviceID( db->GetMonitorID(interface.GetDeviceName().Data()) );
      interface.SetMeasurementTypeID(measurement_type);
      interface.PrintStatus(local_print_flag);

      interface.AddThisEntryToList(entrylist);
    }

  ///   try to access BPM mean and its error
  QwMessage <<  QwColor(Qw::kGreen) << "Beam Current Monitors" <<QwLog::endl;
  for(UInt_t i=0; i< fStripline.size(); i++)
    {
      interface.Reset();
      interface = fStripline[i].GetDBEntry("RelX");
      interface.SetAnalysisID( analysis_id ) ;
      interface.SetDeviceID( db->GetMonitorID(interface.GetDeviceName().Data()) );
      interface.SetMeasurementTypeID(measurement_type);
      interface.PrintStatus(local_print_flag);
      interface.AddThisEntryToList(entrylist);

      interface.Reset();
      interface = fStripline[i].GetDBEntry("RelY");
      interface.SetAnalysisID( analysis_id ) ;
      interface.SetDeviceID( db->GetMonitorID(interface.GetDeviceName().Data()) );
      interface.SetMeasurementTypeID(measurement_type);
      interface.PrintStatus(local_print_flag);
      interface.AddThisEntryToList(entrylist);
    }

  QwMessage << QwColor(Qw::kGreen)   << "Entrylist Size : "
 	    << QwColor(Qw::kBoldRed) << entrylist.size() << QwLog::endl;

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
      QwMessage << "QwBeamLine::FillDB :: This is the case when the entrlylist contains nothing in "<< datatype.Data() << QwLog::endl;
    }

  db->Disconnect();

  return;
};
