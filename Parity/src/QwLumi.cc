/**********************************************************\
* File: QwLumi.C                                      *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwLumi.h"
#include "QwHistogramHelper.h"

#include "QwSubsystemArray.h"
#include <stdexcept>
#include <iostream>

// Register this subsystem with the factory
QwSubsystemFactory<QwLumi> theLumiFactory("QwLumi");

void QwLumi::DefineOptions(QwOptions &options){
  options.AddOptions()
    ("QwLumi.normalize",
     po::value<bool>()->default_value(false)->zero_tokens(),
     "Normalize the detectors by beam current");
}


//*****************************************************************
void QwLumi::ProcessOptions(QwOptions &options){
  bNormalization = options.GetValue<bool>("QwLumi.normalize");
  if (! bNormalization){
    QwWarning << "QwLumi::ProcessOptions:  "
	      << "Detector yields WILL NOT be normalized."
	      << QwLog::endl;
  }
};

//*****************************************************************
Int_t QwLumi::LoadChannelMap(TString mapfile)
{
  Bool_t ldebug=kFALSE;

  TString varname, varvalue;
  TString modtype, dettype, namech, keyword;
  Int_t modnum, channum;
  Int_t currentrocread=0;
  Int_t currentbankread=0;
  Int_t offset = -1;
  Int_t currentsubbankindex=-1;
  Int_t fSample_size=0;

  QwParameterFile mapstr(mapfile.Data());  //Open the file
  while (mapstr.ReadNextLine())
    {
      mapstr.TrimComment('!');   // Remove everything after a '!' character.
      mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
      if (mapstr.LineIsEmpty())  continue;

      if (mapstr.HasVariablePair("=",varname,varvalue))
	{
	  //  This is a declaration line.  Decode it.
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
	    }

	  if(modtype=="VQWK")
	    {
	      offset = QwVQWK_Channel::GetBufferOffset(modnum, channum);
	    }
	  else if(modtype=="SCALER")
	    {
	      offset = QwSIS3801D24_Channel::GetBufferOffset(modnum, channum);
	    }

	  if(offset<0)
	    {
	      QwError << "QwLumi::LoadChannelMap:  Unknown module type: "
		      << modtype <<", the detector "<<namech<<" will not be decoded "
		      << QwLog::endl;
	      lineok=kFALSE;
	      continue;
	    }


	  QwLumiDetectorID localLumiDetectorID;
	  localLumiDetectorID.fdetectorname=namech;
	  localLumiDetectorID.fmoduletype=modtype;
	  localLumiDetectorID.fSubbankIndex=currentsubbankindex;
	  localLumiDetectorID.fdetectortype=dettype;
	  localLumiDetectorID.fWordInSubbank=offset;

	  localLumiDetectorID.fTypeID=GetQwPMTInstrumentType(dettype);
	  if(localLumiDetectorID.fTypeID==-1)
	    {
	      QwError << "QwLumi::LoadChannelMap:  Unknown detector type: "
		      << dettype <<", the detector "<<namech<<" will not be decoded "
		      << QwLog::endl;
	      lineok=kFALSE;
	      continue;
	    }

	  localLumiDetectorID.fIndex=
	    GetDetectorIndex(localLumiDetectorID.fTypeID,
			     localLumiDetectorID.fdetectorname);

	  if(localLumiDetectorID.fIndex==-1)
	    {
	      if(localLumiDetectorID.fTypeID==kQwIntegrationPMT)
		{
		  QwIntegrationPMT localIntegrationPMT(GetSubsystemName(),localLumiDetectorID.fdetectorname);
		  fIntegrationPMT.push_back(localIntegrationPMT);
		  fIntegrationPMT[fIntegrationPMT.size()-1].SetDefaultSampleSize(fSample_size);
		  localLumiDetectorID.fIndex=fIntegrationPMT.size()-1;
		}

	      if(localLumiDetectorID.fTypeID==kQwScalerPMT)
		{
		  QwSIS3801D24_Channel localcounter(localLumiDetectorID.fdetectorname);
		  fScalerPMT.push_back(localcounter);
		  localLumiDetectorID.fIndex=fScalerPMT.size()-1;
		}

	    }


	if(ldebug)
	  {
	    localLumiDetectorID.Print();
	    std::cout<<"line ok=";
	    if(lineok) std::cout<<"TRUE"<<std::endl;
	    else
	      std::cout<<"FALSE"<<std::endl;
	  }

	if(lineok)
	  fLumiDetectorID.push_back(localLumiDetectorID);
      }
  }

  if(ldebug)
    {
      std::cout<<"QwLumi::Done with Load map channel \n";
      for(size_t i=0;i<fLumiDetectorID.size();i++)
	fLumiDetectorID[i].Print();
    }
  ldebug=kFALSE;

  return 0;
};


//*****************************************************************
Int_t QwLumi::LoadEventCuts(TString  filename){
  Double_t ULX, LLX, ULY, LLY;
  Int_t samplesize;
  Int_t check_flag;
  Int_t eventcut_flag;

  TString varname, varvalue, vartypeID;
  TString device_type,device_name;
  std::cout<<" QwLumi::LoadEventCuts  "<<filename<<std::endl;
  QwParameterFile mapstr(filename.Data());  //Open the file

  samplesize = 0;
  check_flag = 0;
  eventcut_flag=1;

  while (mapstr.ReadNextLine()){
    //std::cout<<"********* In the loop  *************"<<std::endl;
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;
    if (mapstr.HasVariablePair("=",varname,varvalue)){
      if (varname=="EVENTCUTS"){
	//varname="";
	eventcut_flag= QwParameterFile::GetUInt(varvalue);
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

      if (device_type == kQwIntegrationPMT){
	LLX = (atof(mapstr.GetNextToken(", ").c_str()));	//lower limit for IntegrationPMT value
	ULX = (atof(mapstr.GetNextToken(", ").c_str()));	//upper limit for IntegrationPMT value

	Int_t det_index=GetDetectorIndex(GetQwPMTInstrumentType(device_type),device_name);
	//std::cout<<"*****************************"<<std::endl;
	//std::cout<<" Type "<<device_type<<" Name "<<device_name<<" Index ["<<det_index <<"] "<<" device flag "<<check_flag<<std::endl;

	fIntegrationPMT[det_index].SetSingleEventCuts(LLX,ULX);
	//std::cout<<"*****************************"<<std::endl;

      }

    }

  }
  for (size_t i=0;i<fIntegrationPMT.size();i++)
    fIntegrationPMT[i].SetEventCutMode(eventcut_flag);

  fQwLumiErrorCount=0; //set the error counter to zero

  return 0;
};

//*****************************************************************
Int_t QwLumi::LoadInputParameters(TString pedestalfile)
{
  Bool_t ldebug=kFALSE;
  TString varname;
  Double_t varped;
  Double_t varcal;
  TString localname;

  Int_t lineread=0;

  if(ldebug)std::cout<<"QwLumi::LoadInputParameters("<< pedestalfile<<")\n";

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
	  if(ldebug) std::cout<<"inputs for channel "<<varname
			      <<": ped="<<varped<<": cal="<<varcal<<"\n";
	  Bool_t notfound=kTRUE;

	  if(notfound)
	    for(size_t i=0;i<fIntegrationPMT.size();i++)
	      if(fIntegrationPMT[i].GetElementName()==varname)
		{
		  fIntegrationPMT[i].SetPedestal(varped);
		  fIntegrationPMT[i].SetCalibrationFactor(varcal);
		  i=fIntegrationPMT.size()+1;
		  notfound=kFALSE;
		  i=fIntegrationPMT.size()+1;
		}
	}

    }
  if(ldebug) std::cout<<" line read in the pedestal + cal file ="<<lineread<<" \n";

  ldebug=kFALSE;
  return 0;
};

//*****************************************************************
void QwLumi::SetRandomEventParameters(Double_t mean, Double_t sigma)
{
  for (size_t i = 0; i < fIntegrationPMT.size(); i++)
    fIntegrationPMT[i].SetRandomEventParameters(mean, sigma);
};

//*****************************************************************
void QwLumi::SetRandomEventAsymmetry(Double_t asymmetry)
{
  for (size_t i = 0; i < fIntegrationPMT.size(); i++)
    fIntegrationPMT[i].SetRandomEventAsymmetry(asymmetry);
};

//*****************************************************************
void QwLumi::RandomizeEventData(int helicity, double time)
{
  // Randomize all QwIntegrationPMT buffers
  for (size_t i = 0; i < fIntegrationPMT.size(); i++)
    fIntegrationPMT[i].RandomizeEventData(helicity, time);
};

//*****************************************************************
void QwLumi::EncodeEventData(std::vector<UInt_t> &buffer)
{
  std::vector<UInt_t> elements;
  elements.clear();

  // Get all buffers in the order they are defined in the map file
  for (size_t i = 0; i < fLumiDetectorID.size(); i++) {
    // This is a QwIntegrationPMT
    if (fLumiDetectorID.at(i).fTypeID == kQwIntegrationPMT)
      fIntegrationPMT[fLumiDetectorID.at(i).fIndex].EncodeEventData(elements);
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
Int_t QwLumi::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  Bool_t lkDEBUG=kFALSE;
  Bool_t firsttime=kTRUE;
  Bool_t issingleevent=kTRUE;

  Int_t index = GetSubbankIndex(roc_id,bank_id);

  if (index>=0 && num_words>0){
    //  We want to process this ROC.  Begin looping through the data.
    if (lkDEBUG)
      std::cout << "QwLumi::ProcessEvBuffer:  "
		<< "Begin processing ROC" << roc_id
		<< " and subbank "<<bank_id
		<< " number of words="<<num_words<<std::endl;

    for(size_t i=0;i<fLumiDetectorID.size();i++)
      {
	if(fLumiDetectorID[i].fSubbankIndex==index)
	  {

	    if(fLumiDetectorID[i].fTypeID == kQwIntegrationPMT)
	      {
		if (lkDEBUG)
		  {
		    std::cout<<"found IntegrationPMT data for "<<fLumiDetectorID[i].fdetectorname<<std::endl;
		    std::cout<<"words left to read in this buffer:"<<num_words-fLumiDetectorID[i].fWordInSubbank<<std::endl;
		  }
		fIntegrationPMT[fLumiDetectorID[i].fIndex].
		  ProcessEvBuffer(&(buffer[fLumiDetectorID[i].fWordInSubbank]),
				  num_words-fLumiDetectorID[i].fWordInSubbank);
	      }

	    if(fLumiDetectorID[i].fTypeID == kQwScalerPMT)
	      {
		if (lkDEBUG)
		  {
		    std::cout<<"found ScalerPMT data for "<<fLumiDetectorID[i].fdetectorname<<std::endl;
		    std::cout<<"word left to read in this buffer:"<<num_words-fLumiDetectorID[i].fWordInSubbank<<std::endl;
		  }

// This was added to check if the buffer contains more than one event.  If it does then throw those events away.  A better way to do this would be to find how many events were in the buffer then change the offset to be able to read them all.
                if (firsttime)
                  {
                    firsttime=kFALSE;
                    if (buffer[0]/32!=1)
                      {
                        issingleevent=kFALSE;
                        std::cout<<"More than one event was found in the buffer.  Setting these events to zero."<<std::endl;
                      }
                  }
                if (issingleevent==kFALSE) continue;





		fScalerPMT[fLumiDetectorID[i].fIndex].
		  ProcessEvBuffer(&(buffer[fLumiDetectorID[i].fWordInSubbank]),
				  num_words-fLumiDetectorID[i].fWordInSubbank);
	      }

	  }
      }
  }

  return 0;
};

//*****************************************************************
Bool_t QwLumi::ApplySingleEventCuts(){

  Bool_t test_IntegrationPMT=kTRUE;
  Bool_t test_IntegrationPMT1=kTRUE;

  for(size_t i=0;i<fIntegrationPMT.size();i++){
    test_IntegrationPMT1=fIntegrationPMT[i].ApplySingleEventCuts();
    test_IntegrationPMT&=test_IntegrationPMT1;
    if(!test_IntegrationPMT1 && bDEBUG)
      std::cout<<"******* QwLumi::SingleEventCuts()->IntegrationPMT[ "<<i<<" , "
	       <<fIntegrationPMT[i].GetElementName()<<" ] ******\n";
  }
  if (!test_IntegrationPMT1 || !test_IntegrationPMT)
   fQwLumiErrorCount++;//BPM falied  event counter for QwLumi

  return test_IntegrationPMT;

};


//*****************************************************************
Int_t QwLumi::GetEventcutErrorCounters(){//inherited from the VQwSubsystemParity; this will display the error summary

  std::cout<<"*********QwLumi Error Summary****************"<<std::endl;
  std::cout<<"Device name ||  Sample || SW_HW || Sequence || SameHW || EventCut\n";
  for(size_t i=0;i<fIntegrationPMT.size();i++){
    //std::cout<<"  IntegrationPMT ["<<i<<"] "<<std::endl;
    fIntegrationPMT[i].ReportErrorCounters();
  }

  return 1;
};


//*****************************************************************
Int_t QwLumi::GetEventcutErrorFlag(){//return the error flag

  return 0;

};

//*****************************************************************
void  QwLumi::ProcessEvent()
{
  for(size_t i=0;i<fIntegrationPMT.size();i++)
    fIntegrationPMT[i].ProcessEvent();
  for(size_t i=0;i<fScalerPMT.size();i++)
    fScalerPMT[i].ProcessEvent();

  return;
};


/**
 * Exchange data between subsystems
 */
