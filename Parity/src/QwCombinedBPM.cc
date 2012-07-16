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
#include "QwVQWK_Channel.h"
#include "QwScaler_Channel.h"
#include "QwParameterFile.h"


template<typename T>
void  QwCombinedBPM<T>::InitializeChannel(TString name)
{

  VQwBPM::InitializeChannel(name);

  fEffectiveCharge.InitializeChannel(name+"_EffectiveCharge","derived");

  for( Short_t axis=kXAxis;axis<kNumAxes;axis++){
    fAbsPos[axis].InitializeChannel(name+kAxisLabel[axis],"derived");
    fSlope[axis].InitializeChannel(name+kAxisLabel[axis]+"Slope","derived");
    fIntercept[axis].InitializeChannel(name+kAxisLabel[axis]+"Intercept","derived");
    fMinimumChiSquare[axis].InitializeChannel(name+kAxisLabel[axis]+"MinChiSquare","derived");
  }

  fixedParamCalculated = false;

  fElement.clear();
  fQWeights.clear();
  fXWeights.clear();
  fYWeights.clear();

  fSumQweights = 0.0;

  for(Short_t axis=kXAxis;axis<kNumAxes;axis++){
    erra[axis]       = 0.0;
    errb[axis]       = 0.0;
    covab[axis]      = 0.0;
    A[axis]          = 0.0;
    B[axis]          = 0.0;
    D[axis]          = 0.0;
    m[axis]          = 0.0;
  }

  return;
}

template<typename T>
void  QwCombinedBPM<T>::InitializeChannel(TString subsystem, TString name)
{

  VQwBPM::InitializeChannel(name);

  fEffectiveCharge.InitializeChannel(subsystem, "QwCombinedBPM", name+"_EffectiveCharge","derived");

  for( Short_t axis=kXAxis;axis<kNumAxes;axis++){
    fAbsPos[axis].InitializeChannel(subsystem, "QwCombinedBPM", name+kAxisLabel[axis],"derived");
    fSlope[axis].InitializeChannel(subsystem, "QwCombinedBPM", name+kAxisLabel[axis]+"Slope","derived");
    fIntercept[axis].InitializeChannel(subsystem, "QwCombinedBPM", name+kAxisLabel[axis]+"Intercept","derived");
    fMinimumChiSquare[axis].InitializeChannel(subsystem, "QwCombinedBPM",name+kAxisLabel[axis]+"MinChiSquare","derived");
  }
  
  fixedParamCalculated = false;

  fElement.clear();
  fQWeights.clear();
  fXWeights.clear();
  fYWeights.clear();

  fSumQweights = 0.0;

  for(Short_t axis=kXAxis;axis<kNumAxes;axis++){
    erra[axis]       = 0.0;
    errb[axis]       = 0.0;
    covab[axis]      = 0.0;
    A[axis]          = 0.0;
    B[axis]          = 0.0;
    D[axis]          = 0.0;
    m[axis]          = 0.0;
  }

  return;
}


template<typename T>
void QwCombinedBPM<T>::ClearEventData()
{
  fEffectiveCharge.ClearEventData();

  for(Short_t axis=kXAxis;axis<kNumAxes;axis++){
    fAbsPos[axis].ClearEventData();
    fSlope[axis].ClearEventData();
    fIntercept[axis].ClearEventData();
  }

  return;
}


template<typename T>
void QwCombinedBPM<T>::SetBPMForCombo(const VQwBPM* bpm, Double_t charge_weight,  Double_t x_weight, Double_t y_weight,
			Double_t sumqw)
{
  fElement.push_back(bpm);
  fQWeights.push_back(charge_weight);
  fXWeights.push_back(x_weight);
  fYWeights.push_back(y_weight);
  fSumQweights=sumqw;

  size_t i = fElement.size();
  if (i>=1){
    i--;
    //    std::cout << "+++++++++++++++++++++++++++\n+++++++++++++++++++++++++++\n" << std::endl;
    //     std::cout << "fElement.size()==" << fElement.size() << " " << i << " "
    // 	      << fElement.at(i)->GetElementName() << " "
    // 	      << fQWeights.at(i) << " "
    // 	      << fXWeights.at(i) << " "
    // 	      << fYWeights.at(i) << " "
    // 	      << "fElement.at(i)->GetEffectiveCharge()==" << fElement.at(i)->GetEffectiveCharge() << " "
    // 	      << std::endl;
    //     fElement.at(i)->GetEffectiveCharge()->PrintInfo();
    //     fElement.at(i)->PrintInfo();
  }
  return;

}


template<typename T>
Bool_t QwCombinedBPM<T>::ApplyHWChecks()
{
  Bool_t eventokay=kTRUE;

  return eventokay;
}


template<typename T>
void QwCombinedBPM<T>::PrintErrorCounters() const
{
  for(Short_t axis=kXAxis;axis<kNumAxes;axis++){
    fAbsPos[axis].PrintErrorCounters();
    fSlope[axis].PrintErrorCounters();
    fIntercept[axis].PrintErrorCounters();
    fMinimumChiSquare[axis].PrintErrorCounters();
  }

  fEffectiveCharge.PrintErrorCounters();
}

