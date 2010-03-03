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
  bRotated=ROTATED;

  SetOffset(0,0,0);  

  fCombinedWSum.InitializeChannel(name+"WSum","derived");


  for(int i=0;i<2;i++)
    fCombinedRelPos[i].InitializeChannel(name+"Rel"+axis[i],"derived");	
   

//   for(int i=0;i<3;i++)
//     fCombinedAbsPos[i].InitializeChannel(name+axis[i],"derived");
 
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
  
  for(int i=0;i<2;i++)
    fCombinedRelPos[i].ClearEventData();
  
//   for(int i=0;i<3;i++)
//     fCombinedAbsPos[i].ClearEventData();

  fCombinedWSum.ClearEventData();

  return;
};
/********************************************************/

Int_t QwCombinedBPM::GetEventcutErrorCounters(){
     
  //std::cout<<"RelX ";
  fCombinedRelPos[0].GetEventcutErrorCounters();
  //std::cout<<"RelY ";
  fCombinedRelPos[1].GetEventcutErrorCounters();

  return 1;
};

/********************************************************/
void QwCombinedBPM::SetRandomEventParameters(Double_t meanX, Double_t sigmaX, Double_t meanY, Double_t sigmaY)
{
  // Average values of the signals in the stripline ADCs
//   Double_t sumX = 1.1e8; // These are just guesses, but I made X and Y different
//   Double_t sumY = 0.9e8; // to make it more interesting for the analyzer...

//   // Rotate the requested position if necessary (this is not tested yet)
//   if (bRotated) {
//     Double_t rotated_meanX = (meanX + meanY) / kRotationCorrection;
//     Double_t rotated_meanY = (meanX - meanY) / kRotationCorrection;
//     meanX = rotated_meanX;
//     meanY = rotated_meanY;
//   }

//   // Determine the asymmetry from the position
//   Double_t meanXP = (1.0 + meanX / kQwStriplineCalibration) * sumX / 2.0;
//   Double_t meanXM = (1.0 - meanX / kQwStriplineCalibration) * sumX / 2.0; // = sumX - meanXP;
//   Double_t meanYP = (1.0 + meanY / kQwStriplineCalibration) * sumY / 2.0;
//   Double_t meanYM = (1.0 - meanY / kQwStriplineCalibration) * sumY / 2.0; // = sumY - meanYP;

//   // Determine the spread of the asymmetry (this is not tested yet)
//   // (negative sigma should work in the QwVQWK_Channel, but still using fabs)
//   Double_t sigmaXP = fabs(sumX * sigmaX / meanX);
//   Double_t sigmaXM = sigmaXP;
//   Double_t sigmaYP = fabs(sumY * sigmaY / meanY);
//   Double_t sigmaYM = sigmaYP;

//   // Propagate these parameters to the ADCs
// //   fCombinedWire[0].SetRandomEventParameters(meanXP, sigmaXP);
// //   fCombinedWire[1].SetRandomEventParameters(meanXM, sigmaXM);
// //   fCombinedWire[2].SetRandomEventParameters(meanYP, sigmaYP);
// //   fCombinedWire[3].SetRandomEventParameters(meanYM, sigmaYM);
};
/********************************************************/
void QwCombinedBPM::RandomizeEventData(int helicity)
{
//   for (int i = 0; i < 4; i++)
//     fCombinedWire[i].RandomizeEventData(helicity);

  return;
};
/********************************************************/
void QwCombinedBPM::SetEventData(Double_t* relpos, UInt_t sequencenumber)
{
  //This needs to be modified to allow setting the position
  for (int i = 0; i < 2; i++) {
    fCombinedRelPos[i].SetHardwareSum(relpos[i], sequencenumber);
  }

  return;
};
/********************************************************/
void QwCombinedBPM::EncodeEventData(std::vector<UInt_t> &buffer)
{
 //  for (int i = 0; i < 4; i++)
//     fCombinedWire[i].EncodeEventData(buffer);
};
/********************************************************/
 