void  QwLumi::ExchangeProcessedData()
{
  bIsExchangedDataValid = kTRUE;
  if (bNormalization){
    // Create a list of all variables that we need
    // TODO This could be a static list to avoid repeated vector initializiations
    std::vector<VQwDataElement*> variable_list;
    variable_list.push_back(&fTargetCharge);
    //variable_list.push_back(&fTargetX);
    //variable_list.push_back(&fTargetY);
    //variable_list.push_back(&fTargetXprime);
    //variable_list.push_back(&fTargetYprime);
    //variable_list.push_back(&fTargetEnergy);


    // Loop over all variables in the list
    std::vector<VQwDataElement*>::iterator variable_iter;
    for (variable_iter  = variable_list.begin();
	 variable_iter != variable_list.end(); variable_iter++)
      {
	VQwDataElement* variable = *variable_iter;
	if (RequestExternalValue(variable->GetElementName(), variable))
	  {
	    if (bDEBUG)
	      dynamic_cast<QwVQWK_Channel*>(variable)->PrintInfo();
	  }
	else
	  {
	    bIsExchangedDataValid = kFALSE;
	    QwError << GetSubsystemName() << " could not get external value for "
		    << variable->GetElementName() << QwLog::endl;
	  }
      } // end of loop over variables
  }
};




