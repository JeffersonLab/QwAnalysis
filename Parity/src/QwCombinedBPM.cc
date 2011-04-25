/**********************************************************\
 * File: QwCombinedBPM.cc                                  *
 *                                                         *
 * Author: B. Waidyawansa                                  *
 * Time-stamp:                                             *
\**********************************************************/

#include "QwCombinedBPM.h"

// System headers
#include <stdexcept>

// Qweak headers
#include "QwDBInterface.h"


void  QwCombinedBPM::InitializeChannel(TString name)
{

  VQwBPM::InitializeChannel(name);

  fEffectiveCharge.InitializeChannel(name+"_EffectiveCharge","derived");

  for( Short_t i=0;i<2;i++){
    fAbsPos[i].InitializeChannel(name+axis[i],"derived");
    fSlope[i].InitializeChannel(name+axis[i]+"Slope","derived");
    fIntercept[i].InitializeChannel(name+axis[i]+"Intercept","derived");
  }

  fixedParamCalculated = false;

  fElement.clear();
  fQWeights.clear();
  fXWeights.clear();
  fYWeights.clear();

  fSumQweights = 0.0;

  for(Short_t i=0;i<2;i++){
    erra[i]       = 0.0;
    errb[i]       = 0.0;
    covab[i]      = 0.0;
    A[i]          = 0.0;
    B[i]          = 0.0;
    D[i]          = 0.0;
    m[i]          = 0.0;
    chi_square[i] = 0.0;
  }

  return;
}

void  QwCombinedBPM::InitializeChannel(TString subsystem, TString name)
{

  VQwBPM::InitializeChannel(name);

  fEffectiveCharge.InitializeChannel(subsystem, "QwCombinedBPM", name+"_EffectiveCharge","derived");

  for( Short_t i=0;i<2;i++){
    fAbsPos[i].InitializeChannel(subsystem, "QwCombinedBPM", name+axis[i],"derived");
    fSlope[i].InitializeChannel(subsystem, "QwCombinedBPM", name+axis[i]+"Slope","derived");
    fIntercept[i].InitializeChannel(subsystem, "QwCombinedBPM", name+axis[i]+"Intercept","derived");
  }

  fixedParamCalculated = false;

  fElement.clear();
  fQWeights.clear();
  fXWeights.clear();
  fYWeights.clear();

  fSumQweights = 0.0;

  for(Short_t i=0;i<2;i++){
    erra[i]       = 0.0;
    errb[i]       = 0.0;
    covab[i]      = 0.0;
    A[i]          = 0.0;
    B[i]          = 0.0;
    D[i]          = 0.0;
    m[i]          = 0.0;
    chi_square[i] = 0.0;
  }

  return;
}


void QwCombinedBPM::ClearEventData()
{
  fEffectiveCharge.ClearEventData();

  for(Short_t i=0;i<2;i++){
    fAbsPos[i].ClearEventData();
    fSlope[i].ClearEventData();
    fIntercept[i].ClearEventData();
  }

  return;
}


void QwCombinedBPM::Set(QwBPMStripline* bpm, Double_t charge_weight,  Double_t x_weight, Double_t y_weight,
			Double_t sumqw)
{
  fElement.push_back(bpm);
  fQWeights.push_back(charge_weight);
  fXWeights.push_back(x_weight);
  fYWeights.push_back(y_weight);
  fSumQweights=sumqw;

  return;

}


Bool_t QwCombinedBPM::ApplyHWChecks()
{
  Bool_t fEventIsGood=kTRUE;

  return fEventIsGood;
}


Int_t QwCombinedBPM::GetEventcutErrorCounters()
{
  for(Short_t i=0;i<2;i++){
    fAbsPos[i].GetEventcutErrorCounters();
    fSlope[i].GetEventcutErrorCounters();
    fIntercept[i].GetEventcutErrorCounters();
  }

  fEffectiveCharge.GetEventcutErrorCounters();

  return 1;
}


