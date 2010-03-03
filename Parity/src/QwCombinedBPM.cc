/**********************************************************\
* File: QwCombinedBPM.cc                                  *
*                                                         *
* Author: B. Waidyawansa                                  *
* Time-stamp:                                             *
\**********************************************************/

#include "QwCombinedBPM.h"
#include "QwHistogramHelper.h"
#include <stdexcept>



const Bool_t QwCombinedBPM::kDEBUG = kFALSE;

const TString QwCombinedBPM::axis[3]={"X","Y","Z"};
/* With X being vertical up and Z being the beam direction toward the beamdump */

/********************************************************/
void  QwCombinedBPM::InitializeChannel(TString name, Bool_t ROTATED)
{

  fCombinedWSum.InitializeChannel(name+"WSum","derived");

  for(int i=0;i<3;i++)
    fCombinedAbsPos[i].InitializeChannel(name+axis[i],"derived");

  for(int i=0;i<2;i++)
    fCombinedAngle[i].InitializeChannel(name+axis[i]+"Angle","derived");
  
  SetElementName(name);
  bFullSave=kTRUE;

  return;
};
/********************************************************/

void QwCombinedBPM::Add(QwBPMStripline* bpm, Double_t charge_weight,  Double_t x_weight, Double_t y_weight){

  fElement.push_back(bpm);
  fQWeights.push_back(charge_weight);
  fXWeights.push_back(x_weight);
  fYWeights.push_back(y_weight);

  }

/********************************************************/

void QwCombinedBPM::ClearEventData()
{
  
  for(int i=0;i<3;i++)
    fCombinedAbsPos[i].ClearEventData();

  for(int i=0;i<2;i++)
    fCombinedAngle[i].ClearEventData();
 
  fCombinedWSum.ClearEventData();

  return;
};
/********************************************************/

Int_t QwCombinedBPM::GetEventcutErrorCounters(){
     
  fCombinedAbsPos[0].GetEventcutErrorCounters();
  fCombinedAbsPos[1].GetEventcutErrorCounters();

  return 1;
};

Bool_t QwCombinedBPM::ApplySingleEventCuts(){
  Bool_t status=kTRUE;
  
  ApplyHWChecks();//first apply HW checks and update HW  error flags.

  if (fDevice_flag==1){//if fDevice_flag==1 then perform the event cut limit test

    //we only need to check two final values 
    if (fCombinedAbsPos[0].ApplySingleEventCuts(fLLimitX,fULimitX)){ //for RelX  
      status=kTRUE;
    }
    else{
      fCombinedAbsPos[0].UpdateEventCutErrorCount();
      status=kFALSE;
      if (bDEBUG) std::cout<<" Rel X event cut failed ";
    }
    fDeviceErrorCode|=fCombinedAbsPos[0].GetEventcutErrorFlag();//Get the Event cut error flag for RelX
    if (fCombinedAbsPos[1].ApplySingleEventCuts(fLLimitY,fULimitY)){
      status&=kTRUE;
    }
    else{
      fCombinedAbsPos[1].UpdateEventCutErrorCount();
      status&=kFALSE;
      if (bDEBUG) std::cout<<" Rel Y event cut failed ";
    }
    fDeviceErrorCode|=fCombinedAbsPos[1].GetEventcutErrorFlag();//Get the Event cut error flag for RelY
	
  }
  else             
    status=kTRUE;
    
  
  return status;
};


/********************************************************/

Int_t QwCombinedBPM::SetSingleEventCuts(std::vector<Double_t> & dEventCuts){
  
  fLLimitX=dEventCuts.at(0);
  fULimitX=dEventCuts.at(1);
  fLLimitY=dEventCuts.at(2);
  fULimitY=dEventCuts.at(3);
  fDevice_flag=(Int_t)dEventCuts.at(4);
  
  return 0; 
};




/********************************************************/


