/**********************************************************\
* File: QwHelicityCorrelatedFeedback.cc                   *
*                                                         *
* Author:Author: Rakitha Beminiwattha                     *
* Time-stamp:Time-stamp: <2010-09-28>                     *
\**********************************************************/

#include "QwHelicityCorrelatedFeedback.h"

/*****************************************************************/


/**
 * Defines configuration options using QwOptions functionality.
 * @param options Options object
 */
void QwHelicityCorrelatedFeedback::DefineOptions(QwOptions &options)
{
  QwHelicityPattern::DefineOptions(options);
};

/*****************************************************************/
void QwHelicityCorrelatedFeedback::ProcessOptions(QwOptions &options)
{
  QwHelicityPattern::ProcessOptions(options);
};

/*****************************************************************/
void QwHelicityCorrelatedFeedback::LoadParameterFile(TString filename){

  QwParameterFile mapstr(filename.Data());  //Open the file
  TString varname, varvalue;
  UInt_t value;
  Double_t dvalue;
  while (mapstr.ReadNextLine()){
    mapstr.TrimComment('#');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;

    if (mapstr.HasVariablePair("=",varname,varvalue)){
      //Decode it.
      varname.ToLower();
 
      if (varname=="patterns"){
	value = QwParameterFile::GetUInt(varvalue);
	fAccumulatePatternMax=value;
      }
      else if (varname=="deltaaq"){
	dvalue = atof(varvalue.Data());
	fChargeAsymPrecision=dvalue;
      }
      else if (varname=="optimalia"){
	dvalue = atof(varvalue.Data());
	fOptimalIA = dvalue;
      }
      else if (varname=="optimalpcp"){
	dvalue = atof(varvalue.Data());
	fOptimalPCP = dvalue;
      }
      else if (varname=="optimalpcn"){
	dvalue = atof(varvalue.Data());
	fOptimalPCN = dvalue;
      }

      else if (varname=="a0"){
	dvalue = atof(varvalue.Data());
	fIASlopeA[0] = dvalue;
      }
      else if (varname=="deltaa0"){
	dvalue = atof(varvalue.Data());
	fDelta_IASlopeA[0] = dvalue;
      }
      else if (varname=="a1"){
	dvalue = atof(varvalue.Data());
	fIASlopeA[1] = dvalue;
      }
      else if (varname=="deltaa1"){
	dvalue = atof(varvalue.Data());
	fDelta_IASlopeA[1] = dvalue;
      }
      else if (varname=="a2"){
	dvalue = atof(varvalue.Data());
	fIASlopeA[2] = dvalue;
      }
      else if (varname=="deltaa2"){
	dvalue = atof(varvalue.Data());
	fDelta_IASlopeA[2] = dvalue;
      }
      else if (varname=="a3"){
	dvalue = atof(varvalue.Data());
	fIASlopeA[3] = dvalue;
      }
      else if (varname=="deltaa3"){
	dvalue = atof(varvalue.Data());
	fDelta_IASlopeA[3] = dvalue;
      }


      
    }
  }
  QwMessage<<"patternMax = "<<fAccumulatePatternMax<<" deltaAq "<<fChargeAsymPrecision<<QwLog::endl;
  QwMessage<<"Optimal values - IA ["<<fOptimalIA<<"] PC+["<<fOptimalPCP<<"] PC-["<<fOptimalPCN<<"]"<<QwLog::endl;
  for (Int_t i=0;i<4;i++)
    QwMessage<<"Slope A"<<i<<" "<<fIASlopeA[i]<<"+-"<<fDelta_IASlopeA[i]<<QwLog::endl;
};


