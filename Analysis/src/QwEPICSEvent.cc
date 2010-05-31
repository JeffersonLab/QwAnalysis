#include "QwEPICSEvent.h"

#define MYSQLPP_SSQLS_NO_STATICS
#include "QwSSQLS.h"
#include "QwDatabase.h"


#include <TStyle.h>
#include <TFile.h>

#include <iostream>
#include <fstream>

#include "StringManip.h" 


/*************************************
 *  Constant definitions.
 *************************************/

const int QwEPICSEvent::kDebug  = 0;  /* Debugging flag, set this to 1 to  *
				       * enable debugging output, otherwise 0.*/
const int QwEPICSEvent::kEPICS_Error = -1;
const int QwEPICSEvent::kEPICS_OK    =  1;
const Double_t QwEPICSEvent::kInvalidEPICSData = -999999.0;


/*************************************
 *  Constructors/Destructors.
 *************************************/
QwEPICSEvent::QwEPICSEvent()
{
  InitDefaultAutogainList();
  DefineEPICSVariables();
  ResetCounters();
  if (kDebug==1) PrintVariableList();
};

QwEPICSEvent::~QwEPICSEvent(){};


/*************************************
 *  Member functions.
 *************************************/


int QwEPICSEvent::AddEPICSTag(TString tag)
{
  return AddEPICSTag(tag, "", EPICSFloat);
};

int QwEPICSEvent::AddEPICSTag(TString tag, TString table)
{
  return AddEPICSTag(tag, table, EPICSFloat);
};

int QwEPICSEvent::AddEPICSTag(TString tag, TString table, 
			      QwEPICSDataType datatype)
{
  fEPICSVariableList.push_back(tag);
  fEPICSTableList.push_back(table);
  fEPICSVariableType.push_back(datatype);
  fNumberEPICSVariables++;
  return 0;
};


void QwEPICSEvent::CalculateRunningValues()
{
  Int_t tagindex;
  Bool_t anyFilled;

  if (kDebug==1) std::cout <<"\n\nHere we are in 'CalculateRunningValues'!!\n"<<std::endl;

  anyFilled = kFALSE;
  for (tagindex=0; tagindex<fNumberEPICSVariables; tagindex++) {
    anyFilled  |=  fEPICSDataEvent[tagindex].Filled;
  }


  if (anyFilled){
    for (tagindex=0; tagindex<fNumberEPICSVariables; tagindex++) {
      if (fEPICSVariableType[tagindex] != EPICSString){
	if (fEPICSDataEvent[tagindex].Filled){
	  
	  if (! fEPICSCumulativeData[tagindex].Filled){
	    fEPICSCumulativeData[tagindex].NumberRecords = 0;
	    fEPICSCumulativeData[tagindex].Sum           = 0.0;
	    fEPICSCumulativeData[tagindex].SquaredSum    = 0.0;
	    fEPICSCumulativeData[tagindex].Maximum       = 
	      fEPICSDataEvent[tagindex].Value;
	    fEPICSCumulativeData[tagindex].Minimum       =  
	      fEPICSDataEvent[tagindex].Value;
	    fEPICSCumulativeData[tagindex].Filled        = kTRUE;
	  }
	  fEPICSCumulativeData[tagindex].NumberRecords++;
	  fEPICSCumulativeData[tagindex].Sum += 
	    fEPICSDataEvent[tagindex].Value;
	  fEPICSCumulativeData[tagindex].SquaredSum +=
	    (fEPICSDataEvent[tagindex].Value)*(fEPICSDataEvent[tagindex].Value);
	  if (fEPICSDataEvent[tagindex].Value
	      > fEPICSCumulativeData[tagindex].Maximum){
	    fEPICSCumulativeData[tagindex].Maximum       = 
	      fEPICSDataEvent[tagindex].Value;
	  }
	  if (fEPICSDataEvent[tagindex].Value
	      < fEPICSCumulativeData[tagindex].Minimum){
	    fEPICSCumulativeData[tagindex].Minimum       =  
	      fEPICSDataEvent[tagindex].Value;
	  }
	  if (kDebug==1) std::cout << "This event has "<<fEPICSVariableList[tagindex]
			     << " equal to "<< fEPICSDataEvent[tagindex].Value
			     << " giving a running average of "
			     << fEPICSCumulativeData[tagindex].Sum/
			  fEPICSCumulativeData[tagindex].NumberRecords
			     <<std::endl;
	} else {
	  if (kDebug==1) std::cout <<fEPICSVariableList[tagindex]
			     <<" seems to be not filled."<<std::endl;
	}
      } else {
	//  This is a string value.
	//  Just check to see if the EPICS string has changed;
	//  if it is the same, increment the counter.

	if (! fEPICSCumulativeData[tagindex].Filled){
	  fEPICSCumulativeData[tagindex].NumberRecords = 0;
	  fEPICSCumulativeData[tagindex].SavedString   = 
	    fEPICSDataEvent[tagindex].StringValue;
	  fEPICSCumulativeData[tagindex].Filled        = kTRUE;
	}
	if (fEPICSCumulativeData[tagindex].SavedString
	    == fEPICSDataEvent[tagindex].StringValue ){
	  fEPICSCumulativeData[tagindex].NumberRecords++;
	}
      }
    }
    fNumberEPICSEvents++;
  }
  std::cout<<"\n\nfNumberEPICSEvents = "<<fNumberEPICSEvents<<std::endl<<std::endl;
};

