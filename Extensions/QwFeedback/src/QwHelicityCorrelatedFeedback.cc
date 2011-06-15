/**********************************************************\
* File: QwHelicityCorrelatedFeedback.cc                   *
*                                                         *
* Author:Author: Rakitha Beminiwattha                     *
* Time-stamp:Time-stamp: <2010-09-28>                     *
\**********************************************************/

#include "QwHelicityCorrelatedFeedback.h"
#include "TSystem.h"

/*****************************************************************/


/**
 * Defines configuration options using QwOptions functionality.
 * @param options Options object
 */
void QwHelicityCorrelatedFeedback::DefineOptions(QwOptions &options)
{

  options.AddOptions("Helicity Correlated Feedback")("Auto-IHWP-Flip", po::value<bool>()->default_value(false)->zero_tokens(),"Set Half wave plate IN. The default is Half wave plate OUT");
  options.AddOptions("Helicity Correlated Feedback")("Half-wave-plate-revert", po::value<bool>()->default_value(false)->zero_tokens(),"Revert half-wave plate status. The default is determined via EPIC");
  //options.AddOptions("Helicity Correlated Feedback")("Half-wave-plate-OUT", po::value<bool>()->default_value(true)->zero_tokens(),"Half wave plate OUT");
  options.AddOptions("Helicity Correlated Feedback")("PITA-Feedback", po::value<bool>()->default_value(false)->zero_tokens(),"Run the PITA charge feedback");
  options.AddOptions("Helicity Correlated Feedback")("IA-Feedback", po::value<bool>()->default_value(false)->zero_tokens(),"Run the IA charge feedback");
  options.AddOptions("Helicity Correlated Feedback")("HA-IA-Feedback", po::value<bool>()->default_value(false)->zero_tokens(),"Run the Hall A IA charge feedback");
  
};

/*****************************************************************/
void QwHelicityCorrelatedFeedback::ProcessOptions(QwOptions &options)
{
  QwHelicityPattern::ProcessOptions(options);

  fHalfWaveRevert      = options.GetValue<bool>("Half-wave-plate-revert");
  fHalfWavePlateStatus = GetHalfWavePlateState();
  fAutoIHWP = options.GetValue<bool>("Auto-IHWP-Flip");



  printf("NOTICE \n QwFeedback got the half-wave plate status %s\n", fHalfWavePlateStatus.Data());

  if(fHalfWavePlateStatus.Contains("IN")) {
    if(fHalfWaveRevert) fHalfWaveIN = false;
    else                fHalfWaveIN = true;
  }
  else {
    if(fHalfWaveRevert) fHalfWaveIN = true;
    else                fHalfWaveIN = false;
  }

  fHalfWaveOUT = !fHalfWaveIN;

  if (fHalfWaveIN)
    printf("NOTICE \n Half-wave-plate-IN\n");
  else
    printf("NOTICE \n Half-wave-plate-OUT\n");


 


  fPITAFB = options.GetValue<bool>("PITA-Feedback");
  fHAIAFB = options.GetValue<bool>("HA-IA-Feedback");
  fIAFB   = options.GetValue<bool>("IA-Feedback"); 

  if (fPITAFB)
    printf("NOTICE \n   PITA-Feedback is running.\n");
  else
    printf("NOTICE \n   PITA-Feedback is not running.\n");
  if (fIAFB)
    printf("NOTICE \n   IA-Feedback is running.\n");
  else
    printf("NOTICE \n   IA-Feedback is not running.\n");
  if (fHAIAFB)
    printf("NOTICE \n   Hall A IA-Feedback is running.\n");
  else
    printf("NOTICE \n   Hall A IA-Feedback is not running.\n");

  if (!fPITAFB && !fIAFB && !fHAIAFB){//no correction applied.
    fEPICSCtrl.Set_FeedbackStatus(0);
    exit(1); 
  }

  if (fAutoIHWP){
    printf("NOTICE \n   AUTO IHWP Flip is ON \n");
    //exit(1);
  }else{
    printf("NOTICE \n   AUTO IHWP Flip is OFF \n");
    //exit(1);
  }
  //exit(1);
};

