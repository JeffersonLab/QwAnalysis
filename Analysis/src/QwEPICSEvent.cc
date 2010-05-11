#include "QwEPICSEvent.h"

#include <TStyle.h>
#include <TFile.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <stdlib.h>

#include <iostream>
using namespace std;



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

  if (kDebug==1) cout <<"\n\nHere we are in 'CalculateRunningValues'!!\n"<<endl;
  
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
	  if (kDebug==1) cout << "This event has "<<fEPICSVariableList[tagindex]
			     << " equal to "<< fEPICSDataEvent[tagindex].Value
			     << " giving a running average of "
			     << fEPICSCumulativeData[tagindex].Sum/
			  fEPICSCumulativeData[tagindex].NumberRecords
			     <<endl;
	} else {
	  if (kDebug==1) cout <<fEPICSVariableList[tagindex]
			     <<" seems to be not filled."<<endl;
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
  cout<<"\n\nfNumberEPICSEvents = "<<fNumberEPICSEvents<<endl<<endl;
};

void QwEPICSEvent::DefineEPICSVariables()
{
  fNumberEPICSVariables = 0;
  fEPICSVariableList.clear();
  fEPICSVariableType.clear();


  AddEPICSTag("ibcm1");
  AddEPICSTag("ibcm2");
  AddEPICSTag("g0tpos_switch", "target", EPICSString);
  //  Add the G0 electronics cage tmeperatures to the readout
  AddEPICSTag("g0cagetemp1","target_measurements");
  AddEPICSTag("g0cagetemp2","target_measurements");
  AddEPICSTag("g0cagetemp3","target_measurements");
  AddEPICSTag("g0tach","target_measurements"); //g0target pumpspeed
  AddEPICSTag("gt3", "target_measurements");
  AddEPICSTag("gpt3", "target_measurements");
  AddEPICSTag("gflyswatter","target_measurements");//flyswatter in/out
  AddEPICSTag("IHM3H01E","target_measurements"); // Halo target position
  AddEPICSTag("g0smsvac", "sms_measurements");
  AddEPICSTag("G0:PSCurrent", "sms_measurements");
  AddEPICSTag("G0:ROXCoilA", "sms_measurements");
  AddEPICSTag("G0:LHeLevel", "sms_measurements");
  AddEPICSTag("G0:LN2Level", "sms_measurements");
  AddEPICSTag("G0:TCCryoBox", "sms_measurements");
  AddEPICSTag("HC:Q_ONOFF", "polarized_source", EPICSString); // Charge Feedback on/off
  AddEPICSTag("HC:Q_ASY", "polarized_source_measurements"); // Charge asymmetry
  AddEPICSTag("HC:DQ_ASY", "polarized_source_measurements"); // Charge asymmetry uncertainty
  AddEPICSTag("HC:bcm_threshold", "polarized_source"); // BCM threshold cut for charge feedback
  AddEPICSTag("HC:bcm_select", "polarized_source"); // BCM selection for charge feedback
  AddEPICSTag("psub_pl_pos", "polarized_source_measurements"); // RHWP angle
  AddEPICSTag("IGL1I00DI24_24M", "polarized_source", EPICSString); // IHWP
  AddEPICSTag("WienAngle", "polarized_source_measurements"); // Wien filter setting
  AddEPICSTag("g0_pivot_halo","target");
  AddEPICSTag("IGL1I00AI24", "polarized_source_measurements"); //IA voltage
  AddEPICSTag("HC:P_ONOFF", "polarized_source");
  AddEPICSTag("HC:X_DIF", "polarized_source_measurements"); 
  AddEPICSTag("HC:DX_DIF", "polarized_source_measurements");
  AddEPICSTag("HC:Y_DIF", "polarized_source_measurements");
  AddEPICSTag("HC:DY_DIF", "polarized_source_measurements");
  AddEPICSTag("HC:bpm_select", "polarized_source");
  AddEPICSTag("IGLdac3:ao_0", "polarized_source_measurements"); // X pzt voltage
  AddEPICSTag("IGLdac3:ao_1", "polarized_source_measurements"); // Y pzt voltage
  AddEPICSTag("IGL1I00AI22", "polarized_source_measurements");  // X G0 pzt voltage
  AddEPICSTag("IGL1I00AI23", "polarized_source_measurements");   // Y G0 pzt voltage
  AddEPICSTag("IGL1I00AI7","polarized_source_measurements");// Gun2 Pita Positive
  AddEPICSTag("IGL1I00AI8","polarized_source_measurements");// Gun2 Pita Negative
  AddEPICSTag("IGL1I00AI27","polarized_source_measurements");// HallA IA voltage
  AddEPICSTag("IGL1I00AI25","polarized_source_measurements");// HallA PZT X voltage
  AddEPICSTag("IGL1I00AI26","polarized_source_measurements");// HallA PZT Y voltage
  AddEPICSTag("IGLdac2:scale_6.A","polarized_source_measurements");//HallB TACO  


  AddEPICSTag("hel_mag_status.D","polarized_source_measurements"); //Integrated helicity bit difference
  AddEPICSTag("IGL1I00AI17","polarized_source_measurements"); // dummy helicity Pockels' cell voltage


  AddEPICSTag("HC:qint_period", "polarized_source_measurements"); // charge asymmetry sampling period
  AddEPICSTag("HC:pint_period", "polarized_source_measurements"); // position difference sampling period
  

  
  /* add injector BPM values in here jyun July 17th, 2003  */
 
  AddEPICSTag("HC:QPD_WSUM", "polarized_source_measurements");
  AddEPICSTag("HC:QPD_DWSUM", "polarized_source_measurements");
  AddEPICSTag("HC:QPD_XDIF", "polarized_source_measurements");
  AddEPICSTag("HC:QPD_DXDIF", "polarized_source_measurements");
  AddEPICSTag("HC:QPD_YDIF", "polarized_source_measurements");
  AddEPICSTag("HC:QPD_DYDIF", "polarized_source_measurements");
  AddEPICSTag("HC:1I02_WSUM", "polarized_source_measurements");
  AddEPICSTag("HC:1I02_DWSUM", "polarized_source_measurements");
  AddEPICSTag("HC:1I02_XDIF", "polarized_source_measurements");
  AddEPICSTag("HC:1I02_DXDIF", "polarized_source_measurements");
  AddEPICSTag("HC:1I02_YDIF", "polarized_source_measurements");
  AddEPICSTag("HC:1I02_DYDIF", "polarized_source_measurements");
  AddEPICSTag("HC:1I04_WSUM", "polarized_source_measurements");
  AddEPICSTag("HC:1I04_DWSUM", "polarized_source_measurements");
  AddEPICSTag("HC:1I04_XDIF", "polarized_source_measurements");
  AddEPICSTag("HC:1I04_DXDIF", "polarized_source_measurements");
  AddEPICSTag("HC:1I04_YDIF", "polarized_source_measurements");
  AddEPICSTag("HC:1I04_DYDIF", "polarized_source_measurements");
  AddEPICSTag("HC:1I06_WSUM", "polarized_source_measurements");
  AddEPICSTag("HC:1I06_DWSUM", "polarized_source_measurements");
  AddEPICSTag("HC:1I06_XDIF", "polarized_source_measurements");
  AddEPICSTag("HC:1I06_DXDIF", "polarized_source_measurements");
  AddEPICSTag("HC:1I06_YDIF", "polarized_source_measurements");
  AddEPICSTag("HC:1I06_DYDIF", "polarized_source_measurements");
  AddEPICSTag("HC:0L02_WSUM", "polarized_source_measurements");
  AddEPICSTag("HC:0L02_DWSUM", "polarized_source_measurements");
  AddEPICSTag("HC:0L02_XDIF", "polarized_source_measurements");
  AddEPICSTag("HC:0L02_DXDIF", "polarized_source_measurements");
  AddEPICSTag("HC:0L02_YDIF", "polarized_source_measurements");
  AddEPICSTag("HC:0L02_DYDIF", "polarized_source_measurements");
  AddEPICSTag("HC:0L03_WSUM", "polarized_source_measurements");
  AddEPICSTag("HC:0L03_DWSUM", "polarized_source_measurements");
  AddEPICSTag("HC:0L03_XDIF", "polarized_source_measurements");
  AddEPICSTag("HC:0L03_DXDIF", "polarized_source_measurements");
  AddEPICSTag("HC:0L03_YDIF", "polarized_source_measurements");
  AddEPICSTag("HC:0L03_DYDIF", "polarized_source_measurements");
  AddEPICSTag("HC:0I05_WSUM", "polarized_source_measurements");
  AddEPICSTag("HC:0I05_DWSUM", "polarized_source_measurements");
  AddEPICSTag("HC:0I05_XDIF", "polarized_source_measurements");
  AddEPICSTag("HC:0I05_DXDIF", "polarized_source_measurements");
  AddEPICSTag("HC:0I05_YDIF", "polarized_source_measurements");
  AddEPICSTag("HC:0I05_DYDIF", "polarized_source_measurements");
  AddEPICSTag("HC:IBCM_QASY", "polarized_source_measurements");
  AddEPICSTag("HC:IBCM_DQASY", "polarized_source_measurements");
  

  
   
  /*  Add the BPM auto-gain variables to the EPICS decoding list.  */
  AddEPICSTag("IPM3C01.XIFG");
  AddEPICSTag("IPM3C01.YIFG");
  AddEPICSTag("IPM3C02.XIFG");
  AddEPICSTag("IPM3C02.YIFG");
  AddEPICSTag("IPM3C03.XIFG");
  AddEPICSTag("IPM3C03.YIFG");
  AddEPICSTag("IPM3C07A.XIFG");
  AddEPICSTag("IPM3C07A.YIFG");
  AddEPICSTag("IPM3C07.XIFG");
  AddEPICSTag("IPM3C07.YIFG");
  AddEPICSTag("IPM3C08.XIFG");
  AddEPICSTag("IPM3C08.YIFG");
  AddEPICSTag("IPM3C12.XIFG");
  AddEPICSTag("IPM3C12.YIFG");
  AddEPICSTag("IPM3C17.XIFG");
  AddEPICSTag("IPM3C17.YIFG");
  AddEPICSTag("IPM3C16.XIFG");
  AddEPICSTag("IPM3C16.YIFG");
  AddEPICSTag("IPM3C17A.XIFG");
  AddEPICSTag("IPM3C17A.YIFG");
  AddEPICSTag("IPM3C20.XIFG");
  AddEPICSTag("IPM3C20.YIFG");
  AddEPICSTag("IPM3C20A.XIFG");
  AddEPICSTag("IPM3C20A.YIFG");
  AddEPICSTag("IPM3H00.XIFG");
  AddEPICSTag("IPM3H00.YIFG");
  AddEPICSTag("IPM3H00A.XIFG");
  AddEPICSTag("IPM3H00A.YIFG");
  AddEPICSTag("IPM3H00B.XIFG");
  AddEPICSTag("IPM3H00B.YIFG");
  AddEPICSTag("IPM3H00C.XIFG");
  AddEPICSTag("IPM3H00C.YIFG");
  AddEPICSTag("IPM3HG0.XIFG");
  AddEPICSTag("IPM3HG0.YIFG");
  AddEPICSTag("IPM3HG0B.XIFG");
  AddEPICSTag("IPM3HG0B.YIFG");

  // Add Accelerator variables to EPICS decoding list 
  // HallA stuff
  AddEPICSTag("SMRPOSA", "polarized_source_measurements");// Hall A Chopper Slit Position
  AddEPICSTag("psub_aa_pos", "polarized_source_measurements"); //Hall A Laser Attenuator
  AddEPICSTag("IBCxxxxCRCUR2","polarized_source_measurements"); // HallA MPS BCM beam current
  AddEPICSTag("hac_bcm_average","polarized_source_measurements"); // HallA BCM beam current
  AddEPICSTag("halla_transmission","polarized_source_measurements"); // HallA independent transmission (%)
  AddEPICSTag("qe_halla","polarized_source_measurements"); // HallA QE (%)
  AddEPICSTag("halla_photocurrent","polarized_source_measurements"); // HallA photocurrent (uA)
  AddEPICSTag("laser_a_wavelength","polarized_source_measurements"); // HallA laser wavelength (nm)
  AddEPICSTag("osc_jitter","polarized_source_measurements");//Laser phase jitter HallA
  // End HallA stuff


  //HallB stuff
  AddEPICSTag("SMRPOSB", "polarized_source_measurements");// Hall B Chopper Slit Position
  AddEPICSTag("psub_ab_pos", "polarized_source_measurements"); //Hall B Laser Attenuator
  AddEPICSTag("IPM2C24A.IENG","polarized_source_measurements"); // HallB 2C24A beam current
  AddEPICSTag("scaler_calc1","polarized_source_measurements"); // HallB fcup nA beam current
  AddEPICSTag("hallb_transmission","polarized_source_measurements"); // HallB independent transmission (%)
  AddEPICSTag("qe_hallb","polarized_source_measurements"); // HallB QE (%)
  AddEPICSTag("hallb_photocurrent","polarized_source_measurements"); // HallB photocurrent (uA)
  AddEPICSTag("laser_b_wavelength","polarized_source_measurements"); // HallB laser wavelength (nm)
  AddEPICSTag("IGL1I00DAC2","polarized_source_measurements"); // HallB control level
  AddEPICSTag("IGL1I00DAC3","polarized_source_measurements"); // HallB seed level
  // End HallB stuff


  //HallC stuff
  AddEPICSTag("SMRPOSC", "polarized_source_measurements");// Hall C Chopper Slit Position
  AddEPICSTag("psub_ac_pos", "polarized_source_measurements"); //Hall C Laser Attenuator
  AddEPICSTag("IBCxxxxCRCUR4","polarized_source_measurements"); // HallC MPS BCM beam current
  AddEPICSTag("hallc_transmission","polarized_source_measurements"); // HallC independent transmission (%)
  AddEPICSTag("qe_hallc","polarized_source_measurements"); // HallC QE (%)
  AddEPICSTag("hallc_photocurrent","polarized_source_measurements"); // HallC photocurrent (uA)
  AddEPICSTag("laser_c_wavelength","polarized_source_measurements"); // HallC laser wavelength (nm)
  AddEPICSTag("osc2_jitter","polarized_source_measurements");//Laser phase jitter HallC
  //End HallC stuff


  AddEPICSTag("ISL0I04DT", "polarized_source_measurements");// Master Slit position
  AddEPICSTag("R00LSUMRY.B2", "polarized_source"); //Prebuncher on/off
  AddEPICSTag("R00PGSET", "polarized_source_measurements"); //Prebuncher setting (dB)
  AddEPICSTag("R00PGMEST", "polarized_source_measurements"); //Prebuncher setting (dB)
  AddEPICSTag("IBCxxxxCLOSS", "polarized_source_measurements"); //Machine Energy loss
  AddEPICSTag("psub_cx_pos", "polarized_source_measurements"); //X laser position on cathode
  AddEPICSTag("psub_cy_pos", "polarized_source_measurements"); //Y laser position on cathode

  AddEPICSTag("EHCFR_LIXWidth", "target_measurements"); // raster x dimension
  AddEPICSTag("EHCFR_LIYWidth", "target_measurements");// raster y dimension
  AddEPICSTag("itov1out", "polarized_source_measurements"); // A1 Slit Position
  AddEPICSTag("itov2out", "polarized_source_measurements"); // A2 Slit Position
  AddEPICSTag("itov5out", "polarized_source_measurements"); // A3 Slit Position
  AddEPICSTag("itov7out", "polarized_source_measurements"); // A4 Slit Position
  AddEPICSTag("IGL1I00PStrans", "polarized_source_measurements"); // Injector
						     // transmission

  AddEPICSTag("EHCFR_LIPRC","polarized_source_measurements");//Raster focus location: 1 = G0, 0 = Pivot

  AddEPICSTag("EHCFR_ENERGY");
  // cout<<"\n\nmyTest for DefineEPICSVariables()\n";
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

  if(kDebug==1) cout <<"Here we are, entering 'ExtractEPICSValues'!!"<<endl;

  for (tagindex=0;tagindex<fNumberEPICSVariables; tagindex++) {
    fEPICSDataEvent[tagindex].Filled = kFALSE;
  }

  //convert the input stream into a stringstream so we can 
  //do manipulation on it just like reading a file
  stringstream ss(data, stringstream::in | stringstream::out);
  while(!ss.eof()){  
    getline(ss,line);



    for (tagindex=0; tagindex<fNumberEPICSVariables; tagindex++) {
      pos = line.find(fEPICSVariableList[tagindex]);//pattern matching
      if(pos!=string::npos){
	/*  This line has EPICS variable "tagindex". */
	pos = line.find_first_of(" \t\n",pos);  //get to the end of the label
	//now use get_next_seg() from StringManip.C
	stmpvalue = get_next_seg(line, pos);
	if(kDebug==1) cout<<line<<" ==> "
			  <<fEPICSVariableList[tagindex]
			  <<"\t"<<stmpvalue<<endl;

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


vector<TString> QwEPICSEvent::GetDefaultAutogainList()
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
  

  cout <<"#####  EPICS Event Summary  #####\n"
       <<"Total number of EPICS events in this run == "
       <<fNumberEPICSEvents <<".\n\n"
       << setw(20) << setiosflags(ios::left) <<"Name" 
       <<"\tMean        Sigma       "
       <<"Minimum     Maximum"<<endl;
  cout << "Non-string EPICS Variables" << endl;
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
      
	cout << setw(20) << setiosflags(ios::left)
	     <<fEPICSVariableList[tagindex]<<"\t"
	     << setprecision(5) << setw(10)
	     <<mean<<"  " << setw(10)<<sigma<<"  " << setw(10)
	     <<fEPICSCumulativeData[tagindex].Minimum<<"  " << setw(10)
	     <<fEPICSCumulativeData[tagindex].Maximum<<endl;
      } else {
	cout << setw(20) << setiosflags(ios::left)
	     <<fEPICSVariableList[tagindex]<<"\t"
	     << setprecision(5) << setw(10)
	     << "No data..."
	     << endl;
      }
    }
  }
  cout << "String EPICS Variables" << endl;
  for (tagindex=0; tagindex<fNumberEPICSVariables; tagindex++) {
    if (fEPICSVariableType[tagindex] == EPICSString) {
      if (fEPICSDataEvent[tagindex].Filled) {
	cout << setw(20) << setiosflags(ios::left)
	     <<fEPICSVariableList[tagindex]<<"\t"
	     <<fEPICSDataEvent[tagindex].StringValue;

	if (fEPICSCumulativeData[tagindex].NumberRecords 
	    != fNumberEPICSEvents){
	  cout << "\t*****\tThis variable changed during the run.  Only "
	       << (Double_t(fEPICSCumulativeData[tagindex].NumberRecords)
		   *100.0 / Double_t(fNumberEPICSEvents))
	       << "% of the events has this value.";
	}
	cout <<endl;
      } else {
	cout << setw(20) << setiosflags(ios::left)
	     <<fEPICSVariableList[tagindex]<<"\t"
	     <<"No data..."<<endl;
      }
    }
  }
};


