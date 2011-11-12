#ifndef __QwEPICSCONTROL__
#define __QwEPICSCONTROL__
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cadef.h>
#include <time.h>
#include <iterator>
#include "Rtypes.h"

#include <TMath.h>
#include <TMatrix.h>




class QwEPICSControl{
public:
  
  QwEPICSControl();
  ~QwEPICSControl();

  void Print_Qasym_Ctrls(){
    int status;

    Double_t value;
    status = ca_get(DBR_DOUBLE, fIDPC_status, &value);
    status = ca_pend_io(10);
    std::cout << "Pockels cell status:         " << value << std::endl;

    status = ca_get(DBR_DOUBLE, fIDPockels_Cell_plus_DAC, &value);
    status = ca_pend_io(10);
    std::cout << "Pockels Cell plus HV value:  " << value << std::endl;

    status = ca_get(DBR_DOUBLE, fIDPockels_Cell_minus_DAC, &value);
    status = ca_pend_io(10);
    std::cout << "Pockels Cell minus HV value: " << value << std::endl;

    status = ca_get(DBR_DOUBLE, fIDIHWP_status, &value);
    status = ca_pend_io(10);
    std::cout << "IHWP status:                 " << value << std::endl;

    status = ca_get(DBR_DOUBLE, fIDRHWP_angle_Get, &value);
    status = ca_pend_io(10);
    std::cout << "RHWP angle:                  " << value << std::endl;

    status = ca_get(DBR_DOUBLE, fIDIADAC11, &value);
    status = ca_pend_io(10);
    std::cout << "Hall C IA DAC11:             " << value << std::endl;
    status = ca_get(DBR_DOUBLE, fIDIADAC12, &value);
    status = ca_pend_io(10);
    std::cout << "Hall C IA DAC12:             " << value << std::endl;
    status = ca_get(DBR_DOUBLE, fIDIADAC13, &value);
    status = ca_pend_io(10);
    std::cout << "Hall C IA DAC13:             " << value << std::endl;
    status = ca_get(DBR_DOUBLE, fIDIADAC14, &value);
    status = ca_pend_io(10);
    std::cout << "Hall C IA DAC14:             " << value << std::endl;

    status = ca_get(DBR_DOUBLE, fIDPCXPos, &value);
    status = ca_pend_io(10);
    std::cout << "PC X position                " << value << std::endl;

    status = ca_get(DBR_DOUBLE, fIDPCYPos, &value);
    status = ca_pend_io(10);
    std::cout << "PC Y position                " << value << std::endl;

    status = ca_get(DBR_DOUBLE, fIDHelicity_frequency, &value);
    status = ca_pend_io(10);
    std::cout << "Helicity frequency:          " << value << " Hz" << std::endl;
  }
  Double_t Get_PCXPos() {
    int status;
    Double_t value;
    status = ca_get(DBR_DOUBLE, fIDPCXPos, &value);
    status = ca_pend_io(10);
    return value;  
  }
  Double_t Get_PCYPos() {
    int status;
    Double_t value;
    status = ca_get(DBR_DOUBLE, fIDPCYPos, &value);
    status = ca_pend_io(10);
    return value;  
  }
  Double_t Get_PCMoving() {
    int status;
    Double_t value;
    status = ca_get(DBR_DOUBLE, fIDPCMoving, &value);
    status = ca_pend_io(10);
    return value;  
  }
  Double_t Get_CLaser_Att() {
    int status;
    Double_t value;
    status = ca_get(DBR_DOUBLE, fIDCLaser_Att, &value);
    status = ca_pend_io(10);
    return value;  
  }
  Double_t Get_FC2_Current() {
    int status;
    Double_t value;
    status = ca_get(DBR_DOUBLE, fIDFC2_Current, &value);
    status = ca_pend_io(10);
    return value;  
  }
  Double_t Get_1I02_Current() { 
    int status;
    Double_t value;
    status = ca_get(DBR_DOUBLE, f_IPM1I02, &value);
    status = ca_pend_io(10);
    return value;  

  }
  Double_t Get_0L02_Current() { 
    int status;
    Double_t value;
    status = ca_get(DBR_DOUBLE, f_IBC0L02Current, &value);
    status = ca_pend_io(10);
    return value;  
  }


/*   Double_t Get_HallAIA(){ */
/*     int status; */
/*     Double_t value; */
/*     status = ca_get(DBR_DOUBLE, fIDHall_A_IA, &value); */
/*     status = ca_pend_io(10); */
/*     return value; */
/*   }; */
/*   Double_t Get_HallCIA(){ */
/*     int status; */
/*     Double_t value; */
/*     status = ca_get(DBR_DOUBLE, fIDHall_C_IA, &value); */
/*     status = ca_pend_io(10); */
/*     return value; */
/*   }; */
   Double_t Get_IADAC(Int_t DACnumber){ 
     int status; 
     Double_t value; 
    if (DACnumber==11) status = ca_get(DBR_DOUBLE, fIDIADAC11, &value);
    else if (DACnumber==3) status = ca_get(DBR_DOUBLE, fIDIADAC3, &value);
    else if (DACnumber==4) status = ca_get(DBR_DOUBLE, fIDIADAC4, &value);
    else if (DACnumber==5) status = ca_get(DBR_DOUBLE, fIDIADAC5, &value);
    else if (DACnumber==6) status = ca_get(DBR_DOUBLE, fIDIADAC6, &value);
    else if (DACnumber==7) status = ca_get(DBR_DOUBLE, fIDIADAC7, &value);
    else if (DACnumber==8) status = ca_get(DBR_DOUBLE, fIDIADAC8, &value);
    else if (DACnumber==9) status = ca_get(DBR_DOUBLE, fIDIADAC9, &value);
    else if (DACnumber==10) status = ca_get(DBR_DOUBLE, fIDIADAC10, &value);
    else if (DACnumber==11) status = ca_get(DBR_DOUBLE, fIDIADAC11, &value);
    else if (DACnumber==12) status = ca_get(DBR_DOUBLE, fIDIADAC12, &value);
    else if (DACnumber==13) status = ca_get(DBR_DOUBLE, fIDIADAC13, &value);
    else if (DACnumber==14) status = ca_get(DBR_DOUBLE, fIDIADAC14, &value);
    else std::cerr << "Get_IADAC(): error: " << DACnumber << " not a valid DAC number\n";
     status = ca_pend_io(10); 
     return value; 
   }; 
/*   Double_t Get_Pockels_Cell_minus(){ */
/*     int status; */
/*     Double_t value; */
/*     status = ca_get(DBR_DOUBLE, fIDPockels_Cell_minus, &value); */
/*     status = ca_pend_io(10); */
/*     return value; */
/*   }; */
/*   Double_t Get_Pockels_Cell_plus(){ */
/*     int status; */
/*     Double_t value; */
/*     status = ca_get(DBR_DOUBLE, fIDPockels_Cell_plus, &value); */
/*     status = ca_pend_io(10); */
/*     return value; */
/*   }; */
  Double_t Get_Pockels_Cell_minus_DAC(){
    int status;
    Double_t value;
    status = ca_get(DBR_DOUBLE, fIDPockels_Cell_minus_DAC, &value);
    status = ca_pend_io(10);
    return value;
  };
  Double_t Get_Pockels_Cell_plus_DAC(){
    int status;
    Double_t value;
    status = ca_get(DBR_DOUBLE, fIDPockels_Cell_plus_DAC, &value);
    status = ca_pend_io(10);
    return value;
  };
  Double_t Get_Pockels_Cell_status(){
    int status;
    Double_t value;
    status = ca_get(DBR_DOUBLE, fIDPC_status, &value);
    status = ca_pend_io(10);
    return value;
  };
  Double_t Get_RHWP_angle(){
    int status;
    Double_t value;
    status = ca_get(DBR_DOUBLE, fIDRHWP_angle_Get, &value);
    status = ca_pend_io(10);
    return value;
  };
  Double_t Print_Pockels_Cell_plus(){
    int status;
    Double_t value;
    status = ca_get(DBR_DOUBLE, fIDPockels_Cell_plus_DAC, &value);
    status = ca_pend_io(10);
    std::cout << "Pockels Cell plus HV value: " << value << std::endl;
    return value;
  };
  Double_t Print_Pockels_Cell_minus(){
    int status;
    Double_t value;
    status = ca_get(DBR_DOUBLE, fIDPockels_Cell_minus_DAC, &value);
    status = ca_pend_io(10);
    std::cout << "Pockels Cell minus HV value: " << value << std::endl;
    return value;
  };
  void Set_IADAC(Double_t value, Int_t DACnumber){
    int status;
    if (DACnumber==11) status = ca_put(DBR_DOUBLE, fIDIADAC11, &value);
    else if (DACnumber==3) status = ca_put(DBR_DOUBLE, fIDIADAC3, &value);
    else if (DACnumber==4) status = ca_put(DBR_DOUBLE, fIDIADAC4, &value);
    else if (DACnumber==5) status = ca_put(DBR_DOUBLE, fIDIADAC5, &value);
    else if (DACnumber==6) status = ca_put(DBR_DOUBLE, fIDIADAC6, &value);
    else if (DACnumber==7) status = ca_put(DBR_DOUBLE, fIDIADAC7, &value);
    else if (DACnumber==8) status = ca_put(DBR_DOUBLE, fIDIADAC8, &value);
    else if (DACnumber==9) status = ca_put(DBR_DOUBLE, fIDIADAC9, &value);
    else if (DACnumber==10) status = ca_put(DBR_DOUBLE, fIDIADAC10, &value);
    else if (DACnumber==11) status = ca_put(DBR_DOUBLE, fIDIADAC11, &value);
    else if (DACnumber==12) status = ca_put(DBR_DOUBLE, fIDIADAC12, &value);
    else if (DACnumber==13) status = ca_put(DBR_DOUBLE, fIDIADAC13, &value);
    else if (DACnumber==14) status = ca_put(DBR_DOUBLE, fIDIADAC14, &value);
    else std::cerr << "Set_IADAC(): error: " << DACnumber << " not a valid DAC number\n";
    status = ca_pend_io(10);
    std::cout << "Setting IA DAC" << DACnumber << " value: " << value << std::endl;
  };
/*   void Set_HallAIA(Double_t value){ */
/*     int status; */
/*     status = ca_put(DBR_DOUBLE, fIDHall_A_IA, &value); */
/*     status = ca_pend_io(10); */
/*     std::cout << "Setting Hall A IA value: " << value << std::endl; */
/*   }; */
/*   void Set_HallCIA(Double_t value){ */
/*     int status; */
/*     status = ca_put(DBR_DOUBLE, fIDHall_C_IA, &value); */
/*     status = ca_pend_io(10); */
/*     std::cout << "Setting Hall C IA value: " << value << std::endl; */
/*   }; */
  void Set_Pockels_Cell_plus_DAC(Double_t value){
    int status;
    if (value<0 || value>65535) {
      std::cout << "Set_Pockels_Cell_plus_DAC::Error " << value << " not between 0 and 65536" << std::endl;
      return;
    }
    status = ca_put(DBR_DOUBLE, fIDPockels_Cell_plus_DAC, &value);
    status = ca_pend_io(10);
    std::cout << "Setting Pockels Cell plus HV value: " << value << std::endl;
  };
  void Set_Pockels_Cell_minus_DAC(Double_t value){
    int status;
    if (value<0 || value>65535) {
      std::cout << "Set_Pockels_Cell_minus_DAC::Error " << value << " not between 0 and 65536" << std::endl;
      return;
    }
    status = ca_put(DBR_DOUBLE, fIDPockels_Cell_minus_DAC, &value);
    status = ca_pend_io(10);
    std::cout << "Setting Pockels Cell minus HV value: " << value << std::endl;
  };


