#ifndef __QwEPICSCONTROL__
#define __QwEPICSCONTROL__



#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <time.h>
#include <iterator>


#include "Rtypes.h"

#include "TMath.h"
#include "TMatrix.h"
#include "TString.h"

#include "cadef.h"


class QwEPICSControl{
public:
  
  QwEPICSControl();
  ~QwEPICSControl();

  void Print_HallAIA(){
    Int_t status;
    //    Char_t tmp[30];
    //    status = ca_get(DBR_STRING, fIDHall_A_IA, tmp);
    //    status = ca_pend_io(10);
    //    std::cout << "Hall A IA value: " << tmp << std::endl;
    Double_t value;
    status = ca_get(DBR_DOUBLE, fIDHall_A_IA, &value);
    status = ca_pend_io(10);
    printf("Hall A IA value: %lf\n", value);
  };


  void Print_Qasym_Ctrls(){
    Int_t status;
    //    Char_t tmp[30];
    //    status = ca_get(DBR_STRING, fIDHall_A_IA, tmp);
    //    status = ca_pend_io(10);
    //    std::cout << "Hall A IA value: " << tmp << std::endl;
    Double_t value;

    status = ca_get(DBR_DOUBLE, fIDPockels_Cell_plus, &value);
    status = ca_pend_io(10);
    std::cout << "Pockels Cell plus HV value: " << value << std::endl;
    status = ca_get(DBR_DOUBLE, fIDPockels_Cell_minus, &value);
    status = ca_pend_io(10);
    std::cout << "Pockels Cell minus HV value: " << value << std::endl;

    status = ca_get(DBR_DOUBLE, fIDHall_C_IA_A0, &value);
    status = ca_pend_io(10);
    std::cout << "Hall C IA value A0: " << value << std::endl;
    status = ca_get(DBR_DOUBLE, fIDHall_C_IA_A1, &value);
    status = ca_pend_io(10);
    std::cout << "Hall C IA value A1: " << value << std::endl;
    status = ca_get(DBR_DOUBLE, fIDHall_C_IA_A2, &value);
    status = ca_pend_io(10);
    std::cout << "Hall C IA value A2: " << value << std::endl;
    status = ca_get(DBR_DOUBLE, fIDHall_C_IA_A3, &value);
    status = ca_pend_io(10);
    std::cout << "Hall C IA value A3: " << value << std::endl;
    
  };

  void Set_HallCIA(Int_t mode, Double_t &value){
    Int_t status;
    switch(mode){
    case 0:
      status = ca_put(DBR_DOUBLE, fIDHall_C_IA_A0, &value);
      status = ca_pend_io(10);
      status = ca_get(DBR_DOUBLE, fIDHall_C_IA_A0, &value);
      status = ca_pend_io(10);
      std::cout << "Hall C IA value A0: " << value << std::endl; 
      break;
    case 1:
      status = ca_put(DBR_DOUBLE, fIDHall_C_IA_A1, &value);
      status = ca_pend_io(10);
      status = ca_get(DBR_DOUBLE, fIDHall_C_IA_A1, &value);
      status = ca_pend_io(10);
      std::cout << "Hall C IA value A1: " << value << std::endl;      
      break;
    case 2:
      status = ca_put(DBR_DOUBLE, fIDHall_C_IA_A2, &value);
      status = ca_pend_io(10);
      status = ca_get(DBR_DOUBLE, fIDHall_C_IA_A2, &value);
      status = ca_pend_io(10);
      std::cout << "Hall C IA value A2: " << value << std::endl;
      break;
    case 3:
      status = ca_put(DBR_DOUBLE, fIDHall_C_IA_A3, &value);
      status = ca_pend_io(10);
      status = ca_get(DBR_DOUBLE, fIDHall_C_IA_A3, &value);
      status = ca_pend_io(10);
      std::cout << "Hall C IA value A3: " << value << std::endl;
      break;
    }
 
  };