void  QwLumi::ProcessEvent_2()
{
  if (bIsExchangedDataValid)
    {
      //data is valid, process it
      if (bDEBUG)
        {
          Double_t  pedestal = fTargetCharge.GetPedestal();
          Double_t  calfactor = fTargetCharge.GetCalibrationFactor();
          Double_t  volts = fTargetCharge.GetAverageVolts();
          std::cout<<"QwLumi::ProcessEvent_2(): processing with exchanged data"<<std::endl;
          std::cout<<"pedestal, calfactor, average volts = "<<pedestal<<", "<<calfactor<<", "<<volts<<std::endl;
        }

      // assume fTargetCharge.fHardwareSum is a calibrated value,
      // detector signals will be normalized to it
      if (bNormalization) this->DoNormalization();
    }
  else
    {
      QwError<<"QwLumi::ProcessEvent_2(): could not get all external values."<<QwLog::endl;
    }
};














//*****************************************************************
Int_t QwLumi::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{

  return 0;
};

//*****************************************************************
Bool_t QwLumi::IsGoodEvent()
{
  Bool_t test=kTRUE;


  return test;
};


//*****************************************************************
void QwLumi::ClearEventData()
{

  for(size_t i=0;i<fIntegrationPMT.size();i++)
    fIntegrationPMT[i].ClearEventData();
  for(size_t i=0;i<fScalerPMT.size();i++)
    fScalerPMT[i].ClearEventData();
  return;
};