Bool_t QwCombinedBPM::ApplySingleEventCuts(){
  Bool_t status=kTRUE;
  
  ApplyHWChecks();//first apply HW checks and update HW  error flags.

  if (fDevice_flag==1){//if fDevice_flag==1 then perform the event cut limit test

    //we only need to check two final values 
    //if (fCombinedRelPos[0].GetHardwareSum()<=fULimitX && fRelPos[0].GetHardwareSum()>=fLLimitX){ //for RelX
    if (fCombinedRelPos[0].ApplySingleEventCuts(fLLimitX,fULimitX)){ //for RelX  
      status=kTRUE;
    }
    else{
      fCombinedRelPos[0].UpdateEventCutErrorCount();
      status=kFALSE;
      if (bDEBUG) std::cout<<" Rel X event cut failed ";
    }
    fDeviceErrorCode|=fCombinedRelPos[0].GetEventcutErrorFlag();//Get the Event cut error flag for RelX
    //if (fRelPos[1].GetHardwareSum()<=fULimitY && fRelPos[1].GetHardwareSum()>=fLLimitY){//for RelY
    if (fCombinedRelPos[1].ApplySingleEventCuts(fLLimitY,fULimitY)){
      status&=kTRUE;
      //std::cout<<" ";
    }
    else{
      fCombinedRelPos[1].UpdateEventCutErrorCount();
      status&=kFALSE;
      if (bDEBUG) std::cout<<" Rel Y event cut failed ";
    }
    fDeviceErrorCode|=fCombinedRelPos[1].GetEventcutErrorFlag();//Get the Event cut error flag for RelY
	
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
  //std::cout<<GetElementName()<<"BPM  fDevice_flag "<<fDevice_flag<<std::endl;
  
  return 0; 
};




/********************************************************/

void QwCombinedBPM::SetDefaultSampleSize(Int_t sample_size){

//   for(int i=0;i<4;i++)
//     fCombinedWire[i].SetDefaultSampleSize((size_t)sample_size);
  
  
};

/********************************************************/

void  QwCombinedBPM::ProcessEvent() 
{
  Bool_t ldebug = kFALSE;
  static QwVQWK_Channel numer("numerator"), denom("denominator");
  static QwVQWK_Channel  tmpQADC, tmpADC;
  std::vector <Double_t> XADC; 
  std::vector <Double_t> YADC; 
  std::vector <Double_t> zpos;
  std::vector <Double_t> zpos2; 
  std::vector <Double_t> x2;
  std::vector <Double_t> zx;
  std::vector <Double_t> y2;
  std::vector <Double_t> zy;
  std::vector <Double_t> unit;
  for(size_t i=0;i<fElement.size();i++) unit.push_back(1);
 
  Double_t  totalq_weights=0; 



 //  fCombinedWSum.ClearEventData(); 

//   for(size_t i=0;i<2;i++)
//     fCombinedRelPos[i].ClearEventData();  //we are going to use the absolute relative positions only
 

  //Currently we don't have the correct BPM/BCM positions in the analyser. So I am going to use a random array of 20 number for the Z positions.
  for(size_t n=0;n<20;n++) zpos.push_back(20*(n+1));
  
  
  for(size_t i=0;i<fElement.size();i++)
    {  
   
      if(ldebug){
	std::cout<<"*******************************\n";
	std::cout<<"QwCombinedBPM: Reading "<<fElement[i]->GetElementName()<<" with charge weight ="<<fQWeights[i]
		 <<" and  x weight ="<<fXWeights[i]
		 <<" and  y weight ="<<fYWeights[i]<<"\n";
	
      }
      
      //to get the weighted charge/4-wire sum;
      tmpQADC.Copy(&(fElement[i]->fWSum));
      tmpQADC=fElement[i]->fWSum;
      tmpQADC.Scale(fQWeights[i]);
      fCombinedWSum+=tmpQADC;
      totalq_weights +=fQWeights[i];
      
      if(ldebug) std::cout<<"got 4-wire.hw_sum = "<<tmpQADC.GetHardwareSum()<<" vs     actual "<<(fElement[i]-> fWSum).GetHardwareSum()<<std::endl;
      
      //to perform least squares fit on relative positions load the X & Y positions in to arrays
         
      XADC.push_back((fElement[i]-> fRelPos[0]).GetHardwareSum());
      YADC.push_back((fElement[i]-> fRelPos[1]).GetHardwareSum());
      
      if(ldebug) {
	std::cout<<"got absolute X position hw_sum = "<<XADC[i]<<" vs     actual "<<(fElement[i]-> fRelPos[0]).GetHardwareSum()<<std::endl;
	std::cout<<"got absolute Y position hw_sum = "<<YADC[i]<<" vs     actual "<<(fElement[i]-> fRelPos[1]).GetHardwareSum()<<std::endl;
      }
      
      zpos2.push_back(zpos[i]*zpos[i]);
      x2.push_back(XADC[i]*XADC[i]);
      zx.push_back(XADC[i]*zpos[i]);
      y2.push_back(YADC[i]*YADC[i]);
      zy.push_back(YADC[i]*zpos[i]);
    }
  


  //Least square fit for X
  LeastSquareFit( 0, SumOver(fXWeights,zpos),
		  SumOver(fXWeights,unit),
		  SumOver(fXWeights,XADC),
		  SumOver(fXWeights,zpos2),
		  SumOver(fXWeights,zx),
		  SumOver(fXWeights,x2));
  
  //Least square fit for Y
  LeastSquareFit( 1, SumOver(fYWeights,zpos),
		  SumOver(fYWeights,unit),
		  SumOver(fYWeights,YADC),
		  SumOver(fYWeights,zpos2),
		  SumOver(fYWeights,zy),
		  SumOver(fYWeights,y2));
  
  

  fCombinedWSum.Scale(1.0/totalq_weights);
  for(size_t n=0;n<2;n++)
    fCombinedRelPos[n].SetHardwareSum(zpos[n]*a[n] + b[n]);
  

  if (ldebug) {
    fCombinedWSum.Print();  
    fCombinedRelPos[0].Print();
    fCombinedRelPos[1].Print();
  }
  
  
  
  
  
//   for(int i=0;i<2;i++)
//     {
//       numer.Difference(fWire[i*2],fWire[i*2+1]);
//       denom.Sum(fWire[i*2],fWire[i*2+1]);
//       fRelPos[i].Ratio(numer,denom);
//       fRelPos[i].Scale(kQwStriplineCalibration);
//       if(kDEBUG)
// 	{
// 	  std::cout<<" stripline name="<<fElementName<<axis[i];
// 	  std::cout<<" event number="<<fWire[i*2].GetSequenceNumber()<<"\n";
// 	  std::cout<<" hw  Wire["<<i*2<<"]="<<fWire[i*2].GetHardwareSum()<<"  ";
// 	  std::cout<<" hw  Wire["<<i*2+1<<"]="<<fWire[i*2+1].GetHardwareSum()<<"\n";
// 	  std::cout<<" hw numerator="<<numer.GetHardwareSum()<<"  ";
// 	  std::cout<<" hw denominator="<<denom.GetHardwareSum()<<"\n";
// 	  std::cout<<" hw  fRelPos["<<i<<"]="<<fRelPos[i].GetHardwareSum()<<"\n \n";
// 	}
//     }
//   if(bRotated)
//     {
//       /* for this one I suppose that the direction [0] is vertical and up,
// 	 direction[3] is the beam line direction toward the beamdump
// 	 if rotated than the frame is rotated by 45 deg counter clockwise*/
//       numer=fRelPos[0];
//       denom=fRelPos[1];
//       fRelPos[0].Sum(numer,denom);
//       fRelPos[1].Difference(numer,denom);
//       fRelPos[0].Scale(kRotationCorrection);
//       fRelPos[1].Scale(kRotationCorrection);
//     }
//   for(int i=0;i<3;i++)
//     fAbsPos[i].Offset(fOffset[i]);  
  
  

  return;
};


Double_t QwCombinedBPM::SumOver(std::vector<Double_t> weight,std::vector <Double_t> val)
{
  Double_t sum = 0;
  for(size_t i=0;i<weight.size();i++){
    sum+=(val[i]*weight[i]);
    //std::cout<<sum<<"\n";
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

  if(ldebug) {
    std::cout<<"////////////////////////////////////////////////\n";
    std::cout<<"Least Squares Fit Parameters for "<<pos<<" are: \na = "<<a[pos]<<"+-"<<erra[pos]
	     <<"\nb = "<<b[pos]<<"+-"<<errb[pos]
	     <<"\ncovariance of a and b = "<<covab[pos]<<"\n";
    std::cout<<"////////////////////////////////////////////////\n";

  }
  return;
}

/********************************************************/
void QwCombinedBPM::Print()
{

  for(int i=0;i<2;i++)
    fCombinedRelPos[i].Print();
  fCombinedWSum.Print();

  return;
}

/********************************************************/
Bool_t QwCombinedBPM::ApplyHWChecks()
{
  Bool_t fEventIsGood=kTRUE;
   
//   fDeviceErrorCode=0;
//   for(int i=0;i<4;i++) 
//     {
//       fDeviceErrorCode|= fCombinedWire[i].ApplyHWChecks();  //OR the error code from each wire
//       fEventIsGood &= (fDeviceErrorCode & 0x0);//AND with 0 since zero means HW is good.	
      
//       if (bDEBUG) std::cout<<" Inconsistent within BPM terminals wire[ "<<i<<" ] "<<std::endl;  
//       if (bDEBUG) std::cout<<" wire[ "<<i<<" ] sequence num "<<fCombinedWire[i].GetSequenceNumber()<<" sample size "<<fWire[i].GetNumberOfSamples()<<std::endl;
//     }
 
  

  return fEventIsGood;
};
/********************************************************/
Int_t QwCombinedBPM::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer,UInt_t index)
{
//   if(index<4)
//     fWire[index].ProcessEvBuffer(buffer,word_position_in_buffer);
//   else
//     std::cerr <<
//       "QwCombinedBPM::ProcessEvBuffer(): attemp to fill in raw date for a wire that doesn't exist \n";

  return word_position_in_buffer;
};
/********************************************************/
void QwCombinedBPM::SetOffset(Double_t Xoffset, Double_t Yoffset, Double_t Zoffset)
{
  fComboOffset[0]=Xoffset;
  fComboOffset[1]=Yoffset;
  fComboOffset[2]=Zoffset;

  return;
};

// void QwCombinedBPM::SetSubElementPedestal(Int_t j, Double_t value)
// {
//  //  fCombinedWire[j].SetPedestal(value);
//   return;
// }

// void QwCombinedBPM::SetSubElementCalibrationFactor(Int_t j, Double_t value)
// {
//   //fCombinedWire[j].SetCalibrationFactor(value);
//   return;
// }
/********************************************************/
// TString QwCombinedBPM::GetSubElementName(Int_t subindex)
// {
// //   TString thisname;
// //   if(subindex<4&&subindex>-1)
// //     thisname=fCombinedWire[subindex].GetElementName();
// //   else
// //     std::cerr<<"QwCombinedBPM::GetSubElementName for "<<
// //       GetElementName()<<" this subindex doesn't exists \n";

// //   return thisname;
// }

// UInt_t QwCombinedBPM::GetSubElementIndex(TString subname)
// {
// //   subname.ToUpper();
// //   UInt_t localindex=999999;
// //   for(int i=0;i<4;i++)
// //     if(subname==subelement[i])localindex=i;

// //   if(localindex>3)
// //     std::cerr << "QwCombinedBPM::GetSubElementIndex is unable to associate the string -"
// // 	      <<subname<<"- to any index"<<std::endl;

// //   return localindex;
// };
/********************************************************/
QwCombinedBPM& QwCombinedBPM::operator= (const QwCombinedBPM &value)
{
  if (GetElementName()!="")
  {
    this->bRotated=value.bRotated;
    this->fCombinedWSum=value.fCombinedWSum;
    for(int i=0;i<2;i++)
      this->fCombinedRelPos[i]=value.fCombinedRelPos[i];
    for(int i=0;i<3;i++)
      {
	//this->fCombinedAbsPos[i]=value.fCombinedAbsPos[i];
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
	this->fCombinedRelPos[i]+=value.fCombinedRelPos[i];
//       for(int i=0;i<3;i++)
//  	this->fCombinedAbsPos[i]+=value.fCombinedAbsPos[i];
    }
  return *this;
};

QwCombinedBPM& QwCombinedBPM::operator-= (const QwCombinedBPM &value)
{
  if (GetElementName()!="")
    {
      this->fCombinedWSum-=value.fCombinedWSum;
      for(int i=0;i<2;i++)
	this->fCombinedRelPos[i]-=value.fCombinedRelPos[i];
      //       for(int i=0;i<3;i++)
// 	this->fCombinedAbsPos[i]-=value.fCombinedAbsPos[i];
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
 	this->fCombinedRelPos[i].Ratio(numer.fCombinedRelPos[i], denom.fCombinedRelPos[i]);
//       for(int i=0;i<3;i++)
// 	this->fCombinedAbsPos[i].Ratio(numer.fCombinedAbsPos[i], denom.fCombinedAbsPos[i]);
    }

  return;
};


void QwCombinedBPM::Scale(Double_t factor)
{
  for(int i=0;i<2;i++)
    {
      fCombinedRelPos[i].Scale(factor);
      //fCombinedAbsPos[i].Scale(factor);
    }
}

void QwCombinedBPM::Calculate_Running_Average(){
  for(int i=0;i<2;i++)
    {
      fCombinedRelPos[i].Calculate_Running_Average();
      //fCombinedAbsPos[i].Calculate_Running_Average();
    }
  fCombinedWSum.Calculate_Running_Average();
};

void QwCombinedBPM::Do_RunningSum(){
  for(int i=0;i<2;i++)
    {
      fCombinedRelPos[i].Do_RunningSum();
      //fCombinedAbsPos[i].Do_RunningSum();
    }
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
	  fCombinedRelPos[i].ConstructHistograms(folder, thisprefix);
//       for(int i=0;i<3;i++)
//        fCombinedAbsPos[i].ConstructHistograms(folder, prefix);
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
	  fCombinedRelPos[i].FillHistograms();
//       for(int i=0;i<3;i++)
// 	fCombinedAbsPos[i].FillHistograms();
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
	fCombinedRelPos[i].DeleteHistograms();
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
	  fCombinedRelPos[i].ConstructBranchAndVector(tree,thisprefix,values);
//  	for(int i=0;i<2;i++)
//  	  fCombinedAbsPos[i].ConstructBranchAndVector(tree,thisprefix,values);

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
	fCombinedRelPos[i].FillTreeVector(values);
      // 	 for(int i=0;i<2;i++)
      // 	   fCombinedAbsPos[i].FillTreeVector(values);
      
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
 	   this->fCombinedRelPos[i].Copy(&(input->fCombinedRelPos[i]));
// 	 for(int i = 0; i < 3; i++)
// 	   this->fCombinedAbsPos[i].Copy(&(input->fCombinedAbsPos[i]));
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
    fCombinedRelPos[i].SetEventCutMode(bcuts);
//   for (Int_t i=0;i<2;i++)
//     fCombinedAbsPos[i].SetEventCutMode(bcuts);
  fCombinedWSum.SetEventCutMode(bcuts);
}