void  QwCombinedBPM::ProcessEvent() 
{
  Bool_t ldebug = kFALSE;
  static QwVQWK_Channel numer("numerator"), denom("denominator");
  static QwVQWK_Channel  tmpQADC, tmpADC;
  Double_t chi_square[2];
  std::vector <Double_t> XADC; 
  std::vector <Double_t> YADC; 
  std::vector <Double_t> ZADC;
  std::vector <Double_t> zpos2; 
  std::vector <Double_t> x2;
  std::vector <Double_t> zx;
  std::vector <Double_t> y2;
  std::vector <Double_t> zy;
  std::vector <Double_t> unit;
  std::vector <Double_t> sx;
  std::vector <Double_t> sy;

  Double_t  totalq_weights=0; 

  for(size_t i=0;i<fElement.size();i++) 
    unit.push_back(1);
 
  // check to see if there are correct number of elements to perform least squares fit on. 
  // For a linear fit the number of points should be greater than 2.

  if(fElement.size()<3)
    std::cout<<"QwCombinedBPM:: Process event can't carry out the lines least square fit with only 2 points!"
	     <<" Abborting process.."<<std::endl;

  else{
    for(size_t i=0;i<fElement.size();i++)
      {  
	if(ldebug){
	  std::cout<<"*******************************\n";
	  std::cout<<"QwCombinedBPM: Reading "<<fElement[i]->GetElementName()<<" with charge weight ="<<fQWeights[i]
		   <<" and  x weight ="<<fXWeights[i]
		   <<" and  y weight ="<<fYWeights[i]<<"\n";
	  
	}
	
	//to get the weighted charge(4-wire) sum;
	tmpQADC.Copy(&(fElement[i]->fWSum));
	tmpQADC=fElement[i]->fWSum;
	tmpQADC.Scale(fQWeights[i]);
	fCombinedWSum+=tmpQADC;
	totalq_weights +=fQWeights[i];
	
	if(ldebug) std::cout<<"got 4-wire.hw_sum = "<<tmpQADC.GetHardwareSum()<<" vs     actual "<<(fElement[i]-> fWSum).GetHardwareSum()<<std::endl;
	
	XADC.push_back((fElement[i]-> fAbsPos[0]).GetHardwareSum());
	YADC.push_back((fElement[i]-> fAbsPos[1]).GetHardwareSum());
	ZADC.push_back((fElement[i]-> fAbsPos[2]).GetHardwareSum());

	if(ldebug) {
	  std::cout<<"got absolute X position hw_sum = "<<XADC[i]<<" vs     actual "<<(fElement[i]-> fAbsPos[0]).GetHardwareSum()<<std::endl;
	  std::cout<<"got absolute Y position hw_sum = "<<YADC[i]<<" vs     actual "<<(fElement[i]-> fAbsPos[1]).GetHardwareSum()<<std::endl;
	  std::cout<<"got absolute Z position hw_sum = "<<ZADC[i]<<" vs     actual "<<(fElement[i]-> fAbsPos[2]).GetHardwareSum()<<std::endl;
	}
	
	zpos2.push_back(ZADC[i]*ZADC[i]);
	x2.push_back(XADC[i]*XADC[i]);
	zx.push_back(XADC[i]*ZADC[i]);
	y2.push_back(YADC[i]*YADC[i]);
	zy.push_back(YADC[i]*ZADC[i]);
      }
    
 
    
    //Least square fit for X
    LeastSquareFit( 0, SumOver(fXWeights,ZADC),
		    SumOver(fXWeights,unit),
		    SumOver(fXWeights,XADC),
		    SumOver(fXWeights,zpos2),
		    SumOver(fXWeights,zx),
		    SumOver(fXWeights,x2));
    
    //Least square fit for Y
    LeastSquareFit( 1, SumOver(fYWeights,ZADC),
		    SumOver(fYWeights,unit),
		    SumOver(fYWeights,YADC),
		    SumOver(fYWeights,zpos2),
		    SumOver(fYWeights,zy),
		    SumOver(fYWeights,y2));
     
    
    fCombinedWSum.Scale(1.0/totalq_weights);

    for(size_t n=0;n<2;n++){
      //absolute positions at the target;
      fCombinedAbsPos[n].SetHardwareSum(ZADC[n]*a[n] + b[n]);
      //angle at the target;
      fCombinedAngle[n].SetHardwareSum(atan(b[n])* 180 / M_PI);
    }
    fCombinedAbsPos[2].SetHardwareSum(ZADC[2]);


    if(ldebug) std::cout<<"QwCombinedBPM:: Projected target X position = "<<fCombinedAbsPos[0].GetHardwareSum()
			<<" and target X angle = "<<fCombinedAngle[0].GetHardwareSum()
			<<"\nProjected target Y position = "<<fCombinedAbsPos[1].GetHardwareSum()
			<<" and target Y angle = "<<fCombinedAngle[1].GetHardwareSum()<<std::endl;
      

    //to perform the minimul chi-square test
    for(size_t i=0;i<fElement.size();i++){
      Double_t tmp=0;
      tmp = XADC[i] - ZADC[0]*a[0] - b[0];
      sx.push_back(tmp*tmp);
      tmp = YADC[i] - ZADC[1]*a[1] - b[1];
      sy.push_back(tmp*tmp);
    }
    
    chi_square[0]=(SumOver(fXWeights,sx))/(fElement.size()-2); //mimul ch-square for fit on x
    chi_square[1]=(SumOver(fYWeights,sy))/(fElement.size()-2); //mimul ch-square for fit on y

    if(ldebug) {
      std::cout<<"QwCombinedBPM:: The minimul chi-square for the fit on X is  "<<chi_square[0]
	       <<" and for Y = "<<chi_square[1]<<std::endl;
      std::cout<<"For a good fit minimul-chisquare should be close to 1!"<<std::endl;
    }
  }
    
    
  if (ldebug) {
    fCombinedWSum.Print(); 
    for(size_t n=0;n<3;n++)
      fCombinedAbsPos[n].Print();
  }

  return;
};