template<typename T>
UInt_t QwCombinedBPM<T>::GetEventcutErrorFlag()
{
  UInt_t error=0;
  for(Short_t axis=kXAxis;axis<kNumAxes;axis++){
    error|=fAbsPos[axis].GetEventcutErrorFlag();
    error|=fSlope[axis].GetEventcutErrorFlag();
    error|=fIntercept[axis].GetEventcutErrorFlag();
    error|=fMinimumChiSquare[axis].GetEventcutErrorFlag();
  }

  error|=fEffectiveCharge.GetEventcutErrorFlag();

  return error;
}



template<typename T>
Bool_t QwCombinedBPM<T>::ApplySingleEventCuts()
{
  Bool_t status=kTRUE;
  Int_t axis=0;
  fErrorFlag=0;
  UInt_t charge_error;
  UInt_t pos_error[2];
  charge_error = 0;
  pos_error[kXAxis]=0;
  pos_error[kYAxis]=0;

  for(size_t i=0;i<fElement.size();i++){
    ///  TODO:  The returned base class should be changed so
    ///         these casts aren't needed, but "GetErrorCode"
    ///         is not meaningful for every VQwDataElement.
    ///         Maybe the return should be a VQwHardwareChannel?

    //To update the event cut faliures in individual BPM devices
    charge_error      |= fElement[i]->GetEffectiveCharge()->GetErrorCode();
    pos_error[kXAxis] |= fElement[i]->GetPosition(kXAxis)->GetErrorCode();
    pos_error[kYAxis] |= fElement[i]->GetPosition(kYAxis)->GetErrorCode();
  }

  //Event cuts for  X & Y slopes
  for(axis=kXAxis;axis<kNumAxes;axis++){
    fSlope[axis].UpdateErrorCode(pos_error[axis]);
    if (fSlope[axis].ApplySingleEventCuts()){ //for X slope
      status&=kTRUE;
    }
    else{
      status&=kFALSE;
      if (bDEBUG) std::cout<<" X Slope event cut failed ";
    }
    //Get the Event cut error flag for SlopeX/Y
    fErrorFlag|=fSlope[axis].GetEventcutErrorFlag();
  }

  //Event cuts for  X & Y intercepts
  for(axis=kXAxis;axis<kNumAxes;axis++){
    fIntercept[axis].UpdateErrorCode(pos_error[axis]);
    if (fIntercept[axis].ApplySingleEventCuts()){ //for X slope
      status&=kTRUE;
    }
    else{
      status&=kFALSE;
      if (bDEBUG) std::cout<<" X Intercept event cut failed ";
    }
    //Get the Event cut error flag for intercept X/Y
    fErrorFlag|=fIntercept[axis].GetEventcutErrorFlag();
  }


  //Event cuts for  X & Y minimum chi square
  for(axis=kXAxis;axis<kNumAxes;axis++){
    fMinimumChiSquare[axis].UpdateErrorCode(pos_error[axis]);
    if (fMinimumChiSquare[axis].ApplySingleEventCuts()){ //for X slope
      status&=kTRUE;
    }
    else{
      status&=kFALSE;
      if (bDEBUG) std::cout<<" X Intercept event cut failed ";
    }
    //Get the Event cut error flag for min chi square X/Y
    fErrorFlag|=fMinimumChiSquare[axis].GetEventcutErrorFlag();
  }


  //Event cuts for  X & Y positions
  for(axis=kXAxis;axis<kNumAxes;axis++){
    fAbsPos[axis].UpdateErrorCode(pos_error[axis]);
    if (fAbsPos[axis].ApplySingleEventCuts()){ 
      status&=kTRUE;
    }
    else{
      status&=kFALSE;
      if (bDEBUG) std::cout<<" Abs X event cut failed ";
    }
    //Get the Event cut error flag for AbsX/Y
    fErrorFlag|=fAbsPos[axis].GetEventcutErrorFlag();
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

template<typename T>
VQwHardwareChannel* QwCombinedBPM<T>::GetSubelementByName(TString ch_name)
{
  VQwHardwareChannel* tmpptr = NULL;
  ch_name.ToLower();
  if (ch_name=="xslope"){
    tmpptr = &fSlope[kXAxis];
  }else if (ch_name=="yslope"){
    tmpptr = &fSlope[kYAxis];
  }else if (ch_name=="xintercept"){
    tmpptr = &fIntercept[kXAxis];
  }else if (ch_name=="yintercept"){
    tmpptr = &fIntercept[kYAxis];
  }else if (ch_name=="xminchisquare"){
    tmpptr = &fMinimumChiSquare[kXAxis];
  }else if (ch_name=="yminchisquare"){
    tmpptr = &fMinimumChiSquare[kYAxis];
  }else  if (ch_name=="absx" || ch_name=="x" ){
    tmpptr = &fAbsPos[kXAxis];
  }else if (ch_name=="absy" || ch_name=="y"){
    tmpptr = &fAbsPos[kYAxis];
  }else if (ch_name=="effectivecharge" || ch_name=="charge"){
    tmpptr = &fEffectiveCharge;
  } else {
    TString loc="QwCombinedBPM::GetSubelementByName for"
      + this->GetElementName() + " was passed "
      + ch_name + ", which is an unrecognized subelement name.";
    throw std::invalid_argument(loc.Data());
  }
  return tmpptr;
}

/*
template<typename T>
void QwCombinedBPM<T>::SetSingleEventCuts(TString ch_name, Double_t minX, Double_t maxX)
{

  if (ch_name=="xslope"){//cuts for the x slope
    QwMessage<<"XSlope LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fSlope[kXAxis].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="yslope"){//cuts for the y slope
    QwMessage<<"YSlope LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fSlope[kYAxis].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="xintercept"){//cuts for the x intercept
    QwMessage<<"XIntercept LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fIntercept[kXAxis].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="yintercept"){//cuts for the y intercept
    QwMessage<<"YIntercept LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fIntercept[kYAxis].SetSingleEventCuts(minX,maxX);

  } else  if (ch_name=="absx"){
  //cuts for the absolute x and y
    QwMessage<<"AbsX LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fAbsPos[kXAxis].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="absy"){
    QwMessage<<"AbsY LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fAbsPos[kYAxis].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="effectivecharge"){ //cuts for the effective charge
    QwMessage<<"EffectveQ LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fEffectiveCharge.SetSingleEventCuts(minX,maxX);
  }
}

template<typename T>
void QwCombinedBPM<T>::SetSingleEventCuts(TString ch_name, UInt_t errorflag,Double_t minX, Double_t maxX, Double_t stability){
  errorflag|=kBPMErrorFlag;//update the device flag
  if (ch_name=="xslope"){//cuts for the x slope
    QwMessage<<"XSlope LL " <<  minX <<" UL " << maxX << " kGlobalCut  "<< (errorflag&kGlobalCut)<< QwLog::endl;
    fSlope[kXAxis].SetSingleEventCuts(errorflag, minX,maxX, stability);

  }else if (ch_name=="yslope"){//cuts for the y slope
    QwMessage<<"YSlope LL " <<  minX <<" UL " << maxX <<" kGlobalCut  "<< (errorflag&kGlobalCut)<< QwLog::endl;
    fSlope[kYAxis].SetSingleEventCuts(errorflag, minX,maxX, stability);

  }else if (ch_name=="xintercept"){//cuts for the x intercept
    QwMessage<<"XIntercept LL " <<  minX <<" UL " << maxX <<" kGlobalCut  "<< (errorflag&kGlobalCut)<< QwLog::endl;
    fIntercept[kXAxis].SetSingleEventCuts(errorflag, minX,maxX, stability);

  }else if (ch_name=="yintercept"){//cuts for the y intercept
    QwMessage<<"YIntercept LL " <<  minX <<" UL " << maxX <<" kGlobalCut  "<< (errorflag&kGlobalCut)<< QwLog::endl;
    fIntercept[kYAxis].SetSingleEventCuts(errorflag, minX,maxX, stability);

  } else  if (ch_name=="absx"){
  //cuts for the absolute x and y
    QwMessage<<"AbsX LL " <<  minX <<" UL " << maxX <<" kGlobalCut  "<< (errorflag&kGlobalCut)<< QwLog::endl;
    fIntercept[kXAxis].SetSingleEventCuts(errorflag, 0,0,0);
    fAbsPos[kXAxis].SetSingleEventCuts(errorflag, minX,maxX, stability);

  }else if (ch_name=="absy"){
    QwMessage<<"AbsY LL " <<  minX <<" UL " << maxX <<" kGlobalCut  "<< (errorflag&kGlobalCut)<< QwLog::endl;
    fIntercept[kYAxis].SetSingleEventCuts(errorflag, 0,0,0);
    fAbsPos[kYAxis].SetSingleEventCuts(errorflag, minX,maxX, stability);

  }else if (ch_name=="effectivecharge"){ //cuts for the effective charge
    QwMessage<<"EffectveQ LL " <<  minX <<" UL " << maxX <<" kGlobalCut  "<< (errorflag&kGlobalCut)<< QwLog::endl;
    fEffectiveCharge.SetSingleEventCuts(errorflag, minX,maxX, stability);
  }
}

*/

template<typename T>
void QwCombinedBPM<T>::UpdateEventcutErrorFlag(const UInt_t error){
  Short_t i=0;

  for(i=kXAxis;i<kNumAxes;i++) {
    fAbsPos[i].UpdateEventcutErrorFlag(error);
    fSlope[i].UpdateEventcutErrorFlag(error);
    fIntercept[i].UpdateEventcutErrorFlag(error);
     fMinimumChiSquare[i].UpdateEventcutErrorFlag(error);
  }
  
  fEffectiveCharge.UpdateEventcutErrorFlag(error);
  
};
template<typename T>
void QwCombinedBPM<T>::UpdateEventcutErrorFlag(VQwBPM *ev_error){
  Short_t i=0;
  VQwDataElement *value_data;
  try {
    if(typeid(*ev_error)==typeid(*this)) {
      // std::cout<<" Here in QwBPMStripline::UpdateEventcutErrorFlag \n";
      if (this->GetElementName()!="") {
        QwCombinedBPM<T>* value_bpm = dynamic_cast<QwCombinedBPM<T>* >(ev_error);
      for(i=kXAxis;i<kNumAxes;i++) {
	  value_data = dynamic_cast<VQwDataElement *>(&(value_bpm->fAbsPos[i]));
	  fAbsPos[i].UpdateEventcutErrorFlag(value_data->GetErrorCode()); 
	  value_data = dynamic_cast<VQwDataElement *>(&(value_bpm->fSlope[i]));
	  fSlope[i].UpdateEventcutErrorFlag(value_data->GetErrorCode()); 
	  value_data = dynamic_cast<VQwDataElement *>(&(value_bpm->fIntercept[i]));
	  fIntercept[i].UpdateEventcutErrorFlag(value_data->GetErrorCode()); 
	  value_data = dynamic_cast<VQwDataElement *>(&(value_bpm->fMinimumChiSquare[i]));
	  fMinimumChiSquare[i].UpdateEventcutErrorFlag(value_data->GetErrorCode()); 
	}
	value_data = dynamic_cast<VQwDataElement *>(&(value_bpm->fEffectiveCharge));
	fEffectiveCharge.UpdateEventcutErrorFlag(value_data->GetErrorCode()); 
      }
    } else {
      TString loc="Standard exception from QwCombinedBPM::UpdateEventcutErrorFlag :"+
        ev_error->GetElementName()+" "+this->GetElementName()+" are not of the "
        +"same type";
      throw std::invalid_argument(loc.Data());
    }
  } catch (std::exception& e) {
    std::cerr<< e.what()<<std::endl;
  }  
};



template<typename T>
void  QwCombinedBPM<T>::ProcessEvent()
{
  Bool_t ldebug = kFALSE;

  static T  tmpQADC("tmpQADC"), tmpADC("tmpADC");

  //check to see if the fixed parameters are calculated
  if(!fixedParamCalculated){
    if(ldebug) std::cout<<"QwCombinedBPM:Calculating fixed parameters..\n";
    CalculateFixedParameter(fXWeights,kXAxis); //for X
    CalculateFixedParameter(fYWeights,kYAxis); //for Y
    fixedParamCalculated = kTRUE;
  }

  for(size_t i=0;i<fElement.size();i++){
    if(ldebug){
      std::cout<<"*******************************\n";
      std::cout<<" QwCombinedBPM: Reading "<<fElement[i]->GetElementName()<<" with charge weight ="<<fQWeights[i]
	       <<" and  x weight ="<<fXWeights[i]
	       <<" and  y weight ="<<fYWeights[i]<<"\n"<<std::flush;

    }
    tmpQADC.AssignValueFrom(fElement[i]->GetEffectiveCharge());
    tmpQADC.Scale(fQWeights[i]);
    fEffectiveCharge+=tmpQADC;


    if(ldebug) {
      std::cout<<"fElement[" << i << "]->GetEffectiveCharge()=="
	       << fElement[i]->GetEffectiveCharge()
	       << std::endl << std::flush;
      fElement[i]->GetEffectiveCharge()->PrintInfo();
      std::cout<<"fElement[" << i << "]->GetPosition(kXAxis)=="
	       << fElement[i]->GetPosition(kXAxis)
	       << std::endl << std::flush;
      std::cout<<"fElement[" << i << "]->GetPosition(kYAxis)=="
	       << fElement[i]->GetPosition(kYAxis)
	       << std::endl << std::flush;

      if (fElement[i]->GetEffectiveCharge()==NULL){
	std::cout<<"fElement[" << i << "]->GetEffectiveCharge returns NULL"
		 << std::endl;
      } else
	std::cout<<"got 4-wire.hw_sum = "<<fEffectiveCharge.GetValue()
		 <<" vs     actual "
		 << fElement[i]->GetEffectiveCharge()->GetValue() 
		 << std::endl << std::flush;

      
      std::cout<<"copied absolute X position hw_sum from device "
	       << fElement[i]->GetPosition(kXAxis)->GetValue() <<std::endl;
      std::cout<<"copied absolute Y position hw_sum from device "
	       << fElement[i]->GetPosition(kYAxis)->GetValue() <<std::endl;

    }
  }

  fEffectiveCharge.Scale(1.0/fSumQweights);
  //fAbsPos[0].ResetErrorFlag(0x4000000);
  //Least squares fit for X
  LeastSquareFit(kXAxis, fXWeights );

  //Least squares fit for Y
  LeastSquareFit(kYAxis, fYWeights );


  if(ldebug){
    std::cout<<" QwCombinedBPM:: Projected target X position = "<<fAbsPos[kXAxis].GetValue()
	     <<" and target X slope = "<<fSlope[kXAxis].GetValue()
	     <<" and target X intercept = "<<fIntercept[kXAxis].GetValue()
	     <<" with mimimum chi square = "<< fMinimumChiSquare[kXAxis].GetValue()
	     <<" \nProjected target Y position = "<<fAbsPos[kYAxis].GetValue()
	     <<" and target Y slope = "<<fSlope[kYAxis].GetValue()
	     <<" and target Y intercept = "<<fIntercept[kYAxis].GetValue()
	     <<" with mimimum chi square = "<< fMinimumChiSquare[kYAxis].GetValue()<<std::endl;
		 
  }


  if (ldebug) {
    fEffectiveCharge.PrintInfo();
    for(Short_t axis=kXAxis;axis<kNumAxes;axis++) {
      fAbsPos[axis].PrintInfo();
      fSlope[axis].PrintInfo();
      fIntercept[axis].PrintInfo();
      fMinimumChiSquare[axis].PrintInfo();
    }
  }

  return;

 }



template<typename T>
 void QwCombinedBPM<T>::CalculateFixedParameter(std::vector<Double_t> fWeights, Int_t pos)
 {

   Bool_t ldebug = kFALSE;
   static Double_t zpos = 0.0;

   for(size_t i=0;i<fElement.size();i++){
     zpos = fElement[i]->GetPositionInZ();
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
    QwWarning << "Angry Divvy: Division by zero in " << this->GetElementName() << QwLog::endl;

   if(ldebug){
     std::cout<<" A = "<<A[pos]<<", B = "<<B[pos]<<", D = "<<D[pos]<<", m = "<<m[pos]<<std::endl;
     std::cout<<"For least square fit, errors are  "<<erra[pos]
	      <<"\ncovariance  = "<<covab[pos]<<"\n\n";
   }

   return;
 }


template<typename T>
 Double_t QwCombinedBPM<T>::SumOver(std::vector<Double_t> weight,std::vector <T> val)
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
       sum+=val[i].GetValue();
     }
   }
   return sum;
 }

