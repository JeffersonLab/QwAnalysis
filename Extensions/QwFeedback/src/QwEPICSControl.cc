#include "QwEPICSControl.h"

QwEPICSControl::QwEPICSControl()
{
  int status;
  status = ca_search("IGLdac3:ao_7", &fIDHall_A_IA);//IA Cell Setpoint  Hall A  
  status = ca_pend_io(10);
  status = ca_search("IGLdac3:ao_4", &fIDHall_C_IA);//IA Cell Setpoint  Hall C
  status = ca_pend_io(10);
  //updated PC values
  status = ca_search("C1068_QDAC01", &fIDPockels_Cell_plus);//PC (PITA) POS
  status = ca_pend_io(10);
  status = ca_search("C1068_QDAC02", &fIDPockels_Cell_minus);//PC (PITA) NEG 
  status = ca_pend_io(10);

  //new Hall C IA variables
  status = ca_search("C1068_QDAC11", &fIDHall_C_IA_A0);//IA Cell Setpoint  Hall C
  status = ca_pend_io(10);
  status = ca_search("C1068_QDAC12", &fIDHall_C_IA_A1);//IA Cell Setpoint  Hall C
  status = ca_pend_io(10);
  status = ca_search("C1068_QDAC13", &fIDHall_C_IA_A2);//IA Cell Setpoint  Hall C
  status = ca_pend_io(10);
  status = ca_search("C1068_QDAC14", &fIDHall_C_IA_A3);//IA Cell Setpoint  Hall C
  status = ca_pend_io(10);

  //Feedback status EPICS variables
  status = ca_search("qw:ChargeFeedback", &fFeedbackStatus);//
  status = ca_pend_io(10);
  status = ca_search("qw:ChargeAsymmetry", &fChargeAsymmetry);//
  status = ca_pend_io(10);
  status = ca_search("qw:ChargeAsymmetryError", &fChargeAsymmetryError);//
  status = ca_pend_io(10);
  status = ca_search("qw:ChargeAsymmetryWidth", &fChargeAsymmetryWidth);//
  status = ca_pend_io(10);
  

};

QwEPICSControl::~QwEPICSControl()
{  

};

