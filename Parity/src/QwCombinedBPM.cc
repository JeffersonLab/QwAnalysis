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

  for(Short_t i=0;i<3;i++)
    fCombinedAbsPos[i].InitializeChannel(name+axis[i],"derived");

  for(Short_t i=0;i<2;i++)
    fCombinedSlope[i].InitializeChannel(name+axis[i]+"Slope","derived");

  SetElementName(name); 

  bFullSave=kTRUE;

  fElement.clear();
  fQWeights.clear();
  fXWeights.clear();
  fYWeights.clear();

  fSumQweights = 0.0;

  for(Short_t i=0;i<3;i++) fComboOffset[0] = 0.0;
  bRotated             = false;
  bFullSave            = false;
  fGoodEvent           = false;
  fixedParamCalculated = false;

  for(Short_t i=0;i<2;i++)
    {
      fIntersept[i] = 0.0;
      erra[i]       = 0.0;
      errb[i]       = 0.0;
      covab[i]      = 0.0;
      A[i]          = 0.0;
      B[i]          = 0.0;
      C[i]          = 0.0;
      D[i]          = 0.0;
      E[i]          = 0.0;
      F[i]          = 0.0;
      m[i]          = 0.0;
      chi_square[i] = 0.0;
    }

  fULimitX = 0.0;
  fLLimitX = 0.0;
  fULimitY = 0.0;
  fLLimitY = 0.0;

  fDevice_flag     = 0;
  fDeviceErrorCode = 0;
  bEVENTCUTMODE    = false;  
  return;
};


void QwCombinedBPM::SetOffset(Double_t Xoffset, Double_t Yoffset, Double_t Zoffset)
{
  fComboOffset[0]=Xoffset;
  fComboOffset[1]=Yoffset;
  fComboOffset[2]=Zoffset;
  return;
};
/********************************************************/

void QwCombinedBPM::Set(QwBPMStripline* bpm, Double_t charge_weight,  Double_t x_weight, Double_t y_weight,
			Double_t sumqw) 
{
  fElement.push_back(bpm);
  fQWeights.push_back(charge_weight);
  fXWeights.push_back(x_weight);
  fYWeights.push_back(y_weight);
  fSumQweights=sumqw;

  return;

};

/********************************************************/

void QwCombinedBPM::ClearEventData()
{
  
  for(Short_t i=0;i<3;i++)  fCombinedAbsPos[i].ClearEventData();
  for(Short_t i=0;i<2;i++)  fCombinedSlope [i].ClearEventData();
  fCombinedWSum.ClearEventData();

  return;
};
/********************************************************/

Int_t QwCombinedBPM::GetEventcutErrorCounters(){
for(Int_t i=0;i<2;i++){     
  fCombinedAbsPos[i].GetEventcutErrorCounters();
  fCombinedSlope[i].GetEventcutErrorCounters();
 }
  fCombinedWSum.GetEventcutErrorCounters();

  return 1;
};

Bool_t QwCombinedBPM::ApplySingleEventCuts(){
  Bool_t status=kTRUE;
  Int_t i=0;

  for(i=0;i<2;i++){
    if (fCombinedAbsPos[i].ApplySingleEventCuts()){ //for absolute X    
      status&=kTRUE;
    }
    else{
      fCombinedAbsPos[i].UpdateEventCutErrorCount();
      status&=kFALSE;
      if (bDEBUG) std::cout<<" X event cut failed ";
    }
    fDeviceErrorCode|=fCombinedAbsPos[i].GetEventcutErrorFlag();//Get the Event cut error flag for X/Y  
  }
  //Event cuts for  X & Y slopes
  for(i=0;i<2;i++){
    if (fCombinedSlope[i].ApplySingleEventCuts()){ //for X slope   
      status&=kTRUE;
    }
    else{
      fCombinedSlope[i].UpdateEventCutErrorCount();
      status&=kFALSE;
      if (bDEBUG) std::cout<<" X Slope event cut failed ";
    }
    fDeviceErrorCode|=fCombinedSlope[i].GetEventcutErrorFlag();//Get the Event cut error flag for SlopeX/Y    
  }
  
  //Event cuts for four wire sum (WSum)
  if (fCombinedWSum.ApplySingleEventCuts()){ //for WSum  
      status&=kTRUE;
  }
  else{
    fCombinedWSum.UpdateEventCutErrorCount();
    status&=kFALSE;
    if (bDEBUG) std::cout<<" WSum event cut failed ";
  }
  
  return status;
};


/********************************************************/

