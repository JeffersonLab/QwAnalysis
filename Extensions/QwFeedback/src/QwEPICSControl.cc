#include "QwEPICSControl.h"

QwEPICSControl::QwEPICSControl()
{
  int status;
  status = ca_search("IGLdac3:ao_7", &fIDHall_A_IA);//IA Cell Setpoint  Hall A  
  status = ca_pend_io(10);
  status = ca_search("IGLdac3:ao_4", &fIDHall_C_IA);//IA Cell Setpoint  Hall C
  status = ca_pend_io(10);
  status = ca_search("IGLdac2:G2Ch3Pos", &fIDPockels_Cell_plus);//Gun2: PC (PITA) POS
  status = ca_pend_io(10);
  status = ca_search("IGLdac2:G2Ch4Neg", &fIDPockels_Cell_minus);//Gun2: PC (PITA) NEG 
  status = ca_pend_io(10);
  /*
//I could not find these names in the ~/qweak/coda26/scripts/epics.dictionary - Rakitha 09-24-2010
  status = ca_search("hel_even_1", &fIDMagnet_1_Even);
  status = ca_pend_io(10);
  status = ca_search("hel_odd_1", &fIDMagnet_1_Odd);
  status = ca_pend_io(10);
  status = ca_search("hel_even_2", &fIDMagnet_2_Even);
  status = ca_pend_io(10);
  status = ca_search("hel_odd_2", &fIDMagnet_2_Odd);
  status = ca_pend_io(10);
  status = ca_search("hel_even_3", &fIDMagnet_3_Even);
  status = ca_pend_io(10);
  status = ca_pend_io(10);
  status = ca_search("hel_odd_3", &fIDMagnet_3_Odd);
  status = ca_pend_io(10);
  status = ca_search("hel_even_4", &fIDMagnet_4_Even);
  status = ca_pend_io(10);
  status = ca_search("hel_odd_4", &fIDMagnet_4_Odd);
  status = ca_pend_io(10);
  */
};

QwEPICSControl::~QwEPICSControl()
{  

};