/*****************************************************************/
void QwHelicityCorrelatedFeedback::LoadParameterFile(TString filename){
  char buffer [10];
  TString prev_IHWP_State;

  QwParameterFile mapstr(filename.Data());  //Open the file
  TString varname, varvalue;
  UInt_t value;
  Double_t dvalue;
  fIASetpointlow=11000;
  fIASetpointup=40000;
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
      else if (varname=="ha_patterns"){
	value = QwParameterFile::GetUInt(varvalue);
	fHAAccumulatePatternMax=value;
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
      else if (varname=="haa0"){
	dvalue = atof(varvalue.Data());
	fHAIASlopeA[0] = dvalue;
      }
      else if (varname=="haa1"){
	dvalue = atof(varvalue.Data());
	fHAIASlopeA[1] = dvalue;
      }
      else if (varname=="haa2"){
	dvalue = atof(varvalue.Data());
	fHAIASlopeA[2] = dvalue;
      }
      else if (varname=="haa3"){
	dvalue = atof(varvalue.Data());
	fHAIASlopeA[3] = dvalue;
      }
      else if (varname=="ia_low"){
	dvalue = atof(varvalue.Data());
	if (dvalue>0)
	  fIASetpointlow = dvalue;
      }
      else if (varname=="ia_up"){
	dvalue = atof(varvalue.Data());
	if (dvalue>0)
	  fIASetpointup = dvalue;
      }
      else if (varname=="pitaslope_in"){
	dvalue = atof(varvalue.Data());
	fPITASlopeIN = dvalue;
      }
      else if (varname=="pitaslope_out"){

	dvalue = atof(varvalue.Data());
	fPITASlopeOUT = dvalue;
      }
      else if (varname=="pc_pos_t0_in"){
	dvalue = atof(varvalue.Data());
	fPITASetpointPOS_t0_IN = dvalue;
      } 
      else if (varname=="pc_neg_t0_in"){
	dvalue = atof(varvalue.Data());
	fPITASetpointNEG_t0_IN = dvalue;
      } 
      else if (varname=="pc_pos_t0_out"){
	dvalue = atof(varvalue.Data());
	fPITASetpointPOS_t0_OUT = dvalue;
      }
      else if (varname=="pc_neg_t0_out"){
	dvalue = atof(varvalue.Data());
	fPITASetpointNEG_t0_OUT = dvalue;
      } 
      else if (varname=="pc_up"){
	dvalue = atof(varvalue.Data());
	if (dvalue>0)
	  fPITASetpointup = dvalue;
      }
      else if (varname=="pc_low"){
	dvalue = atof(varvalue.Data());
	if (dvalue>0)
	  fPITASetpointlow = dvalue;
      }else if (varname=="min_charge_asym"){  
	dvalue = atof(varvalue.Data());
	if (dvalue>0)
	  fPITA_MIN_Charge_asym = dvalue;
      } else if (varname=="damping"){  
	dvalue = atof(varvalue.Data());
	if (dvalue>0)
	  fFeedbackDamping=kTRUE;
	else
	  fFeedbackDamping=kFALSE;
      } 


      
    }
  }

  
  QwMessage<<"Hall C patternMax = "<<fAccumulatePatternMax<<" Hall C patternMax = "<<fHAAccumulatePatternMax<<" deltaAq "<<fChargeAsymPrecision<<"ppm"<<QwLog::endl;
  //QwMessage<<"Optimal values - IA ["<<fOptimalIA<<"] PC+["<<fOptimalPCP<<"] PC-["<<fOptimalPCN<<"]"<<QwLog::endl;
  QwMessage<<"IA DAC counts limits "<<fIASetpointlow<<" to "<< fIASetpointup <<QwLog::endl;
  for (Int_t i=0;i<kHelModes;i++)
    QwMessage<<"Slope A["<<i<<"] "<<fIASlopeA[i]<<"+-"<<fDelta_IASlopeA[i]<<QwLog::endl;
  QwMessage<<"PITA slopes: H-wave IN "<<fPITASlopeIN<<" H-wave OUT "<<fPITASlopeOUT<<QwLog::endl;
  QwMessage<<"PC dac limits "<<fPITASetpointlow<<" to "<<fPITASetpointup<<QwLog::endl;

  fInitialCorrection=kFALSE;
  QwMessage<<"t_0 corrections for IN  +["<<fPITASetpointPOS_t0_IN<<"] -["<<fPITASetpointNEG_t0_IN<<"]"<<QwLog::endl;
  QwMessage<<"t_0 corrections for OUT +["<<fPITASetpointPOS_t0_OUT<<"] -["<<fPITASetpointNEG_t0_OUT<<"]"<<QwLog::endl;

  if (fAutoIHWP){
    if (IHWP_State!=NULL){
      fgets(buffer , 10 ,IHWP_State);//read the previous IHWP mode
      //printf("NOTICE \n %s \n",buffer);
      prev_IHWP_State=buffer;
      if (prev_IHWP_State.Contains("IN") || prev_IHWP_State.Contains("OUT")){
	if (fHalfWavePlateStatus.IsNull())//EPICS IHWP state is missing
	  fHalfWavePlateStatus=prev_IHWP_State;//in case EPICS IHWP state is not existing set the previous run IHWP state
      }
    }
    else
      prev_IHWP_State="";
    if (!prev_IHWP_State.Contains("IN") && !prev_IHWP_State.Contains("OUT") ){//first time the file created
      if (IHWP_State!=NULL)
	fclose(IHWP_State); 
      IHWP_State = fopen("/local/scratch/qweak/Feedback_IHWP.txt", "w");
      printf("NOTICE \n previous half-wave plate status not found. Writing current IHWP state %s \n",fHalfWavePlateStatus.Data());
      if (IHWP_State!=NULL){
	fprintf(IHWP_State,"%s",fHalfWavePlateStatus.Data());
	fclose(IHWP_State);
      }
    }
    else{
      if (prev_IHWP_State!=fHalfWavePlateStatus){
	printf("NOTICE \n Half-wave plate status has changed from %s to %s \n",prev_IHWP_State.Data(),fHalfWavePlateStatus.Data());
	if (fHalfWaveIN){
	  if (fPITASetpointPOS_t0_IN && fPITASetpointNEG){
	    fEPICSCtrl.Set_Pockels_Cell_plus(fPITASetpointPOS_t0_IN);
	    fEPICSCtrl.Set_Pockels_Cell_minus(fPITASetpointNEG_t0_IN);
	  }
	} else{
	  if (fPITASetpointPOS_t0_OUT && fPITASetpointNEG_t0_OUT){
	    fEPICSCtrl.Set_Pockels_Cell_plus(fPITASetpointPOS_t0_OUT);
	    fEPICSCtrl.Set_Pockels_Cell_minus(fPITASetpointNEG_t0_OUT);
	  }
	}
      }
      printf("NOTICE \n previous half-wave plate status %s\n",prev_IHWP_State.Data());
      if (IHWP_State!=NULL)
	fclose(IHWP_State); 
      IHWP_State = fopen("/local/scratch/qweak/Feedback_IHWP.txt", "w");
      printf("NOTICE \n previous half-wave plate status found. Updating new IHWP %s \n",fHalfWavePlateStatus.Data());
      if (IHWP_State!=NULL){
	fprintf(IHWP_State,"%s",fHalfWavePlateStatus.Data());   
	fclose(IHWP_State);
      }
 
    }  
  }

};