//*****************************************************************
Int_t QwLumi::GetDetectorIndex(EQwPMTInstrumentType type_id, TString name)
{
  Bool_t ldebug=kFALSE;
  if(ldebug)
    {
      std::cout<<"QwLumi::GetDetectorIndex\n";
      std::cout<<"type_id=="<<type_id<<" name="<<name<<"\n";
      std::cout<<fLumiDetectorID.size()<<" already registered detector\n";
    }

  Int_t result=-1;
  for(size_t i=0;i<fLumiDetectorID.size();i++)
    {
      if(fLumiDetectorID[i].fTypeID==type_id)
	if(fLumiDetectorID[i].fdetectorname==name)
	  result=fLumiDetectorID[i].fIndex;
      if(ldebug)
	{
	  std::cout<<"testing against ("<<fLumiDetectorID[i].fTypeID
		   <<","<<fLumiDetectorID[i].fdetectorname<<")=>"<<result<<"\n";
	}
    }

  return result;
};

//*****************************************************************
QwIntegrationPMT* QwLumi::GetChannel(const TString name)
{
  return GetIntegrationPMT(name);
};


//*****************************************************************
QwIntegrationPMT* QwLumi::GetIntegrationPMT(const TString name)
{
  if (! fIntegrationPMT.empty()) {
    for (std::vector<QwIntegrationPMT>::iterator IntegrationPMT = fIntegrationPMT.begin(); IntegrationPMT != fIntegrationPMT.end(); ++IntegrationPMT) {
      if (IntegrationPMT->GetElementName() == name) {
        return &(*IntegrationPMT);
      }
    }
  }
  return 0;
};