Bool_t QwCombinedBPM::ApplySingleEventCuts()
{
  Bool_t status=kTRUE;
  Int_t i=0;
  fErrorFlag=0;
  UInt_t charge_error;
  UInt_t pos_error[2];
  charge_error = 0;
  pos_error[0]=0;
  pos_error[1]=0;
  for(size_t i=0;i<fElement.size();i++){
    charge_error |= fElement[i]->fEffectiveCharge.GetErrorCode();
    pos_error[0] |= fElement[i]->fAbsPos[0].GetErrorCode();
    pos_error[1] |= fElement[i]->fAbsPos[1].GetErrorCode();
  }

  //Event cuts for  X & Y slopes
  for(i=0;i<2;i++){
    fSlope[i].UpdateErrorCode(pos_error[i]);
    if (fSlope[i].ApplySingleEventCuts()){ //for X slope
      status&=kTRUE;
    }
    else{
      status&=kFALSE;
      if (bDEBUG) std::cout<<" X Slope event cut failed ";
    }
    //Get the Event cut error flag for SlopeX/Y
    fErrorFlag|=fSlope[i].GetEventcutErrorFlag();
  }

  //Event cuts for  X & Y intercepts
  for(i=0;i<2;i++){
    fIntercept[i].UpdateErrorCode(pos_error[i]);
    if (fIntercept[i].ApplySingleEventCuts()){ //for X slope
      status&=kTRUE;
    }
    else{
      status&=kFALSE;
      if (bDEBUG) std::cout<<" X Intercept event cut failed ";
    }
    //Get the Event cut error flag for SlopeX/Y
    fErrorFlag|=fIntercept[i].GetEventcutErrorFlag();
  }

  for(i=0;i<2;i++){
    fAbsPos[i].UpdateErrorCode(pos_error[i]);
    if (fAbsPos[i].ApplySingleEventCuts()){ 
      status&=kTRUE;
    }
    else{
      status&=kFALSE;
      if (bDEBUG) std::cout<<" Abs X event cut failed ";
    }
    //Get the Event cut error flag for AbsX/Y
    fErrorFlag|=fAbsPos[i].GetEventcutErrorFlag();
    //if (fAbsPos[i].GetElementName()=="qwk_targetX")
    //std::cout<<" Abs X event cut "<<fAbsPos[i].GetEventcutErrorFlag()<<std::endl;
  }

  //Event cuts for four wire sum (EffectiveCharge)
  fEffectiveCharge.UpdateErrorCode(charge_error);
  if (fEffectiveCharge.ApplySingleEventCuts()){
      status&=kTRUE;
  }
  else{
    status&=kFALSE;
    if (bDEBUG) std::cout<<"EffectiveCharge event cut failed ";
  }
  //Get the Event cut error flag for EffectiveCharge
  fErrorFlag|=fEffectiveCharge.GetEventcutErrorFlag();

  return status;
}



void QwCombinedBPM::SetSingleEventCuts(TString ch_name, Double_t minX, Double_t maxX)
{

  if (ch_name=="xslope"){//cuts for the x slope
    QwMessage<<"XSlope LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fSlope[0].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="yslope"){//cuts for the y slope
    QwMessage<<"YSlope LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fSlope[1].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="xintercept"){//cuts for the x intercept
    QwMessage<<"XIntercept LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fIntercept[0].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="yintercept"){//cuts for the y intercept
    QwMessage<<"YIntercept LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fIntercept[1].SetSingleEventCuts(minX,maxX);

  } else  if (ch_name=="absx"){
  //cuts for the absolute x and y
    QwMessage<<"AbsX LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fAbsPos[0].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="absy"){
    QwMessage<<"AbsY LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fAbsPos[1].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="effectivecharge"){ //cuts for the effective charge
    QwMessage<<"EffectveQ LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fEffectiveCharge.SetSingleEventCuts(minX,maxX);
  }
}

