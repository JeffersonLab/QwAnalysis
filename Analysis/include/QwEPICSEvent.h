/*
Adopted from G0EPICSEvent class.
*/

#ifndef __QwEPICSEVENT__
#define __QwEPICSEVENT__
#include <vector>
#include <string>
#include <Rtypes.h>
//c++ stream classes
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include "StringManip.h" // need to commit this and it's source file too.
#include "TRegexp.h"

#include "QwDatabase.h"
#include "TSQLRow.h"
#include "TSQLResult.h"


//#include "G0SQL.h"




#include "TString.h" 
#include "TMath.h" 
#include <cstring> 
#include <iostream>
using namespace std;



class QwDatabase;
class QwEPICSEvent
{
 public:
  enum QwEPICSDataType {EPICSString, EPICSFloat, EPICSInt};  //aamer

  QwEPICSEvent();
  ~QwEPICSEvent();

  int AddEPICSTag(TString tag);            // Add a tag to the list
  int AddEPICSTag(TString tag, TString table);
  int AddEPICSTag(TString tag, TString table, 
		  QwEPICSEvent::QwEPICSDataType datatype);



  vector<Double_t> ReportAutogains();
  vector<Double_t> ReportAutogains(vector<TString> tag_list);

  void ExtractEPICSValues(const string& data, int event);

  Int_t FindIndex(TString tag);        // return index to variable or EPI_ERR

  Double_t GetDataValue(TString tag);      // get recent value corr. to tag


  int SetDataValue(TString tag, TString value, int event);
  int SetDataValue(TString tag, Double_t value, int event);
  int SetDataValue(Int_t index, Double_t value, int event);



   


  void  CalculateRunningValues();
  void  PrintAverages();
  void  PrintVariableList();
  void  DefineEPICSVariables();
  void  ReportEPICSData();


  vector<TString>  GetDefaultAutogainList();
  void  SetDefaultAutogainList(vector<TString> input_list);

  void  ResetCounters(); 

/*   void FillDB(QwDatabase *db); */
/*   Int_t WriteBaseTable(QwDatabase *db, TString table); */
/*   void WriteMeasurementsTable(QwDatabase *db, TString table, Int_t table_id); */
/*   void FillTargetTables(QwDatabase *db); */
/*   void  WriteDatabase(QwDatabase *db); */

 private:
  static const int kDebug;
  static const int kEPICS_Error;
  static const int kEPICS_OK;
  static const Double_t kInvalidEPICSData;
  // Int_t maxsize = 300;

  vector<TString> fDefaultAutogainList;
  void  InitDefaultAutogainList();


  struct EPICSVariableRecord{   //One EPICS variable record.
    Int_t     EventNumber;
    Bool_t    Filled;
    Double_t  Value;
    TString   StringValue;
  };
  vector<EPICSVariableRecord> fEPICSDataEvent;


  /*  The next two variables will contain the running sum and sum  *
   *  of squares for use in calculating the average and variance.  */
  struct EPICSCumulativeRecord{
    Bool_t   Filled;
    Int_t    NumberRecords;
    Double_t Sum;
    Double_t SquaredSum;
    Double_t Minimum;
    Double_t Maximum;
    TString  SavedString; 
  };
  vector<EPICSCumulativeRecord> fEPICSCumulativeData;


  Int_t    fNumberEPICSEvents;     //  The total number of EPICS events in the run.
  Int_t   fNumberEPICSVariables;       // Number of defined EPICS variables.
  vector<TString> fEPICSVariableList;  // List of defined EPICS variables.
  vector<QwEPICSDataType> fEPICSVariableType;
  vector<TString> fEPICSTableList;     // List of DB tables to write

};

#endif