Double_t QwCombinedBPM::SumOver(std::vector<Double_t> weight,std::vector <Double_t> val)
{
  Double_t sum = 0;
  if(weight.size()!=fElement.size()) std::cout<<"QwCombinedBPM:: Number of devices doesnt match the number of weights."
					      <<" Exiting calculating parameters for the least squares fit"<<std::endl;
  else{
    for(size_t i=0;i<weight.size();i++){
      sum+=(val[i]*weight[i]);
    }
  }
  return sum;
}

void QwCombinedBPM::LeastSquareFit(Int_t pos, Double_t A,Double_t B,Double_t C,Double_t D,Double_t E,Double_t F )
{
  Bool_t ldebug = kFALSE;

  if(ldebug) std::cout<<"A ="<<A<<" -- B ="<<B<<" --C ="<<C<<" --D ="<<D<<" --E ="<<E<<" --F ="<<F<<"\n";

  Double_t m = D*B-A*A;
  a[pos]=(E*B-C*A)/m;
  b[pos]= (D*C-E*A)/m;
  erra[pos]= B/m;
  errb[pos]=D/m;
  covab[pos]= -A/m;

  if(ldebug)    std::cout<<"Least Squares Fit Parameters for "<<pos<<" are: \na = "<<a[pos]<<"+-"<<erra[pos]
			 <<"\nb = "<<b[pos]<<"+-"<<errb[pos]
			 <<"\ncovariance of a and b = "<<covab[pos]<<"\n";
 
  return;
}


/********************************************************/
void QwCombinedBPM::Print()
{

  for(int i=0;i<3;i++)
    fCombinedAbsPos[i].Print();
  for(int i=0;i<2;i++)
    fCombinedAngle[i].Print();
  fCombinedWSum.Print();

  return;
}

