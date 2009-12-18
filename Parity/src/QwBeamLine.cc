/**********************************************************\
* File: QwBeamLine.C                                      *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwBeamLine.h"
#include "QwHistogramHelper.h"
#include <stdexcept>

//*****************************************************************
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

  std::vector<Double_t> fBCMEventCuts;//for initializing event cuts  
  fBCMEventCuts.push_back(0);
  fBCMEventCuts.push_back(0);
  fBCMEventCuts.push_back(0);//device_flag 
  std::vector<Double_t> fBPMEventCuts;//for initializing event cuts
  fBPMEventCuts.push_back(0);
  fBPMEventCuts.push_back(0);
  fBPMEventCuts.push_back(0);
  fBPMEventCuts.push_back(0);
  fBPMEventCuts.push_back(0);//device_flag

  std::vector<TString> fDeviceName;
  std::vector<Double_t> fDeviceWeight;
 
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
	  //std::cout<<"QwBeamline :: Decoding BCM combo!\n";
	  combolistdecoded = kFALSE;
	  combotype = varvalue;
	  //std::cout<<"Combo type :"<<combotype<<"\n";

	  while(mapstr.ReadNextLine()&&!combolistdecoded){
	    mapstr.TrimComment('!');   // Remove everything after a '!' character.
	    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.

	    if (mapstr.LineIsEmpty())  continue;
	    
	    if (mapstr.HasVariablePair("=",varname,varvalue)) {
	      if (varname=="endCOMBO")
		{
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
		//std::cout<<"Got element "<<dev_name<<"\n";
		fDeviceName.push_back(dev_name);
		fDeviceWeight.push_back( atof(mapstr.GetNextToken(", ").c_str()));				
	      }
	  }

	  
	  QwBeamDetectorID localBCMComboID;
	  localBCMComboID.fdetectortype=combotype;
	  localBCMComboID.fTypeID=GetDetectorTypeID(combotype);
	  localBCMComboID.fdetectorname=comboname;

	  localBCMComboID.fIndex=
	    GetDetectorIndex(localBCMComboID.fTypeID,
			     localBCMComboID.fdetectorname);


	if(localBCMComboID.fIndex==-1)
	  {
	 
	    QwCombinedBCM localcombo(localBCMComboID.fdetectorname); //create a new combo with combo name
	    fBCMCombo.push_back(localcombo); //add to the array of combos
	    // for(Int_t n=0;n<fBCMCombo.size();n++){
	    for(Int_t i=0;i<fDeviceName.size();i++)
	      {
		index=GetDetectorIndex(GetDetectorTypeID(combotype),fDeviceName[i]);
		//std::cout<<"QwBeamline :: device index"<<index<<"for device "<<fDeviceName[i]<<"with weight :"<<fDeviceWeight[i]<<"\n";
		fBCMCombo[fBCMCombo.size()-1].Add(&fBCM[index],fDeviceWeight[i]);
	
	      }
	    fDeviceName.clear();   //reset the device vector for the next combo
	    fDeviceWeight.clear(); //reset the device weights for the next combo
	    //}
	  }
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
	
	QwBeamDetectorID localBeamDetectorID;
	localBeamDetectorID.fdetectorname=namech;
	localBeamDetectorID.fmoduletype=modtype;
	localBeamDetectorID.fSubbankIndex=currentsubbankindex;
	localBeamDetectorID.fdetectortype=dettype;
	
	localBeamDetectorID.fWordInSubbank=wordsofar;
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
	
	localBeamDetectorID.fTypeID=GetDetectorTypeID(dettype);
	if(localBeamDetectorID.fTypeID==-1)
	  {
	    std::cerr << "QwBeamLine::LoadChannelMap:  Unknown detector type: "
		      << dettype <<", the detector "<<namech<<" will not be decoded "
		      << std::endl;
	    lineok=kFALSE;
	    continue;
	  }
	
	if(DetectorTypes[localBeamDetectorID.fTypeID]=="bpmstripline")
	  localBeamDetectorID.fdetectorname=namech(0,namech.Sizeof()-3);
	
	localBeamDetectorID.fIndex=
	  GetDetectorIndex(localBeamDetectorID.fTypeID,
			   localBeamDetectorID.fdetectorname);
	
	if(localBeamDetectorID.fIndex==-1)
	  {
	    if(DetectorTypes[localBeamDetectorID.fTypeID]=="bpmstripline")
	      {
		Bool_t unrotated(keyword=="unrotated");
		
		QwBPMStripline localstripline(localBeamDetectorID.fdetectorname,!unrotated);
		fStripline.push_back(localstripline);
		  //std::cout<<" BCM sample size "<<fSample_size<<std::endl;
		fStripline[fStripline.size()-1].SetSingleEventCuts(fBPMEventCuts);//initialize the event cuts to zero. This is before reading the event cut file. So for any device that is not on the list will carry these default values.
		fStripline[fStripline.size()-1].SetDefaultSampleSize(fSample_size);
		localBeamDetectorID.fIndex=fStripline.size()-1;
	      }
	    if(DetectorTypes[localBeamDetectorID.fTypeID]=="bcm")
	      {
		QwBCM localbcm(localBeamDetectorID.fdetectorname);
		fBCM.push_back(localbcm);
		  //std::cout<<"BPM sample size "<<fSample_size<<std::endl;
		fBCM[fBCM.size()-1].SetDefaultSampleSize(fSample_size);
		fBCM[fBCM.size()-1].SetSingleEventCuts(fBCMEventCuts);//initialize the event cuts to zero
		localBeamDetectorID.fIndex=fBCM.size()-1;
	      }
	  }
	
	if(DetectorTypes[localBeamDetectorID.fTypeID]=="bpmstripline")
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

  
  //  std::cout<<"QwBeamline::Done decoding device list! \n";
  
  if(ldebug)
    {
      std::cout<<"Done with Load map channel \n";
      for(size_t i=0;i<fBeamDetectorID.size();i++)
	fBeamDetectorID[i].Print();
    }
  ldebug=kFALSE;

  return 0;
};
//*****************************************************************

Int_t QwBeamLine::LoadEventCuts(TString  filename){
  Double_t ULX, LLX, ULY, LLY;
  Int_t samplesize;
  Int_t check_flag;
  Int_t eventcut_flag;
  std::vector<Double_t> fBCMEventCuts;
  std::vector<Double_t> fBPMEventCuts;
  TString varname, varvalue, vartypeID,varname2, varvalue2;
  TString device_type,device_name;
  std::cout<<" QwBeamLine::LoadEventCuts  "<<filename<<std::endl; 
  QwParameterFile mapstr(filename.Data());  //Open the file

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
	//std::cout<<" Type "<<device_type<<" Name "<<device_name<<" Index ["<<det_index <<"] "<<" device flag "<<check_flag<<std::endl;
	//update the Double vector
	fBCMEventCuts.clear();
	fBCMEventCuts.push_back(LLX);
	fBCMEventCuts.push_back(ULX);
	fBCMEventCuts.push_back(1);
      
	//fBCM[det_index].Print();
	fBCM[det_index].SetSingleEventCuts(fBCMEventCuts);
	//std::cout<<"*****************************"<<std::endl;
	
      }
      else if (device_type == "bpmstripline"){
      
	LLX = (atof(mapstr.GetNextToken(", ").c_str()));	//lower limit for BPMStripline X
	ULX = (atof(mapstr.GetNextToken(", ").c_str()));	//upper limit for BPMStripline X
	LLY = (atof(mapstr.GetNextToken(", ").c_str()));	//lower limit for BPMStripline Y
	ULY = (atof(mapstr.GetNextToken(", ").c_str()));	//upper limit for BPMStripline Y

	Int_t det_index=GetDetectorIndex(GetDetectorTypeID(device_type),device_name);	
	//update the Double vector
	fBPMEventCuts.clear();
	fBPMEventCuts.push_back(LLX);
	fBPMEventCuts.push_back(ULX);
	fBPMEventCuts.push_back(LLY);
	fBPMEventCuts.push_back(ULY);
	fBPMEventCuts.push_back(1);
	//std::cout<<"*****************************"<<std::endl;
	//std::cout<<" Name "<<device_name<<" Index ["<<det_index <<"] "<<" device flag "<<check_flag<<std::endl;
	fStripline[det_index].SetSingleEventCuts(fBPMEventCuts);	
	//fStripline[det_index].Print();
	//std::cout<<"*****************************"<<std::endl;
      }
    }
        
  }
  //update the event cut ON/OFF for all the devices
  //std::cout<<"EVENT CUT FLAG"<<eventcut_flag<<std::endl;
  for (size_t i=0;i<fStripline.size();i++)
    fStripline[i].SetEventCutMode(eventcut_flag);

  for (size_t i=0;i<fBCM.size();i++)
    fBCM[i].SetEventCutMode(eventcut_flag);
    

  fQwBeamLineErrorCount=0; //set the error counter to zero

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
void QwBeamLine::RandomizeEventData(int helicity)
{
  // Randomize all QwBPMStripline buffers
  for (size_t i = 0; i < fStripline.size(); i++)
    fStripline[i].RandomizeEventData(helicity);

  // Randomize all QwBCM buffers
  for (size_t i = 0; i < fBCM.size(); i++)
    fBCM[i].RandomizeEventData(helicity);

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
Int_t QwBeamLine::ProcessEvBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
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
  //currently this will check the IsGoodEvent() only!
  //std::cout<<" QwBeamLine::SingleEventCuts() ";

  Bool_t test_BCM=kTRUE;
  Bool_t test_BCM1=kTRUE;

  
  for(size_t i=0;i<fBCM.size();i++){
    //std::cout<<"  BCM ["<<i<<"] "<<std::endl;
    test_BCM1=fBCM[i].ApplySingleEventCuts(); 
    test_BCM&=test_BCM1;
    if(!test_BCM1 && bDEBUG) std::cout<<"******* QwBeamLine::SingleEventCuts()->BCM[ "<<i<<" , "<<fBCM[i].GetElementName()<<" ] ******\n";
  }
  //if (!test_BCM)
  //fNumError_Evt_BCM++;//BCM falied  event counter for QwBeamLine
    
  
  for(size_t i=0;i<fStripline.size();i++){
    //std::cout<<"  BPM ["<<i<<"] "<<std::endl;
    test_BCM1=fStripline[i].ApplySingleEventCuts();
    test_BCM&=test_BCM1;
    if(!test_BCM1 && bDEBUG) std::cout<<"******** QwBeamLine::SingleEventCuts()->BPMStripline[ "<<i<<" , "<<fStripline[i].GetElementName()<<" ] *****\n";
    }
  if (!test_BCM1 || !test_BCM)
   fQwBeamLineErrorCount++;//BPM falied  event counter for QwBeamLine
    

  
    
  

  return test_BCM;
   
};

Int_t QwBeamLine::GetEventcutErrorCounters(){//inherited from the VQwSubsystemParity; this will display the error summary

  std::cout<<"*********QwBeamLine Error Summary****************"<<std::endl;
  std::cout<<"Device name ||  Sample || SW_HW || Sequence || SameHW || EventCut\n";
  for(size_t i=0;i<fBCM.size();i++){
    //std::cout<<"*"<<std::endl;
    fBCM[i].GetEventcutErrorCounters();    
  } 

   for(size_t i=0;i<fStripline.size();i++){
     fStripline[i].GetEventcutErrorCounters();
   }
   //std::cout<<"Total failed events "<<  fQwBeamLineErrorCount<<std::endl;
   //std::cout<<"*****End of error QwBeamLine reporting - Total failed events "<<fQwBeamLineErrorCount<<"*****"<<std::endl;
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
  return ErrorFlag;

}



void  QwBeamLine::ProcessEvent()
{
  for(size_t i=0;i<fStripline.size();i++)
    fStripline[i].ProcessEvent();

  for(size_t i=0;i<fBCM.size();i++)
    fBCM[i].ProcessEvent();
  
  for(size_t i=0;i<fBCMCombo.size();i++){
    // std::cout<<"new combo :"<<fBCMCombo[i].GetElementName()<<"\n";
    fBCMCombo[i].ProcessEvent();
  }
  
  return;
};

Int_t QwBeamLine::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{

  return 0;
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
  return;
};
//*****************************************************************
Int_t QwBeamLine::GetDetectorTypeID(TString name)
{
  Int_t result=-1;
  for(size_t i=0;i<DetectorTypes.size();i++)
    if(name==DetectorTypes[i])
      {result=i;i=DetectorTypes.size()+1;}
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
//*****************************************************************
VQwSubsystem&  QwBeamLine::operator=  (VQwSubsystem *value)
{
  //  std::cout<<" here in QwBeamLine::operator= \n";
  if(Compare(value))
    {
      //QwBeamLine* input = (QwBeamLine*) value;
      QwBeamLine* input = dynamic_cast<QwBeamLine*>(value);
      for(size_t i=0;i<input->fStripline.size();i++)
	this->fStripline[i]=input->fStripline[i];
      for(size_t i=0;i<input->fBCM.size();i++)
	this->fBCM[i]=input->fBCM[i];
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

  return;
};

void QwBeamLine::Calculate_Running_Average(){
  std::cout<<"*********QwBeamLine device Averages****************"<<std::endl;
  std::cout<<"Device \t    ||  Average\t || error\t || events"<<std::endl;
  for(size_t i=0;i<fStripline.size();i++)
    fStripline[i].Calculate_Running_Average();
  for(size_t i=0;i<fBCM.size();i++)
    fBCM[i].Calculate_Running_Average();
  for(size_t i=0;i<fBCMCombo.size();i++)
    fBCMCombo[i].Calculate_Running_Average();
  std::cout<<"---------------------------------------------------"<<std::endl;
  std::cout<<std::endl;
};

void QwBeamLine::Do_RunningSum(){
  for(size_t i=0;i<fStripline.size();i++)
    fStripline[i].Do_RunningSum();
  for(size_t i=0;i<fBCM.size();i++)
    fBCM[i].Do_RunningSum();
  for(size_t i=0;i<fBCMCombo.size();i++)
    fBCMCombo[i].Do_RunningSum();
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
  return;
};


//*****************************************************************
void  QwBeamLine::Print()
{
  std::cout<<"Name of the subsystem ="<<fSystemName<<"\n";
  std::cout<<"there are "<<fStripline.size()<<" striplines \n";
  std::cout<<"there are "<<fBCM.size()<<" bcm \n";
  std::cout<<"there are "<<fBCMCombo.size()<<" bcm combos \n";

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
	  for(size_t i=0;i<this->fBCMCombo.size();i++)
	    this->fBCMCombo[i].Copy(&(input->fBCMCombo[i]));
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
  // this->Print();

  return;
}


VQwSubsystem*  QwBeamLine::Copy()
{

  QwBeamLine* TheCopy=new QwBeamLine("Injector Beamline Copy");
  TheCopy->Copy(this);
  return TheCopy;
}