void QwEPICSEvent::DefineEPICSVariables()
{
  fNumberEPICSVariables = 0;
  fEPICSVariableList.clear();
  fEPICSVariableType.clear();


  /*
   In the following the symbol //-- means the following line: 
   Error: QwDatabase::GetSlowControlDetectorID() => Unable to determine valid ID for the epics variable




The following epics variable that are currently in the database are not in the QwEPICSEvent.cc:

sc_detector_id                Variable                       comment

  6              |            R00PGSET           |           | Prebuncher Setting 



  */
  
  AddEPICSTag("ibcm1","slow_controls_data"); //--
  AddEPICSTag("ibcm2","slow_controls_data");//--
  //  Add the G0 electronics cage tmeperatures to the readout
  //AddEPICSTag("g0tpos_switch", "target", EPICSString);//--
  AddEPICSTag("g0tpos_switch", "slow_controls_data");//-- 
  AddEPICSTag("g0cagetemp1","slow_controls_data");//--
  AddEPICSTag("g0cagetemp2","slow_controls_data");//--
  AddEPICSTag("g0cagetemp3","slow_controls_data");//--
  AddEPICSTag("g0tach","slow_controls_data"); //g0target pumpspeed //--
  AddEPICSTag("gt3", "slow_controls_data");//--
  AddEPICSTag("gpt3", "slow_controls_data");//--
  AddEPICSTag("gflyswatter","slow_controls_data");//flyswatter in/out //--
  AddEPICSTag("IHM3H01E","slow_controls_data"); // Halo target position //--
  AddEPICSTag("g0smsvac", "slow_controls_data");//--
  AddEPICSTag("G0:PSCurrent", "slow_controls_data");//--
  AddEPICSTag("G0:ROXCoilA", "slow_controls_data");//--
  AddEPICSTag("G0:LHeLevel", "slow_controls_data");//--
  AddEPICSTag("G0:LN2Level", "slow_controls_data");//--
  AddEPICSTag("G0:TCCryoBox", "slow_controls_data");//--

  //AddEPICSTag("HC:Q_ONOFF", "slow_controls_data", EPICSString); // Charge Feedback on/off //--
  AddEPICSTag("HC:Q_ONOFF", "slow_controls_data"); // Charge Feedback on/off //--

  AddEPICSTag("HC:Q_ASY", "slow_controls_data"); // Charge asymmetry
  AddEPICSTag("HC:DQ_ASY", "slow_controls_data"); // Charge asymmetry uncertainty
  AddEPICSTag("HC:bcm_threshold", "slow_controls_data"); // BCM threshold cut for charge feedback
  AddEPICSTag("HC:bcm_select", "slow_controls_data"); // BCM selection for charge feedback //--

  AddEPICSTag("psub_pl_pos", "slow_controls_data"); // RHWP angle

  AddEPICSTag("IGL1I00DI24_24M", "slow_controls_data"); // IHWP //--
  AddEPICSTag("WienAngle", "slow_controls_data"); // Wien filter setting
  AddEPICSTag("g0_pivot_halo","slow_controls_data"); //--
  AddEPICSTag("IGL1I00AI24", "slow_controls_data"); //IA voltage
  AddEPICSTag("HC:P_ONOFF", "slow_controls_data"); //--
  AddEPICSTag("HC:X_DIF", "slow_controls_data"); 
  AddEPICSTag("HC:DX_DIF", "slow_controls_data");
  AddEPICSTag("HC:Y_DIF", "slow_controls_data");
  AddEPICSTag("HC:DY_DIF", "slow_controls_data");
  AddEPICSTag("HC:bpm_select", "slow_controls_data"); //--
  AddEPICSTag("IGLdac3:ao_0", "slow_controls_data"); // X pzt voltage
  AddEPICSTag("IGLdac3:ao_1", "slow_controls_data"); // Y pzt voltage
  AddEPICSTag("IGL1I00AI22", "slow_controls_data");  // X G0 pzt voltage
  AddEPICSTag("IGL1I00AI23", "slow_controls_data");   // Y G0 pzt voltage
  AddEPICSTag("IGL1I00AI7","slow_controls_data");// Gun2 Pita Positive
  AddEPICSTag("IGL1I00AI8","slow_controls_data");// Gun2 Pita Negative
  AddEPICSTag("IGL1I00AI27","slow_controls_data");// HallA IA voltage //sc_detector_id = 8
  AddEPICSTag("IGL1I00AI25","slow_controls_data");// HallA PZT X voltage
  AddEPICSTag("IGL1I00AI26","slow_controls_data");// HallA PZT Y voltage
  AddEPICSTag("IGLdac2:scale_6.A","slow_controls_data");//HallB TACO   //sc_detector_id = 7


  AddEPICSTag("hel_mag_status.D","slow_controls_data"); //Integrated helicity bit difference
  AddEPICSTag("IGL1I00AI17","slow_controls_data"); // dummy helicity Pockels' cell voltage


  AddEPICSTag("HC:qint_period", "slow_controls_data"); // charge asymmetry sampling period //sc_detector_id = 1
  AddEPICSTag("HC:pint_period", "slow_controls_data"); // position difference sampling period
  

  
  /* add injector BPM values in here jyun July 17th, 2003  */
 
  AddEPICSTag("HC:QPD_WSUM", "slow_controls_data");
  AddEPICSTag("HC:QPD_DWSUM", "slow_controls_data");
  AddEPICSTag("HC:QPD_XDIF", "slow_controls_data");
  AddEPICSTag("HC:QPD_DXDIF", "slow_controls_data");
  AddEPICSTag("HC:QPD_YDIF", "slow_controls_data");
  AddEPICSTag("HC:QPD_DYDIF", "slow_controls_data");
  AddEPICSTag("HC:1I02_WSUM", "slow_controls_data");
  AddEPICSTag("HC:1I02_DWSUM", "slow_controls_data");
  AddEPICSTag("HC:1I02_XDIF", "slow_controls_data");
  AddEPICSTag("HC:1I02_DXDIF", "slow_controls_data");
  AddEPICSTag("HC:1I02_YDIF", "slow_controls_data");
  AddEPICSTag("HC:1I02_DYDIF", "slow_controls_data");
  AddEPICSTag("HC:1I04_WSUM", "slow_controls_data");
  AddEPICSTag("HC:1I04_DWSUM", "slow_controls_data");
  AddEPICSTag("HC:1I04_XDIF", "slow_controls_data");
  AddEPICSTag("HC:1I04_DXDIF", "slow_controls_data");
  AddEPICSTag("HC:1I04_YDIF", "slow_controls_data");
  AddEPICSTag("HC:1I04_DYDIF", "slow_controls_data");
  AddEPICSTag("HC:1I06_WSUM", "slow_controls_data");
  AddEPICSTag("HC:1I06_DWSUM", "slow_controls_data");
  AddEPICSTag("HC:1I06_XDIF", "slow_controls_data");
  AddEPICSTag("HC:1I06_DXDIF", "slow_controls_data");
  AddEPICSTag("HC:1I06_YDIF", "slow_controls_data");
  AddEPICSTag("HC:1I06_DYDIF", "slow_controls_data");
  AddEPICSTag("HC:0L02_WSUM", "slow_controls_data");
  AddEPICSTag("HC:0L02_DWSUM", "slow_controls_data");
  AddEPICSTag("HC:0L02_XDIF", "slow_controls_data");
  AddEPICSTag("HC:0L02_DXDIF", "slow_controls_data");
  AddEPICSTag("HC:0L02_YDIF", "slow_controls_data");
  AddEPICSTag("HC:0L02_DYDIF", "slow_controls_data");
  AddEPICSTag("HC:0L03_WSUM", "slow_controls_data"); //sc_detector_id = 3
  AddEPICSTag("HC:0L03_DWSUM", "slow_controls_data");
  AddEPICSTag("HC:0L03_XDIF", "slow_controls_data");
  AddEPICSTag("HC:0L03_DXDIF", "slow_controls_data");
  AddEPICSTag("HC:0L03_YDIF", "slow_controls_data");
  AddEPICSTag("HC:0L03_DYDIF", "slow_controls_data");
  AddEPICSTag("HC:0I05_WSUM", "slow_controls_data");
  AddEPICSTag("HC:0I05_DWSUM", "slow_controls_data");
  AddEPICSTag("HC:0I05_XDIF", "slow_controls_data"); //--
  AddEPICSTag("HC:0I05_DXDIF", "slow_controls_data");
  AddEPICSTag("HC:0I05_YDIF", "slow_controls_data");
  AddEPICSTag("HC:0I05_DYDIF", "slow_controls_data");
  AddEPICSTag("HC:IBCM_QASY", "slow_controls_data");
  AddEPICSTag("HC:IBCM_DQASY", "slow_controls_data");
  

  
   
  /*  Add the BPM auto-gain variables to the EPICS decoding list.  */
  AddEPICSTag("IPM3C01.XIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3C01.YIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3C02.XIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3C02.YIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3C03.XIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3C03.YIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3C07A.XIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3C07A.YIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3C07.XIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3C07.YIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3C08.XIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3C08.YIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3C12.XIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3C12.YIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3C17.XIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3C17.YIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3C16.XIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3C16.YIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3C17A.XIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3C17A.YIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3C20.XIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3C20.YIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3C20A.XIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3C20A.YIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3H00.XIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3H00.YIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3H00A.XIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3H00A.YIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3H00B.XIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3H00B.YIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3H00C.XIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3H00C.YIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3HG0.XIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3HG0.YIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3HG0B.XIFG", "slow_controls_data"); //--
  AddEPICSTag("IPM3HG0B.YIFG", "slow_controls_data"); //--

  // Add Accelerator variables to EPICS decoding list 
  // HallA stuff
  AddEPICSTag("SMRPOSA", "slow_controls_data");// Hall A Chopper Slit Position
  AddEPICSTag("psub_aa_pos", "slow_controls_data"); //Hall A Laser Attenuator
  AddEPICSTag("IBCxxxxCRCUR2","slow_controls_data"); // HallA MPS BCM beam current
  AddEPICSTag("hac_bcm_average","slow_controls_data"); // HallA BCM beam current
  AddEPICSTag("halla_transmission","slow_controls_data"); // HallA independent transmission (%)
  AddEPICSTag("qe_halla","slow_controls_data"); // HallA QE (%)
  AddEPICSTag("halla_photocurrent","slow_controls_data"); // HallA photocurrent (uA)
  AddEPICSTag("laser_a_wavelength","slow_controls_data"); // HallA laser wavelength (nm) //sc_detector_id = 2
  AddEPICSTag("osc_jitter","slow_controls_data");//Laser phase jitter HallA
  // End HallA stuff


  //HallB stuff
  AddEPICSTag("SMRPOSB", "slow_controls_data");// Hall B Chopper Slit Position //sc_detector_id = 5
  AddEPICSTag("psub_ab_pos", "slow_controls_data"); //Hall B Laser Attenuator
  AddEPICSTag("IPM2C24A.IENG","slow_controls_data"); // HallB 2C24A beam current
  AddEPICSTag("scaler_calc1","slow_controls_data"); // HallB fcup nA beam current
  AddEPICSTag("hallb_transmission","slow_controls_data"); // HallB independent transmission (%)
  AddEPICSTag("qe_hallb","slow_controls_data"); // HallB QE (%)
  AddEPICSTag("hallb_photocurrent","slow_controls_data"); // HallB photocurrent (uA)
  AddEPICSTag("laser_b_wavelength","slow_controls_data"); // HallB laser wavelength (nm)
  AddEPICSTag("IGL1I00DAC2","slow_controls_data"); // HallB control level
  AddEPICSTag("IGL1I00DAC3","slow_controls_data"); // HallB seed level
  // End HallB stuff


  //HallC stuff
  AddEPICSTag("SMRPOSC", "slow_controls_data");// Hall C Chopper Slit Position
  AddEPICSTag("psub_ac_pos", "slow_controls_data"); //Hall C Laser Attenuator
  AddEPICSTag("IBCxxxxCRCUR4","slow_controls_data"); // HallC MPS BCM beam current
  AddEPICSTag("hallc_transmission","slow_controls_data"); // HallC independent transmission (%)
  AddEPICSTag("qe_hallc","slow_controls_data"); // HallC QE (%)
  AddEPICSTag("hallc_photocurrent","slow_controls_data"); // HallC photocurrent (uA)
  AddEPICSTag("laser_c_wavelength","slow_controls_data"); // HallC laser wavelength (nm) //sc_detector_id = 9
  AddEPICSTag("osc2_jitter","slow_controls_data");//Laser phase jitter HallC
  //End HallC stuff


  AddEPICSTag("ISL0I04DT", "slow_controls_data");// Master Slit position
  AddEPICSTag("R00LSUMRY.B2", "slow_controls_data"); //Prebuncher on/off //--
  AddEPICSTag("R00PGSET", "slow_controls_data"); //Prebuncher setting (dB)
  AddEPICSTag("R00PGMEST", "slow_controls_data"); //Prebuncher setting (dB)
  AddEPICSTag("IBCxxxxCLOSS", "slow_controls_data"); //Machine Energy loss
  AddEPICSTag("psub_cx_pos", "slow_controls_data"); //X laser position on cathode //sc_detector_id = 4
  AddEPICSTag("psub_cy_pos", "slow_controls_data"); //Y laser position on cathode

  AddEPICSTag("EHCFR_LIXWidth", "slow_controls_data"); // raster x dimension //--
  AddEPICSTag("EHCFR_LIYWidth", "slow_controls_data");// raster y dimension //--
  AddEPICSTag("itov1out", "slow_controls_data"); // A1 Slit Position
  AddEPICSTag("itov2out", "slow_controls_data"); // A2 Slit Position
  AddEPICSTag("itov5out", "slow_controls_data"); // A3 Slit Position
  AddEPICSTag("itov7out", "slow_controls_data"); // A4 Slit Position
  AddEPICSTag("IGL1I00PStrans", "slow_controls_data"); // Injector
						     // transmission

  AddEPICSTag("EHCFR_LIPRC","slow_controls_data");//Raster focus location: 1 = G0, 0 = Pivot

  AddEPICSTag("EHCFR_ENERGY","slow_controls_data");  //--
  // std::cout<<"\n\nmyTest for DefineEPICSVariables()\n";
};