Int_t QwCombinedBPM::SetSingleEventCuts(TString ch_name, Double_t minX, Double_t maxX){
  QwMessage<<" Event Cut Device "<<fElementName;
  if (ch_name=="x"){//cuts for absolute x
    QwMessage<<"X LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fCombinedAbsPos[0].SetSingleEventCuts(minX,maxX);
  }else if (ch_name=="y"){//cuts for absolute y
    QwMessage<<"Y LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fCombinedAbsPos[1].SetSingleEventCuts(minX,maxX);
  }else if (ch_name=="xslope"){//cuts for the x slope
    QwMessage<<"XSlope LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fCombinedSlope[0].SetSingleEventCuts(minX,maxX);
  }else if (ch_name=="yslope"){//cuts for the y slope
    QwMessage<<"YSlope LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fCombinedSlope[1].SetSingleEventCuts(minX,maxX);
  }else if (ch_name=="wsum"){//cuts for the 4 wire sum
    QwMessage<<"WSum LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fCombinedWSum.SetSingleEventCuts(minX,maxX);
  }
  
};




/********************************************************/


void  QwCombinedBPM::ProcessEvent() 
{
  Bool_t ldebug = kFALSE;
  Bool_t display_min_chi = kFALSE;
  static QwVQWK_Channel  tmpQADC("tmpQADC"), tmpADC("tmpADC");


  // check to see if there are correct number of elements to perform least squares fit on. 
  // For a linear fit the number of points should be greater than 2.
  // If not stop the process.
    
  if(fElement.size()<3)
    {
      std::cout<<" QwCombinedBPM:: Process event can't carry out the linear least square fit with only 2 points!"
	       <<" Abborting process.."<<std::endl;
      return;
    }
     
  //check to see if the fixed parameters are calculated 
  if(!fixedParamCalculated) 
    {
      if(ldebug) std::cout<<"Calculating fixed parameters..\n"; 
      CalculateFixedParameter(fXWeights,0); //for X
      CalculateFixedParameter(fYWeights,1); //for Y
      fixedParamCalculated = kTRUE;
    }

  for(size_t i=0;i<fElement.size();i++)
    {  
      if(ldebug){
	std::cout<<"*******************************\n";
	std::cout<<" QwCombinedBPM: Reading "<<fElement[i]->GetElementName()<<" with charge weight ="<<fQWeights[i]
		 <<" and  x weight ="<<fXWeights[i]
		 <<" and  y weight ="<<fYWeights[i]<<"\n";
	  
      }
	
      tmpQADC=fElement[i]->fWSum;
      tmpQADC.Scale(fQWeights[i]);
      fCombinedWSum+=tmpQADC;
	
	
      if(ldebug) {
	std::cout<<"got 4-wire.hw_sum = "<<fCombinedWSum.GetHardwareSum()<<" vs     actual "<<(fElement[i]-> fWSum).GetHardwareSum()<<std::endl;
	std::cout<<"copied absolute X position hw_sum from device "<<(fElement[i]-> fAbsPos[0]).GetHardwareSum()<<std::endl;
	std::cout<<"copied absolute Y position hw_sum from device "<<(fElement[i]-> fAbsPos[1]).GetHardwareSum()<<std::endl;
      }

    }
    
  fCombinedWSum.Scale(1.0/fSumQweights);

  //absolute beam position on tareget in z  == absolute target position in Z 
  fCombinedAbsPos[2].SetHardwareSum(fComboOffset[2]); 

  //Least squares fit for X
  LeastSquareFit(0, fXWeights );
    
  //Least squares fit for Y
  LeastSquareFit(1, fYWeights );    
    
    
  if(ldebug)
    {
      std::cout<<" QwCombinedBPM:: Projected target X position = "<<fCombinedAbsPos[0].GetHardwareSum()
	       <<" and target X slope = "<<fCombinedSlope[0].GetHardwareSum()
	       <<" \nProjected target Y position = "<<fCombinedAbsPos[1].GetHardwareSum()
	       <<" and target Y slope = "<<fCombinedSlope[1].GetHardwareSum()<<std::endl;  
    }
   
  if(display_min_chi) 
    {
      std::cout<<" QwCombinedBPM:: The minimul chi-square for the fit on X is  "<<chi_square[0]
	       <<" and for Y = "<<chi_square[1]<<std::endl;
      std::cout<<" For a good fit minimul-chisquare should be close to 1!"<<std::endl;
    }
    

 
  if (ldebug) 
    {
      fCombinedWSum.Print(); 
      for(Short_t n=0;n<3;n++) fCombinedAbsPos[n].Print();
      for(Short_t n=0;n<2;n++) fCombinedSlope[n].Print();
    }
    
  return;
 
};