/*****************************************************************/
void QwHelicityCorrelatedFeedback::FeedIASetPoint(){
  //  Let's clear the values for the scan data & set "not clean".
  QwMessage<<"QwHelicityCorrelatedFeedback::FeedIASetPoint() "<<fChargeAsymmetry<<"+/-"<<fChargeAsymmetryError<<QwLog::endl;
  QwMessage<<"Current tgt charge asym "<<fCurrentChargeAsymmetry.GetHardwareSum()<<QwLog::endl;
  QwMessage<<"Previous tgt charge asym"<<fPreviousChargeAsymmetry.GetHardwareSum()<<QwLog::endl;
  QwMessage<<"Current IA charge asym"<<fCurrentIAAsymmetry.GetHardwareSum()<<QwLog::endl;
  QwMessage<<"Previous IA charge asym"<<fPreviousIAAsymmetry.GetHardwareSum()<<QwLog::endl;


  

  //fScanCtrl.SCNSetValue(1,0);
  //fScanCtrl.SCNSetValue(2,0);
  //fScanCtrl.CheckScan();
  //fScanCtrl.PrintScanInfo();
  //fScanCtrl.Close();  
  fEPICSCtrl.Print_Qasym_Ctrls();
};
/*****************************************************************/
void QwHelicityCorrelatedFeedback::FeedPCPos(){
};
/*****************************************************************/
void QwHelicityCorrelatedFeedback::FeedPCNeg(){
};
/*****************************************************************/
void QwHelicityCorrelatedFeedback::LogParameters(){
  out_file = fopen("Feedback_log.txt", "a");
  fprintf(out_file," Feedback at %d current A_q:%5.8f+/-%5.8f IA:%5.3f PCP:%5.3f PCN:%5.3f \n",fQuartetNumber,fCurrentChargeAsymmetry.GetHardwareSum(),fCurrentChargeAsymmetry.GetHardwareSumError(),fCurrentIA,fCurrentPCP,fCurrentPCN);
  fclose(out_file);
};
/*****************************************************************/
void QwHelicityCorrelatedFeedback::UpdateGMClean(Int_t state){
  //fScanCtrl.Open();
  if (state==0)
    fScanCtrl.SCNSetStatus(SCN_INT_NOT);
  if (state)
    fScanCtrl.SCNSetStatus(SCN_INT_CLN);
  fScanCtrl.CheckScan();
  fScanCtrl.PrintScanInfo();
  fScanCtrl.Close();  
};
/*****************************************************************/
void QwHelicityCorrelatedFeedback::UpdateGMScanParameters(){
};
/*****************************************************************/
Bool_t QwHelicityCorrelatedFeedback::IsAqPrecisionGood(){
  QwBeamCharge tmp;
  tmp.InitializeChannel("q_targ","derived");
  Bool_t status=kFALSE;
  GetTargetChargeStat();
  if (fChargeAsymmetry==-1 && fChargeAsymmetryError == -1 && fChargeAsymmetryWidth==-1){//target asymmetry not published or accesible
    QwError<<"target asymmetry not published or accesible"<<QwLog::endl;
    status=kFALSE;
  }
  fChargeAsymmetryError=fChargeAsymmetryError*1e+6;//converts to ppm
  fChargeAsymmetry=fChargeAsymmetry*1e+6;//converts to ppm
  if (fChargeAsymmetryError>fChargeAsymPrecision){
    QwError<<"Charge Asymmetry precision not reached current value "<<fChargeAsymmetryError<<" Expected "<<fChargeAsymPrecision<<QwLog::endl;
    status=kFALSE;
  }
  else{
    QwError<<"Charge Asymmetry precision current value "<<fChargeAsymmetryError<<" Expected "<<fChargeAsymPrecision<<QwLog::endl;
    status=kTRUE;
  }
  if(fGoodPatternCounter==0){
    fChargeAsymmetry0=fTargetCharge;
    fPreviousChargeAsymmetry=fTargetCharge;
    fCurrentChargeAsymmetry=fTargetCharge;
  }
  else{
    tmp=fCurrentIAAsymmetry;
    fCurrentIAAsymmetry-=fPreviousIAAsymmetry;
    fCurrentIAAsymmetry-=fPreviousChargeAsymmetry;
    fPreviousIAAsymmetry=tmp;

    fPreviousChargeAsymmetry=fCurrentChargeAsymmetry;
    fCurrentChargeAsymmetry=fTargetCharge;
  }
  

  return status;
};
/*****************************************************************/
Bool_t QwHelicityCorrelatedFeedback::IsPatternsAccumulated(){
  if (fGoodPatternCounter>=fAccumulatePatternMax)
    return kTRUE;

  return kFALSE;
};

