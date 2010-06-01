/*
Adopted from G0EPICSEvent class.
*/

#ifndef __QwEPICSEVENT__
#define __QwEPICSEVENT__
#include <vector>
#include <string>
#include <Rtypes.h>

#include "TString.h" 

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



  std::vector<Double_t> ReportAutogains();
  std::vector<Double_t> ReportAutogains(std::vector<TString> tag_list);

  void ExtractEPICSValues(const std::string& data, int event);

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

  std::vector<TString>  GetDefaultAutogainList();
  void  SetDefaultAutogainList(std::vector<TString> input_list);

  void  ResetCounters(); 

  void FillDB(QwDatabase *db);
  void FillSlowControlsData(QwDatabase *db);
  Int_t   fNumberEPICSVariables;       // Number of defined EPICS variables.
  std::vector<TString> fEPICSVariableList;  // List of defined EPICS variables.

 private:
  static const int kDebug;
  static const int kEPICS_Error;
  static const int kEPICS_OK;
  static const Double_t kInvalidEPICSData;
  // Int_t maxsize = 300;

  std::vector<TString> fDefaultAutogainList;
  void  InitDefaultAutogainList();


  struct EPICSVariableRecord{   //One EPICS variable record.
    Int_t     EventNumber;
    Bool_t    Filled;
    Double_t  Value;
    TString   StringValue;
  };
  std::vector<EPICSVariableRecord> fEPICSDataEvent;


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
  std::vector<EPICSCumulativeRecord> fEPICSCumulativeData;


  Int_t    fNumberEPICSEvents;     //  The total number of EPICS events in the run.
  //Int_t   fNumberEPICSVariables;       // Number of defined EPICS variables.
  //std::vector<TString> fEPICSVariableList;  // List of defined EPICS variables.
  std::vector<QwEPICSDataType> fEPICSVariableType;
  std::vector<TString> fEPICSTableList;     // List of DB tables to write

};

#endif
