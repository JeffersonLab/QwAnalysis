#include "QwEPICSEvent.h"

#define MYSQLPP_SSQLS_NO_STATICS
#include "QwSSQLS.h"
#include "QwDatabase.h"


#include <TStyle.h>
#include <TFile.h>

#include <iostream>
#include <fstream>

#include "StringManip.h" 

#include "QwParameterFile.h"

/*************************************
 *  Constant definitions.
 *************************************/

const int QwEPICSEvent::kDebug  = 1;  /* Debugging flag, set this to 1 to  *
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
  if (kDebug==1) PrintVariableList();
};

QwEPICSEvent::~QwEPICSEvent(){};


/*************************************
 *  Member functions.
 *************************************/



Int_t QwEPICSEvent::LoadEpicsVariableMap(TString mapfile) {
  TString varname, dbtable, datatype;
  EQwEPICSDataType datatypeid;
  Int_t lineread=0;  
  QwParameterFile mapstr(mapfile.Data());  //Open the file
  std::cout<<"\nReading epics parameter file: "<<mapfile<<"\n";
   fNumberEPICSVariables = 0;
  fEPICSVariableList.clear();
  fEPICSVariableType.clear();
  while (mapstr.ReadNextLine()) {
    lineread+=1;
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;
    else {
      varname     = mapstr.GetNextToken(" \t").c_str();
      dbtable     = mapstr.GetNextToken(" \t").c_str();
      datatype    = mapstr.GetNextToken(" \t").c_str();
      datatype.ToLower();
      std::cout<<"\t "<<varname<<"\t\t\t\t "<<dbtable<<"\t\t "<<datatype<<"\n";
      if (datatype == "") {
	AddEPICSTag(varname,dbtable);
	} else {
	if (datatype=="string")  datatypeid = kEPICSString;
	if (datatype=="float")   datatypeid = kEPICSFloat;
	if (datatype=="int")     datatypeid = kEPICSInt;
	AddEPICSTag(varname,dbtable,datatypeid);
	}
    }
  }
  //std::cout<<"\n\nfNumberEPICSVariables = "<<fNumberEPICSVariables<<std::endl<<std::endl;
  std::cout<<" line read in the parameter file ="<<lineread<<" \n";
  ResetCounters();
  return 0;
};


//HC:Q_ONOFF          	slow_controls_data     	   	kEPICSString 
//IGL1I00DI24_24M     	slow_controls_data     	   	kEPICSString  !halfwave plate status

int QwEPICSEvent::AddEPICSTag(TString tag)
{
  return AddEPICSTag(tag, "", kEPICSFloat);
};

int QwEPICSEvent::AddEPICSTag(TString tag, TString table)
{
  return AddEPICSTag(tag, table, kEPICSFloat);
};