void QwCombinedBPM::CalculateFixedParameter(std::vector<Double_t> fWeights, Int_t pos)
{

  Bool_t ldebug = kFALSE;
  static QwVQWK_Channel tmp("tmp");

 
  for(size_t i=0;i<fElement.size();i++) 
    {
      tmp = fElement[i]->fAbsPos[2];
      tmp.Scale(fWeights[i]);
      A[pos] += tmp.GetHardwareSum(); 
      B[pos] += fWeights[i];
      tmp.Product(tmp, fElement[i]->fAbsPos[2]); 
      D[pos] += tmp.GetHardwareSum();
    }
  
  m[pos]     = D[pos]*B[pos]-A[pos]*A[pos];
  erra[pos]  = B[pos]/m[pos];
  errb[pos]  = D[pos]/m[pos];
  covab[pos] = -A[pos]/m[pos];
  
  
  if(ldebug){
    std::cout<<" A = "<<A[pos]<<", B = "<<B[pos]<<", D = "<<D[pos]<<", m = "<<m[pos]<<std::endl;
    std::cout<<"From least square fit error are  "<<erra[pos]
	     <<"\ncovariance  = "<<covab[pos]<<"\n\n";
  }

  return;
};


Double_t QwCombinedBPM::SumOver(std::vector<Double_t> weight,std::vector <QwVQWK_Channel> val)
{
  Double_t sum = 0.0;
  if(weight.size()!=fElement.size()) 
    {
      std::cout
	<<"QwCombinedBPM:: Number of devices doesnt match the number of weights."
	<<" Exiting calculating parameters for the least squares fit"
	<<std::endl;
    }
  else
    {
      for(size_t i=0;i<weight.size();i++){
	val[i].Scale(weight[i]);
	sum+=val[i].GetHardwareSum();
      }
    }
  return sum;
};

void QwCombinedBPM::LeastSquareFit(Int_t n, std::vector<Double_t> fWeights) 
{
  Bool_t ldebug = kFALSE;
  static QwVQWK_Channel tmp1("tmp1");
  static QwVQWK_Channel tmp2("tmp2");

  C[n]=0; E[n]=0; F[n]=0;

  for(size_t i=0;i<fElement.size();i++)
    {
      tmp2.ClearEventData(); 
      tmp1 = fElement[i]->fAbsPos[n];
      tmp1.Scale(fWeights[i]);//xw
      C[n]+= tmp1.GetHardwareSum(); 
      tmp2.Product(tmp1,(fElement[i]-> fAbsPos[2])); //xzw
      E[n]+= tmp2.GetHardwareSum();
      tmp1.Product(tmp1,(fElement[i]-> fAbsPos[n])); //xxw
      F[n]+= tmp1.GetHardwareSum();

    }
  
  if(ldebug) std::cout<<"\nA ="<<A[n]<<" -- B ="<<B[n]<<" --C ="<<C[n]<<" --D ="<<D[n]<<" --E ="<<E[n]<<" --F ="<<F[n]<<"\n";  

  fCombinedSlope[n].SetHardwareSum(E[n]*B[n]-C[n]*A[n]);
  fCombinedSlope[n].Scale(1.0/m[n]);
  fIntersept[n]= (D[n]*C[n]-E[n]*A[n])/m[n];
  
  if(ldebug)    std::cout<<" Least Squares Fit Parameters for "<<n<<" are: \n slope = "<<fCombinedSlope[n].GetHardwareSum()
			 <<" \n intercept = "<<fIntersept[n]<<"\n\n";
  
  
  //absolute positions at target  using X = Za + b
  tmp1.ClearEventData();
  fCombinedAbsPos[n].SetHardwareSum(fIntersept[n]); // X =  b
  tmp1.Product(fCombinedAbsPos[2],fCombinedSlope[n]); //X = Za + b
  fCombinedAbsPos[n]+=tmp1;

  //to perform the minimul chi-square test
  tmp2.ClearEventData(); 
  chi_square[n]=0;
  for(size_t i=0;i<fElement.size();i++){
    tmp1.ClearEventData(); 
    tmp1.Difference(fElement[i]->fAbsPos[n],fCombinedAbsPos[n]); // = X-Za-b 
    tmp1.Product(tmp1,tmp1); // = (X-Za-b)^2
    tmp1.Scale(fWeights[i]); // = [(X-Za-b)^2]W
    tmp2+=tmp1; //sum over
  } 

  chi_square[n]=tmp2.GetHardwareSum()/(fElement.size()-2); //mimul ch-square 


  return;
}


/********************************************************/
void QwCombinedBPM::Print()
{

  for(Short_t i=0;i<3;i++) fCombinedAbsPos[i].Print();
  for(Short_t i=0;i<2;i++) fCombinedSlope[i].Print();
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
      for(Short_t i=0;i<2;i++)
	this->fCombinedSlope[i]=value.fCombinedSlope[i];
      for(Short_t i=0;i<3;i++)
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
      for(Short_t i=0;i<2;i++) 	this->fCombinedSlope[i]+=value.fCombinedSlope[i];
      for(Short_t i=0;i<3;i++) 	this->fCombinedAbsPos[i]+=value.fCombinedAbsPos[i];
    }
  return *this;
};