/*****************************************************************/
void QwHelicityCorrelatedFeedback::FeedIASetPoint(Int_t mode){
  //calculate the new setpoint
  fEPICSCtrl.Get_HallCIA(mode,fPrevIASetpoint[mode]);
  if (fIASlopeA[mode]!=0)
    fIASetpoint[mode]=fPrevIASetpoint[mode] - fChargeAsym[mode]/fIASlopeA[mode];
  else
    fIASetpoint[mode]=fPrevIASetpoint[mode];
  
  if (fIASetpoint[mode]>fIASetpointup)
    fIASetpoint[mode]=fIASetpointup;
  else if (fIASetpoint[mode]<fIASetpointlow)
    fIASetpoint[mode]=fIASetpointlow;

  QwMessage<<"FeedIASetPoint("<<mode<<") "<<fChargeAsym[mode]<<"+/-"<<fChargeAsymError[mode]<<" new set point  "<<fIASetpoint[mode]<<QwLog::endl;
  //send the new IA setpoint 
  fEPICSCtrl.Set_HallCIA(mode,fIASetpoint[mode]);
  //updating the standard asymmetry statistics
  GetTargetChargeStat();
  fEPICSCtrl.Set_ChargeAsymmetry(fChargeAsymmetry,fChargeAsymmetryError,fChargeAsymmetryWidth);//updates the epics values


  //Greenmonster stuffs
  //fScanCtrl.SCNSetValue(1,0);
  //fScanCtrl.SCNSetValue(2,0);
  //fScanCtrl.CheckScan();
  //fScanCtrl.PrintScanInfo();
  //fScanCtrl.Close();  
};

/*****************************************************************/
void QwHelicityCorrelatedFeedback::FeedHAIASetPoint(Int_t mode){
  //calculate the new setpoint
  
  fEPICSCtrl.Get_HallAIA(mode,fPrevHAIASetpoint[mode]);
  if (fIASlopeA[mode]!=0){

    if (fHalfWaveIN)
      fHAIASetpoint[mode]=fPrevHAIASetpoint[mode] - fHAChargeAsym[mode]/fHAIASlopeA[0];//IHWP IN slope is stored at fHAIASlopeA[0]
    else
      fHAIASetpoint[mode]=fPrevHAIASetpoint[mode] - fHAChargeAsym[mode]/fHAIASlopeA[1];//IHWP OUT slope is stored at fHAIASlopeA[1]

  } else
    fHAIASetpoint[mode]=fPrevHAIASetpoint[mode];
  
  if ((fHAIASetpoint[mode]>fIASetpointup) || (fHAIASetpoint[mode]<fIASetpointlow)){//if correction is out-of bound no correction applied
    QwMessage<<"Hall A FeedIASetPoint out-of-bounds "<<fHAChargeAsym[mode]<<" +/- "<<fHAChargeAsymError[mode]<<" new set point[+]  "<<fHAIASetpoint[mode]<<" aborting correction for this time! "<<QwLog::endl;
    return;
  }

  QwMessage<<"FeedIASetPoint("<<mode<<") "<<fHAChargeAsym[mode]<<"+/-"<<fHAChargeAsymError[mode]<<" new set point  "<<fHAIASetpoint[mode]<<QwLog::endl;
  //send the new IA setpoint 
  for (Int_t i=0;i<4;i++)
    fEPICSCtrl.Set_HallAIA(i,fHAIASetpoint[mode]);//do the same correction to 4 DACs
  
  fScalerChargeRunningSum.ClearEventData();//reset the running sums
};


