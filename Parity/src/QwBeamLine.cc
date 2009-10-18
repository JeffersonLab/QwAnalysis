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
  Int_t modnum, channum;
  Int_t currentrocread=0;
  Int_t currentbankread=0;
  Int_t wordsofar=0;
  Int_t currentsubbankindex=-1;
 
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
		localBeamDetectorID.fIndex=fStripline.size()-1;
	      }
	    if(DetectorTypes[localBeamDetectorID.fTypeID]=="bcm")
	      {
		QwBCM localbcm(localBeamDetectorID.fdetectorname);
		fBCM.push_back(localbcm);
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

Int_t QwBeamLine::LoadEventCuts(TString & filename){
  return 0;
};

//*****************************************************************
Int_t QwBeamLine::LoadInputParameters(TString pedestalfile)
{
  Bool_t ldebug=kFALSE;
  TString varname;
  Double_t varped;
  Double_t varcal;
  TString localname;

  Int_t lineread=0;

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
	  if(ldebug) std::cout<<"inputs for channel "<<varname
			      <<": ped="<<varped<<": cal="<<varcal<<"\n"; 
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

Bool_t QwBeamLine::SingleEventCuts(){
  //currently this will check the IsGoodEvent() only!
  std::cout<<" QwBeamLine::SingleEventCuts()"<<std::endl;
  return IsGoodEvent();
};


void  QwBeamLine::ProcessEvent()
{
  for(size_t i=0;i<fStripline.size();i++)
    fStripline[i].ProcessEvent();

  for(size_t i=0;i<fBCM.size();i++)
    fBCM[i].ProcessEvent();
  

  // fStripline[0].Print();
  //  fBCM[0].Print();

  return;
};

Int_t QwBeamLine::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{

  return 0;
};

//*****************************************************************
Bool_t QwBeamLine::IsGoodEvent()
{
  Bool_t test=kTRUE;
  std::cerr << "in QwBeamLine::IsGoodEvent\n";

  for(size_t i=0;i<fStripline.size();i++)
    test&=fStripline[i].IsGoodEvent();
  for(size_t i=0;i<fBCM.size();i++)
    test&=fBCM[i].IsGoodEvent();	

  if(!test) std::cerr<<" this is not a good event\n";
  return test;  
}


//*****************************************************************
void QwBeamLine::ClearEventData()
{
  for(size_t i=0;i<fStripline.size();i++)
    fStripline[i].ClearEventData();
  for(size_t i=0;i<fBCM.size();i++)
    fBCM[i].ClearEventData();	
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
VQwSubsystem&  QwBeamLine::operator=  (VQwSubsystem *value)
{
  //  std::cout<<" here in QwBeamLine::operator= \n";
  if(Compare(value))
    {
      QwBeamLine* input= (QwBeamLine*)value;
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
      QwBeamLine* input= (QwBeamLine*)value ;
      for(size_t i=0;i<input->fStripline.size();i++)
	this->fStripline[i]+=input->fStripline[i];
      for(size_t i=0;i<input->fBCM.size();i++)
	this->fBCM[i]+=input->fBCM[i];
    }
  return *this;
};

VQwSubsystem&  QwBeamLine::operator-=  (VQwSubsystem *value)
{

  if(Compare(value))
    {      
      QwBeamLine* input= (QwBeamLine*)value;
      for(size_t i=0;i<input->fStripline.size();i++)
	this->fStripline[i]-=input->fStripline[i];
      for(size_t i=0;i<input->fBCM.size();i++)
	this->fBCM[i]-=input->fBCM[i];
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
      QwBeamLine* innumer= (QwBeamLine*)numer ;
      QwBeamLine* indenom= (QwBeamLine*)denom ;
      for(size_t i=0;i<innumer->fStripline.size();i++)
	this->fStripline[i].Ratio(innumer->fStripline[i],indenom->fStripline[i]);
      for(size_t i=0;i<innumer->fBCM.size();i++)
	this->fBCM[i].Ratio(innumer->fBCM[i],indenom->fBCM[i]);
    }
  return;
};	


void QwBeamLine::Scale(Double_t factor)
{
  for(size_t i=0;i<fStripline.size();i++)
    fStripline[i].Scale(factor);
  for(size_t i=0;i<fBCM.size();i++)
    fBCM[i].Scale(factor);
  
  return;
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
      QwBeamLine* input= (QwBeamLine*)value;
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

  return;
};

void  QwBeamLine::DeleteHistograms()
{
  for(size_t i=0;i<fStripline.size();i++)
    fStripline[i].DeleteHistograms();

  for(size_t i=0;i<fBCM.size();i++)
    fBCM[i].DeleteHistograms();

  return;
};

void  QwBeamLine::FillHistograms()
{
  for(size_t i=0;i<fStripline.size();i++)
    fStripline[i].FillHistograms();
  for(size_t i=0;i<fBCM.size();i++)
    fBCM[i].FillHistograms();

  return;
};



void QwBeamLine::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values)
{
  for(size_t i=0;i<fStripline.size();i++)
    fStripline[i].ConstructBranchAndVector(tree, prefix,values);
  for(size_t i=0;i<fBCM.size();i++)
    fBCM[i].ConstructBranchAndVector(tree, prefix,values);
  return;
};

void QwBeamLine::FillTreeVector(std::vector<Double_t> &values)
{
  for(size_t i=0;i<fStripline.size();i++)
    fStripline[i].FillTreeVector(values);
  for(size_t i=0;i<fBCM.size();i++)
    fBCM[i].FillTreeVector(values);
  return;
};


//*****************************************************************
void  QwBeamLine::Print()
{
  std::cout<<"Name of the subsystem ="<<fSystemName<<"\n";
  std::cout<<"there are "<<fStripline.size()<<" striplines \n";
  std::cout<<"there are "<<fBCM.size()<<" bcm \n";

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

  std::cout<<"==========================================\n";
  
  return;
}

//*****************************************************************
void  QwBeamLine::Copy(VQwSubsystem *source)
{

  try
    {
     if(typeid(*source)==typeid(*this))
	{
	  QwBeamLine* input=((QwBeamLine*)source);
	  this->fStripline.resize(input->fStripline.size());
	  for(size_t i=0;i<this->fStripline.size();i++)
	      this->fStripline[i].Copy(&(input->fStripline[i]));
	  this->fBCM.resize(input->fBCM.size());
	  for(size_t i=0;i<this->fBCM.size();i++)
	    this->fBCM[i].Copy(&(input->fBCM[i]));
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