void QwEPICSEvent::ExtractEPICSValues(const string& data, int event)
{
  /* This routine will decode the input string, which holds the  *
   * epics buffer and extract the EPICS values from it.          */

  string line;
  string stmplabel, stmpvalue;
  Double_t tmpvalue;

  Int_t tagindex;

  UInt_t pos = 0; //iterator

  tmpvalue = -999999.;

  if(kDebug==1) std::cout <<"Here we are, entering 'ExtractEPICSValues'!!"<<std::endl;

  for (tagindex=0;tagindex<fNumberEPICSVariables; tagindex++) {
    fEPICSDataEvent[tagindex].Filled = kFALSE;
  }

  //convert the input stream into a stringstream so we can 
  //do manipulation on it just like reading a file
  std::stringstream ss(data, std::stringstream::in | std::stringstream::out);
  while(!ss.eof()){  
    getline(ss,line);



    for (tagindex=0; tagindex<fNumberEPICSVariables; tagindex++) {
      pos = line.find(fEPICSVariableList[tagindex]);//pattern matching
      if(pos!=string::npos){
	/*  This line has EPICS variable "tagindex". */
	pos = line.find_first_of(" \t\n",pos);  //get to the end of the label
	//now use get_next_seg() from StringManip.C
	stmpvalue = get_next_seg(line, pos);
	if(kDebug==1) std::cout<<line<<" ==> "
			  <<fEPICSVariableList[tagindex]
			  <<"\t"<<stmpvalue<<std::endl;

	SetDataValue(fEPICSVariableList[tagindex], TString(stmpvalue.c_str()), event);

	/*  Now break out of the for loop.  */
	break;
      }
    }
  }
  
};