/*****************************************************************/
void QwHelicityCorrelatedFeedback::FeedPITASetPoints(){


  //calculate the new setpoint
  if (fHalfWaveIN)
    fPITASlope=fPITASlopeIN;
  else
    fPITASlope=fPITASlopeOUT;

  

  fEPICSCtrl.Get_Pockels_Cell_plus(fPrevPITASetpointPOS);
  fEPICSCtrl.Get_Pockels_Cell_minus(fPrevPITASetpointNEG);

  if (fPITASlope!=0) {
    Double_t correction = fChargeAsymmetry/fPITASlope;
    //damp the correction when charge asymmetry is close to zero
    if (fFeedbackDamping){
      if (TMath::Abs(fChargeAsymmetry)<0.01)//ppm
	correction*=0.01;
      else if (TMath::Abs(fChargeAsymmetry)<0.1)//ppm
	correction*=0.1;
      else if (TMath::Abs(fChargeAsymmetry)<1)//ppm
	correction*=0.25;
      else if (TMath::Abs(fChargeAsymmetry)<2)//ppm
	correction*=0.5; 
      else if (TMath::Abs(fChargeAsymmetry)<5)//ppm
	correction*=0.75;
    }

    fPITASetpointPOS=fPrevPITASetpointPOS + correction;
    fPITASetpointNEG=fPrevPITASetpointNEG - correction;


  } else {
    fPITASetpointPOS=fPrevPITASetpointPOS;
    fPITASetpointNEG=fPrevPITASetpointNEG;
  }




  if ((fPITASetpointPOS>fPITASetpointup) || (fPITASetpointPOS<fPITASetpointlow) || (fPITASetpointNEG>fPITASetpointup) || (fPITASetpointNEG<fPITASetpointlow)){//if correction is out-of bound
    QwMessage<<"FeedPITASetPoint out-of-bounds "<<fChargeAsymmetry<<" +/- "<<fChargeAsymmetryError<<" new set point[+]  "<<fPITASetpointPOS<<" [-] "<<fPITASetpointNEG<<" aborting correction for this time! "<<QwLog::endl;
    return;//do nothing but can we reset to t_0 correction and start all over???????
  }

  QwMessage<<"FeedPITASetPoint "<<" "<<fChargeAsymmetry<<" +/- "<<fChargeAsymmetryError<<" new set point[+]  "<<fPITASetpointPOS<<" [-] "<<fPITASetpointNEG<<QwLog::endl;
  
  //send the new PITA setpoint
  fEPICSCtrl.Set_Pockels_Cell_plus(fPITASetpointPOS);
  fEPICSCtrl.Set_Pockels_Cell_minus(fPITASetpointNEG);
  fEPICSCtrl.Set_ChargeAsymmetry(fChargeAsymmetry,fChargeAsymmetryError,fChargeAsymmetryWidth);//updates the epics values
  /*
  if (fFeedbackStatus){
      fFeedbackStatus=kFALSE;
      fEPICSCtrl.Set_FeedbackStatus(2.0);//Setting to 2 seems to be not updating. Check back later
  }
  */

  //Greenmonster stuffs
  //fScanCtrl.SCNSetValue(1,0);
  //fScanCtrl.SCNSetValue(2,0);
  //fScanCtrl.CheckScan();
  //fScanCtrl.PrintScanInfo();
  //fScanCtrl.Close();  
  ClearRunningSum();//reset the running sum only if PITA correction applied
};

/*****************************************************************/
void QwHelicityCorrelatedFeedback::FeedPCPos(){
}; 
/*****************************************************************/
void QwHelicityCorrelatedFeedback::FeedPCNeg(){
}; 

/*****************************************************************/
void QwHelicityCorrelatedFeedback::LogParameters(Int_t mode){
  out_file_IA = fopen("/local/scratch/qweak/Feedback_IA_log.txt", "a");
  //   out_file_IA = fopen("/dev/shm/Feedback_IA_log.txt", "a");    
  //  fprintf(out_file," Feedback at %d current A_q[%d]:%5.8f+/-%5.8f IA Setpoint:%5.3f  IA Previous Setpoint:%5.3f\n",fQuartetNumber,mode,fChargeAsym[mode],fChargeAsymError[mode],fIASetpoint[mode],fPrevIASetpoint[mode]);
  fprintf(out_file_IA," %10.0d A_q[%1.0d] %20.4f +/-  %20.4f  %20.2f  %20.2f\n",fQuartetNumber,mode,fChargeAsym[mode],fChargeAsymError[mode],fIASetpoint[mode],fPrevIASetpoint[mode]);
  fclose(out_file_IA);
};