void QwCombinedBPM::SetSingleEventCuts(TString ch_name, UInt_t errorflag,Double_t minX, Double_t maxX, Double_t stability){
  errorflag|=kBPMErrorFlag;//update the device flag
  if (ch_name=="xslope"){//cuts for the x slope
    QwMessage<<"XSlope LL " <<  minX <<" UL " << maxX << " kGlobalCut  "<< (errorflag&kGlobalCut)<< QwLog::endl;
    fSlope[0].SetSingleEventCuts(errorflag, minX,maxX, stability);

  }else if (ch_name=="yslope"){//cuts for the y slope
    QwMessage<<"YSlope LL " <<  minX <<" UL " << maxX <<" kGlobalCut  "<< (errorflag&kGlobalCut)<< QwLog::endl;
    fSlope[1].SetSingleEventCuts(errorflag, minX,maxX, stability);

  }else if (ch_name=="xintercept"){//cuts for the x intercept
    QwMessage<<"XIntercept LL " <<  minX <<" UL " << maxX <<" kGlobalCut  "<< (errorflag&kGlobalCut)<< QwLog::endl;
    fIntercept[0].SetSingleEventCuts(errorflag, minX,maxX, stability);

  }else if (ch_name=="yintercept"){//cuts for the y intercept
    QwMessage<<"YIntercept LL " <<  minX <<" UL " << maxX <<" kGlobalCut  "<< (errorflag&kGlobalCut)<< QwLog::endl;
    fIntercept[1].SetSingleEventCuts(errorflag, minX,maxX, stability);

  } else  if (ch_name=="absx"){
  //cuts for the absolute x and y
    QwMessage<<"AbsX LL " <<  minX <<" UL " << maxX <<" kGlobalCut  "<< (errorflag&kGlobalCut)<< QwLog::endl;
    fIntercept[0].SetSingleEventCuts(errorflag, 0,0,0);
    fAbsPos[0].SetSingleEventCuts(errorflag, minX,maxX, stability);

  }else if (ch_name=="absy"){
    QwMessage<<"AbsY LL " <<  minX <<" UL " << maxX <<" kGlobalCut  "<< (errorflag&kGlobalCut)<< QwLog::endl;
    fIntercept[1].SetSingleEventCuts(errorflag, 0,0,0);
    fAbsPos[1].SetSingleEventCuts(errorflag, minX,maxX, stability);

  }else if (ch_name=="effectivecharge"){ //cuts for the effective charge
    QwMessage<<"EffectveQ LL " <<  minX <<" UL " << maxX <<" kGlobalCut  "<< (errorflag&kGlobalCut)<< QwLog::endl;
    fEffectiveCharge.SetSingleEventCuts(errorflag, minX,maxX, stability);
  }
}