/*****************************************************************/
Bool_t QwHelicityCorrelatedFeedback::IsPatternsAccumulated(Int_t mode){
  Int_t patterns=-1;
  switch(mode){
  case 0:
    patterns=fHelModeGoodPatternCounter[0];
    break;
  case 1:
    patterns=fHelModeGoodPatternCounter[1];
    break;
  case 2:
    patterns=fHelModeGoodPatternCounter[2];
    break;
  case 3:
    patterns=fHelModeGoodPatternCounter[3];
    break;	    
  }
  if (patterns>=fAccumulatePatternMax)
    return kTRUE;

  return kFALSE;
};
/*****************************************************************/
Int_t QwHelicityCorrelatedFeedback::GetLastGoodHelicityPatternType(){
  return fCurrentHelPatMode;
};
/*****************************************************************/

//*****************************************************************
/**
 * Calculate asymmetries for the current pattern.
 */
void  QwHelicityCorrelatedFeedback::CalculateAsymmetry()
{

  Bool_t localdebug=kFALSE;

  if(localdebug)  std::cout<<"Entering QwHelicityPattern::CalculateAsymmetry \n";

  Int_t plushel  = 1;
  Int_t minushel = 0;
  Int_t checkhel = 0;
  Bool_t firstplushel=kTRUE;
  Bool_t firstminushel=kTRUE;

  fPositiveHelicitySum.ClearEventData();
  fNegativeHelicitySum.ClearEventData();

  if (fIgnoreHelicity){
    //  Don't check to see if we have equal numbers of even and odd helicity states in this pattern.
    //  Build an asymmetry with even-parity phases as "+" and odd-parity phases as "-"
    for (size_t i = 0; i < (size_t) fPatternSize; i++) {
      Int_t localhel = 1;
      for (size_t j = 0; j < (size_t) fPatternSize/2; j++) {
	localhel ^= ((i >> j)&0x1);
      }
      if (localhel == plushel) {
	if (firstplushel) {
	  fPositiveHelicitySum = fEvents.at(i);
	  firstplushel = kFALSE;
	} else {
	  fPositiveHelicitySum += fEvents.at(i);
	}
      } else if (localhel == minushel) {
	if (firstminushel) {
	  fNegativeHelicitySum = fEvents.at(i);
	  firstminushel = kFALSE;
	} else {
	  fNegativeHelicitySum += fEvents.at(i);
	}
      }
    }
  } else {
    //  
    fCurrentHelPat=0;
    for (size_t i = 0; i < (size_t) fPatternSize; i++) {
      if (fHelicity[i] == plushel) {
	if (fPatternSize==4){//currently works only for pattern size of 4
	  switch(i){
	  case 0:
	    fCurrentHelPat+=1;
	    break;
	  case 1:
	    fCurrentHelPat+=10;
	    break;
	  case 2:
	    fCurrentHelPat+=100;
	    break;
	  case 3:
	    fCurrentHelPat+=1000;
	    break;	    
	  }
	}

	if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:: here filling fPositiveHelicitySum \n";
	if (firstplushel) {
	  if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:: with = \n";
	  fPositiveHelicitySum = fEvents.at(i);
	  firstplushel = kFALSE;
	} else {
	  if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:: with += \n";
	  fPositiveHelicitySum += fEvents.at(i);
	}
	checkhel += 1;
      } else if (fHelicity[i] == minushel) {
	if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:: here filling fNegativeHelicitySum \n";
	if (firstminushel) {
	  if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:: with = \n";
	  fNegativeHelicitySum = fEvents.at(i);
	  firstminushel = kFALSE;
	} else {
	  if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:: with += \n";
	  fNegativeHelicitySum += fEvents.at(i);
	}
	checkhel -= 1;
      } else {
	QwError<< "QwHelicityPattern::CalculateAsymmetry =="
	       << "Helicity should be "<<plushel<<" or "<<minushel<<" but is"<< fHelicity[i]
	       << "; Asymmetry computation aborted!"<<QwLog::endl;
	ClearEventData();
	i = fPatternSize;
	checkhel = -9999;
	// This is an unknown helicity event.
      }
      
    }

  }

  if (checkhel == -9999) {
    //do nothing the asymmetry computation has been aborted earlier in this function
    fPatternIsGood = kFALSE;
  } else if (checkhel!=0) {
    fPatternIsGood = kFALSE;
    // there is a different number of plus and minus helicity window.
    QwError<<" QwHelicityPattern::CalculateAsymmetry == \n"
	   <<" you do not have the same number of positive and negative \n"
	   <<" impossible to compute assymetry \n"
	   <<" dropping every thing -- pattern number ="<<fCurrentPatternNumber<<QwLog::endl;
  } else {
    //  This is a good pattern.
    //  Calculate the asymmetry.

    //Now set the HelPatMode 
    if (fPreviousHelPat){
      if (fPreviousHelPat==kHelPat1 && fCurrentHelPat==kHelPat1)
	fCurrentHelPatMode=1;
      else if (fPreviousHelPat==kHelPat1 && fCurrentHelPat==kHelPat2)
	fCurrentHelPatMode=2;
      else if (fPreviousHelPat==kHelPat2 && fCurrentHelPat==kHelPat1)
	fCurrentHelPatMode=3;
      else if (fPreviousHelPat==kHelPat2 && fCurrentHelPat==kHelPat2)
	fCurrentHelPatMode=4;
      else
	fCurrentHelPatMode=-1;//error
    }
    //save the current pattern size to previouspat
    if (localdebug) QwMessage<<"QwHelicityPattern::CalculateAsymmetry current helpat is "<<fCurrentHelPat<<" Prev pat "<<fPreviousHelPat<<" Mode "<<fCurrentHelPatMode<<" \n"; 
    fPreviousHelPat=fCurrentHelPat;
    
    fPatternIsGood = kTRUE;
    fGoodPatternCounter++;//increment the quartet number - reset after each feedback operation
    fQuartetNumber++;//Then increment the quartet number - continously count
    //std::cout<<" quartet count ="<<fQuartetNumber<<"\n";

    fYield.Sum(fPositiveHelicitySum,fNegativeHelicitySum);
    fDifference.Difference(fPositiveHelicitySum,fNegativeHelicitySum);
    if (! fIgnoreHelicity){
      //  Only blind the difference if we're using the real helicity.
      fBlinder.Blind(fDifference,fYield);
    }
    fAsymmetry.Ratio(fDifference,fYield);

    /*
      With additional two asymmetry calculations
      Don't blind them!

      quartet pattern + - - +
                      1 2 3 4
                      fAsymmetry  = (1+4)-(2+3)/(1+2+3+4)
                      fAsymmetry1 = (1+2)-(3+4)/(1+2+3+4)
                      fAsymmetry2 = (1+3)-(2+4)/(1+2+3+4)
    */
    if (fEnableAlternateAsym) {
      //  fAsymmetry1:  (first 1/2 pattern - second 1/2 pattern)/fYield
      fPositiveHelicitySum.ClearEventData();
      fNegativeHelicitySum.ClearEventData();
      fPositiveHelicitySum = fEvents.at(0);
      fNegativeHelicitySum = fEvents.at(fPatternSize/2);
      if (fPatternSize/2 > 1){
	for (size_t i = 1; i < (size_t) fPatternSize/2 ; i++){
	  fPositiveHelicitySum += fEvents.at(i);
	  fNegativeHelicitySum += fEvents.at(fPatternSize/2 +i);
	}
      }
      fAlternateDiff.ClearEventData();
      fAlternateDiff.Difference(fPositiveHelicitySum,fNegativeHelicitySum);
      //  Do not blind this helicity-uncorrelated difference.
      fAsymmetry1.Ratio(fAlternateDiff,fYield);
      //  fAsymmetry2:  (even events - odd events)/fYield
      //  Only build fAsymmetry2 if fPatternSize>2.
      if (fPatternSize > 2) {
	fPositiveHelicitySum.ClearEventData();
	fNegativeHelicitySum.ClearEventData();
	fPositiveHelicitySum = fEvents.at(0);
	fNegativeHelicitySum = fEvents.at(1);
	if (fPatternSize/2 > 1){
	  for (size_t i = 1; i < (size_t) fPatternSize/2 ; i++){
	    fPositiveHelicitySum += fEvents.at(2*i);
	    fNegativeHelicitySum += fEvents.at(2*i + 1);
	  }
	}
	fAlternateDiff.ClearEventData();
	fAlternateDiff.Difference(fPositiveHelicitySum,fNegativeHelicitySum);
	//  Do not blind this helicity-uncorrelated difference.
	fAsymmetry2.Ratio(fAlternateDiff,fYield);
      }
    }

    // Accumulate the burst and running sums
    if (fEnableBurstSum)   AccumulateBurstSum();
    if (fEnableRunningSum) {      
      AccumulateRunningSum();
    }
    
    if (localdebug) QwDebug << " pattern number =" << fQuartetNumber << QwLog::endl;
  }

  return;
};