  void Set_Pockels_X_Steps(Double_t value){
    int status;
    if (value<0 || value>25000) {
      std::cout << "Set_Pockels_X_Steps::Error " << value << " not between 0 and 25000" << std::endl;
      return;
    }
    status = ca_put(DBR_DOUBLE, fIDPCXSteps, &value);
    status = ca_pend_io(10);
    std::cout << "Setting Pockels X steps: " << value << std::endl;
  };
  void Set_Pockels_Move_Right(){
    int status;
    Int_t value = 1;
    status = ca_put(DBR_DOUBLE, f_PCXMoveRight, &value);
    std::cout << "Moving PC to the Right, status = " << status << std::endl;
    status = ca_pend_io(10);
  };
  void Set_Pockels_Move_Left(){
    int status;
    Int_t value = 1;
    status = ca_put(DBR_DOUBLE, f_PCXMoveLeft, &value);
    std::cout << "Moving PC to the Left, status = " << status;
    //    std::cout << "Moving PC to the Left." << std::endl;
    status = ca_pend_io(10);
    std::cout << ", status = " << status << std::endl;
  };



/*   void Set_Pockels_Cell_plus(Double_t value){ */
/*     int status; */
/*     if (value<0 || value>10) { */
/*       std::cout << "Set_Pockels_Cell_plus::Error " << value << " not between 0 and 10" << std::endl; */
/*       return; */
/*     } */
/*     status = ca_put(DBR_DOUBLE, fIDPockels_Cell_plus_vernier, &value); */
/*     status = ca_pend_io(10); */
/*     std::cout << "Setting Pockels Cell plus HV value: " << value << std::endl; */
/*   }; */
/*   void Set_Pockels_Cell_minus(Double_t value){ */
/*     int status; */
/*     if (value<0 || value>10) { */
/*       std::cout << "Set_Pockels_Cell_minus::Error " << value << " not between 0 and 10" << std::endl; */
/*       return; */
/*     } */
/*     status = ca_put(DBR_DOUBLE, fIDPockels_Cell_minus_vernier, &value); */
/*     status = ca_pend_io(10); */
/*     std::cout << "Setting Pockels Cell minus HV value: " << value << std::endl; */
/*   }; */
  void Set_RHWP_angle(Double_t value){
    int status;
    if (value<50) value=50;
    if (value>9000) value=9000;
    status = ca_put(DBR_DOUBLE, fIDRHWP_angle_Set, &value);
    status = ca_pend_io(10);
    std::cout << "Setting RHWP value: " << value << std::endl;
  };