void  QwCombinedBPM::ProcessEvent()
{
  Bool_t ldebug = kFALSE;


  static QwVQWK_Channel  tmpQADC("tmpQADC"), tmpADC("tmpADC");


  //check to see if the fixed parameters are calculated
  if(!fixedParamCalculated){
    if(ldebug) std::cout<<"QwCombinedBPM:Calculating fixed parameters..\n";
    CalculateFixedParameter(fXWeights,0); //for X
    CalculateFixedParameter(fYWeights,1); //for Y
    fixedParamCalculated = kTRUE;
  }

  for(size_t i=0;i<fElement.size();i++){
    if(ldebug){
      std::cout<<"*******************************\n";
      std::cout<<" QwCombinedBPM: Reading "<<fElement[i]->GetElementName()<<" with charge weight ="<<fQWeights[i]
	       <<" and  x weight ="<<fXWeights[i]
	       <<" and  y weight ="<<fYWeights[i]<<"\n";

    }

    tmpQADC=fElement[i]->fEffectiveCharge;
    tmpQADC.Scale(fQWeights[i]);
    fEffectiveCharge+=tmpQADC;


    if(ldebug) {
      std::cout<<"got 4-wire.hw_sum = "<<fEffectiveCharge.GetHardwareSum()<<" vs     actual "<<(fElement[i]-> fEffectiveCharge).GetHardwareSum()<<std::endl;
      std::cout<<"copied absolute X position hw_sum from device "<<(fElement[i]-> fAbsPos[0]).GetHardwareSum()<<std::endl;
      std::cout<<"copied absolute Y position hw_sum from device "<<(fElement[i]-> fAbsPos[1]).GetHardwareSum()<<std::endl;
    }

  }

  fEffectiveCharge.Scale(1.0/fSumQweights);
  //fAbsPos[0].ResetErrorFlag(0x4000000);
  //Least squares fit for X
  LeastSquareFit(0, fXWeights );

  //Least squares fit for Y
  LeastSquareFit(1, fYWeights );


  if(ldebug){
    std::cout<<" QwCombinedBPM:: Projected target X position = "<<fAbsPos[0].GetHardwareSum()
	     <<" and target X slope = "<<fSlope[0].GetHardwareSum()
	     <<" and target X intercept = "<<fIntercept[0].GetHardwareSum()
	     <<" \nProjected target Y position = "<<fAbsPos[1].GetHardwareSum()
	     <<" and target Y slope = "<<fSlope[1].GetHardwareSum()
	     <<" and target Y intercept = "<<fIntercept[1].GetHardwareSum()<<std::endl;
  }

  if(ldebug){
    std::cout<<" QwCombinedBPM:: The minimul chi-square for the fit on X is  "<<chi_square[0]
	     <<" and for Y = "<<chi_square[1]<<std::endl;
    std::cout<<" For a good fit minimul-chisquare should be close to 1!"<<std::endl;
  }


  if (ldebug) {
    fEffectiveCharge.PrintInfo();
    for(Short_t n=0;n<2;n++) {
      fAbsPos[n].PrintInfo();
      fSlope[n].PrintInfo();
      fIntercept[1].PrintInfo();
    }
  }

  return;

 }



 void QwCombinedBPM::CalculateFixedParameter(std::vector<Double_t> fWeights, Int_t pos)
 {

   Bool_t ldebug = kFALSE;
   static Double_t zpos = 0.0;

   for(size_t i=0;i<fElement.size();i++){
     zpos = fElement[i]->fPositionCenter[2];
     A[pos] += zpos*fWeights[i]; //zw
     B[pos] += fWeights[i]; //w
     D[pos] += zpos*zpos*fWeights[i]; //z^2w
   }

   m[pos]     = D[pos]*B[pos]-A[pos]*A[pos];
   erra[pos]  = B[pos]/m[pos];
   errb[pos]  = D[pos]/m[pos];
   covab[pos] = -A[pos]/m[pos];

  // Divvy
  if (m[pos] == 0)
    QwWarning << "Angry Divvy: Division by zero in " << GetElementName() << QwLog::endl;

   if(ldebug){
     std::cout<<" A = "<<A[pos]<<", B = "<<B[pos]<<", D = "<<D[pos]<<", m = "<<m[pos]<<std::endl;
     std::cout<<"For least square fit, errors are  "<<erra[pos]
	      <<"\ncovariance  = "<<covab[pos]<<"\n\n";
   }

   return;
 }


 Double_t QwCombinedBPM::SumOver(std::vector<Double_t> weight,std::vector <QwVQWK_Channel> val)
 {
   Double_t sum = 0.0;
   if(weight.size()!=fElement.size()){
     std::cout
       <<"QwCombinedBPM:: Number of devices doesnt match the number of weights."
       <<" Exiting calculating parameters for the least squares fit"
       <<std::endl;
   }
   else{
     for(size_t i=0;i<weight.size();i++){
       val[i].Scale(weight[i]);
       sum+=val[i].GetHardwareSum();
     }
   }
   return sum;
 }

 void QwCombinedBPM::LeastSquareFit(Int_t n, std::vector<Double_t> fWeights)
 {

   /**
      REF : W.R Leo

      For Y = aX +b

      A = sigma(X * Wy)     B = sigma(Wy)    C = sigma(Y*Wy)    D = sigma(X *X * Wy)     E = sigma(X*Y*Wy)   F = sigma(Y * Y *Wy)

      then
      a = (EB-CA)/(DB-AA)      b =(DC-EA)/(DB-AA)   
   **/


   Bool_t ldebug = kFALSE;
   static Double_t zpos = 0;
   static QwVQWK_Channel tmp1;
   static QwVQWK_Channel tmp2;
   static QwVQWK_Channel C[2];
   static QwVQWK_Channel E[2];

   // initialize the VQWK_Channel arrays
   tmp1.InitializeChannel("tmp1","derived");
   tmp2.InitializeChannel("tmp2","derived");
   C[0].InitializeChannel("cx","derived");
   C[1].InitializeChannel("cy","derived");
   E[0].InitializeChannel("ex","derived");
   E[1].InitializeChannel("ey","derived");

   for(size_t i=0;i<fElement.size();i++){
     tmp1.ClearEventData();
     tmp2.ClearEventData();
     tmp1 = fElement[i]->fAbsPos[n];
     zpos = fElement[i]->fPositionCenter[2];
     tmp1.Scale(fWeights[i]);
     C[n]+= tmp1; //xw or yw
     tmp1.Scale(zpos);//xzw or yzw
     E[n]+= tmp1;
   }

   if(ldebug) std::cout<<"\n A ="<<A[n]
		       <<" -- B ="<<B[n]
		       <<" --C ="<<C[n].GetHardwareSum()
		       <<" --D ="<<D[n]
		       <<" --E ="<<E[n].GetHardwareSum()<<"\n";

   // calculate the slope  a = E*erra + C*covab
   tmp1.ClearEventData();
   tmp1 = E[n];
   tmp1.Scale(erra[n]);
   tmp2.ClearEventData();
   tmp2 = C[n];
   tmp2.Scale(covab[n]);
   fSlope[n]+= tmp1;
   fSlope[n]+= tmp2;

   // calculate the intercept  b = C*errb + E*covab
   tmp1.ClearEventData();
   tmp1 = C[n];
   tmp1.Scale(errb[n]);
   tmp2.ClearEventData();
   tmp2 = E[n];
   tmp2.Scale(covab[n]);
   fIntercept[n]+= tmp1;
   fIntercept[n]+= tmp2;

   if(ldebug)    std::cout<<" Least Squares Fit Parameters for "<<n
			  <<" are: \n slope = "<< fSlope[n].GetHardwareSum()
			  <<" \n intercept = " << fIntercept[n].GetHardwareSum()<<"\n\n";


   // absolute positions at target  using X = Za + b
   tmp1.ClearEventData();
   // Absolute position of the combined bpm is not a physical position but a derived one.


   zpos = this->fPositionCenter[2];
   //UInt_t err_flag=fAbsPos[n].GetEventcutErrorFlag();   
   fAbsPos[n]= fIntercept[n]; // X =  b
   //fAbsPos[n].ResetErrorFlag(err_flag);
   tmp1 = fSlope[n];
   tmp1.Scale(zpos); //az
   fAbsPos[n]+=tmp1;  //X = az+b

   // to perform the minimul chi-square test
   tmp2.ClearEventData();
   chi_square[n]=0;
   for(size_t i=0;i<fElement.size();i++){
     tmp1.ClearEventData();
     tmp1.Difference(fElement[i]->fAbsPos[n],fAbsPos[n]); // = X-Za-b
     tmp1.Product(tmp1,tmp1); // = (X-Za-b)^2
     tmp1.Scale(fWeights[i]); // = [(X-Za-b)^2]W
     tmp2+=tmp1; //sum over
   }

   chi_square[n]=tmp2.GetHardwareSum()/(fElement.size()-2); //minimul chi-square


   return;
 }


 Int_t QwCombinedBPM::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer,UInt_t index)
 {
   return word_position_in_buffer;
 }



 void QwCombinedBPM::PrintValue() const
 {
   Short_t i;

   for(i = 0; i < 3; i++) fAbsPos[i].PrintValue();
   for(i = 0; i < 2; i++) {
     fSlope[i].PrintValue();
     fIntercept[i].PrintValue();
   }
   fEffectiveCharge.PrintValue();

   return;
 }


 void QwCombinedBPM::PrintInfo() const
 {

   Short_t i;

   for(i = 0; i < 3; i++) fAbsPos[i].PrintInfo();
  for(i = 0; i < 2; i++) {
    fSlope[i].PrintInfo();
    fIntercept[i].PrintInfo();
  }
  fEffectiveCharge.PrintInfo();

  return;
}