void QwHelicityCorrelatedFeedback::AccumulateRunningSum(){
  QwHelicityPattern::AccumulateRunningSum();

  switch(fCurrentHelPatMode){
  case 1:
    fFBRunningAsymmetry[0].AccumulateRunningSum(fAsymmetry);
    fHelModeGoodPatternCounter[0]++;
    break;
  case 2:
    fFBRunningAsymmetry[1].AccumulateRunningSum(fAsymmetry);
    fHelModeGoodPatternCounter[1]++;
    break;
  case 3:
    fFBRunningAsymmetry[2].AccumulateRunningSum(fAsymmetry);
    fHelModeGoodPatternCounter[2]++;
    break;
  case 4:
    fFBRunningAsymmetry[3].AccumulateRunningSum(fAsymmetry);
    fHelModeGoodPatternCounter[3]++;
    break;
  }
};



void QwHelicityCorrelatedFeedback::CalculateRunningAverage(){
  QwHelicityPattern::CalculateRunningAverage();
};

void QwHelicityCorrelatedFeedback::CalculateRunningAverage(Int_t mode){

  switch(mode){
  case 1:
    fFBRunningAsymmetry[0].CalculateRunningAverage();
    break;
  case 2:
    fFBRunningAsymmetry[1].CalculateRunningAverage();
    break;
  case 3:
    fFBRunningAsymmetry[2].CalculateRunningAverage();
    break;
  case 4:
    fFBRunningAsymmetry[3].CalculateRunningAverage();
    break;
  }
};


