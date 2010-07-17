#include "QwEPICSControl.h"

QwEPICSControl::QwEPICSControl()
{
  int status;
  status = ca_search("IGLdac3:ao_7", &fIDHall_A_IA);
  status = ca_pend_io(10);
  status = ca_search("IGLdac3:ao_4", &fIDHall_C_IA);
  status = ca_pend_io(10);
  status = ca_search("IGLdac2:G2Ch3Pos", &fIDPockels_Cell_plus);
  status = ca_pend_io(10);
  status = ca_search("IGLdac2:G2Ch4Neg", &fIDPockels_Cell_minus);
  status = ca_pend_io(10);
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
  status = ca_search("hel_odd_3", &fIDMagnet_3_Odd);
  status = ca_pend_io(10);
  status = ca_search("hel_even_4", &fIDMagnet_4_Even);
  status = ca_pend_io(10);
  status = ca_search("hel_odd_4", &fIDMagnet_4_Odd);
  status = ca_pend_io(10);
  status = ca_search("psub_pl_pos", &fIDRHWP_angle_Get);
  status = ca_pend_io(10);
  status = ca_search("psub_pl_ipos", &fIDRHWP_angle_Set);
  status = ca_pend_io(10);
  status = ca_search("IGL1I00OD16_8", &fIDPC_status);
  status = ca_pend_io(10);
  status = ca_search("IGL1I00OD16_16", &fIDIHWP_status);
  status = ca_pend_io(10);
  status = ca_search("HELFREQ", &fIDHelicity_frequency);
  status = ca_pend_io(10);
};

QwEPICSControl::~QwEPICSControl()
{  
  /*   Destructor function.   */
};