QwCombinedBPM& QwCombinedBPM::operator= (const QwCombinedBPM &value)
{
  VQwBPM::operator= (value);
  if (GetElementName()!=""){
    this->fEffectiveCharge=value.fEffectiveCharge;
    for(Short_t i=0;i<2;i++){
      this->fSlope[i]=value.fSlope[i];
      this->fIntercept[i] = value.fIntercept[i];
      this->fAbsPos[i]=value.fAbsPos[i];
    }
  }
  return *this;
}


QwCombinedBPM& QwCombinedBPM::operator+= (const QwCombinedBPM &value)
{

     if (GetElementName()!=""){
       this->fEffectiveCharge+=value.fEffectiveCharge;
       for(Short_t i=0;i<2;i++) 	{
	 this->fSlope[i]+=value.fSlope[i];
	 this->fIntercept[i]+=value.fIntercept[i];
	 this->fAbsPos[i]+=value.fAbsPos[i];

       }
     }
     return *this;
}

QwCombinedBPM& QwCombinedBPM::operator-= (const QwCombinedBPM &value)
{

  if (GetElementName()!=""){
    this->fEffectiveCharge-=value.fEffectiveCharge;
    for(Short_t i=0;i<2;i++){
      this->fSlope[i]-=value.fSlope[i];
      this->fIntercept[i]-=value.fIntercept[i];
      this->fAbsPos[i]-=value.fAbsPos[i];
    }
  }
  return *this;
}