//*****************************************************************
/**
/// \brief retrieves the target charge asymmetry,asymmetry error ,asymmetry width
*/
void QwHelicityCorrelatedFeedback::GetTargetChargeStat(){
  fRunningAsymmetry.CalculateRunningAverage();
  if (fRunningAsymmetry.RequestExternalValue("q_targ",&fTargetCharge)){
    fTargetCharge.PrintInfo();
    fChargeAsymmetry=fTargetCharge.GetHardwareSum();
    fChargeAsymmetryError=fTargetCharge.GetHardwareSumError();
    fChargeAsymmetryWidth=fTargetCharge.GetHardwareSumM2();
    return ;
  }
  QwError << " Could not get external value setting parameters to  q_targ" <<QwLog::endl;
  fChargeAsymmetry=-1;
  fChargeAsymmetryError=-1;
  fChargeAsymmetryWidth=-1;

  return;  
};

//*****************************************************************
/**
 * Clear the running sums of yield, difference and asymmetry.
 * Also clear the running burst sums if enabled.
 */
void  QwHelicityCorrelatedFeedback::ClearRunningSum()
{
  QwHelicityPattern::ClearRunningSum();
  fGoodPatternCounter=0;//Reset after each feedback operation
}

void  QwHelicityCorrelatedFeedback::ClearRunningSum(Int_t mode)
{
  switch(mode){
  case 1:
    fFBRunningAsymmetry[0].ClearEventData();
    fHelModeGoodPatternCounter[0]=0;
    break;
  case 2:
    fFBRunningAsymmetry[1].ClearEventData();
    fHelModeGoodPatternCounter[1]=0;
    break;
  case 3:
    fFBRunningAsymmetry[2].ClearEventData();
    fHelModeGoodPatternCounter[2]=0;
    break;
  case 4:
    fHelModeGoodPatternCounter[3]=0;
    fFBRunningAsymmetry[3].ClearEventData();
    break;
  }

}