template<typename T>
 void QwCombinedBPM<T>::LeastSquareFit(VQwBPM::EBeamPositionMonitorAxis axis, std::vector<Double_t> fWeights)
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
   static T tmp1("tmp1","derived");
   static T tmp2("tmp2","derived");
   static T tmp3("tmp3","derived");
   static T C[kNumAxes];
   static T E[kNumAxes];

   // initialize the VQWK_Channel arrays
   C[kXAxis].InitializeChannel("cx","derived");
   C[kYAxis].InitializeChannel("cy","derived");
   E[kXAxis].InitializeChannel("ex","derived");
   E[kYAxis].InitializeChannel("ey","derived");

   C[axis].ClearEventData();
   E[axis].ClearEventData();
   for(size_t i=0;i<fElement.size();i++){
     zpos = fElement[i]->GetPositionInZ();
     tmp1.ClearEventData();
     tmp1.AssignValueFrom(fElement[i]->GetPosition(axis));
     tmp1.Scale(fWeights[i]);
     C[axis] += tmp1; //xw or yw
     tmp1.Scale(zpos);//xzw or yzw
     E[axis] += tmp1;
   }

   if(ldebug) std::cout<<"\n A ="<<A[axis]
		       <<" -- B ="<<B[axis]
		       <<" --C ="<<C[axis].GetValue()
		       <<" --D ="<<D[axis]
		       <<" --E ="<<E[axis].GetValue()<<"\n";

   // calculate the slope  a = E*erra + C*covab
   fSlope[axis].AssignScaledValue(E[axis], erra[axis]);
   tmp2.AssignScaledValue(C[axis], covab[axis]);
   fSlope[axis] += tmp2;

   // calculate the intercept  b = C*errb + E*covab
   fIntercept[axis].AssignScaledValue(C[axis], errb[axis]);
   tmp2.AssignScaledValue(E[axis], covab[axis]);
   fIntercept[axis] += tmp2;

   if(ldebug)    std::cout<<" Least Squares Fit Parameters for "<< axis
			  <<" are: \n slope = "<< fSlope[axis].GetValue()
			  <<" \n intercept = " << fIntercept[axis].GetValue()<<"\n\n";


   // absolute positions at target  using X = Za + b
   tmp1.ClearEventData();
   // Absolute position of the combined bpm is not a physical position but a derived one.


   zpos = this->GetPositionInZ();
   //UInt_t err_flag=fAbsPos[axis].GetEventcutErrorFlag();    
   fAbsPos[axis] = fIntercept[axis]; // X =  b
   //fAbsPos[axis].ResetErrorFlag(err_flag);
   tmp1.AssignScaledValue(fSlope[axis],zpos); //az
   fAbsPos[axis] += tmp1;  //X = az+b


   // to perform the minimul chi-square test
   // We want to calculte (X-az-b)^2 for each bpm in the combination and sum over the values
   tmp3.ClearEventData();
   fMinimumChiSquare[axis].ClearEventData();

   for(size_t i=0;i<fElement.size();i++){
     tmp1.ClearEventData();
     tmp2.ClearEventData();
     //std::cout<<"\nName -------- ="<<(fElement[i]->GetElementName())<<std::endl;
     //std::cout<<"\nRead value ="<<(fElement[i]->GetPosition(axis))->GetValue()<<std::endl;

     tmp1.AssignValueFrom(fElement[i]->GetPosition(axis)); // = X
     //std::cout<<"Read value ="<<tmp1.GetValue()<<std::endl;

     tmp2.AssignScaledValue(fSlope[axis],fElement[i]->GetPositionInZ());
     tmp2+=fIntercept[axis];
     //std::cout<<"Calculated abs value ="<<tmp2.GetValue()<<std::endl;

     tmp1 -= tmp2;      // = X-Za-b
     //std::cout<<"Read-calculated ="<<tmp1.GetValue()<<std::endl;
     tmp1.Product(tmp1,tmp1); // = (X-Za-b)^2
     //std::cout<<"(Read-calculated)^2 ="<<tmp1.GetValue()<<std::endl;
     tmp1.Scale(fWeights[i]*fWeights[i]); // = [(X-Za-b)^2]W
     //std::cout<<"(Read-calculated)^2/weight ="<<tmp1.GetValue()<<std::endl;
     tmp3+=tmp1; //sum over
     //std::cout<<"Sum (Read-calculated)^2/weight +="<<tmp3.GetValue()<<std::endl;
   }

   fMinimumChiSquare[axis].AssignScaledValue(tmp3,1.0/(fElement.size()-2)); //minimul chi-square
   
   return;
 }