void QwCombinedBPM::Ratio(QwCombinedBPM &numer, QwCombinedBPM &denom)
{
  // this function is called when forming asymmetries. In this case waht we actually want for the
  // combined bpm is the difference only not the asymmetries

  *this=numer;
  this->fEffectiveCharge.Ratio(numer.fEffectiveCharge,denom.fEffectiveCharge);
  if (GetElementName()!=""){
    //    The slope, intercept and absolute positions should all be differences, not asymmetries.
    for(Short_t i=0;i<2;i++) {
      this->fSlope[i]     = numer.fSlope[i];
      this->fIntercept[i] = numer.fIntercept[i];
      this->fAbsPos[i]    = numer.fAbsPos[i];
    }
  }

  return;
}


void QwCombinedBPM::Scale(Double_t factor)
{
  fEffectiveCharge.Scale(factor);
  for(Short_t i=0;i<2;i++){
    fSlope[i].Scale(factor);
    fIntercept[i].Scale(factor);
    fAbsPos[i].Scale(factor);
  }
  return;
}

void QwCombinedBPM::CalculateRunningAverage()
{
  fEffectiveCharge.CalculateRunningAverage();

  for (Short_t i = 0; i < 2; i++) {
    fSlope[i].CalculateRunningAverage();
    fIntercept[i].CalculateRunningAverage();
    fAbsPos[i].CalculateRunningAverage();
  }
}

void QwCombinedBPM::AccumulateRunningSum(const QwCombinedBPM& value)
{
  fEffectiveCharge.AccumulateRunningSum(value.fEffectiveCharge);

  // TODO This is unsafe (see QwBeamline::AccumulateRunningSum)
  for (Short_t i = 0; i < 2; i++){
    fSlope[i].AccumulateRunningSum(value.fSlope[i]);
    fIntercept[i].AccumulateRunningSum(value.fIntercept[i]);
    fAbsPos[i].AccumulateRunningSum(value.fAbsPos[i]);
  }
}