Int_t QwEPICSEvent::FindIndex(TString tag)
{
  Int_t tagindex;

  for (tagindex=0; tagindex<fNumberEPICSVariables; tagindex++) {
    if (tag == fEPICSVariableList[tagindex]){
      /*  This tag matches EPICS variable "tagindex". */
      return tagindex;
    }
  }
  return kEPICS_Error;
};


Double_t QwEPICSEvent::GetDataValue (TString tag)
{
 
  Double_t data_value = 0.0;
  Int_t    tagindex   = 0;

  data_value = kInvalidEPICSData;
  tagindex = FindIndex(tag);
  if (tagindex != QwEPICSEvent::kEPICS_Error){
    if (fEPICSDataEvent[tagindex].Filled){
      data_value = fEPICSDataEvent[tagindex].Value;
    }
  }
  return data_value;
};


std::vector<TString> QwEPICSEvent::GetDefaultAutogainList()
{
  return fDefaultAutogainList;
};


void QwEPICSEvent::InitDefaultAutogainList()
{
  fDefaultAutogainList.clear();  //clear the vector first

  fDefaultAutogainList.push_back("IPM3C01.XIFG");
  fDefaultAutogainList.push_back("IPM3C01.YIFG");
  fDefaultAutogainList.push_back("IPM3C02.XIFG");
  fDefaultAutogainList.push_back("IPM3C02.YIFG");
  fDefaultAutogainList.push_back("IPM3C03.XIFG");
  fDefaultAutogainList.push_back("IPM3C03.YIFG");
  fDefaultAutogainList.push_back("IPM3C07A.XIFG");
  fDefaultAutogainList.push_back("IPM3C07A.YIFG");
  fDefaultAutogainList.push_back("IPM3C07.XIFG");
  fDefaultAutogainList.push_back("IPM3C07.YIFG");
  fDefaultAutogainList.push_back("IPM3C08.XIFG");
  fDefaultAutogainList.push_back("IPM3C08.YIFG");
  fDefaultAutogainList.push_back("IPM3C12.XIFG");
  fDefaultAutogainList.push_back("IPM3C12.YIFG");
  fDefaultAutogainList.push_back("IPM3C17.XIFG");
  fDefaultAutogainList.push_back("IPM3C17.YIFG");
  fDefaultAutogainList.push_back("IPM3C16.XIFG");
  fDefaultAutogainList.push_back("IPM3C16.YIFG");
  fDefaultAutogainList.push_back("IPM3C17A.XIFG");
  fDefaultAutogainList.push_back("IPM3C17A.YIFG");
  fDefaultAutogainList.push_back("IPM3C20.XIFG");
  fDefaultAutogainList.push_back("IPM3C20.YIFG");
  fDefaultAutogainList.push_back("IPM3C20A.XIFG");
  fDefaultAutogainList.push_back("IPM3C20A.YIFG");
  fDefaultAutogainList.push_back("IPM3H00.XIFG");
  fDefaultAutogainList.push_back("IPM3H00.YIFG");
  fDefaultAutogainList.push_back("IPM3H00A.XIFG");
  fDefaultAutogainList.push_back("IPM3H00A.YIFG");
  fDefaultAutogainList.push_back("IPM3H00B.XIFG");
  fDefaultAutogainList.push_back("IPM3H00B.YIFG");
  fDefaultAutogainList.push_back("IPM3H00C.XIFG");
  fDefaultAutogainList.push_back("IPM3H00C.YIFG");
  fDefaultAutogainList.push_back("IPM3HG0.XIFG");
  fDefaultAutogainList.push_back("IPM3HG0.YIFG");
  fDefaultAutogainList.push_back("IPM3HG0B.XIFG");
  fDefaultAutogainList.push_back("IPM3HG0B.YIFG");
};