template<typename T>
 Int_t QwCombinedBPM<T>::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer,UInt_t index)
 {
   return word_position_in_buffer;
 }



template<typename T>
void QwCombinedBPM<T>::PrintValue() const
{
   Short_t axis;

   for(axis = kXAxis; axis < kNumAxes; axis++){
     fAbsPos[axis].PrintValue();
   }
   ///  TODO:  To print the Z position, we need to use GetPositionInZ()
   for(axis = kXAxis; axis < kNumAxes; axis++) {
     fSlope[axis].PrintValue();
     fIntercept[axis].PrintValue();
     fMinimumChiSquare[axis].PrintValue();
   }
   fEffectiveCharge.PrintValue();

   return;
 }


template<typename T>
void QwCombinedBPM<T>::PrintInfo() const
{

  Short_t axis;

  for(axis = kXAxis; axis < kNumAxes; axis++){
    fAbsPos[axis].PrintInfo();
  }
  ///  TODO:  To print the Z position, we need to use GetPositionInZ()
  for(axis = kXAxis; axis < kNumAxes; axis++) {
    fSlope[axis].PrintInfo();
    fIntercept[axis].PrintInfo();
    fMinimumChiSquare[axis].PrintInfo();
  }
  fEffectiveCharge.PrintInfo();

  return;
}