/********************************************************/
Bool_t QwCombinedBPM::ApplyHWChecks()
{
  Bool_t fEventIsGood=kTRUE;
   
  return fEventIsGood;
};
/********************************************************/
Int_t QwCombinedBPM::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer,UInt_t index)
{
  return word_position_in_buffer;
};

/********************************************************/


QwCombinedBPM& QwCombinedBPM::operator= (const QwCombinedBPM &value)
{
  if (GetElementName()!="")
  {
    this->bRotated=value.bRotated;
    this->fCombinedWSum=value.fCombinedWSum;
    for(int i=0;i<2;i++)
      this->fCombinedAngle[i]=value.fCombinedAngle[i];
    for(int i=0;i<3;i++)
      {
	this->fCombinedAbsPos[i]=value.fCombinedAbsPos[i];
	this->fComboOffset[i]=value.fComboOffset[i];
      }
  }
  return *this;
};

QwCombinedBPM& QwCombinedBPM::operator+= (const QwCombinedBPM &value)
{
  if (GetElementName()!="")
    {
      this->fCombinedWSum+=value.fCombinedWSum;
      for(int i=0;i<2;i++)
	this->fCombinedAngle[i]+=value.fCombinedAngle[i];
      for(int i=0;i<3;i++)
 	this->fCombinedAbsPos[i]+=value.fCombinedAbsPos[i];
    }
  return *this;
};

QwCombinedBPM& QwCombinedBPM::operator-= (const QwCombinedBPM &value)
{
  if (GetElementName()!="")
    {
      this->fCombinedWSum-=value.fCombinedWSum;
      for(int i=0;i<2;i++)
	this->fCombinedAngle[i]-=value.fCombinedAngle[i];
      for(int i=0;i<3;i++)
	this->fCombinedAbsPos[i]-=value.fCombinedAbsPos[i];
    }
  return *this;
};


void QwCombinedBPM::Sum(QwCombinedBPM &value1, QwCombinedBPM &value2){
  *this =  value1;
  *this += value2;
};

void QwCombinedBPM::Difference(QwCombinedBPM &value1, QwCombinedBPM &value2){
  *this =  value1;
  *this -= value2;
};

void QwCombinedBPM::Ratio(QwCombinedBPM &numer, QwCombinedBPM &denom)
{
  // this function is called when forming asymmetries. In this case waht we actually want for the
  // stripline is the difference only not the asymmetries
 
  *this=numer;
  this->fCombinedWSum.Ratio(numer.fCombinedWSum,denom.fCombinedWSum);
  if (GetElementName()!="")
    {
      for(int i=0;i<2;i++)
 	this->fCombinedAngle[i].Ratio(numer.fCombinedAngle[i], denom.fCombinedAngle[i]);
      for(int i=0;i<3;i++)
	this->fCombinedAbsPos[i].Ratio(numer.fCombinedAbsPos[i], denom.fCombinedAbsPos[i]);
    }

  return;
};


void QwCombinedBPM::Scale(Double_t factor)
{
  for(int i=0;i<2;i++)
      fCombinedAngle[i].Scale(factor);
  for(int i=0;i<3;i++)
      fCombinedAbsPos[i].Scale(factor);
}

void QwCombinedBPM::Calculate_Running_Average(){
  for(int i=0;i<2;i++)
      fCombinedAngle[i].Calculate_Running_Average();
  for(int i=0;i<2;i++)
    fCombinedAbsPos[i].Calculate_Running_Average();

  fCombinedWSum.Calculate_Running_Average();
};

void QwCombinedBPM::Do_RunningSum(){
  for(int i=0;i<2;i++)
      fCombinedAngle[i].Do_RunningSum();
  for(int i=0;i<3;i++)
    fCombinedAbsPos[i].Do_RunningSum();
  fCombinedWSum.Do_RunningSum();
};


/********************************************************/
void QwCombinedBPM::SetRootSaveStatus(TString &prefix)
{
  if(prefix=="diff_"||prefix=="yield_"|| prefix=="asym_")
    bFullSave=kFALSE;

  return;
}