/*****************************************************************/
void QwHelicityCorrelatedFeedback::LogParameters(){
  out_file_PITA = fopen("/local/scratch/qweak/Feedback_PITA_log.txt", "a");
  // out_file_PITA = fopen("/dev/shm/Feedback_PITA_log.txt", "a"); 
  fprintf(out_file_PITA,"%10.0d %+22.2f %16.2f %16.2f %26.2f %26.2f %26.2f %26.2f \n",fQuartetNumber,fChargeAsymmetry,fChargeAsymmetryError,TMath::Abs(fPITASetpointPOS-fPrevPITASetpointPOS),fPITASetpointPOS,fPrevPITASetpointPOS,fPITASetpointNEG,fPrevPITASetpointNEG);
  fclose(out_file_PITA);

  if(TMath::Abs(fChargeAsymmetry)<5){
    //These files save the last good PC hw count value for IHWP IN and OUT
    if (fHalfWaveIN){
      out_file_PC_IN_pos = fopen("/local/scratch/qweak/Last_good_PC_pos_IN", "w");//Open in write mode
      out_file_PC_IN_neg = fopen("/local/scratch/qweak/Last_good_PC_neg_IN", "w");//Open in write mode      
      fprintf(out_file_PC_IN_pos,"%5.0f \n",fPrevPITASetpointPOS);
      fprintf(out_file_PC_IN_neg,"%5.0f \n",fPrevPITASetpointNEG);
      fclose(out_file_PC_IN_pos);
      fclose(out_file_PC_IN_neg);      
    }else{
      out_file_PC_OUT_pos = fopen("/local/scratch/qweak/Last_good_PC_pos_OUT", "w");//Open in write mode
      out_file_PC_OUT_neg = fopen("/local/scratch/qweak/Last_good_PC_neg_OUT", "w");//Open in write mode
      fprintf(out_file_PC_OUT_pos,"%5.0f \n",fPrevPITASetpointPOS);
      fprintf(out_file_PC_OUT_neg,"%5.0f \n",fPrevPITASetpointNEG);      
      fclose(out_file_PC_OUT_pos);
      fclose(out_file_PC_OUT_neg);    
    }
  }
};

/*****************************************************************/
void QwHelicityCorrelatedFeedback::LogHAParameters(Int_t mode){
  
  out_file_HA_IA = fopen("/local/scratch/qweak/Feedback_HA_IA_log.txt", "a");
  //fQuartetNumber only available when we have good stable Hall C beam
  fprintf(out_file_HA_IA," %10.0d  %20.2f  %15.2f %15.0f %20.0f  %20.0f \n",fQuartetNumber,fHAChargeAsym[mode],fHAChargeAsymError[mode],TMath::Abs(fHAIASetpoint[mode]-fPrevHAIASetpoint[mode]),fHAIASetpoint[mode],fPrevHAIASetpoint[mode]);
  fclose(out_file_HA_IA);
  
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
  Bool_t status=kFALSE;
  GetTargetChargeStat();//call the calculate running sum routine and access the q_targ published value
  if (fChargeAsymmetry==-1 && fChargeAsymmetryError == -1 && fChargeAsymmetryWidth==-1){//target asymmetry not published or accesible
    QwError<<"target asymmetry not published or accesible"<<QwLog::endl;
    status=kFALSE;
  }
  fChargeAsymmetryError=fChargeAsymmetryError*1e+6;//converts to ppm
  fChargeAsymmetry=fChargeAsymmetry*1e+6;//converts to ppm
  fChargeAsymmetryWidth=fChargeAsymmetryWidth*1e+6;//converts to ppm
  if (fChargeAsymmetryError>fChargeAsymPrecision){
    QwError<<"Charge Asymmetry precision not reached current value "<<fChargeAsymmetryError<<" Expected "<<fChargeAsymPrecision<<QwLog::endl;
    status=kFALSE;
    fGoodPatternCounter=0;
  }
  else{
    QwError<<"Charge Asymmetry precision current value "<<fChargeAsymmetryError<<" Expected "<<fChargeAsymPrecision<<QwLog::endl;
    UpdateGMClean(0);//set to not clean
    FeedPITASetPoints();//set the new PITA values
    LogParameters();//Log PITA setting after feedback
    UpdateGMClean(1);//set back to clean
    status=kTRUE;
    fGoodPatternCounter=0;//Reset after each feedback operation
  }


  return status;
};

