/////////////////////////////////////////////////////////////////////////////////////
//NPDGamma Experiment Data Visualization and Analysis
//
//Version 2.01
//
//Definitions file, containing all common constants
/////////////////////////////////////////////////////////////////////////////////////


#ifndef NPDGMAINDEF_H
#define NPDGMAINDEF_H

#include "RDataWindowDef.h"
#include "RDataContainerDef.h"

/* #define NPDG_DEBUG */

#define TARGET "Cl"

//#ifndef Pi
//#define Pi                       3.14159265359
//#endif

#ifndef MSG_QUEUE_MAX
#define MSG_QUEUE_MAX         10           //Maximum entries per message
#endif                                     //queue
#ifndef MSG_SIZE_MAX
#define MSG_SIZE_MAX          10000        //Maximum size of each message
#endif
#ifndef NAME_STR_MAX
#define NAME_STR_MAX          200          //Maximum file name length
#endif

//Define the allowed time difference between corresponding macropulses from
//the 3 VME crates
//Also Define the same for consecutive macro pulses in the same VME file
//***********************************************************************************
#define VME_TIME_DIFF_MAX                 1000 //In microseconds
#define PULSE_TIME_DIFF_MAX              60000 //In microseconds
#define PULSE_TIME_DIFF_MIN              40000 //In microseconds
//***********************************************************************************

#define ANL_MAX_OBJ_CNT                  10000 //Maximum number of obj used in 
                                               //Analysis
#define ADC_CONV_FACTOR                 3276.8 //ADC sample unit conversion factor
                                               //(counts per Volt)
#define AMPL_NOISE                     1.9e-14 //amps per sqrt(Hz) from amplifier
#define MAIN_GAIN                         50e6 //First amp stage gain (Ohms)
#define SAMPL_FAC                          0.6 //from bessel filtering (see lab book)
#define SAMPL_BW                          4200 //Bandwidth (Hz) set by the Sum-Diff Amps 
#define DAQ_SDFAC                            1 //Summing and Differencing
                                               //noise enhancement factor
#define COSMIC_DEP_MEV                      85 //Energy deposited in a detector
                                               //by a single cosmic traversing
                                               //the entire detector length (MeV)
#define e_CHARGE                1.60217733e-19 //electron charge
#define v_LIGHT                 2.99792458e8   //speed of light in m/s
#define n_MASS                  939.56533e6    //neutron mass in eV/c^2             

#ifndef kTrue
#define kTrue                             1
#endif
#ifndef kFalse
#define kFalse                            0
#endif

#ifndef MP_SAMPLE_WINDOW
#define MP_SAMPLE_WINDOW               40.0   //Size of the sampling window in ms 
#endif

#define N_AVOG                    6.0221367e23

#define FIGFILEMAX                       50 //Maximum number of figures per log file

#define ERR_RET_VAL                 400000000 

#define DATA_CONFIG            "NPDGData.conf"
#define DATA_DIR               "../data"

#define QUEUE_GRPUFLOW_ERROR   "ERROR: Trying to read beyond end of file.\n"
#define FILE_RANGE_ERROR       "ERROR: Invalid macro pulse range selected.\n"
#define ANL_ALLOC_ERROR        "ERROR: Unable to allocate memory for analysis\n"
#define ANL_BINNING_ERROR      "ERROR: Unable to auto establish binning of histos\n"

//Error Messages
#define ALL_MP                                -1
#define LIST_ITEM_CREATE_ERROR                1011
#define LIST_ITEM_DELETE_ERROR                1012
#ifndef PROCESS_OK
#define PROCESS_OK                            1013
#endif
#ifndef PROCESS_FAILED
#define PROCESS_FAILED                        1014
#endif
#define PROCESS_CANCELED                      1015
#define MEMORY_ALLOC_ERROR                    1016
#ifndef LOG_PROCESS_ERROR
#define LOG_PROCESS_ERROR                     1017
#endif
#ifndef LOG_PROCESS_OK
#define LOG_PROCESS_OK                        1018
#endif