void  QwCombinedBPM::ConstructHistograms(TDirectory *folder, TString &prefix)
{

  if (GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      //we calculate the asym_ for the fCombinedWSum becasue its an asymmetry and not a difference.
      fCombinedWSum.ConstructHistograms(folder, prefix);    
      TString thisprefix=prefix;
      if(prefix=="asym_")
	thisprefix="diff_";
      SetRootSaveStatus(prefix);
     
      for(int i=0;i<2;i++)
	  fCombinedAngle[i].ConstructHistograms(folder, thisprefix);
	for(int i=0;i<3;i++)
	fCombinedAbsPos[i].ConstructHistograms(folder, prefix);
    }
  return;
};

void  QwCombinedBPM::FillHistograms()
{
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      fCombinedWSum.FillHistograms();
      for(int i=0;i<2;i++)
	  fCombinedAngle[i].FillHistograms();
      for(int i=0;i<3;i++)
	fCombinedAbsPos[i].FillHistograms();
    }
  return;
};

void  QwCombinedBPM::DeleteHistograms()
{
  if (GetElementName()=="")
    {
    }
  else
    {
      fCombinedWSum.DeleteHistograms();
      for(int i=0;i<2;i++)
	fCombinedAngle[i].DeleteHistograms();
      for(int i=0;i<3;i++)
	fCombinedAbsPos[i].DeleteHistograms();

    }
  return;
};


void  QwCombinedBPM::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip constructing trees.
  } else
    {
      TString thisprefix=prefix;
      if(prefix=="asym_")
	thisprefix="diff_";

      SetRootSaveStatus(prefix);

      fCombinedWSum.ConstructBranchAndVector(tree,prefix,values);

      for(int i=0;i<2;i++)
	  fCombinedAngle[i].ConstructBranchAndVector(tree,thisprefix,values);
 	for(int i=0;i<3;i++)
 	  fCombinedAbsPos[i].ConstructBranchAndVector(tree,thisprefix,values);
	
    }
  return;
};

void  QwCombinedBPM::FillTreeVector(std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the tree.
  } else
    {
      fCombinedWSum.FillTreeVector(values);
	 
      for(int i=0;i<2;i++)
	fCombinedAngle[i].FillTreeVector(values);

      for(int i=0;i<3;i++)
	fCombinedAbsPos[i].FillTreeVector(values);
	 
    }
  return;
};

//******************************************************************
void QwCombinedBPM::Copy(VQwDataElement *source)
{
  try
    {
     if(typeid(*source)==typeid(*this))
       {
	 QwCombinedBPM* input = ((QwCombinedBPM*)source);
	 this->fElementName = input->fElementName;
	 this->fCombinedWSum.Copy(&(input->fCombinedWSum));
	 this->bRotated = input->bRotated;
	 this->bFullSave = input->bFullSave;
 	 for(int i = 0; i < 3; i++)
 	   this->fComboOffset[i] = input->fComboOffset[i];
 	 for(int i = 0; i < 2; i++)
 	   this->fCombinedAngle[i].Copy(&(input->fCombinedAngle[i]));
	 for(int i = 0; i < 3; i++)
	   this->fCombinedAbsPos[i].Copy(&(input->fCombinedAbsPos[i]));
       }
     else
	{
	  TString loc="Standard exception from QwCombinedBPM::Copy = "
	    +source->GetElementName()+" "
	    +this->GetElementName()+" are not of the same type";
	  throw std::invalid_argument(loc.Data());
	}
    }

  catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
    }

  return;
}

void QwCombinedBPM::SetEventCutMode(Int_t bcuts){

  bEVENTCUTMODE=bcuts;
  for (Int_t i=0;i<2;i++)
    fCombinedAngle[i].SetEventCutMode(bcuts);
  for (Int_t i=0;i<3;i++)
    fCombinedAbsPos[i].SetEventCutMode(bcuts);
  fCombinedWSum.SetEventCutMode(bcuts);
}