int QwEPICSEvent::SetDataValue(TString tag, Double_t value, int event)
{
  Int_t tagindex = FindIndex(tag);
  return (SetDataValue(tagindex, value, event));
};

int QwEPICSEvent::SetDataValue(Int_t tagindex, Double_t value, int event)
{
  if ( tagindex == kEPICS_Error ) return kEPICS_Error;
  if ( tagindex < 0 ) return kEPICS_Error;

  if (value != kInvalidEPICSData) {
    fEPICSDataEvent[tagindex].EventNumber = event;
    fEPICSDataEvent[tagindex].Value       = value;
    fEPICSDataEvent[tagindex].StringValue = "";
    fEPICSDataEvent[tagindex].Filled      = kTRUE;
    return 0;
  }
  return kEPICS_Error;
};


int QwEPICSEvent::SetDataValue(TString tag, TString value, int event)
{
  Double_t tmpvalue;
  long tmpint;
  Int_t tagindex; 

  tagindex = FindIndex(tag); 
  tmpvalue = kInvalidEPICSData;
  switch (fEPICSVariableType[tagindex]) {
    case EPICSString:
      fEPICSDataEvent[tagindex].EventNumber = event;
      fEPICSDataEvent[tagindex].Value       = 0.0;
      fEPICSDataEvent[tagindex].StringValue = value;
      fEPICSDataEvent[tagindex].Filled      = kTRUE;
      return 0;
      break;

    case EPICSFloat:
      if(IsNumber(value.Data())) tmpvalue = atof(value.Data());
      return SetDataValue(tagindex, tmpvalue, event);
      break;

    case EPICSInt:
      if(IsNumber(value.Data())){
	tmpint = atol(value.Data());
        tmpvalue = (double) tmpint;
      }
           
      return SetDataValue(tagindex, tmpvalue, event);
      break;
  }
  return kEPICS_Error;
};