//*****************************************************************
VQwSubsystem&  QwLumi::operator=  (VQwSubsystem *value)
{
  //  std::cout<<" here in QwLumi::operator= \n";
  if(Compare(value))
    {
      VQwSubsystem::operator=(value);
      QwLumi* input = dynamic_cast<QwLumi*> (value);

      for(size_t i=0;i<input->fIntegrationPMT.size();i++)
	this->fIntegrationPMT[i]=input->fIntegrationPMT[i];
      for(size_t i=0;i<input->fScalerPMT.size();i++)
	this->fScalerPMT[i]=input->fScalerPMT[i];

    }
  return *this;
};

//*****************************************************************
VQwSubsystem&  QwLumi::operator+=  (VQwSubsystem *value)
{
  if(Compare(value))
    {
      QwLumi* input= dynamic_cast<QwLumi*>(value) ;

      for(size_t i=0;i<input->fIntegrationPMT.size();i++)
	this->fIntegrationPMT[i]+=input->fIntegrationPMT[i];
      for(size_t i=0;i<input->fScalerPMT.size();i++)
	this->fScalerPMT[i]+=input->fScalerPMT[i];
    }
  return *this;
};

//*****************************************************************
VQwSubsystem&  QwLumi::operator-=  (VQwSubsystem *value)
{

  if(Compare(value))
    {
      QwLumi* input= dynamic_cast<QwLumi*>(value);

      for(size_t i=0;i<input->fIntegrationPMT.size();i++)
	this->fIntegrationPMT[i]-=input->fIntegrationPMT[i];
      for(size_t i=0;i<input->fScalerPMT.size();i++)
	this->fScalerPMT[i]-=input->fScalerPMT[i];

    }
  return *this;
};