void  QwCombinedBPM::ConstructHistograms(TDirectory *folder, TString &prefix)
{

  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else{
    //we calculate the asym_ for the fEffectiveCharge becasue its an asymmetry and not a difference.
    fEffectiveCharge.ConstructHistograms(folder, prefix);
    TString thisprefix=prefix;
    if(prefix=="asym_")
      thisprefix="diff_";
    SetRootSaveStatus(prefix);

    for(Short_t i=0;i<2;i++) {
	fSlope[i].ConstructHistograms(folder, thisprefix);
	fIntercept[i].ConstructHistograms(folder, thisprefix);
	fAbsPos[i].ConstructHistograms(folder, thisprefix);
    }

  }
  return;
}

void  QwCombinedBPM::FillHistograms()
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else{
    fEffectiveCharge.FillHistograms();
    for(Short_t i=0;i<2;i++) {
      fSlope[i].FillHistograms();
      fIntercept[i].FillHistograms();
      fAbsPos[i].FillHistograms();
    }
  }
  return;
}

void  QwCombinedBPM::DeleteHistograms()
{
  if (GetElementName()=="") {
    //  This channel is not used, so skip filling the histograms.
  }
  else{
    fEffectiveCharge.DeleteHistograms();
    for(Short_t i=0;i<2;i++) {
      fSlope[i].DeleteHistograms();
      fIntercept[i].DeleteHistograms();
      fAbsPos[i].DeleteHistograms();
    }

  }
  return;
}


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

      fEffectiveCharge.ConstructBranchAndVector(tree,prefix,values);
      for(Short_t i=0;i<2;i++){
	fSlope[i].ConstructBranchAndVector(tree,thisprefix,values);
	fIntercept[i].ConstructBranchAndVector(tree,thisprefix,values);
	fAbsPos[i].ConstructBranchAndVector(tree,thisprefix,values);
      }

    }

  return;
}

void  QwCombinedBPM::ConstructBranch(TTree *tree, TString &prefix)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip constructing trees.
  } else
    {
      TString thisprefix=prefix;
      if(prefix=="asym_")
	thisprefix="diff_";


      fEffectiveCharge.ConstructBranch(tree,prefix);

      for(Short_t i=0;i<2;i++){
	fSlope[i].ConstructBranch(tree,thisprefix);
	fIntercept[i].ConstructBranch(tree,thisprefix);
	fAbsPos[i].ConstructBranch(tree,thisprefix);
      }

    }
  return;
}

void  QwCombinedBPM::ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist)
{
  TString devicename;
  devicename=GetElementName();
  devicename.ToLower();

  if (GetElementName()==""){
    //  This channel is not used, so skip constructing trees.
  } else
    {
      if (modulelist.HasValue(devicename)){
	TString thisprefix=prefix;
	if(prefix=="asym_")
	  thisprefix="diff_";


	fEffectiveCharge.ConstructBranch(tree,prefix);

	for(Short_t i=0;i<2;i++){
	  fSlope[i].ConstructBranch(tree,thisprefix);
	  fIntercept[i].ConstructBranch(tree,thisprefix);
	  fAbsPos[i].ConstructBranch(tree,thisprefix);
	}
	QwMessage <<" Tree leave added to "<<devicename<<QwLog::endl;
    }

    }
  return;
}


void  QwCombinedBPM::FillTreeVector(std::vector<Double_t> &values) const
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the tree.
  }
  else{
    fEffectiveCharge.FillTreeVector(values);

    for(Short_t i=0;i<2;i++){
      fSlope[i].FillTreeVector(values);
      fIntercept[i].FillTreeVector(values);
      fAbsPos[i].FillTreeVector(values);
    }
  }
  return;
}


void QwCombinedBPM::Copy(VQwDataElement *source)
{
  try
    {
      if(typeid(*source)==typeid(*this))
	{
	  QwCombinedBPM* input = ((QwCombinedBPM*)source);
	  this->fElementName = input->fElementName;
	  this->fEffectiveCharge.Copy(&(input->fEffectiveCharge));
	  this->bFullSave = input->bFullSave;
	  for(Short_t i = 0; i < 3; i++)
	    this->fPositionCenter[i] = input->fPositionCenter[i];
	  for(Short_t i = 0; i < 2; i++){
	    this->fSlope[i].Copy(&(input->fSlope[i]));
	    this->fIntercept[i].Copy(&(input->fIntercept[i]));
	    this->fAbsPos[i].Copy(&(input->fAbsPos[i]));
	  }
	}
      else
	{
	  TString loc="Standard exception from QwCombinedBPM::Copy = "
	    +source->GetElementName()+" "
	    +this->GetElementName()+" are not of the same type";
	  throw std::invalid_argument(loc.Data());
	}
    }

  catch (std::exception& e){
    std::cerr << e.what() << std::endl;
  }

  return;
}