  void Get_HallCIA(Int_t mode, Double_t &value){
    Int_t status;
    switch(mode){
    case 0:
      status = ca_get(DBR_DOUBLE, fIDHall_C_IA_A0, &value);
      status = ca_pend_io(10);
      std::cout << "Hall C IA value A0: " << value << std::endl; 
      break;
    case 1:
      status = ca_get(DBR_DOUBLE, fIDHall_C_IA_A1, &value);
      status = ca_pend_io(10);
      std::cout << "Hall C IA value A1: " << value << std::endl;      
      break;
    case 2:
      status = ca_get(DBR_DOUBLE, fIDHall_C_IA_A2, &value);
      status = ca_pend_io(10);
      std::cout << "Hall C IA value A2: " << value << std::endl;
      break;
    case 3:
      status = ca_get(DBR_DOUBLE, fIDHall_C_IA_A3, &value);
      status = ca_pend_io(10);
      std::cout << "Hall C IA value A3: " << value << std::endl;
      break;
    }
 
    
  }


  void Set_Pockels_Cell_plus(Double_t &value){
    Int_t status;
    status = ca_put(DBR_DOUBLE, fIDPockels_Cell_plus, &value);
    status = ca_pend_io(10);
    status = ca_get(DBR_DOUBLE, fIDPockels_Cell_plus, &value);
    status = ca_pend_io(10);
    std::cout << "Pockels Cell pos HW-count value: " << value << std::endl;

  };
  void Set_Pockels_Cell_minus(Double_t &value){
    Int_t status;
    status = ca_put(DBR_DOUBLE, fIDPockels_Cell_minus, &value);
    status = ca_pend_io(10);
    status = ca_get(DBR_DOUBLE, fIDPockels_Cell_minus, &value);
    status = ca_pend_io(10);
    std::cout << "Pockels Cell minus HW-count value: " << value << std::endl;
  };

  void Get_Pockels_Cell_plus(Double_t &value){ 
    Int_t status;
    status = ca_get(DBR_DOUBLE, fIDPockels_Cell_plus, &value);
    status = ca_pend_io(10);
    std::cout << "Pockels Cell pos HW-count value: " << value << std::endl;

  };
  void Get_Pockels_Cell_minus(Double_t &value){
    Int_t status;
    status = ca_get(DBR_DOUBLE, fIDPockels_Cell_minus, &value);
    status = ca_pend_io(10);
    std::cout << "Pockels Cell minus HW-count value: " << value << std::endl;
  };

  void Set_ChargeAsymmetry(Double_t &value, Double_t &value_error, Double_t &value_width){
    Int_t status;
    status = ca_put(DBR_DOUBLE,fChargeAsymmetry , &value);
    status = ca_pend_io(10);
    status = ca_get(DBR_DOUBLE,fChargeAsymmetry , &value);
    status = ca_pend_io(10);
    status = ca_put(DBR_DOUBLE,fChargeAsymmetryError , &value_error);
    status = ca_pend_io(10);
    status = ca_get(DBR_DOUBLE,fChargeAsymmetryError , &value_error);
    status = ca_pend_io(10);
    status = ca_put(DBR_DOUBLE,fChargeAsymmetryWidth , &value_width);
    status = ca_pend_io(10);
    status = ca_get(DBR_DOUBLE,fChargeAsymmetryWidth , &value_width);
    status = ca_pend_io(10);

    std::cout << "EPICS Charge asymmetry updated " << value <<" +/- "<<value_error<<" width "<<value_width<< std::endl;

  };

  void Get_ChargeAsymmetry(Double_t &value, Double_t &value_error, Double_t &value_width){
    Int_t status;
    status = ca_get(DBR_DOUBLE,fChargeAsymmetry , &value);
    status = ca_pend_io(10);
    status = ca_get(DBR_DOUBLE,fChargeAsymmetryError , &value_error);
    status = ca_pend_io(10);
    status = ca_get(DBR_DOUBLE,fChargeAsymmetryWidth , &value_width);
    status = ca_pend_io(10);
    std::cout << "Charge asymmetry  " << value <<" +/- "<<value_error<<" width "<<value_width<< std::endl;

  };
 
  
  void Set_FeedbackStatus(Double_t value){
    Int_t status;
    status = ca_put(DBR_DOUBLE, fFeedbackStatus, &value);
    status = ca_pend_io(10);
    status = ca_get(DBR_DOUBLE, fFeedbackStatus, &value);
    status = ca_pend_io(10);
    std::cout << "Feedback status updated " << value << std::endl;

  };
  