void QwEPICSEvent::PrintVariableList()
{

  
  //Prints all valid Epics variables on the terminal while creating a rootfile.

  Int_t tagindex;

  for (tagindex=0; tagindex<fNumberEPICSVariables; tagindex++) {
    cout << "fEPICSVariableList[" << tagindex << "] == "
	 << fEPICSVariableList[tagindex] <<endl;
  }
};


vector<Double_t> QwEPICSEvent::ReportAutogains()
{
  return ReportAutogains(fDefaultAutogainList);
};



vector<Double_t> QwEPICSEvent::ReportAutogains(vector<TString> tag_list)
{
  vector<Double_t> autogain_values;
  vector<TString>::iterator ptr_tag;

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
  theEPICSDataFile += "/QwEPICSData.txt";// puts QwEPICSData.txt in /u/group/qweak/rsubedi/QwAnalysis/scratch/tmp/ diretory.
  output.open(theEPICSDataFile,ofstream::out);

  if (output.is_open()) { 


  output <<"#####  EPICS Event Summary  #####\n"
	 <<"Total number of EPICS events in this run == "
	 <<fNumberEPICSEvents <<".\n\n"
	 << setw(20) << setiosflags(ios::left) <<"Name"
	 <<"\tMean        Sigma       "
	 <<"Minimum     Maximum"<<endl;
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
      
	output << setw(20) << setiosflags(ios::left)
	       <<fEPICSVariableList[tagindex]<<"\t"
	       << setprecision(5) << setw(10)
	       <<mean<<"  " << setw(10)<<sigma<<"  " << setw(10)
	       <<fEPICSCumulativeData[tagindex].Minimum<<"  " << setw(10)
	       <<fEPICSCumulativeData[tagindex].Maximum<<endl;
      } else {
	output << setw(20) << setiosflags(ios::left)
	       <<fEPICSVariableList[tagindex]<<"\t"
	       << setprecision(5) << setw(10)
	       << "No data..."
	       << endl;
      }
    }
  }
  for (tagindex=0; tagindex<fNumberEPICSVariables; tagindex++) {
    if (fEPICSVariableType[tagindex] == EPICSString) {
      if (fEPICSDataEvent[tagindex].Filled) {
	output << setw(20) << setiosflags(ios::left)
	       <<fEPICSVariableList[tagindex]<<"\t"
	       <<fEPICSDataEvent[tagindex].StringValue<<endl;
      } else {
	output << setw(20) << setiosflags(ios::left)
	       <<fEPICSVariableList[tagindex]<<"\t"
	       <<"No data..."<<endl;
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
    cerr << "The EPICS variable label and type arrays are not the same size!\n"
	 << "EPICS readback may be corrupted!"<<endl;
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

void QwEPICSEvent::SetDefaultAutogainList(vector<TString> input_list)
{
  fDefaultAutogainList.clear();  //clear the vector first
  fDefaultAutogainList = input_list;
};




// void QwEPICSEvent::FillDB(QwDatabase *db)
// {
//   TString datatype; 
//   vector<QwParityDB::target> entrylist;
 
//   //this->FillTargetTables(QwDatabase *db);
//   // and so on...

//   db->Connect();
//   // Check the entrylist size, if it isn't zero, start to query..
//   if( entrylist.size() )
//     {
//       mysqlpp::Query query= db->Query();
// 	  query.insert(entrylist.begin(), entrylist.end());
// 	  query.execute();
//     }
//   else
//     {
//       printf("This is the case when the entrlylist contains nothing in %s \n", datatype.Data());
//     }

//   db->Disconnect();

//   return;

// };


// void QwEPICSEvent::FillTargetTables(QwDatabase *db)
// {
//   QwParityDB::target row(0);
// //   //  Figure out if the target table has this run_id in it already,
// //   //  if not, create a new entry with the current run_id.
  
// //   //  Now get the current target_id for this run_id.
  
  
//   vector<QwParityDB::target_measurement> entrylist;
  
//   TString table_name = "target_measurement";
  
//   // Loop over EPICS variables
//   for (Int_t tagindex = 0; tagindex < fNumberEPICSVariables; tagindex++) {
    
//     // Look for variables to write into this table
//     if (fEPICSTableList[tagindex] == table_name) {
//       QwParityDB::target_measurement tmp_row(0);
      
//       tmp_row.target_id = thetargetidthatwefoundabove;
//       tmp_row.target_monitor_id = somefuntiontolookupthetargetmonitorid;
      
//       if (! wefoundatargetmonitorid) continue;
      
//       // Calculate average and error
//       if (fEPICSVariableType[tagindex] == EPICSFloat
//           || fEPICSVariableType[tagindex] == EPICSInt) {
//         mean     = 0.0;
//         variance = 0.0;
//         sigma    = 0.0;
//         if (fEPICSCumulativeData[tagindex].NumberRecords > 0){
//           mean     = (fEPICSCumulativeData[tagindex].Sum)/
//             ((Double_t) fEPICSCumulativeData[tagindex].NumberRecords);
//           average_of_squares = (fEPICSCumulativeData[tagindex].SquaredSum)/
//             ((Double_t) fEPICSCumulativeData[tagindex].NumberRecords);
//           variance = average_of_squares - (mean * mean);
//           if (variance < 0.0){
//             sigma    = sqrt(-1.0 * variance);
//           } else {
//             sigma    = sqrt(variance);
//           }
//         }
//       }
//       tmp_row.average_value = mean;
//       tmp_row.error         = sigma;
//       tmp_row.min_value     = fEPICSCumulativeData[tagindex].Minimum;
//       tmp_row.max_value     = fEPICSCumulativeData[tagindex].Maximum;
      
//       entrylist.push_back(tmp_row);
//     }
//   }  
//  };










