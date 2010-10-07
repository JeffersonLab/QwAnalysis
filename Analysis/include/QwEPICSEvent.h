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
  enum EQwEPICSDataType {kEPICSString, kEPICSFloat, kEPICSInt};

  QwEPICSEvent();
  ~QwEPICSEvent();

  int AddEPICSTag(TString tag);            // Add a tag to the list
  int AddEPICSTag(TString tag, TString table);
  int AddEPICSTag(TString tag, TString table,
		  QwEPICSEvent::EQwEPICSDataType datatype);

  Int_t LoadEpicsVariableMap(TString mapfile);

  std::vector<Double_t> ReportAutogains();
  std::vector<Double_t> ReportAutogains(std::vector<TString> tag_list);

  void ExtractEPICSValues(const std::string& data, int event);

  Int_t FindIndex(const TString& tag) const;        // return index to variable or EPI_ERR

  Double_t GetDataValue(const TString& tag) const;      // get recent value corr. to tag


  int SetDataValue(const TString& tag, const TString& value, const int event);
  int SetDataValue(const TString& tag, Double_t value, const int event);
  int SetDataValue(const Int_t index,  Double_t value, const int event);

  void  CalculateRunningValues();

  void  PrintAverages() const;
  void  PrintVariableList() const;
  // void DefineEPICSVariables();
  void  ReportEPICSData() const;

  std::vector<TString>  GetDefaultAutogainList();
  void  SetDefaultAutogainList(std::vector<TString> input_list);

  void  ResetCounters();

  void FillDB(QwDatabase *db);
  void FillSlowControlsData(QwDatabase *db);
  void FillSlowControlsStrigs(QwDatabase *db);
  void FillSlowControlsSettings(QwDatabase *db);

 private:
  static const int kDebug;
  static const int kEPICS_Error;
  static const int kEPICS_OK;
  static const Double_t kInvalidEPICSData;
  // Int_t maxsize = 300;

  std::vector<TString> fDefaultAutogainList;
  void  InitDefaultAutogainList();

  // Test whether the string is a number string or not
  Bool_t IsNumber(const std::string& word) {
    size_t pos;
    pos = word.find_first_not_of("0123456789.+-eE"); // white space not allowed
    if (pos != std::string::npos) return kFALSE;
    else return kTRUE;
  }
  // TODO (wdc) should disappear
  std::string get_next_seg(const std::string& inputstring, size_t& pos);

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


  Int_t fNumberEPICSEvents;     // Number of EPICS events in the run
  Int_t fNumberEPICSVariables;  // Number of defined EPICS variables
  std::vector<TString> fEPICSVariableList;  // List of defined EPICS variables
  std::vector<TString> fEPICSTableList;     // List of DB tables to write
  std::vector<EQwEPICSDataType> fEPICSVariableType;

}; // class QwEPICSEvent


// TODO (wdc) should disappear!
//
#include <cstdlib>
#include <iostream>
//
//take the next segment of a line separated by a spaces, tabs or \n from
//position pos. NOTE, pos is passed by reference and it will change!
inline std::string QwEPICSEvent::get_next_seg(const std::string& inputstring, size_t& pos)
{
  std::string seg;
  size_t first_pos = 0;
  size_t next_pos = 0;
  first_pos = inputstring.find_first_not_of(" \t\n",pos);
  if(first_pos==std::string::npos){//not found a real character, end of line already
    std::cerr<<"end of line has reached!"<<std::endl;
    exit(1);
  }
  else{//found the beginning of a valid string, try to find the end of it
    next_pos = inputstring.find_first_of(" \t\n",first_pos);
    if(next_pos==std::string::npos){//not found the next separator, just take the
      //string starting from first_pos
      seg = inputstring.substr(first_pos, inputstring.size());
      pos = inputstring.size();
    }
    else{//found the next separator, take the segment, move pos to the position
      //of the next separator
      seg = inputstring.substr(first_pos, next_pos-first_pos);
      pos = next_pos;
    }
    //    std::cerr << seg << std::endl;
    return seg;
  }
};

#endif