  Double_t Get_FeedbackStatus(){
   Int_t status;
   Double_t fbstat;
   status = ca_get(DBR_DOUBLE, fFeedbackStatus, &fbstat);
   status = ca_pend_io(10); 
   return fbstat;
  }

  //
  // not test yet, duplicated one to compare with QwHelicityCorrelatedFeedback::GetHalfWavePlateState()
  //
  // Wednesday, January 12 11:59:09 EST 2011, jhlee
  //
  TString Get_HalfWavePlateState() {
    Int_t plate_status;
    //   const char* epic_half_wave_plate = "IGL1I00DI24_24M";
    plate_status = ca_get(DBF_STRING, fHalfWavePlateStatus, &plate_status);
    if(plate_status == 1) {
      return "IN";
    }
    else {
      return "OUT";
    }
  };

  /*  
      static const Double_t kOn;
      static const Double_t kOff;
      static const Double_t kQAsymCut;
      static const Double_t kPosDiffCut;
      static const Double_t kQAsymCutHallA;

      void GetFeedbackParameters();
      void GetHallAIAFeedbackParameters();
      void Get33Matrix();
      void PutFeedbackParameters();
      void PutFeedbackParametersPos();
      void CheckBeamDevice();
      void OutputFeedbackValues(Helicity*, Asymmetry*);
      void GetFeedbackValuesPos();
      Bool_t BeamCut(QwTriggerSupervisor*);
      Bool_t ASYCut();
      Bool_t ASYCutPos();	
      void AccumulateFeedbackValues();
      void AccumulateFeedbackValuesPos();
      void GetFeedbackValues();
      Bool_t TimeIsUp();
      Bool_t TimeIsUpForPos();
      void ResetCount();
      void ResetCountPos();
      void FeedbackLoggerCharge();
      void FeedbackLogger(const UShort_t chargefeed=1, const UShort_t f33=1);
      void FeedbackLoggerPos();
  
      void GetFeedbackValues33(const Bool_t oneshot=kTRUE);

      Bool_t ChargeFeedbackIsOn(void) const {
      if(log_qonoff==kOn) return kTRUE;
      else return kFALSE;
      }
  
      Bool_t PositionFeedbackIsOn(void) const {
      if(log_ponoff==kOn) return kTRUE;
      else return kFALSE;
      }
      void CorrectingIAVoltage();
      void CorrectingHMDACs();

      //for Mark's revised 3x3 feedback plan
      Bool_t TimeIsUpForFastFeedBack();
      Bool_t TimeIsUpForSlowFeedBack();
      
      Bool_t ASYCutHallA() const;
      Bool_t CurrentCutHallA(QwTriggerSupervisor*);
      Bool_t TimeIsUpForHallAFeedBack();
      Bool_t HallAFeedbackIsOn() const;

      void GetFeedbackValuesHallA();
      void FeedbackLoggerHallA();
      void PutFeedbackParametersHallA();
      void CorrectingHallAIAVoltage();
      void AccumulateFeedbackValuesHallA();
      void ResetCountHallA();
  */

 protected:

 private:
  /*
   *  Some of the private variables should be the EPICS variable names,
   *  the buffer containing the quartet information, the histograms (if
   *  used), the accumulated BPM and BCM feedback values and errors,
   *  event cuts, etc.
   *
   */

  chid fIDHall_A_IA;
  chid fIDHall_C_IA;
  chid fIDHall_C_IA_A0;//hallc IA A0
  chid fIDHall_C_IA_A1;//hallc IA A1
  chid fIDHall_C_IA_A2;//hallc IA A2
  chid fIDHall_C_IA_A3;//hallc IA A3

  

  chid fIDPockels_Cell_plus;
  chid fIDPockels_Cell_minus;

  chid fChargeAsymmetry;
  chid fChargeAsymmetryError;
  chid fChargeAsymmetryWidth;
  chid fFeedbackStatus;

  chid fHalfWavePlateStatus;
  
  chid fIDMagnet_1_Even;
  chid fIDMagnet_1_Odd;
  chid fIDMagnet_2_Even;
  chid fIDMagnet_2_Odd;
  chid fIDMagnet_3_Even;
  chid fIDMagnet_3_Odd;
  chid fIDMagnet_4_Even;
  chid fIDMagnet_4_Odd;


;