//*****************************************************************
void  QwLumi::Sum(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if(Compare(value1)&&Compare(value2))
    {


      *this =  value1;
      *this += value2;
    }
};

//*****************************************************************
void  QwLumi::Difference(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if(Compare(value1)&&Compare(value2))
    {

      *this =  value1;
      *this -= value2;
    }
};

//*****************************************************************
void QwLumi::Ratio(VQwSubsystem  *numer, VQwSubsystem  *denom)
{
  if(Compare(numer)&&Compare(denom))
    {
      QwLumi* innumer= dynamic_cast<QwLumi*>(numer) ;
      QwLumi* indenom= dynamic_cast<QwLumi*>(denom) ;

      for(size_t i=0;i<innumer->fIntegrationPMT.size();i++)
	this->fIntegrationPMT[i].Ratio(innumer->fIntegrationPMT[i],indenom->fIntegrationPMT[i]);
      for(size_t i=0;i<innumer->fScalerPMT.size();i++)
	this->fScalerPMT[i].Ratio(innumer->fScalerPMT[i],indenom->fScalerPMT[i]);

    }
  return;
};

//*****************************************************************
void QwLumi::Scale(Double_t factor)
{
  for(size_t i=0;i<fIntegrationPMT.size();i++)
    fIntegrationPMT[i].Scale(factor);
  for(size_t i=0;i<fScalerPMT.size();i++)
    fScalerPMT[i].Scale(factor);
  return;
};

//*****************************************************************
Bool_t QwLumi::Compare(VQwSubsystem *value)
{
  //  std::cout<<" Here in QwLumi::Compare \n";

  Bool_t res=kTRUE;
  if(typeid(*value)!=typeid(*this))
    {
      res=kFALSE;
      //      std::cout<<" types are not ok \n";
      //      std::cout<<" this is bypassed just for now but should be fixed eventually \n";
    }
  else
    {
      QwLumi* input= dynamic_cast<QwLumi*>(value);
	if(input->fIntegrationPMT.size()!=fIntegrationPMT.size())
	  {
	    res=kFALSE;
	  }
	if(input->fScalerPMT.size()!=fScalerPMT.size())
	  {
	    res=kFALSE;
	  }
    }
  return res;
}


//*****************************************************************
void  QwLumi::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  for(size_t i=0;i<fIntegrationPMT.size();i++)
      fIntegrationPMT[i].ConstructHistograms(folder,prefix);
  for(size_t i=0;i<fScalerPMT.size();i++)
    fScalerPMT[i].ConstructHistograms(folder,prefix);

  return;
};


//*****************************************************************
void  QwLumi::DeleteHistograms()
{
  for(size_t i=0;i<fIntegrationPMT.size();i++)
    fIntegrationPMT[i].DeleteHistograms();
  for(size_t i=0;i<fScalerPMT.size();i++)
    fScalerPMT[i].DeleteHistograms();

  return;
};

//*****************************************************************
void  QwLumi::FillHistograms()
{

  for(size_t i=0;i<fIntegrationPMT.size();i++)
    fIntegrationPMT[i].FillHistograms();
  for(size_t i=0;i<fScalerPMT.size();i++)
    fScalerPMT[i].FillHistograms();

  return;
};


//*****************************************************************
void QwLumi::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values)
{

  for(size_t i = 0; i < fIntegrationPMT.size(); i++)
    fIntegrationPMT[i].ConstructBranchAndVector(tree, prefix, values);
  for(size_t i = 0; i < fScalerPMT.size(); i++)
    fScalerPMT[i].ConstructBranchAndVector(tree, prefix, values);

  return;
};

