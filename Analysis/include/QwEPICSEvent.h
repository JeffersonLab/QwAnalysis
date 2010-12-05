/*
Adopted from G0EPICSEvent class.
*/

#ifndef __QWEPICSEVENT__
#define __QWEPICSEVENT__

// System headers
#include <map>
#include <vector>
#include <string>
using std::string;

// ROOT headers
#include "Rtypes.h"
#include "TString.h"
#include "TTree.h"

// Forward declarations
class QwDatabase;


class QwEPICSEvent
{
 public:

	/// EPICS data types
  enum EQwEPICSDataType {kEPICSString, kEPICSFloat, kEPICSInt};


  /// Default constructor
  QwEPICSEvent();
  /// Virtual destructor
  virtual ~QwEPICSEvent();



  // Add a tag to the list
  Int_t AddEPICSTag(const string& tag, const string& table = "",
      EQwEPICSDataType datatype = kEPICSFloat);

  Int_t LoadChannelMap(TString mapfile);

  std::vector<Double_t> ReportAutogains(std::vector<std::string> tag_list = fDefaultAutogainList);

  void ExtractEPICSValues(const string& data, int event);

  /// Find the index of an EPICS variable, or return error
  Int_t FindIndex(const string& tag) const;

  Double_t GetDataValue(const string& tag) const;      // get recent value corr. to tag
  TString  GetDataString(const string& tag) const;


  int SetDataValue(const string& tag, const Double_t value, const int event);
  int SetDataValue(const string& tag, const string& value, const int event);
  int SetDataValue(const Int_t index,  const Double_t value, const int event);
  int SetDataValue(const Int_t index,  const string& value, const int event);

  void  CalculateRunningValues();

  void  PrintAverages() const;
  void  PrintVariableList() const;
  // void DefineEPICSVariables();
  void  ReportEPICSData() const;

  void  ResetCounters();

  void FillDB(QwDatabase *db);
  void FillSlowControlsData(QwDatabase *db);
  void FillSlowControlsStrigs(QwDatabase *db);
  void FillSlowControlsSettings(QwDatabase *db);


 private:

  // Tree array indices
  size_t fTreeArrayIndex;
  size_t fTreeArrayNumEntries;

 public:

  /// \brief Construct the branch and tree vector
  void ConstructBranchAndVector(TTree *tree, TString& prefix, std::vector<Double_t>& values);
  /// \brief Fill the tree vector
  void FillTreeVector(std::vector<Double_t>& values) const;


 public:

  static std::vector<std::string> GetDefaultAutogainList() { return fDefaultAutogainList; };
  static void SetDefaultAutogainList(std::vector<std::string>& input_list);

 private:

  /// Default autogain list
  static std::vector<std::string> fDefaultAutogainList;
  /// Initialize the default autogain list
  static void InitDefaultAutogainList();


 private:

  static const int kDebug;
  static const int kEPICS_Error;
  static const int kEPICS_OK;
  static const Double_t kInvalidEPICSData;
  // Int_t maxsize = 300;


  // Test whether the string is a number string or not
  Bool_t IsNumber(const string& word) {
    size_t pos;
    pos = word.find_first_not_of("0123456789.+-eE"); // white space not allowed
    if (pos != std::string::npos) return kFALSE;
    else return kTRUE;
  }

  struct EPICSVariableRecord {   //One EPICS variable record.
    Int_t     EventNumber;
    Bool_t    Filled;
    Double_t  Value;
    TString   StringValue;
  };
  std::vector<EPICSVariableRecord> fEPICSDataEvent;


  /*  The next two variables will contain the running sum and sum  *
   *  of squares for use in calculating the average and variance.  */
  struct EPICSCumulativeRecord {
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
  std::vector<std::string> fEPICSVariableList;  // List of defined EPICS variables
  std::vector<std::string> fEPICSTableList;     // List of DB tables to write
  std::vector<EQwEPICSDataType> fEPICSVariableType;

  std::map<std::string,Int_t> fEPICSVariableMap;

}; // class QwEPICSEvent

#endif // __QWEPICSEVENT__