 protected:

 private:
  /*
   *  Some of the private variables should be the EPICS variable names,
   *  the buffer containing the quartet information, the histograms (if
   *  used), the accumulated BPM and BCM feedback values and errors,
   *  event cuts, etc.
   *
   */


/*   chid fIDHall_A_IA; */
/*   chid fIDHall_C_IA; */
/*   chid fIDPockels_Cell_plus_vernier; //old */
/*   chid fIDPockels_Cell_minus_vernier; //old */
  chid fIDPockels_Cell_plus_DAC;
  chid fIDPockels_Cell_minus_DAC;
  chid fIDIADAC3;
  chid fIDIADAC4;
  chid fIDIADAC5;
  chid fIDIADAC6;
  chid fIDIADAC7;
  chid fIDIADAC8;
  chid fIDIADAC9;
  chid fIDIADAC10;
  chid fIDIADAC11;
  chid fIDIADAC12;
  chid fIDIADAC13;
  chid fIDIADAC14;
/*   chid fIDPockels_Cell_plus_DAC_readval; */
/*   chid fIDPockels_Cell_minus_DAC_readval; */
  chid fIDMagnet_1_Even;
  chid fIDMagnet_1_Odd;
  chid fIDMagnet_2_Even;
  chid fIDMagnet_2_Odd;
  chid fIDMagnet_3_Even;
  chid fIDMagnet_3_Odd;
  chid fIDMagnet_4_Even;
  chid fIDMagnet_4_Odd;
  chid fIDRHWP_angle_Get;
  chid fIDRHWP_angle_Set;
  chid fIDPC_status;
  chid fIDIHWP_status;
  chid fIDHelicity_frequency;
  chid fIDFC2_Current;
  chid fIDCLaser_Att;
  chid fIDPCXPos;
  chid fIDPCYPos;
  chid fIDPCMoving;
  chid fIDPCXSteps;
  chid f_PCXMoveRight;
  chid f_PCXMoveLeft;
  chid f_IPM1I02;
  chid f_IBC0L02Current

;


};

#endif