//*****************************************************************
void QwLumi::ConstructBranch(TTree *tree, TString & prefix)
{

  for(size_t i = 0; i < fIntegrationPMT.size(); i++)
    fIntegrationPMT[i].ConstructBranch(tree, prefix);
  for(size_t i = 0; i < fScalerPMT.size(); i++)
    fScalerPMT[i].ConstructBranch(tree, prefix);

  return;
};

//*****************************************************************
void QwLumi::ConstructBranch(TTree *tree, TString & prefix, QwParameterFile& trim_file)
{
  TString tmp;
  QwParameterFile* nextmodule;

  tmp="QwIntegrationPMT";
  trim_file.RewindToFileStart();
  if (trim_file.FileHasModuleHeader(tmp)){
    nextmodule=trim_file.ReadUntilNextModule();
    //This section contains sub modules and or channels to be included in the tree

    for(size_t i = 0; i < fIntegrationPMT.size(); i++)
      fIntegrationPMT[i].ConstructBranch(tree, prefix,*nextmodule);
    for(size_t i = 0; i < fIntegrationPMT.size(); i++)
      fScalerPMT[i].ConstructBranch(tree, prefix);

  }
  return;
};

//*****************************************************************
void QwLumi::FillTreeVector(std::vector<Double_t> &values) const
{

  for(size_t i = 0; i < fIntegrationPMT.size(); i++)
    fIntegrationPMT[i].FillTreeVector(values);
  for(size_t i = 0; i < fScalerPMT.size(); i++)
    fScalerPMT[i].FillTreeVector(values);
  return;
};


//*****************************************************************
void  QwLumi::PrintValue() const
{
  QwMessage << "=== QwLumi: " << GetSubsystemName() << " ===" << QwLog::endl;
  for (size_t i = 0; i < fIntegrationPMT.size(); i++)
    fIntegrationPMT[i].PrintValue();
  for (size_t i = 0; i < fScalerPMT.size(); i++)
    fScalerPMT[i].PrintValue();
};

//*****************************************************************
void  QwLumi::PrintInfo() const
{
  std::cout<<"Name of the subsystem ="<<fSystemName<<"\n";
  std::cout<<"There are "<<fIntegrationPMT.size()<<" IntegrationPMT \n";
  std::cout<<"There are "<<fScalerPMT.size()<<" halo counters \n";


  std::cout<<" Printing Running AVG and other channel info for IntegrationPMTs"<<std::endl;
  for (size_t i = 0; i < fIntegrationPMT.size(); i++)
    fIntegrationPMT[i].PrintInfo();
  for (size_t i = 0; i < fScalerPMT.size(); i++)
    fScalerPMT[i].PrintInfo();

  return;
};

//*****************************************************************
void  QwLumi::PrintDetectorID() const
{
  for (size_t i=0;i<fLumiDetectorID.size();i++)
    {
      std::cout<<"============================="<<std::endl;
      std::cout<<" Detector ID="<<i<<std::endl;
      fLumiDetectorID[i].Print();
    }
  return;
};

//*****************************************************************
void  QwLumiDetectorID::Print() const
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
  std::cout<<"==========================================\n";

  return;
};