/*****************************************************************/
Bool_t QwHelicityCorrelatedFeedback::IsAqPrecisionGood(Int_t mode){
  if (mode<0 ||  mode>3){
    QwError << " Could not get external value setting parameters to  q_targ" <<QwLog::endl;
    return kFALSE;
  }
  QwMessage<<"IsAqPrecisionGood["<<mode<<"]\n";
  Bool_t HAstatus=kFALSE;
  Bool_t HCstatus=kFALSE;

  
  //GetTargetChargeStat(mode);
  GetHAChargeStat(mode);
  /*  
//commented out Hall C IA feedback correction
  if (fChargeAsym[mode]==-1 && fChargeAsymError[mode] == -1 && fChargeAsymWidth[mode]==-1){//target asymmetry not published or accesible
    QwError<<"target asymmetry not published or accesible"<<QwLog::endl;
    HCstatus=kFALSE;
  }else{
    fChargeAsymError[mode]=fChargeAsymError[mode]*1e+6;//converts to ppm
    fChargeAsym[mode]=fChargeAsym[mode]*1e+6;//converts to ppm
    fChargeAsymWidth[mode]=fChargeAsymWidth[mode]*1e+6;//converts to ppm
    if (fChargeAsymError[mode]>fChargeAsymPrecision){
      QwError<<"Charge Asymmetry["<<mode<<"] precision not reached current value "<<fChargeAsymError[mode]<<" Expected "<<fChargeAsymPrecision<<QwLog::endl;
      QwError<<"--------------------------------------------------------------------------------------------------------------------------------"<<QwLog::endl;
      HCstatus=kFALSE;
      fHelModeGoodPatternCounter[mode]=0;
    }else
      HCstatus=kTRUE;
  }
  */
  if (fHAChargeAsym[mode]==-1 && fHAChargeAsymError[mode] == -1 && fHAChargeAsymWidth[mode]==-1){//target asymmetry not published or accesible
    QwError<<"Hall A asymmetry not published or accesible"<<QwLog::endl;
    HAstatus=kFALSE;
  }else{
    fHAChargeAsymError[mode]=fHAChargeAsymError[mode]*1.0e+6;//converts to ppm
    fHAChargeAsym[mode]=fHAChargeAsym[mode]*1e+6;//converts to ppm
    fHAChargeAsymWidth[mode]=fHAChargeAsymWidth[mode]*1e+6;//converts to ppm
    if (fHAChargeAsymError[mode]>fChargeAsymPrecision){
      QwError<<"Hall A Charge Asymmetry["<<mode<<"] precision not reached current value "<<fHAChargeAsymError[mode]<<" Expected "<<fChargeAsymPrecision<<QwLog::endl;
      QwError<<"--------------------------------------------------------------------------------------------------------------------------------"<<QwLog::endl;
      HAstatus=kFALSE;
      fHelModeGoodPatternCounter[mode]=0;
    }else
      HAstatus=kTRUE;
  }
  
  
  if (HCstatus && fIAFB){
    QwError<<"Charge Asymmetry["<<mode<<"] precision current value "<<fChargeAsymError[mode]<<" Expected "<<fChargeAsymPrecision<<QwLog::endl;
    UpdateGMClean(0);//set to not clean
    FeedIASetPoint(mode);
    LogParameters(mode);  
    UpdateGMClean(1);//set back to clean
    fHelModeGoodPatternCounter[mode]=0;
  }
  if (HAstatus && fHAIAFB){
    QwError<<"Hall A Charge Asymmetry["<<mode<<"] precision current value "<<fHAChargeAsymError[mode]<<" Expected "<<fChargeAsymPrecision<<QwLog::endl;
    //UpdateGMClean(0);//set to not clean
    FeedHAIASetPoint(mode);
    LogHAParameters(mode);  
    //UpdateGMClean(1);//set back to clean
    fHelModeGoodPatternCounter[mode]=0;
  }

  //if ((HCstatus && fIAFB) || (HAstatus && fHAIAFB)){ //this could be a problem when we use both Hall A annd Hall C IA if only on hall gets the correction due to precision check
    //ClearRunningSum(mode);//reset the running sum
  //}
  

  return (HCstatus || HAstatus);
};
/*****************************************************************/
void QwHelicityCorrelatedFeedback::ApplyFeedbackCorrections(){
  //Hall A & C IA feedback 
  if (fIAFB || fHAIAFB){
    //    for (Int_t i=0;i<kHelModes;i++){
      if (IsPatternsAccumulated(0)){
	QwMessage<<"IsPatternsAccumulated for Mode[0] (Hall A IA)"<<QwLog::endl;
	IsAqPrecisionGood(0);//calling mode = 0 for HA IA feedback
      }
      //  }
  }
  //End IA feedback

  //PITA feedback
  if (fPITAFB){
    if (IsPatternsAccumulated()){
      fHalfWavePlateStatus = GetHalfWavePlateState();
      if(fHalfWavePlateStatus.Contains("IN")) {
	if(fHalfWaveRevert) fHalfWaveIN = false;
	else                fHalfWaveIN = true;
      }
      else {
	if(fHalfWaveRevert) fHalfWaveIN = true;
	else                fHalfWaveIN = false;
      }
      
      fHalfWaveOUT = !fHalfWaveIN;

      if (fHalfWaveIN)
	printf("NOTICE \n Half-wave-plate-IN\n");
      else
	printf("NOTICE \n Half-wave-plate-OUT\n");

      IsAqPrecisionGood();//PITA corrections initiate here
    }
  }
  //End PITA feedback

  return;

};