void QwEPICSEvent::PrintAverages()
{
  Int_t tagindex;
  Double_t mean     = 0.0;
  Double_t average_of_squares = 0.0;
  Double_t variance = 0.0;
  Double_t sigma    = 0.0;
  
  std::cout <<"#####  EPICS Event Summary from PrintAverages() #####\n"
       <<"Total number of EPICS events in this run == "
       <<fNumberEPICSEvents <<".\n\n"
       << std::setw(20) << std::setiosflags(std::ios::left) <<"Name" 
       <<"\tMean        Sigma       "
       <<"Minimum     Maximum"<<std::endl;
  std::cout << "Non-string EPICS Variables" << std::endl;
  for (tagindex=0; tagindex<fNumberEPICSVariables; tagindex++) {
    if (fEPICSVariableType[tagindex] != EPICSString) { 
      mean     = 0.0;
      variance = 0.0;
      sigma    = 0.0;
      if (fEPICSCumulativeData[tagindex].NumberRecords > 0){
	mean     = (fEPICSCumulativeData[tagindex].Sum)/
	  ((Double_t) fEPICSCumulativeData[tagindex].NumberRecords);
	average_of_squares = (fEPICSCumulativeData[tagindex].SquaredSum)/
	  ((Double_t) fEPICSCumulativeData[tagindex].NumberRecords);
	variance = average_of_squares - (mean * mean);
	if (variance < 0.0){
	  sigma    = sqrt(-1.0 * variance);
	} else {
	  sigma    = sqrt(variance);
	}
      
	std::cout << std::setw(20) << std::setiosflags(std::ios::left)
	     <<fEPICSVariableList[tagindex]<<"\t"
	     << std::setprecision(5) << std::setw(10)
	     <<mean<<"  " << std::setw(10)<<sigma<<"  " << std::setw(10)
	     <<fEPICSCumulativeData[tagindex].Minimum<<"  " << std::setw(10)
	     <<fEPICSCumulativeData[tagindex].Maximum<<std::endl;
      } else {
	std::cout << std::setw(20) << std::setiosflags(std::ios::left)
	     <<fEPICSVariableList[tagindex]<<"\t"
	     << std::setprecision(5) << std::setw(10)
	     << "No data..."
	     << std::endl;
      }
    }
  }
  std::cout << "String EPICS Variables" << std::endl;
  for (tagindex=0; tagindex<fNumberEPICSVariables; tagindex++) {
    if (fEPICSVariableType[tagindex] == EPICSString) {
      if (fEPICSDataEvent[tagindex].Filled) {
	std::cout << std::setw(20) << std::setiosflags(std::ios::left)
	     <<fEPICSVariableList[tagindex]<<"\t"
	     <<fEPICSDataEvent[tagindex].StringValue;

	if (fEPICSCumulativeData[tagindex].NumberRecords 
	    != fNumberEPICSEvents){
	  std::cout << "\t*****\tThis variable changed during the run.  Only "
	       << (Double_t(fEPICSCumulativeData[tagindex].NumberRecords)
		   *100.0 / Double_t(fNumberEPICSEvents))
	       << "% of the events has this value.";
	}
	std::cout <<std::endl;
      } else {
	std::cout << std::setw(20) << std::setiosflags(std::ios::left)
	     <<fEPICSVariableList[tagindex]<<"\t"
	     <<"No data..."<<std::endl;
      }
    }
  }
};