int QwEPICSEvent::AddEPICSTag(TString tag, TString table, 
			      EQwEPICSDataType datatype)
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
  if (kDebug==1) std::cout <<"\n\nHere we are in 'CalculateRunningValues'!!"<<std::endl;
  if (kDebug==1) std::cout<<"fNumberEPICSVariables = "<<fNumberEPICSVariables<<std::endl<<std::endl;
  anyFilled = kFALSE;
  for (tagindex=0; tagindex<(Int_t)fEPICSVariableList.size(); tagindex++) {
    anyFilled  |=  fEPICSDataEvent[tagindex].Filled;
  }
  if (anyFilled){
    //QwError << "--Did I enter anyFilled?--" << QwLog::endl;
    for (tagindex=0; tagindex<fNumberEPICSVariables; tagindex++) {
    if (fEPICSVariableType[tagindex] == kEPICSFloat || fEPICSVariableType[tagindex] == kEPICSInt) {
      //if (fEPICSVariableType[tagindex] != kEPICSString){
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
    }

    ////////////////////////////////
	else {
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
    //////////////////////



// //////////////////////
//     }    
//     for (tagindex=0; tagindex<fNumberEPICSVariables; tagindex++) {
//       if (fEPICSVariableType[tagindex] == kEPICSString){
// 	  //  This is a string value.
// 	  //  Just check to see if the EPICS string has changed;
// 	  //  if it is the same, increment the counter.

// 	  if (! fEPICSCumulativeData[tagindex].Filled){
// 	    fEPICSCumulativeData[tagindex].NumberRecords = 0;
// 	    fEPICSCumulativeData[tagindex].SavedString   = 
// 	      fEPICSDataEvent[tagindex].StringValue;
// 	  fEPICSCumulativeData[tagindex].Filled        = kTRUE;
// 	  }
// 	  if (fEPICSCumulativeData[tagindex].SavedString
// 	      == fEPICSDataEvent[tagindex].StringValue ){
// 	    fEPICSCumulativeData[tagindex].NumberRecords++;
// 	  }
//       }
//     }
//     fNumberEPICSEvents++;
//     ////////////////////////////////


    }
    std::cout<<"\nfNumberEPICSEvents = "<<fNumberEPICSEvents<<std::endl;
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
    for (tagindex=0; tagindex<(Int_t)fEPICSVariableList.size(); tagindex++) {
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

  for (tagindex=0; tagindex<(Int_t)fEPICSVariableList.size(); tagindex++) {
    //for (tagindex=0; tagindex<fNumberEPICSVariables; tagindex++) {
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

  fDefaultAutogainList.push_back("IPM3C17.XIFG");
  fDefaultAutogainList.push_back("IPM3C17.YIFG");
  fDefaultAutogainList.push_back("IBC3C17.XIFG");
  fDefaultAutogainList.push_back("IBC3C17.YIFG");
  fDefaultAutogainList.push_back("IPM3C18.XIFG");
  fDefaultAutogainList.push_back("IPM3C18.YIFG");
  fDefaultAutogainList.push_back("IPM3C19.XIFG");
  fDefaultAutogainList.push_back("IPM3C19.YIFG");
  fDefaultAutogainList.push_back("IPM3C03.YIFG");
  fDefaultAutogainList.push_back("IPM3P01.XIFG");
  fDefaultAutogainList.push_back("IPM3P01.YIFG");
  fDefaultAutogainList.push_back("IPM3P02A.XIFG");
  fDefaultAutogainList.push_back("IPM3P02A.YIFG");
  fDefaultAutogainList.push_back("IPM3P03A.XIFG");
  fDefaultAutogainList.push_back("IPM3P03A.YIFG");
  fDefaultAutogainList.push_back("IPM3P02B.XIFG");
  fDefaultAutogainList.push_back("IPM3P02B.YIFG");
  fDefaultAutogainList.push_back("IPM3C20.XIFG");
  fDefaultAutogainList.push_back("IPM3C20.YIFG");
  fDefaultAutogainList.push_back("IPM3C21.XIFG");
  fDefaultAutogainList.push_back("IPM3C21.YIFG");
  fDefaultAutogainList.push_back("IPM3H02.XIFG");
  fDefaultAutogainList.push_back("IPM3H02.YIFG");
  fDefaultAutogainList.push_back("IPM3H04.XIFG");
  fDefaultAutogainList.push_back("IPM3H04.YIFG");
  fDefaultAutogainList.push_back("IPM3H07A.XIFG");
  fDefaultAutogainList.push_back("IPM3H07A.YIFG");
  fDefaultAutogainList.push_back("IPM3H07B.XIFG");
  fDefaultAutogainList.push_back("IPM3H07B.YIFG");
  fDefaultAutogainList.push_back("IPM3H07C.XIFG");
  fDefaultAutogainList.push_back("IPM3H07C.YIFG");
  fDefaultAutogainList.push_back("IPM3H09.XIFG");
  fDefaultAutogainList.push_back("IPM3H09.YIFG");
  fDefaultAutogainList.push_back("IPM3H09B.XIFG");
  fDefaultAutogainList.push_back("IPM3H09B.YIFG");
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
    case kEPICSString:
      fEPICSDataEvent[tagindex].EventNumber = event;
      fEPICSDataEvent[tagindex].Value       = 0.0;
      fEPICSDataEvent[tagindex].StringValue = value;
      fEPICSDataEvent[tagindex].Filled      = kTRUE;
      return 0; 
      break;

    case kEPICSFloat:
      if(IsNumber(value.Data())) tmpvalue = atof(value.Data());
      return SetDataValue(tagindex, tmpvalue, event);
      break;

    case kEPICSInt:
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
  std::cout << "*****Non-string EPICS Variables*****" << std::endl;
  for (tagindex=0; tagindex<(Int_t)fEPICSVariableList.size(); tagindex++) {
    if (fEPICSVariableType[tagindex] == kEPICSFloat
	|| fEPICSVariableType[tagindex] == kEPICSInt) {
      //if (fEPICSVariableType[tagindex] != kEPICSString) { 
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
	     << "...No data..."
	     << std::endl;
      }
    }
  }
  std::cout << "*****String EPICS Variables are below, if any*****" << std::endl;
  for (tagindex=0; tagindex<(Int_t)fEPICSVariableList.size(); tagindex++) {
    if (fEPICSVariableType[tagindex] == kEPICSString) {      
      if (fEPICSDataEvent[tagindex].Filled) {
	std::cout << std::setw(20) << std::setiosflags(std::ios::left)
	     <<fEPICSVariableList[tagindex]<<"\t"
	     <<fEPICSDataEvent[tagindex].StringValue;

	if (fEPICSCumulativeData[tagindex].NumberRecords!= fNumberEPICSEvents){
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
  for (tagindex=0; tagindex<(Int_t)fEPICSVariableList.size(); tagindex++) {
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
  for (tagindex=0; tagindex<(Int_t)fEPICSVariableList.size(); tagindex++) {
    if (fEPICSVariableType[tagindex] == kEPICSFloat
	|| fEPICSVariableType[tagindex] == kEPICSInt) {
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
	       << "-No data-"
	       << std::endl;
      }
    }
    
  }
  for (tagindex=0; tagindex<(Int_t)fEPICSVariableList.size(); tagindex++) {
    if (fEPICSVariableType[tagindex] == kEPICSString) {
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
  for (tagindex=0; tagindex<(Int_t)fEPICSVariableList.size(); tagindex++) {
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
};

void QwEPICSEvent::SetDefaultAutogainList(std::vector<TString> input_list)
{
  fDefaultAutogainList.clear();  //clear the vector first
  fDefaultAutogainList = input_list;
};


 void QwEPICSEvent::FillDB(QwDatabase *db)
 {
   FillSlowControlsData(db);
   // FillSlowControlsSettings(db);
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
  TString table1 = "polarized_source";
  
  // QwError << "Entering the loop " << QwLog::endl;
  // Loop over EPICS variables
  for (Int_t tagindex=0; tagindex<(Int_t)fEPICSVariableList.size(); tagindex++) {
  //for (Int_t tagindex = 0; tagindex < fNumberEPICSVariables; tagindex++) {
    // Look for variables to write into this table
    if (fEPICSTableList[tagindex] == table) {
      QwParityDB::slow_controls_data tmp_row(0);
      
      //  Now get the current sc_detector_id for the above runlet_id.
       sc_detector_id = db->GetSlowControlDetectorID(fEPICSVariableList[tagindex].Data()); 
       
       tmp_row.runlet_id      = runlet_id;
       tmp_row.sc_detector_id = sc_detector_id;
       
       if (!sc_detector_id) continue;
       
       
       // Calculate average and error
       if (fEPICSVariableType[tagindex] == kEPICSFloat
	   || fEPICSVariableType[tagindex] == kEPICSInt) {
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

    
    
//     else if (fEPICSTableList[tagindex] == table1) {
//       QwParityDB::slow_controls_data tmp_row(0);
//       if (fEPICSVariableType[tagindex] == kEPICSString) {
// 	if (fEPICSDataEvent[tagindex].Filled) {	  
// 	  tmp_row.value         = fEPICSDataEvent[tagindex].StringValue;
// 	}
//       }

//       entrylist.push_back(tmp_row);
//     }
    
    
    
  }
  
  db->Connect();
  // Check the entrylist size, if it isn't zero, start to query..
  if( entrylist.size() ) {
    QwMessage << "Writing to database now" << QwLog::endl;
    mysqlpp::Query query= db->Query();
    //    if(query)
    //	{
    //query.insert(entrylist.begin(), entrylist.end());
    //query.execute();

    ///////////////////////////////
    try {
      query.insert(entrylist.begin(), entrylist.end());
      QwMessage << "Query: " << query.str() << QwLog::endl;
      query.execute();
      QwMessage << "Done executing MySQL query" << QwLog::endl;
    } catch (const mysqlpp::Exception &er) {
      QwError << "MySQL exception: " << er.what() << QwLog::endl;
    }

    /////////////////////////////////////

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

// void QwEPICSEvent::FillSlowControlsSettings(QwDatabase *db)
// {

// };