#define MPQ_PULS_MATCH_ERROR                  1019
#define MPQ_VME1_ORDER_ERROR                  1019
#define MPQ_VME2_ORDER_ERROR                  1020
#define MPQ_VME3_ORDER_ERROR                  1021
#define MPQ_VMEX_RANGE_ERROR                  1022
#define MPQ_VME1_STATE_ERROR                  1023
#define MPQ_VME2_STATE_ERROR                  1024
#define MPQ_VME3_STATE_ERROR                  1025
#define MPQ_SPIN_MATCH_ERROR                  1026

#define kC_PR_DIALOG                   1500
#define kCM_PR_MSG                      150
#define kC_PLO_DIALOG                  1060
#define kCM_PLO_MSG                     106
#define kC_MATH_DIALOG                 1400
#define kCM_MATH_MSG                    140
#define kC_ANL_ENT_DIALOG              1100
#define kCM_ANL_ENT_MSG                 110


enum CommandIdentifiers {

  M_HP_LOGTXTTS,
  M_HP_LOGTXTNS,
  M_HP_CLOSED,
  M_DC_LOGTXTTS,
  M_DC_LOGTXTNS,
  M_DC_CLOSED,
  M_HS_LOGTXTTS,
  M_HS_LOGTXTNS,
  M_HS_CLOSED,
  M_PL_LOGTXTTS,
  M_PL_LOGTXTNS,
  M_PLO_CLOSED,
  M_MATH_LOGTXTTS,
  M_MATH_LOGTXTNS,
  M_MATH_ERROR_MSG,

  M_HPE_LOGTXTTS,
  M_HPE_LOGTXTNS,
  M_HPE_CLOSED,
  M_HSE_LOGTXTTS,
  M_HSE_LOGTXTNS,
  M_HSE_CLOSED,
  M_PLE_LOGTXTTS,
  M_PLE_LOGTXTNS,
  M_PLE_CLOSED,
  M_CALC_SFL_EFF,
  M_SET_SFL_EFF_PARMS,

  M_ANL_PR_SEQ,
  M_ANL_PR_RUN,
  M_ANL_CLOSE,
  M_ANL_CLOSED,
  M_ANLE_CLOSE,
  M_ANLE_CLOSED,
  M_ANL_PLOT_SELFUNC,
  M_TOF_DEFAULTS,
  M_PR_SEQ,
  M_PR_SEQ2,
  M_PR_RUN,
  M_PR_CLOSED,
  M_PR_CLOSE,
  M_PR_STOP,
  M_PR_PAUSE,
  M_PR_RESUME,

  M_CSDE_CLOSE,
  M_CSDE_CLOSED,
  
  M_WRITE_RC_DATA,

  M_FILE_OPEN,
  M_ROOT_FILE_OPEN,
  M_ROOT_FILE_EVENT_OPEN,
  M_RUN_FILE_OPEN,
  M_RUN_FILE_EVENT_OPEN,

  M_DBASE_OPEN,
  M_FILE_SAVE,
  M_FILE_SAVEAS,
  M_FILE_PRINT,
  M_FILE_PRINTSETUP,
  M_FILE_EXIT,
  M_FILE_CLOSE,
  M_ROOT_FILE_CLOSE,
  M_DBASE_CLOSE,
  M_DBASE_QUERY,
  M_FILE_ANALYZE,
  M_FILE_NOISESTAT,
  M_FILE_ASYMSTAT,
  M_DRAW_ARRAY,
  M_OPEN_DATAWINDOW,

  M_VIEW_LOG,
  M_VIEW_BROWSER,
  M_SEGMENT_SELECT,
  M_ADD_SEGMENT,
  M_PREFIX_SELECT,
  M_TARGET_SELECT,
  M_RUN_SELECT,
  M_LOG_ENTRY,
  
  M_HELP_USER,
  M_HELP_CODE,
  M_HELP_SEARCH,
  M_HELP_ABOUT,

  M_CANVAS,

  M_HISTO_CLOSE,
  M_HISTO_ENTRY_SET,
  M_HISTO_ENTRY_CANCEL,
  
  M_TABS   //always needs to be last in the list!
};

enum ENDataType {
  DDT_DB,
  DDT_MD,
  DDT_BCM,
  DDT_BPM,
  DDT_HEL,
};


enum ERFileStatus {
  FS_NEW,
  FS_OLD,
};

struct ClineArgs {

  int            bckgr;
  int            mute;
  int            realtime;
  int            checkmode;
  char           file[1000];

};

#endif