  /*  OLD VARIABLES */
  /*
    Int_t loopIteration;
    Double_t loopLimit;
    
    Int_t ploopIteration;
    Double_t ploopLimit;
    
    time_t currentTime;
    Int_t ncount;
    Int_t pncount;
    dbr_double_t  qtime;
    char  * qtime_cha;
    dbr_double_t  ptime;
    char  * ptime_cha;
	
		
    Int_t IHWP_status;
    Double_t hallc_I;
    dbr_double_t bcm_threshold_feed;
    dbr_double_t bcm_select_feed;
    dbr_double_t bpm_threshold_feed;
    dbr_double_t bpm_select_feed;
    dbr_double_t sample_rate_feed;
    dbr_double_t psample_rate_feed;
    dbr_double_t q_asy;
    dbr_double_t g0_IA_voltage_corrected;
    dbr_double_t g0_IA_voltage_read;
    
    dbr_double_t g0_hm1_DAC_corrected;
    dbr_double_t g0_hm1_DAC_read;
    dbr_double_t g0_hm2_DAC_corrected;
    dbr_double_t g0_hm2_DAC_read;
    dbr_double_t g0_hm3_DAC_corrected;
    dbr_double_t g0_hm3_DAC_read;
    dbr_double_t g0_hm4_DAC_corrected;
    dbr_double_t g0_hm4_DAC_read;
    
    dbr_double_t p_asy_x;
    dbr_double_t p_asy_y;
    double bpm_const_feed; 
    int status;
    int stat;

    Double_t bcm_scale_feed;
    Double_t bcm_offset_feed;
    Double_t bcm_readout_feed;
 	
    Double_t x_feed;  
    Double_t y_feed;  
    Double_t E_feed;

    Double_t qasy;
    Double_t q_asy_sum;
    Double_t q_asy_sq_sum;
    Double_t q_asy_d;
    Double_t p_diff_x_sum;
    Double_t p_diff_y_sum;
    Double_t p_diff_E_sum;
    Double_t p_diff_x_sq_sum;
    Double_t p_diff_y_sq_sum;
    Double_t p_diff_E_sq_sum;

    Double_t p_diff_x;
    Double_t p_diff_x_d;
    Double_t p_diff_y;
    Double_t p_diff_y_d;
    Double_t p_diff_E;
    Double_t p_diff_E_d;

    Double_t log_bcm1;
    Double_t log_bcm2;	
    Double_t log_qonoff;
    Double_t log_ponoff;
    Double_t log_ia;
    Double_t log_wave;
    Double_t log_pita_pos, log_pita_neg;
    Double_t log_hel_even_1;
    Double_t log_hel_even_2;
    Double_t log_hel_even_3;
    Double_t log_hel_even_4;
    Double_t log_ia_slope;

    //2 by 2 matrix
    Double_t log_m11; 
    Double_t log_m12;
    Double_t log_m21;
    Double_t log_m22;
    //3 by 3 matrix
    Double_t fMatIAHM[3][3];
    
    UInt_t numFastFeedback;//fast feedback loop counter
    Bool_t fTimeIsUpForSlowFeedback;
    UInt_t fNumFastLoopsInSlowLoop; //limit of fast feedbacks in the
    //slow feedback loop
    
    Double_t log_halla_i, halla_I, halla_bcm_readout_feed;
    Double_t fAsym_q_halla, fAsym_q_halla_sum, fAsym_q_halla_sq_sum;
    
    Double_t fHallA_IA_voltage_corrected, fHallA_IA_voltage_read;
    Double_t fHallA_IA_slope;
    
    Double_t fHallAFeedbackTConst;
    
    Double_t fHallAFeedbackIsOn;
	
    Double_t fAsym_q_halla_ave_log, fAsym_q_halla_err_log;

    Double_t fAsym_q_halla_ave, fAsym_q_halla_ave_err;
    //two more vars going into the log file

    Int_t ancount;
    Int_t aloopIteration;
    Double_t aloopLimit;

    Double_t fCurrentCutHallA;
  */
  
};

#endif
