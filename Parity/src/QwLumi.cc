/**********************************************************\
* File: QwLumi.C                                      *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwLumi.h"
#include "QwHistogramHelper.h"
#include <stdexcept>

// Register this subsystem with the factory
QwSubsystemFactory<QwLumi> theLumiFactory("QwLumi");

//*****************************************************************
void QwLumi::ProcessOptions(QwOptions &options){
      //Handle command line options
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
  Int_t wordsofar=0;
  Int_t currentsubbankindex=-1;
  Int_t fSample_size=0;

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
	  //	  currentrocread=value;
	  //	  RegisterROCNumber(value,currentbankread);
	  //
	  //???  Why did you do the things you did above?
	  //???  Paul; 2008nov05, 17:13
	  //
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

	if(currentsubbankindex!=GetSubbankIndex(currentrocread,currentbankread))
	  {
	    currentsubbankindex=GetSubbankIndex(currentrocread,currentbankread);
	    wordsofar=0;
	  }

	QwLumiDetectorID localLumiDetectorID;
	localLumiDetectorID.fdetectorname=namech;
	localLumiDetectorID.fmoduletype=modtype;
	localLumiDetectorID.fSubbankIndex=currentsubbankindex;
	localLumiDetectorID.fdetectortype=dettype;

	localLumiDetectorID.fWordInSubbank=wordsofar;
	if(modtype=="VQWK")wordsofar+=6;
	else if(modtype=="SCALER")wordsofar+=1;
	else
	  {
	    //std::cerr << "QwLumi::LoadChannelMap:  Unknown module type: "<< modtype <<", the detector "<<namech<<" will not be decoded "<< std::endl;
	    lineok=kFALSE;
	    continue;
	  }

	localLumiDetectorID.fTypeID=GetDetectorTypeID(dettype);
	if(localLumiDetectorID.fTypeID==-1)
	  {
	    //std::cerr << "QwLumi::LoadChannelMap:  Unknown detector type: "<< dettype <<", the detector "<<namech<<" will not be decoded "<< std::endl;
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
		QwIntegrationPMT localIntegrationPMT(localLumiDetectorID.fdetectorname);
		fIntegrationPMT.push_back(localIntegrationPMT);
		fIntegrationPMT[fIntegrationPMT.size()-1].SetDefaultSampleSize(fSample_size);
		localLumiDetectorID.fIndex=fIntegrationPMT.size()-1;
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
      std::cout<<"Done with Load map channel \n";
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

      if (device_type == "IntegrationPMT"){

	//std::cout<<" device name "<<device_name<<" device flag "<<check_flag<<std::endl;

	LLX = (atof(mapstr.GetNextToken(", ").c_str()));	//lower limit for IntegrationPMT value
	ULX = (atof(mapstr.GetNextToken(", ").c_str()));	//upper limit for IntegrationPMT value

	Int_t det_index=GetDetectorIndex(GetDetectorTypeID(device_type),device_name);
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
}

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
}
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
}

//*****************************************************************
Int_t QwLumi::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  Bool_t lkDEBUG=kFALSE;

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
		    std::cout<<"word left to read in this buffer:"<<num_words-fLumiDetectorID[i].fWordInSubbank<<std::endl;
		  }
		fIntegrationPMT[fLumiDetectorID[i].fIndex].
		  ProcessEvBuffer(&(buffer[fLumiDetectorID[i].fWordInSubbank]),
				  num_words-fLumiDetectorID[i].fWordInSubbank);
	      }
	  }
      }
  }

  return 0;
};

Bool_t QwLumi::ApplySingleEventCuts(){
  //currently this will check the IsGoodEvent() only!
  //std::cout<<" QwLumi::SingleEventCuts() ";

  Bool_t test_IntegrationPMT=kTRUE;
  Bool_t test_IntegrationPMT1=kTRUE;





  for(size_t i=0;i<fIntegrationPMT.size();i++){
    //std::cout<<"  IntegrationPMT ["<<i<<"] "<<std::endl;
    test_IntegrationPMT1=fIntegrationPMT[i].ApplySingleEventCuts();
    test_IntegrationPMT&=test_IntegrationPMT1;
    if(!test_IntegrationPMT1 && bDEBUG) std::cout<<"******* QwLumi::SingleEventCuts()->IntegrationPMT[ "<<i<<" , "<<fIntegrationPMT[i].GetElementName()<<" ] ******\n";
  }
  //if (!test_IntegrationPMT)
  //fNumError_Evt_IntegrationPMT++;//IntegrationPMT falied  event counter for QwLumi


  if (!test_IntegrationPMT1 || !test_IntegrationPMT)
   fQwLumiErrorCount++;//BPM falied  event counter for QwLumi






  return test_IntegrationPMT;

};

Int_t QwLumi::GetEventcutErrorCounters(){//inherited from the VQwSubsystemParity; this will display the error summary

  std::cout<<"*********QwLumi Error Summary****************"<<std::endl;
  std::cout<<"Device name ||  Sample || SW_HW || Sequence || SameHW || EventCut\n";
  for(size_t i=0;i<fIntegrationPMT.size();i++){
    //std::cout<<"  IntegrationPMT ["<<i<<"] "<<std::endl;
    fIntegrationPMT[i].ReportErrorCounters();
  }

  //std::cout<<"Total failed events "<<  fQwLumiErrorCount<<std::endl;
  //std::cout<<"*********End of error QwLumi reporting****************"<<std::endl;

  return 1;
}

Int_t QwLumi::GetEventcutErrorFlag(){//return the error flag

  return 0;

}


void  QwLumi::ProcessEvent()
{
  for(size_t i=0;i<fIntegrationPMT.size();i++)
    fIntegrationPMT[i].ProcessEvent();


  // fStripline[0].Print();
  //  fIntegrationPMT[0].Print();

  return;
};

Int_t QwLumi::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{

  return 0;
};

//*****************************************************************
Bool_t QwLumi::IsGoodEvent()
{
  Bool_t test=kTRUE;


  return test;
}


//*****************************************************************
void QwLumi::ClearEventData()
{

  for(size_t i=0;i<fIntegrationPMT.size();i++)
    fIntegrationPMT[i].ClearEventData();
  return;
};
//*****************************************************************
EQwPMTInstrumentType QwLumi::GetDetectorTypeID(TString name)
{
  return GetQwPMTInstrumentType(name);
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
    }
  return *this;
};

VQwSubsystem&  QwLumi::operator+=  (VQwSubsystem *value)
{
  if(Compare(value))
    {
      QwLumi* input= dynamic_cast<QwLumi*>(value) ;

      for(size_t i=0;i<input->fIntegrationPMT.size();i++)
	this->fIntegrationPMT[i]+=input->fIntegrationPMT[i];
    }
  return *this;
};

VQwSubsystem&  QwLumi::operator-=  (VQwSubsystem *value)
{

  if(Compare(value))
    {
      QwLumi* input= dynamic_cast<QwLumi*>(value);

      for(size_t i=0;i<input->fIntegrationPMT.size();i++)
	this->fIntegrationPMT[i]-=input->fIntegrationPMT[i];
    }
  return *this;
};

void  QwLumi::Sum(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if(Compare(value1)&&Compare(value2))
    {


      *this =  value1;
      *this += value2;
    }
};

void  QwLumi::Difference(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if(Compare(value1)&&Compare(value2))
    {

      *this =  value1;
      *this -= value2;
    }
};

void QwLumi::Ratio(VQwSubsystem  *numer, VQwSubsystem  *denom)
{
  if(Compare(numer)&&Compare(denom))
    {
      QwLumi* innumer= dynamic_cast<QwLumi*>(numer) ;
      QwLumi* indenom= dynamic_cast<QwLumi*>(denom) ;

      for(size_t i=0;i<innumer->fIntegrationPMT.size();i++)
	this->fIntegrationPMT[i].Ratio(innumer->fIntegrationPMT[i],indenom->fIntegrationPMT[i]);
    }
  return;
};


void QwLumi::Scale(Double_t factor)
{
  for(size_t i=0;i<fIntegrationPMT.size();i++)
    fIntegrationPMT[i].Scale(factor);

  return;
};

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
	  //	  std::cout<<" not the same number of IntegrationPMTs \n";
	  }
    }
  return res;
}


//*****************************************************************

void  QwLumi::ConstructHistograms(TDirectory *folder, TString &prefix)
{

  //  std::cout<<" here is QwLumi::ConstructHistogram with prefix ="<<prefix<<"\n";


  for(size_t i=0;i<fIntegrationPMT.size();i++)
      fIntegrationPMT[i].ConstructHistograms(folder,prefix);

  return;
};

void  QwLumi::DeleteHistograms()
{


  for(size_t i=0;i<fIntegrationPMT.size();i++)
    fIntegrationPMT[i].DeleteHistograms();

  return;
};

void  QwLumi::FillHistograms()
{

  for(size_t i=0;i<fIntegrationPMT.size();i++)
    fIntegrationPMT[i].FillHistograms();

  return;
};



void QwLumi::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values)
{

  for(size_t i = 0; i < fIntegrationPMT.size(); i++)
    fIntegrationPMT[i].ConstructBranchAndVector(tree, prefix, values);
  return;
};

void QwLumi::FillTreeVector(std::vector<Double_t> &values)
{

  for(size_t i = 0; i < fIntegrationPMT.size(); i++)
    fIntegrationPMT[i].FillTreeVector(values);
  return;
};


//*****************************************************************
void  QwLumi::Print()
{
  std::cout<<"Name of the subsystem ="<<fSystemName<<"\n";

  std::cout<<"there are "<<fIntegrationPMT.size()<<" IntegrationPMT \n";


  std::cout<<" Printing Running AVG and other channel info for IntegrationPMTs"<<std::endl;
  for(size_t i=0;i<fIntegrationPMT.size();i++)
    fIntegrationPMT[i].Print();

  return;
}

void  QwLumi::PrintDetectorID()
{
  for (size_t i=0;i<fLumiDetectorID.size();i++)
    {
      std::cout<<"============================="<<std::endl;
      std::cout<<" Detector ID="<<i<<std::endl;
      fLumiDetectorID[i].Print();
    }
  return;
}


void  QwLumiDetectorID::Print()
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
}

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
}


VQwSubsystem*  QwLumi::Copy()
{

  QwLumi* TheCopy=new QwLumi("Injector Lumi Copy");
  TheCopy->Copy(this);
  return TheCopy;
}

void QwLumi::CalculateRunningAverage()
{
  UInt_t i = 0;
  std::cout<<"*********QwLumiDetector   Averages****************"<<std::endl;
  std::cout<<"Device \t    ||  Average\t || error\t || events"<<std::endl;
  for(i=0; i<fIntegrationPMT.size(); i++) fIntegrationPMT[i].CalculateRunningAverage();
  std::cout<<"---------------------------------------------------"<<std::endl;
  std::cout<<std::endl;
  return;
};

void QwLumi::AccumulateRunningSum(VQwSubsystem* value1)
{
  if (Compare(value1)) {
    QwLumi* value = dynamic_cast<QwLumi*>(value1);

    for (size_t i = 0; i < fIntegrationPMT.size(); i++)
      fIntegrationPMT[i].AccumulateRunningSum(value->fIntegrationPMT[i]);
  }
};




void QwLumi::FillDB(QwDatabase *db, TString datatype)
{

  QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
  QwMessage << "                         QwLumi::FillDB                          " << QwLog::endl;
  QwMessage << " --------------------------------------------------------------- " << QwLog::endl;

  Bool_t local_print_flag = true;
  QwDBInterface interface;
  std::vector<QwParityDB::lumi_data> entrylist;

  UInt_t analysis_id = db->GetAnalysisID();

  TString yield_type(db->GetMeasurementID(12)); // yp
  TString asymm_type(db->GetMeasurementID(0));//a

  Char_t measurement_type[4];

  if(datatype.Contains("yield")) {
    sprintf(measurement_type, "%s", yield_type.Data());
  }
  else if (datatype.Contains("asymmetry")) {
    sprintf(measurement_type, "%s", asymm_type.Data());
  }
  else {
    sprintf(measurement_type, "%s", " ");
  }

  QwMessage <<  QwColor(Qw::kGreen) << "IntegrationPMT" <<QwLog::endl;

  for(UInt_t i=0; i< fIntegrationPMT.size(); i++)
    {
      interface.Reset();
      interface = fIntegrationPMT[i].GetDBEntry("");
      // QwIntegrationPMT has only one element, thus noname "" on it.
      interface.SetAnalysisID( analysis_id );
      interface.SetDeviceID( db->GetLumiDetectorID(interface.GetDeviceName().Data()) );
      interface.SetMeasurementTypeID(measurement_type);
      interface.PrintStatus(local_print_flag);

      interface.AddThisEntryToList(entrylist);
    }

  QwMessage << QwColor(Qw::kGreen) << "Entrylist Size : "
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
      QwMessage << "QwLumi::FillDB :: This is the case when the entrlylist contains nothing in "<< datatype.Data() << QwLog::endl;
    }

  db->Disconnect();

  return;
};