QwCombinedBPM& QwCombinedBPM::operator-= (const QwCombinedBPM &value)
{
  if (GetElementName()!="")
    {
      this->fCombinedWSum-=value.fCombinedWSum;
      for(Short_t i=0;i<2;i++) this->fCombinedSlope[i]-=value.fCombinedSlope[i];
      for(Short_t i=0;i<3;i++) this->fCombinedAbsPos[i]-=value.fCombinedAbsPos[i];
    }
  return *this;
};


void QwCombinedBPM::Sum(QwCombinedBPM &value1, QwCombinedBPM &value2)
{
  *this  = value1;
  *this += value2;

  return;
};

void QwCombinedBPM::Difference(QwCombinedBPM &value1, QwCombinedBPM &value2)
{
  *this  = value1;
  *this -= value2;
  return;
};

void QwCombinedBPM::Ratio(QwCombinedBPM &numer, QwCombinedBPM &denom)
{
  // this function is called when forming asymmetries. In this case waht we actually want for the
  // combined bpm is the difference only not the asymmetries
 
  *this=numer;
  this->fCombinedWSum.Ratio(numer.fCombinedWSum,denom.fCombinedWSum);
  if (GetElementName()!="")
    {
      for(Short_t i=0;i<2;i++)
 	this->fCombinedSlope[i].Ratio(numer.fCombinedSlope[i], denom.fCombinedSlope[i]);
      for(Short_t i=0;i<3;i++)
	this->fCombinedAbsPos[i].Ratio(numer.fCombinedAbsPos[i], denom.fCombinedAbsPos[i]);
    }

  return;
};


void QwCombinedBPM::Scale(Double_t factor)
{
  fCombinedWSum.Scale(factor);
  for(Short_t i=0;i<2;i++) fCombinedSlope[i].Scale(factor);
  for(Short_t i=0;i<3;i++) fCombinedAbsPos[i].Scale(factor);
  return;
}

void QwCombinedBPM::Calculate_Running_Average()
{
  for(Short_t i=0;i<2;i++) fCombinedSlope[i].Calculate_Running_Average();
  for(Short_t i=0;i<2;i++) fCombinedAbsPos[i].Calculate_Running_Average();
  fCombinedWSum.Calculate_Running_Average();
  return;
};

void QwCombinedBPM::Do_RunningSum()
{
  fCombinedWSum.Do_RunningSum();
  for(Short_t i=0;i<2;i++) fCombinedSlope[i].Do_RunningSum();
  for(Short_t i=0;i<3;i++) fCombinedAbsPos[i].Do_RunningSum();
  return;
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
     
      for(Short_t i=0;i<2;i++) fCombinedSlope[i].ConstructHistograms(folder, thisprefix);
      for(Short_t i=0;i<3;i++) fCombinedAbsPos[i].ConstructHistograms(folder, prefix);
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
      for(Short_t i=0;i<2;i++) fCombinedSlope[i].FillHistograms();
      for(Short_t i=0;i<3;i++) fCombinedAbsPos[i].FillHistograms();
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
      for(Short_t i=0;i<2;i++) fCombinedSlope[i].DeleteHistograms();
      for(Short_t i=0;i<3;i++) fCombinedAbsPos[i].DeleteHistograms();

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
      for(Short_t i=0;i<2;i++) fCombinedSlope[i].ConstructBranchAndVector(tree,thisprefix,values);
      for(Short_t i=0;i<3;i++) fCombinedAbsPos[i].ConstructBranchAndVector(tree,thisprefix,values);
	
    }
  return;
};

void  QwCombinedBPM::FillTreeVector(std::vector<Double_t> &values)
{
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip filling the tree.
    } 
  else  
    {
      fCombinedWSum.FillTreeVector(values);
      
      for(Short_t i=0;i<2;i++) fCombinedSlope[i].FillTreeVector(values);
      for(Short_t i=0;i<3;i++) fCombinedAbsPos[i].FillTreeVector(values);
      
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
	  for(Short_t i = 0; i < 3; i++)
	    this->fComboOffset[i] = input->fComboOffset[i];
	  for(Short_t i = 0; i < 2; i++)
	    this->fCombinedSlope[i].Copy(&(input->fCombinedSlope[i]));
	  for(Short_t i = 0; i < 3; i++)
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
};

void QwCombinedBPM::SetEventCutMode(Int_t bcuts)
{
  
  bEVENTCUTMODE=bcuts;
  for (Short_t i=0;i<2;i++) fCombinedSlope[i].SetEventCutMode(bcuts);
  for (Short_t i=0;i<3;i++) fCombinedAbsPos[i].SetEventCutMode(bcuts);
  fCombinedWSum.SetEventCutMode(bcuts);
  return;
};


