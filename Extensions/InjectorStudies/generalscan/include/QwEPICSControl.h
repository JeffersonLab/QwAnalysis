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

  void Print_HallAIA(){
    int status;
    //    Char_t tmp[30];
    //    status = ca_get(DBR_STRING, fIDHall_A_IA, tmp);
    //    status = ca_pend_io(10);
    //    std::cout << "Hall A IA value: " << tmp << std::endl;
    Double_t value;
    status = ca_get(DBR_DOUBLE, fIDHall_A_IA, &value);
    status = ca_pend_io(10);
    std::cout << "Hall A IA value: " << value << std::endl;
  };


  void Print_Qasym_Ctrls(){
    int status;
    //    Char_t tmp[30];
    //    status = ca_get(DBR_STRING, fIDHall_A_IA, tmp);
    //    status = ca_pend_io(10);
    //    std::cout << "Hall A IA value: " << tmp << std::endl;
    Double_t value;
    status = ca_get(DBR_DOUBLE, fIDPC_status, &value);
    status = ca_pend_io(10);
    std::cout << "Pockels cell status:         " << value << std::endl;

    status = ca_get(DBR_DOUBLE, fIDPockels_Cell_plus, &value);
    status = ca_pend_io(10);
    std::cout << "Pockels Cell plus HV value:  " << value << std::endl;

    status = ca_get(DBR_DOUBLE, fIDPockels_Cell_minus, &value);
    status = ca_pend_io(10);
    std::cout << "Pockels Cell minus HV value: " << value << std::endl;

    status = ca_get(DBR_DOUBLE, fIDIHWP_status, &value);
    status = ca_pend_io(10);
    std::cout << "IHWP status:                 " << value << std::endl;

    status = ca_get(DBR_DOUBLE, fIDRHWP_angle_Get, &value);
    status = ca_pend_io(10);
    std::cout << "RHWP angle:                  " << value << std::endl;

    status = ca_get(DBR_DOUBLE, fIDHall_C_IA, &value);
    status = ca_pend_io(10);
    std::cout << "Hall C IA value:             " << value << std::endl;

    status = ca_get(DBR_DOUBLE, fIDHelicity_frequency, &value);
    status = ca_pend_io(10);
    std::cout << "Helicity frequency:          " << value << " Hz" << std::endl;
  };
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
  Double_t Get_HallAIA(){
    int status;
    Double_t value;
    status = ca_get(DBR_DOUBLE, fIDHall_A_IA, &value);
    status = ca_pend_io(10);
    return value;
  };
  Double_t Get_HallCIA(){
    int status;
    Double_t value;
    status = ca_get(DBR_DOUBLE, fIDHall_C_IA, &value);
    status = ca_pend_io(10);
    return value;
  };
  Double_t Get_Pockels_Cell_minus(){
    int status;
    Double_t value;
    status = ca_get(DBR_DOUBLE, fIDPockels_Cell_minus, &value);
    status = ca_pend_io(10);
    return value;
  };
  Double_t Get_Pockels_Cell_plus(){
    int status;
    Double_t value;
    status = ca_get(DBR_DOUBLE, fIDPockels_Cell_plus, &value);
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
    status = ca_get(DBR_DOUBLE, fIDPockels_Cell_plus, &value);
    status = ca_pend_io(10);
    std::cout << "Pockels Cell plus HV value: " << value << std::endl;
    return value;
  };
  Double_t Print_Pockels_Cell_minus(){
    int status;
    Double_t value;
    status = ca_get(DBR_DOUBLE, fIDPockels_Cell_minus, &value);
    status = ca_pend_io(10);
    std::cout << "Pockels Cell minus HV value: " << value << std::endl;
    return value;
  };

  void Set_HallAIA(Double_t value){
    int status;
    status = ca_put(DBR_DOUBLE, fIDHall_A_IA, &value);
    status = ca_pend_io(10);
    std::cout << "Setting Hall A IA value: " << value << std::endl;
  };
  void Set_HallCIA(Double_t value){
    int status;
    status = ca_put(DBR_DOUBLE, fIDHall_C_IA, &value);
    status = ca_pend_io(10);
    std::cout << "Setting Hall A IA value: " << value << std::endl;
  };
  void Set_Pockels_Cell_plus(Double_t value){
    int status;
    if (value<0 || value>10) {
      std::cout << "Set_Pockels_Cell_plus::Error " << value << " not between 0 and 10" << std::endl;
      return;
    }
    status = ca_put(DBR_DOUBLE, fIDPockels_Cell_plus, &value);
    status = ca_pend_io(10);
    std::cout << "Setting Pockels Cell plus HV value: " << value << std::endl;
  };
  void Set_Pockels_Cell_minus(Double_t value){
    int status;
    if (value<0 || value>10) {
      std::cout << "Set_Pockels_Cell_minus::Error " << value << " not between 0 and 10" << std::endl;
      return;
    }
    status = ca_put(DBR_DOUBLE, fIDPockels_Cell_minus, &value);
    status = ca_pend_io(10);
    std::cout << "Setting Pockels Cell minus HV value: " << value << std::endl;
  };
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


  chid fIDHall_A_IA;
  chid fIDHall_C_IA;
  chid fIDPockels_Cell_plus;
  chid fIDPockels_Cell_minus;
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

;


};

#endif