template<typename T>
VQwBPM& QwCombinedBPM<T>::operator= (const VQwBPM &value)
{
  *(dynamic_cast<const QwCombinedBPM<T>*>(this))=
    *(dynamic_cast<const QwCombinedBPM<T>*>(&value));
  return *this;
}

template<typename T>
QwCombinedBPM<T>& QwCombinedBPM<T>::operator= (const QwCombinedBPM<T> &value)
{
  VQwBPM::operator= (value);
  if (this->GetElementName()!=""){
    this->fEffectiveCharge=value.fEffectiveCharge;
    for(Short_t axis=kXAxis;axis<kNumAxes;axis++){
      this->fSlope[axis]=value.fSlope[axis];
      this->fIntercept[axis] = value.fIntercept[axis];
      this->fAbsPos[axis]=value.fAbsPos[axis];
      this->fMinimumChiSquare[axis]=value.fMinimumChiSquare[axis];
    }
  }
  return *this;
}


template<typename T>
VQwBPM& QwCombinedBPM<T>::operator+= (const VQwBPM &value)
{
  *(dynamic_cast<const QwCombinedBPM<T>*>(this))+=
    *(dynamic_cast<const QwCombinedBPM<T>*>(&value));
  return *this;
}

template<typename T>
QwCombinedBPM<T>& QwCombinedBPM<T>::operator+= (const QwCombinedBPM<T> &value)
{

     if (this->GetElementName()!=""){
       this->fEffectiveCharge+=value.fEffectiveCharge;
       for(Short_t axis=kXAxis;axis<kNumAxes;axis++) 	{
	 this->fSlope[axis]+=value.fSlope[axis];
	 this->fIntercept[axis]+=value.fIntercept[axis];
	 this->fAbsPos[axis]+=value.fAbsPos[axis];
	 this->fMinimumChiSquare[axis]+=value.fMinimumChiSquare[axis];
       }
     }
     return *this;
}