void QwCombinedBPM::SetEventCutMode(Int_t bcuts)
{

  bEVENTCUTMODE=bcuts;
  for (Short_t i=0;i<2;i++){
    fSlope[i].SetEventCutMode(bcuts);
    fIntercept[i].SetEventCutMode(bcuts);
    fAbsPos[i].SetEventCutMode(bcuts);
  }
  fEffectiveCharge.SetEventCutMode(bcuts);

  return;
}


void QwCombinedBPM::MakeBPMComboList()
{
  UShort_t i = 0;

  QwVQWK_Channel combo_bpm_sub_element;
  
  for(i=0;i<2;i++) {
    combo_bpm_sub_element.ClearEventData();
    combo_bpm_sub_element.Copy(&fAbsPos[i]);
    combo_bpm_sub_element = fAbsPos[i];
    fBPMComboElementList.push_back( combo_bpm_sub_element );
    combo_bpm_sub_element.Copy(&fSlope[i]);
    combo_bpm_sub_element = fSlope[i];
    fBPMComboElementList.push_back( combo_bpm_sub_element );
    combo_bpm_sub_element.Copy(&fIntercept[i]);
    combo_bpm_sub_element = fIntercept[i];
    fBPMComboElementList.push_back( combo_bpm_sub_element );
  }
  combo_bpm_sub_element.Copy(&fEffectiveCharge);
  combo_bpm_sub_element = fEffectiveCharge;
  fBPMComboElementList.push_back( combo_bpm_sub_element );


  return;
}



std::vector<QwDBInterface> QwCombinedBPM::GetDBEntry()
{
  UShort_t i = 0;
  UShort_t n_element = 0;

  std::vector <QwDBInterface> row_list;
  row_list.clear();

  QwDBInterface row;

  TString name;
  Double_t avg         = 0.0;
  Double_t err         = 0.0;
  UInt_t beam_subblock = 0;
  UInt_t beam_n        = 0;


  for(n_element=0; n_element<fBPMComboElementList.size(); n_element++) {

    row.Reset();
    // the element name and the n (number of measurements in average)
    // is the same in each block and hardwaresum.

    name          = fBPMComboElementList.at(n_element).GetElementName();
    beam_n        = fBPMComboElementList.at(n_element).GetGoodEventCount();

    // Get HardwareSum average and its error
    avg           = fBPMComboElementList.at(n_element).GetHardwareSum();
    err           = fBPMComboElementList.at(n_element).GetHardwareSumError();
    // ADC subblock sum : 0 in MySQL database
    beam_subblock = 0;

    row.SetDetectorName(name);
    row.SetSubblock(beam_subblock);
    row.SetN(beam_n);
    row.SetValue(avg);
    row.SetError(err);

    row_list.push_back(row);

    // Get four Block averages and thier errors

    for(i=0; i<4; i++) {
      row.Reset();
      avg           = fBPMComboElementList.at(n_element).GetBlockValue(i);
      err           = fBPMComboElementList.at(n_element).GetBlockErrorValue(i);
      beam_subblock = (UInt_t) (i+1);
      // QwVQWK_Channel  | MySQL
      // fBlock[0]       | subblock 1
      // fBlock[1]       | subblock 2
      // fBlock[2]       | subblock 3
      // fBlock[3]       | subblock 4
      row.SetDetectorName(name);
      row.SetSubblock(beam_subblock);
      row.SetN(beam_n);
      row.SetValue(avg);
      row.SetError(err);

      row_list.push_back(row);
    }
  }

  return row_list;

}
