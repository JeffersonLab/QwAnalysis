#include "QwEPICSControl.h"

QwEPICSControl::QwEPICSControl()
{
  int status;
//   status = ca_search("IGLdac3:ao_7", &fIDHall_A_IA);
//   status = ca_pend_io(10);
//   status = ca_search("IGLdac3:ao_4", &fIDHall_C_IA);
//   status = ca_pend_io(10);
  // These are the old pockels cell channels
//   status = ca_search("IGLdac2:G2Ch3Pos", &fIDPockels_Cell_plus);
//   status = ca_pend_io(10);
//   status = ca_search("IGLdac2:G2Ch4Neg", &fIDPockels_Cell_minus);
//   status = ca_pend_io(10);
  status = ca_search("C1068_QDAC01", &fIDPockels_Cell_plus_DAC);
  status = ca_pend_io(10);
  status = ca_search("C1068_QDAC02", &fIDPockels_Cell_minus_DAC);
  status = ca_pend_io(10);
  status = ca_search("C1068_QDAC03", &fIDIADAC3);
  status = ca_pend_io(10);
  status = ca_search("C1068_QDAC04", &fIDIADAC4);
  status = ca_pend_io(10);
  status = ca_search("C1068_QDAC05", &fIDIADAC5);
  status = ca_pend_io(10);
  status = ca_search("C1068_QDAC06", &fIDIADAC6);
  status = ca_pend_io(10);
  status = ca_search("C1068_QDAC07", &fIDIADAC7);
  status = ca_pend_io(10);
  status = ca_search("C1068_QDAC08", &fIDIADAC8);
  status = ca_pend_io(10);
  status = ca_search("C1068_QDAC09", &fIDIADAC9);
  status = ca_pend_io(10);
  status = ca_search("C1068_QDAC10", &fIDIADAC10);
  status = ca_pend_io(10);
  status = ca_search("C1068_QDAC11", &fIDIADAC11);
  status = ca_pend_io(10);
  status = ca_search("C1068_QDAC12", &fIDIADAC12);
  status = ca_pend_io(10);
  status = ca_search("C1068_QDAC13", &fIDIADAC13);
  status = ca_pend_io(10);
  status = ca_search("C1068_QDAC14", &fIDIADAC14);
  status = ca_pend_io(10);
//   status = ca_search("C1068_QDAC01r.RVAL", &fIDPockels_Cell_plus_DAC_readval);
//   status = ca_pend_io(10);
//   status = ca_search("C1068_QDAC02r.RVAL", &fIDPockels_Cell_minus_DAC_readval);
//   status = ca_pend_io(10);
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
  status = ca_search("FCupsCORRECTED.VAL", &fIDFC2_Current);
  status = ca_pend_io(10);
  status = ca_search("psub_ac_pos", &fIDCLaser_Att);
  status = ca_pend_io(10);
  status = ca_search("cdipc_xpos", &fIDPCXPos);
  status = ca_pend_io(10);
  status = ca_search("cdipc_ypos", &fIDPCYPos);
  status = ca_pend_io(10);
  status = ca_search("OPCR00Lpico_moving", &fIDPCMoving);
  status = ca_pend_io(10);
  status = ca_search("OPCR00LC2hordered_steps", &fIDPCXSteps);
  status = ca_pend_io(10);
  status = ca_search("OPCR00LC2hmove_ccw", &f_PCXMoveRight);
  status = ca_pend_io(10);
  status = ca_search("OPCR00LC2hmove_cw", &f_PCXMoveLeft);
  status = ca_pend_io(10);
  status = ca_search("IPM1I02", &f_IPM1I02);
  status = ca_pend_io(10);
  status = ca_search("IBC0L02Current", &f_IBC0L02Current);
  status = ca_pend_io(10);
};


QwEPICSControl::~QwEPICSControl()
{  
  /*   Destructor function.   */
};