template<typename T>
VQwBPM& QwCombinedBPM<T>::operator-= (const VQwBPM &value)
{
  *(dynamic_cast<const QwCombinedBPM<T>*>(this))-=
    *(dynamic_cast<const QwCombinedBPM<T>*>(&value));
  return *this;
}


template<typename T>
QwCombinedBPM<T>& QwCombinedBPM<T>::operator-= (const QwCombinedBPM<T> &value)
{

  if (this->GetElementName()!=""){
    this->fEffectiveCharge-=value.fEffectiveCharge;
    for(Short_t axis=kXAxis;axis<kNumAxes;axis++){
      this->fSlope[axis]-=value.fSlope[axis];
      this->fIntercept[axis]-=value.fIntercept[axis];
      this->fAbsPos[axis]-=value.fAbsPos[axis];
      this->fMinimumChiSquare[axis]-=value.fMinimumChiSquare[axis];
    }
  }
  return *this;
}


template<typename T>
void QwCombinedBPM<T>::Ratio(VQwBPM &numer, VQwBPM &denom)
{
  Ratio(*dynamic_cast<QwCombinedBPM<T>*>(&numer),
      *dynamic_cast<QwCombinedBPM<T>*>(&denom));
}

template<typename T>
void QwCombinedBPM<T>::Ratio(QwCombinedBPM<T> &numer,
    QwCombinedBPM<T> &denom)
{
  // this function is called when forming asymmetries. In this case waht we actually want for the
  // combined bpm is the difference only not the asymmetries

  *this=numer;
  this->fEffectiveCharge.Ratio(numer.fEffectiveCharge,denom.fEffectiveCharge);
  if (this->GetElementName()!=""){
    //    The slope, intercept and absolute positions should all be differences, not asymmetries.
    for(Short_t axis=kXAxis;axis<kNumAxes;axis++) {
      this->fSlope[axis]     = numer.fSlope[axis];
      this->fIntercept[axis] = numer.fIntercept[axis];
      this->fAbsPos[axis]    = numer.fAbsPos[axis];
      this->fMinimumChiSquare[axis]    = numer.fMinimumChiSquare[axis];
    }
  }

  return;
}