/*****************************************************************/
Bool_t QwHelicityCorrelatedFeedback::IsPatternsAccumulated(Int_t mode){
  if (fHelModeGoodPatternCounter[mode]>=fHAAccumulatePatternMax){
    //QwMessage<<"fHelModeGoodPatternCounter["<<mode<<"]\n";
    return kTRUE;
  }

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
	    //fCurrentHelPat+=1;
	    fCurrentHelPat+=1000;
	    break;
	  case 1:
	    //fCurrentHelPat+=10;
	    fCurrentHelPat+=100;
	    break;
	  case 2:
	    //fCurrentHelPat+=100;
	    fCurrentHelPat+=10;
	    break;
	  case 3:
	    //fCurrentHelPat+=1000;
	    fCurrentHelPat+=1;
	    break;	    
	  }
	}

	if (localdebug) 
	  std::cout<<"QwHelicityPattern::CalculateAsymmetry:: here filling fPositiveHelicitySum \n";
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
	fCurrentHelPatMode=0;
      else if (fPreviousHelPat==kHelPat1 && fCurrentHelPat==kHelPat2)
	fCurrentHelPatMode=1;
      else if (fPreviousHelPat==kHelPat2 && fCurrentHelPat==kHelPat1)
	fCurrentHelPatMode=2;
      else if (fPreviousHelPat==kHelPat2 && fCurrentHelPat==kHelPat2)
	fCurrentHelPatMode=3;
      else
	fCurrentHelPatMode=-1;//error
    }
    //save the current pattern size to previouspat
    if (localdebug) 
      QwMessage<<"QwHelicityPattern::CalculateAsymmetry current helpat is "<<fCurrentHelPat<<" Prev pat "<<fPreviousHelPat<<" Mode "<<fCurrentHelPatMode<<" \n"; 
    fPreviousHelPat=fCurrentHelPat;
    
    fPatternIsGood = kTRUE;
    fPatternCounter++;
    //std::cout<<" quartet count ="<<fQuartetNumber<<"\n";

    fYield.Sum(fPositiveHelicitySum,fNegativeHelicitySum);
    fDifference.Difference(fPositiveHelicitySum,fNegativeHelicitySum);
    if (! fIgnoreHelicity){
      //  Only blind the difference if we're using the real helicity.
      fBlinder.Blind(fDifference,fYield);
    }
    fAsymmetry.Ratio(fDifference,fYield);

    if (fAsymmetry.GetEventcutErrorFlag()==0){//good pattern
      //fPatternIsGood = kTRUE;
      fGoodPatternCounter++;//increment the quartet number - reset after each feedback operation
      fQuartetNumber++;//Then increment the quartet number - continously count
    }

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

  fAsymmetry.RequestExternalValue("sca_bcm", &fScalerCharge);
  //fScalerChargeRunningSum.PrintValue();
  fScalerChargeRunningSum.AccumulateRunningSum(fScalerCharge);
  if (fScalerCharge.GetDeviceErrorCode()==0 && fAsymmetry.GetEventcutErrorFlag()==0)
    fHelModeGoodPatternCounter[0]++;//update the good HA asymmetry counter


  switch(fCurrentHelPatMode){
  case 0:
    fFBRunningAsymmetry[0].AccumulateRunningSum(fAsymmetry);
    fHelModeGoodPatternCounter[0]++;
    break;
  case 1:
    fFBRunningAsymmetry[1].AccumulateRunningSum(fAsymmetry);
    fHelModeGoodPatternCounter[1]++;
    break;
  case 2:
    fFBRunningAsymmetry[2].AccumulateRunningSum(fAsymmetry);
    fHelModeGoodPatternCounter[2]++;
    break;
  case 3:
    fFBRunningAsymmetry[3].AccumulateRunningSum(fAsymmetry);
    fHelModeGoodPatternCounter[3]++;
    break;
  }

  
};



void QwHelicityCorrelatedFeedback::CalculateRunningAverage(){
  QwHelicityPattern::CalculateRunningAverage();
};

void QwHelicityCorrelatedFeedback::CalculateRunningAverage(Int_t mode){
  fFBRunningAsymmetry[mode].CalculateRunningAverage();
};


