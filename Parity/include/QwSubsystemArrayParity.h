/**********************************************************\
* File: QwSubsystemArrayParity.h                           *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2009-02-04 10:30>                           *
\**********************************************************/

#ifndef __QWSUBSYSTEMARRAYPARITY__
#define __QWSUBSYSTEMARRAYPARITY__

#include <vector>
#include <TTree.h>

#include "QwSubsystemArray.h"
#include "VQwSubsystemParity.h"

///
/// \ingroup QwAnalysis_ADC
///
/// \ingroup QwAnalysis_BL
class QwSubsystemArrayParity:  public QwSubsystemArray {

 public:
  QwSubsystemArrayParity() {};
  virtual ~QwSubsystemArrayParity(){
  };

  VQwSubsystemParity* GetSubsystem(const TString name);

  void ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values);
  void ConstructBranchAndVector(TTree *tree, std::vector <Double_t> &values) {
    TString tmpstr("");
    ConstructBranchAndVector(tree,tmpstr,values);
  };
  void FillTreeVector(std::vector<Double_t> &values);
  void FillDB(QwDatabase *db, TString type);

  void Copy(QwSubsystemArrayParity *source);

  QwSubsystemArrayParity& operator=  (const QwSubsystemArrayParity &value);
  QwSubsystemArrayParity& operator+= (const QwSubsystemArrayParity &value);
  QwSubsystemArrayParity& operator-= (const QwSubsystemArrayParity &value);
  void Sum(QwSubsystemArrayParity &value1, QwSubsystemArrayParity &value2);
  void Difference(QwSubsystemArrayParity &value1, QwSubsystemArrayParity &value2);
  void Ratio(QwSubsystemArrayParity &numer, QwSubsystemArrayParity &denom);
  void Calculate_Running_Average();
  void Do_RunningSum();//calculate the running sum at event level
  Bool_t ApplySingleEventCuts();
  Int_t GetEventcutErrorCounters();
  Int_t GetEventcutErrorFlag();//return the error flag 
 
 public:
  std::vector<TString> sFailedSubsystems;


  //Int_t fSubsystem_Error_Flag;
  //static const Int_t kErrorFlag_Helicity=0x2;   // in Decimal 2. Helicity bit faliure
  //static const Int_t kErrorFlag_Beamline=0x4;    // in Decimal 4.  Beamline faliure
  

};

#endif