template<typename T>
void QwCombinedBPM<T>::Scale(Double_t factor)
{
  fEffectiveCharge.Scale(factor);
  for(Short_t axis=kXAxis;axis<kNumAxes;axis++){
    fSlope[axis].Scale(factor);
    fIntercept[axis].Scale(factor);
    fAbsPos[axis].Scale(factor);
    fMinimumChiSquare[axis].Scale(factor);
  }
  return;
}

template<typename T>
void QwCombinedBPM<T>::CalculateRunningAverage()
{
  fEffectiveCharge.CalculateRunningAverage();

  for (Short_t axis = kXAxis; axis < kNumAxes; axis++) {
    fSlope[axis].CalculateRunningAverage();
    fIntercept[axis].CalculateRunningAverage();
    fAbsPos[axis].CalculateRunningAverage();
    fMinimumChiSquare[axis].CalculateRunningAverage();
  }
}


template<typename T>
void QwCombinedBPM<T>::AccumulateRunningSum(const VQwBPM& value)
{
  AccumulateRunningSum(*dynamic_cast<const QwCombinedBPM<T>* >(&value));
}

template<typename T>
void QwCombinedBPM<T>::AccumulateRunningSum(const QwCombinedBPM<T>& value)
{
  for (Short_t axis = kXAxis; axis < kNumAxes; axis++){
    fSlope[axis].AccumulateRunningSum(value.fSlope[axis]);
    fIntercept[axis].AccumulateRunningSum(value.fIntercept[axis]);
    fAbsPos[axis].AccumulateRunningSum(value.fAbsPos[axis]);
    fMinimumChiSquare[axis].AccumulateRunningSum(value.fMinimumChiSquare[axis]);
  }
  fEffectiveCharge.AccumulateRunningSum(value.fEffectiveCharge);
}

template<typename T>
void QwCombinedBPM<T>::DeaccumulateRunningSum(VQwBPM& value)
{
  DeaccumulateRunningSum(*dynamic_cast<QwCombinedBPM<T>* >(&value));
}

template<typename T>
void QwCombinedBPM<T>::DeaccumulateRunningSum(QwCombinedBPM<T>& value)
{

  for (Short_t axis = kXAxis; axis < kNumAxes; axis++){
    fSlope[axis].DeaccumulateRunningSum(value.fSlope[axis]);
    fIntercept[axis].DeaccumulateRunningSum(value.fIntercept[axis]);
    fAbsPos[axis].DeaccumulateRunningSum(value.fAbsPos[axis]);
    fMinimumChiSquare[axis].DeaccumulateRunningSum(value.fMinimumChiSquare[axis]);
  }
  fEffectiveCharge.DeaccumulateRunningSum(value.fEffectiveCharge);

}





template<typename T>
void  QwCombinedBPM<T>::ConstructHistograms(TDirectory *folder, TString &prefix)
{

  if (this->GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else{
    //we calculate the asym_ for the fEffectiveCharge becasue its an asymmetry and not a difference.
    fEffectiveCharge.ConstructHistograms(folder, prefix);
    TString thisprefix=prefix;
    if(prefix=="asym_")
      thisprefix="diff_";
    this->SetRootSaveStatus(prefix);

    for(Short_t axis=kXAxis;axis<kNumAxes;axis++) {
	fSlope[axis].ConstructHistograms(folder, thisprefix);
	fIntercept[axis].ConstructHistograms(folder, thisprefix);
	fAbsPos[axis].ConstructHistograms(folder, thisprefix);
	fMinimumChiSquare[axis].ConstructHistograms(folder, thisprefix);
    }

  }
  return;
}

template<typename T>
void  QwCombinedBPM<T>::FillHistograms()
{
  if (this->GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else{
    fEffectiveCharge.FillHistograms();
    for(Short_t axis=kXAxis;axis<kNumAxes;axis++) {
      fSlope[axis].FillHistograms();
      fIntercept[axis].FillHistograms();
      fAbsPos[axis].FillHistograms();
      fMinimumChiSquare[axis].FillHistograms();
    }
  }
  return;
}

template<typename T>
void  QwCombinedBPM<T>::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (this->GetElementName()==""){
    //  This channel is not used, so skip constructing trees.
  } else
    {

      TString thisprefix=prefix;
      if(prefix=="asym_")
	thisprefix="diff_";

      this->SetRootSaveStatus(prefix);

      fEffectiveCharge.ConstructBranchAndVector(tree,prefix,values);
      for(Short_t axis=kXAxis;axis<kNumAxes;axis++){
	fSlope[axis].ConstructBranchAndVector(tree,thisprefix,values);
	fIntercept[axis].ConstructBranchAndVector(tree,thisprefix,values);
	fAbsPos[axis].ConstructBranchAndVector(tree,thisprefix,values);
	fMinimumChiSquare[axis].ConstructBranchAndVector(tree,thisprefix,values);
      }

    }

  return;
}

template<typename T>
void  QwCombinedBPM<T>::ConstructBranch(TTree *tree, TString &prefix)
{
  if (this->GetElementName()==""){
    //  This channel is not used, so skip constructing trees.
  } else
    {
      TString thisprefix=prefix;
      if(prefix=="asym_")
	thisprefix="diff_";


      fEffectiveCharge.ConstructBranch(tree,prefix);

      for(Short_t axis=kXAxis;axis<kNumAxes;axis++){
	fSlope[axis].ConstructBranch(tree,thisprefix);
	fIntercept[axis].ConstructBranch(tree,thisprefix);
	fAbsPos[axis].ConstructBranch(tree,thisprefix);
	fMinimumChiSquare[axis].ConstructBranch(tree,thisprefix);
      }

    }
  return;
}

template<typename T>
void  QwCombinedBPM<T>::ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist)
{
  TString devicename;
  devicename=this->GetElementName();
  devicename.ToLower();

  if (this->GetElementName()==""){
    //  This channel is not used, so skip constructing trees.
  } else
    {
      if (modulelist.HasValue(devicename)){
	TString thisprefix=prefix;
	if(prefix=="asym_")
	  thisprefix="diff_";


	fEffectiveCharge.ConstructBranch(tree,prefix);

	for(Short_t axis=kXAxis;axis<kNumAxes;axis++){
	  fSlope[axis].ConstructBranch(tree,thisprefix);
	  fIntercept[axis].ConstructBranch(tree,thisprefix);
	  fAbsPos[axis].ConstructBranch(tree,thisprefix);
	  fMinimumChiSquare[axis].ConstructBranch(tree,thisprefix);
	}
	QwMessage <<" Tree leave added to "<<devicename<<QwLog::endl;
    }

    }
  return;
}