void QwEPICSEvent::PrintVariableList()
{

  
  //Prints all valid Epics variables on the terminal while creating a rootfile.

  Int_t tagindex;

  for (tagindex=0; tagindex<fNumberEPICSVariables; tagindex++) {
    std::cout << "fEPICSVariableList[" << tagindex << "] == "
	 << fEPICSVariableList[tagindex] <<std::endl;
  }
};


std::vector<Double_t> QwEPICSEvent::ReportAutogains()
{
  return ReportAutogains(fDefaultAutogainList);
};



std::vector<Double_t> QwEPICSEvent::ReportAutogains(std::vector<TString> tag_list)
{
  std::vector<Double_t> autogain_values;
  std::vector<TString>::iterator ptr_tag;

  autogain_values.clear(); //clear the vector first

  /*  Iterate through the input vector of the variable names     *
   *  and fill the output vector with the values.                */
  ptr_tag = tag_list.begin();

  while (ptr_tag < tag_list.end()) {
    autogain_values.push_back(GetDataValue(*ptr_tag));
    ptr_tag++;
  }

  return autogain_values;
};



void QwEPICSEvent::ReportEPICSData()
{
  Int_t tagindex;
  Double_t mean     = 0.0;
  Double_t average_of_squares = 0.0;
  Double_t variance = 0.0;
  Double_t sigma    = 0.0;


  ofstream output;
  TString theEPICSDataFile;
  theEPICSDataFile =  getenv("QW_TMP");
  theEPICSDataFile += "/QwEPICSData.txt";// puts QwEPICSData.txt in  QwAnalysis_DB_01.00.0000/scratch/tmp/ diretory.
  output.open(theEPICSDataFile,ofstream::out);

  if (output.is_open()) { 


  output <<"#####  EPICS Event Summary  #####\n"
	 <<"Total number of EPICS events in this run == "
	 <<fNumberEPICSEvents <<".\n\n"
	 << std::setw(20) << std::setiosflags(std::ios::left) <<"Name"
	 <<"\tMean        Sigma       "
	 <<"Minimum     Maximum"<<std::endl;
  for (tagindex=0; tagindex<fNumberEPICSVariables; tagindex++) {
    if (fEPICSVariableType[tagindex] == EPICSFloat
	|| fEPICSVariableType[tagindex] == EPICSInt) {
      mean     = 0.0;
      variance = 0.0;
      sigma    = 0.0;
      if (fEPICSCumulativeData[tagindex].NumberRecords > 0){
	mean     = (fEPICSCumulativeData[tagindex].Sum)/
	  ((Double_t) fEPICSCumulativeData[tagindex].NumberRecords);
	average_of_squares = (fEPICSCumulativeData[tagindex].SquaredSum)/
	  ((Double_t) fEPICSCumulativeData[tagindex].NumberRecords);
	variance = average_of_squares - (mean * mean);
	if (variance < 0.0){
	  sigma    = sqrt(-1.0 * variance);
	} else {
	  sigma    = sqrt(variance);
	}
      
	output << std::setw(20) << std::setiosflags(std::ios::left)
	       <<fEPICSVariableList[tagindex]<<"\t"
	       << std::setprecision(5) << std::setw(10)
	       <<mean<<"  " << std::setw(10)<<sigma<<"  " << std::setw(10)
	       <<fEPICSCumulativeData[tagindex].Minimum<<"  " << std::setw(10)
	       <<fEPICSCumulativeData[tagindex].Maximum<<std::endl;
      } else {
	output << std::setw(20) << std::setiosflags(std::ios::left)
	       <<fEPICSVariableList[tagindex]<<"\t"
	       << std::setprecision(5) << std::setw(10)
	       << "No data..."
	       << std::endl;
      }
    }
  }
  for (tagindex=0; tagindex<fNumberEPICSVariables; tagindex++) {
    if (fEPICSVariableType[tagindex] == EPICSString) {
      if (fEPICSDataEvent[tagindex].Filled) {
	output << std::setw(20) << std::setiosflags(std::ios::left)
	       <<fEPICSVariableList[tagindex]<<"\t"
	       <<fEPICSDataEvent[tagindex].StringValue<<std::endl;
      } else {
	output << std::setw(20) << std::setiosflags(std::ios::left)
	       <<fEPICSVariableList[tagindex]<<"\t"
	       <<"No data..."<<std::endl;
      }
    }
  }
  
  } output.close();
};


  
void  QwEPICSEvent::ResetCounters()
{
  Int_t tagindex;
  Int_t listlength;
  Int_t typelength;


  /*  Verify that the variable list and variable type vectors
      are the same size, and agree with fNumberEPICSVariables.  */
  listlength = fEPICSVariableList.size();
  typelength = fEPICSVariableType.size();
  if (listlength != typelength || typelength != fNumberEPICSVariables
      || listlength != fNumberEPICSVariables){
    std::cerr << "The EPICS variable label and type arrays are not the same size!\n"
	 << "EPICS readback may be corrupted!"<<std::endl;
  }


  /*  Reset the sizes of the fEPICSDataEvent and
      fEPICSCumulativeData vectors.                             */
  fEPICSDataEvent.resize(fNumberEPICSVariables);
  fEPICSCumulativeData.resize(fNumberEPICSVariables);
  for (tagindex=0; tagindex<fNumberEPICSVariables; tagindex++){
    fEPICSDataEvent[tagindex].Filled      = kFALSE;
    fEPICSDataEvent[tagindex].EventNumber = 0;
    fEPICSDataEvent[tagindex].Value       = 0.0;
    fEPICSDataEvent[tagindex].StringValue = "";
    fEPICSCumulativeData[tagindex].Filled        = kFALSE;
    fEPICSCumulativeData[tagindex].NumberRecords = 0;
    fEPICSCumulativeData[tagindex].Sum           = 0.0;
    fEPICSCumulativeData[tagindex].SquaredSum    = 0.0;
    fEPICSCumulativeData[tagindex].Minimum       = 0.0;
    fEPICSCumulativeData[tagindex].Maximum       = 0.0;
  }
  fNumberEPICSEvents = 0;
}