//*****************************************************************
/**
/// \brief retrieves the target charge asymmetry,asymmetry error ,asymmetry width
*/
void QwHelicityCorrelatedFeedback::GetTargetChargeStat(){
  fRunningAsymmetry.CalculateRunningAverage();
  if (fRunningAsymmetry.RequestExternalValue("q_targ",&fTargetCharge)){
    QwMessage<<"Reading published charge value stats"<<QwLog::endl;
    fTargetCharge.PrintInfo();
    fChargeAsymmetry=fTargetCharge.GetHardwareSum();
    fChargeAsymmetryError=fTargetCharge.GetHardwareSumError();
    fChargeAsymmetryWidth=fTargetCharge.GetHardwareSumWidth();
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
/// \brief retrieves the target charge asymmetry,asymmetry error ,asymmetry width
*/
void QwHelicityCorrelatedFeedback::GetTargetChargeStat(Int_t mode){
  if (!fIAFB){
     fChargeAsym[mode]=-1;
     fChargeAsymError[mode]=-1;
     fChargeAsymWidth[mode]=-1;
  }

  if (mode<0 ||  mode>3){
    QwError << " Could not get external value setting parameters to  q_targ" <<QwLog::endl;
    return;
  }
  /*
  fFBRunningAsymmetry[mode].CalculateRunningAverage();
  if (fFBRunningAsymmetry[mode].RequestExternalValue("q_targ",&fTargetCharge)){
    QwMessage<<"Reading published charge value stats"<<QwLog::endl;
    fTargetCharge.PrintInfo();
    fChargeAsym[mode]=fTargetCharge.GetHardwareSum();
    fChargeAsymError[mode]=fTargetCharge.GetHardwareSumError();
    fChargeAsymWidth[mode]=fTargetCharge.GetHardwareSumWidth();
    return ;
  }
  */
  //QwError << " Could not get external value setting parameters to  q_targ" <<QwLog::endl;
  fChargeAsym[mode]=-1;
  fChargeAsymError[mode]=-1;
  fChargeAsymWidth[mode]=-1;

  return;  
};

//*****************************************************************
/**
/// \brief retrieves the Hall A charge asymmetry,asymmetry error ,asymmetry width
*/
void QwHelicityCorrelatedFeedback::GetHAChargeStat(Int_t mode){

  if (!fHAIAFB){
     fHAChargeAsym[mode]=-1;
     fHAChargeAsymError[mode]=-1;
     fHAChargeAsymWidth[mode]=-1;
  }
  if (mode<0 ||  mode>3){
    QwError << " Could not get external value setting parameters to  sca_bcm " <<QwLog::endl;
    return;
  }
  //fFBRunningAsymmetry[mode].CalculateRunningAverage();
  fScalerChargeRunningSum.CalculateRunningAverage();
  //if (fFBRunningAsymmetry[mode].RequestExternalValue("sca_bcm",&fScalerCharge)){
    QwError<<"Reading published Hall A charge value stats "<<QwLog::endl;
    fScalerChargeRunningSum.PrintValue();
    fHAChargeAsym[mode]=fScalerChargeRunningSum.GetValue();
    fHAChargeAsymError[mode]=fScalerChargeRunningSum.GetValueError();
    fHAChargeAsymWidth[mode]=fScalerChargeRunningSum.GetValueWidth();
    QwError<<" Scaler asym "<<fHAChargeAsym[mode]<<" error "<<fHAChargeAsymError[mode]<<" width "<<fHAChargeAsymWidth[mode]<<QwLog::endl;
    return ;
    //}
    // QwError << " Could not get external value setting parameters to sca_bcm " <<QwLog::endl;
    //fHAChargeAsym[mode]=-1;
    //fHAChargeAsymError[mode]=-1;
    //fHAChargeAsymWidth[mode]=-1;

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
  
}

void  QwHelicityCorrelatedFeedback::ClearRunningSum(Int_t mode)
{
  fFBRunningAsymmetry[mode].ClearEventData();
  fHelModeGoodPatternCounter[mode]=0;

};

void  QwHelicityCorrelatedFeedback::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values){
  QwHelicityPattern::ConstructBranchAndVector(tree,prefix,values);
};

void  QwHelicityCorrelatedFeedback::FillTreeVector(std::vector<Double_t> &values) const{
  QwHelicityPattern::FillTreeVector(values);
};


TString  QwHelicityCorrelatedFeedback::GetHalfWavePlateState()
{
  TString plate_status = gSystem->GetFromPipe("caget -t -w 0.1 IGL1I00DI24_24M");
  return plate_status;
};


void QwHelicityCorrelatedFeedback::CheckFeedbackStatus()
{
  Double_t old_status = 0.0;
  old_status = fEPICSCtrl.Get_FeedbackStatus();
  if(old_status == 1.0) { 
    // EPIC says, feedback is on, but we start it now. 
    // qwfeedback was killed by "kill -9 option" or was ended improperly.
    fEPICSCtrl.Set_FeedbackStatus(0.0);
    fFeedbackStatus=kFALSE;
    gSystem->Sleep(4000); // wait 4 secs
  }
  fEPICSCtrl.Set_FeedbackStatus(1.0);
  fFeedbackStatus=kTRUE;
}