template<typename T>
void  QwCombinedBPM<T>::FillTreeVector(std::vector<Double_t> &values) const
{
  if (this->GetElementName()==""){
    //  This channel is not used, so skip filling the tree.
  }
  else{
    fEffectiveCharge.FillTreeVector(values);

    for(Short_t axis=kXAxis;axis<kNumAxes;axis++){
      fSlope[axis].FillTreeVector(values);
      fIntercept[axis].FillTreeVector(values);
      fAbsPos[axis].FillTreeVector(values);
      fMinimumChiSquare[axis].FillTreeVector(values);
    }
  }
  return;
}

template<typename T>
void QwCombinedBPM<T>::SetEventCutMode(Int_t bcuts)
{

  //  bEVENTCUTMODE=bcuts;
  for (Short_t axis=kXAxis;axis<kNumAxes;axis++){
    fSlope[axis].SetEventCutMode(bcuts);
    fIntercept[axis].SetEventCutMode(bcuts);
    fAbsPos[axis].SetEventCutMode(bcuts);
    fMinimumChiSquare[axis].SetEventCutMode(bcuts);
  }
  fEffectiveCharge.SetEventCutMode(bcuts);

  return;
}


template<typename T>
void QwCombinedBPM<T>::MakeBPMComboList()
{
  for (size_t axis = kXAxis; axis < kNumAxes; axis++) {
    T abspos(fAbsPos[axis]);
    abspos = fAbsPos[axis];
    fBPMComboElementList.push_back(abspos);
    T slope(fSlope[axis]);
    slope = fSlope[axis];
    fBPMComboElementList.push_back(slope);
    T intercept(fIntercept[axis]);
    intercept = fIntercept[axis];
    fBPMComboElementList.push_back(intercept);
    T minimumchisquare(fMinimumChiSquare[axis]);
    minimumchisquare = fMinimumChiSquare[axis];
    fBPMComboElementList.push_back(minimumchisquare);
  }
  T effectivecharge(fEffectiveCharge);
  effectivecharge = fEffectiveCharge;
  fBPMComboElementList.push_back(effectivecharge);
}



template<typename T>
std::vector<QwDBInterface> QwCombinedBPM<T>::GetDBEntry()
{
  std::vector <QwDBInterface> row_list;
  row_list.clear();
  for(size_t axis=kXAxis;axis<kNumAxes;axis++) {
    fAbsPos[axis].AddEntriesToList(row_list);
    fSlope[axis].AddEntriesToList(row_list);
    fIntercept[axis].AddEntriesToList(row_list);
    fMinimumChiSquare[axis].AddEntriesToList(row_list);
  }
  fEffectiveCharge.AddEntriesToList(row_list);
  return row_list;
}



template<typename T>
std::vector<QwErrDBInterface> QwCombinedBPM<T>::GetErrDBEntry()
{
  std::vector <QwErrDBInterface> row_list;
  row_list.clear();
  for(size_t axis=kXAxis;axis<kNumAxes;axis++) {
    fAbsPos[axis].AddErrEntriesToList(row_list);
    fSlope[axis].AddErrEntriesToList(row_list);
    fIntercept[axis].AddErrEntriesToList(row_list);
    fMinimumChiSquare[axis].AddErrEntriesToList(row_list);
  }
  //  fEffectiveCharge.AddErrEntriesToList(row_list);
  return row_list;
}

template class QwCombinedBPM<QwVQWK_Channel>; 
template class QwCombinedBPM<QwSIS3801_Channel>; 
template class QwCombinedBPM<QwSIS3801D24_Channel>;