void QwEPICSEvent::SetDefaultAutogainList(std::vector<TString> input_list)
{
  fDefaultAutogainList.clear();  //clear the vector first
  fDefaultAutogainList = input_list;
};




 void QwEPICSEvent::FillDB(QwDatabase *db)
 {
   FillSlowControlsData(db);
   
 };


void QwEPICSEvent::FillSlowControlsData(QwDatabase *db)
{

  QwMessage << " -------------------------------------------------------------------------- " << QwLog::endl;
  QwMessage << "                         QwEPICSEvent::FillSlowControlsData(QwDatabase *db) " << QwLog::endl;
  QwMessage << " -------------------------------------------------------------------------- " << QwLog::endl;

  Double_t mean, average_of_squares, variance, sigma;

 //  Figure out if the target table has this runlet_id in it already,
 //  if not, create a new entry with the current runlet_id.

  UInt_t runlet_id = db->GetRunletID();

  std::vector<QwParityDB::slow_controls_data> entrylist;

  UInt_t sc_detector_id;

  TString table = "slow_controls_data";

  // Loop over EPICS variables
  for (Int_t tagindex = 0; tagindex < fNumberEPICSVariables; tagindex++) {
    // Look for variables to write into this table
    if (fEPICSTableList[tagindex] == table) {
    
      QwParityDB::slow_controls_data tmp_row(0);

//  Now get the current sc_detector_id for the above runlet_id.   
 
      sc_detector_id = db->GetSlowControlDetectorID(fEPICSVariableList[tagindex].Data());
      
      tmp_row.runlet_id      = runlet_id;
      tmp_row.sc_detector_id = sc_detector_id;

      if (!sc_detector_id) continue;
      


      // Calculate average and error
      if (fEPICSVariableType[tagindex] == EPICSFloat
          || fEPICSVariableType[tagindex] == EPICSInt) {
        mean     = 0.0;
        variance = 0.0;
        sigma    = 0.0;
        if (fEPICSCumulativeData[tagindex].NumberRecords > 0){

          mean     = (fEPICSCumulativeData[tagindex].Sum)/
            ((Double_t) fEPICSCumulativeData[tagindex].NumberRecords);
          average_of_squares = (fEPICSCumulativeData[tagindex].SquaredSum)/
            ((Double_t) fEPICSCumulativeData[tagindex].NumberRecords);
          variance = average_of_squares - (mean * mean);
          if (variance < 0.0){
            sigma    = sqrt(-1.0 * variance);
          } else {
            sigma    = sqrt(variance);
          }
        }
      }
      tmp_row.value         = mean;
      tmp_row.error         = sigma;
      tmp_row.min_value     = fEPICSCumulativeData[tagindex].Minimum;
      tmp_row.max_value     = fEPICSCumulativeData[tagindex].Maximum;
      
      entrylist.push_back(tmp_row);

    }
}

  db->Connect();
  // Check the entrylist size, if it isn't zero, start to query..
  if( entrylist.size() ) {
    mysqlpp::Query query= db->Query();
    //    if(query)
    //	{
    query.insert(entrylist.begin(), entrylist.end());
    query.execute();
    //	  query.reset(); // do we need?
    //	}
    //      else
    //	{
    //	  printf("Query is empty\n");
    //	}
  } else {
    QwMessage << "QwEPICSEvent::FillSlowControlsData :: This is the case when the entrylist contains nothing " << QwLog::endl;
  }
  db->Disconnect();


};