//*****************************************************************
void  QwLumi::Copy(VQwSubsystem *source)
{

  try
    {
     if(typeid(*source)==typeid(*this))
	{
	  VQwSubsystem::Copy(source);
	  QwLumi* input= dynamic_cast<QwLumi*>(source);

	  this->fIntegrationPMT.resize(input->fIntegrationPMT.size());
	  for(size_t i=0;i<this->fIntegrationPMT.size();i++)
	    this->fIntegrationPMT[i].Copy(&(input->fIntegrationPMT[i]));

	  this->fScalerPMT.resize(input->fScalerPMT.size());
	  for(size_t i=0;i<this->fScalerPMT.size();i++)
	    this->fScalerPMT[i].Copy(&(input->fScalerPMT[i]));

	}
      else
	{
	  TString loc="Standard exception from QwLumi::Copy = "
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
};

//*****************************************************************
VQwSubsystem*  QwLumi::Copy()
{

  QwLumi* TheCopy=new QwLumi("Injector Lumi Copy");
  TheCopy->Copy(this);
  return TheCopy;
};

//*****************************************************************
void QwLumi::CalculateRunningAverage()
{
  for (size_t i = 0; i < fIntegrationPMT.size(); i++)
    fIntegrationPMT[i].CalculateRunningAverage();
};

//*****************************************************************
void QwLumi::AccumulateRunningSum(VQwSubsystem* value1)
{
  if (Compare(value1)) {
    QwLumi* value = dynamic_cast<QwLumi*>(value1);

    for (size_t i = 0; i < fIntegrationPMT.size(); i++)
      fIntegrationPMT[i].AccumulateRunningSum(value->fIntegrationPMT[i]);
  }
};


void QwLumi::DoNormalization(Double_t factor)
{
  static Bool_t notwarned = kTRUE;

  if (bIsExchangedDataValid)
    {
      try
        {
          Double_t  norm = fTargetCharge.GetHardwareSum()*factor;
	  if (norm >1e-9){
	    this->Scale(1.0/norm);
	    notwarned = kTRUE;
	  } else if (notwarned) {
	    notwarned = kFALSE;
	    QwError << "QwLumi::DoNormalization:  Charge is too small to do the "
		    << "normalization (fTargetCharge==" << fTargetCharge.GetHardwareSum()
		    << ")" << QwLog::endl;
	  }
        }
      catch (std::exception& e)
        {
          std::cerr << e.what() << std::endl;
        }
    }
}







//*****************************************************************
void QwLumi::FillDB(QwDatabase *db, TString datatype)
{

  Bool_t local_print_flag = true;
  if(local_print_flag){
    QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
    QwMessage << "                         QwLumi::FillDB                          " << QwLog::endl;
    QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
  }

  std::vector<QwDBInterface> interface;
  std::vector<QwParityDB::lumi_data> entrylist;

  UInt_t analysis_id = db->GetAnalysisID();

  Char_t measurement_type[4];

  if(datatype.Contains("yield")) {
    sprintf(measurement_type, "%s", "y");
  }
  else if (datatype.Contains("asymmetry")) {
    sprintf(measurement_type, "%s", "a");
  }
  else {
    sprintf(measurement_type, "%s", "");
  }


  UInt_t i,j;
  i = j = 0;
  if(local_print_flag) QwMessage <<  QwColor(Qw::kGreen) << "IntegrationPMT" <<QwLog::endl;

  for(i=0; i< fIntegrationPMT.size(); i++) {
    interface.clear();
    interface = fIntegrationPMT[i].GetDBEntry();
    for(j=0; j<interface.size(); j++){
      interface.at(j).SetAnalysisID( analysis_id );
      interface.at(j).SetLumiDetectorID( db );
      interface.at(j).SetMeasurementTypeID( measurement_type );
      interface.at(j).PrintStatus( local_print_flag );
      interface.at(j).AddThisEntryToList( entrylist );
    }
  }
  if(local_print_flag) {
    QwMessage << QwColor(Qw::kGreen) << "Entrylist Size : "
	      << QwColor(Qw::kBoldRed) << entrylist.size()
              << QwColor(Qw::kNormal) << QwLog::endl;
  }

  db->Connect();
  // Check the entrylist size, if it isn't zero, start to query..
  if( entrylist.size() ) {
    mysqlpp::Query query= db->Query();
    query.insert(entrylist.begin(), entrylist.end());
    query.execute();
  }
  else {
    QwMessage << "QwLumi::FillDB :: This is the case when the entrlylist contains nothing in "<< datatype.Data() << QwLog::endl;
  }

  db->Disconnect();

  return;
};
