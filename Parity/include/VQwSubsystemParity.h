/**********************************************************\
* File: VQwSubsystemParity.h                               *
*                                                          *
* Author: P. M. King, Rakitha Beminiwattha                 *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/


#ifndef __VQWSUBSYSTEMPARITY__
#define __VQWSUBSYSTEMPARITY__


#include "VQwSubsystem.h"

#include <TTree.h>



///
/// \ingroup QwAnalysis_ADC
///
/// \ingroup QwAnalysis_BL
class VQwSubsystemParity : public VQwSubsystem {
  /******************************************************************
   *  Class: VQwSubsystemBeamLine
   *         Virtual base class for the classes containing the
   *         event-based information from each Beamline subsystem.
   *         This will define the interfaces used in communicating
   *         with the CODA routines.
   *
   ******************************************************************/
 public:
  VQwSubsystemParity(TString region_tmp):VQwSubsystem(region_tmp){ };

  virtual ~VQwSubsystemParity(){};
  
  virtual void ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values)=0;
  virtual void ConstructBranchAndVector(TTree *tree, std::vector <Double_t> &values)
    {
      TString tmpstr("");
      ConstructBranchAndVector(tree,tmpstr,values);
    };
  
  virtual void FillTreeVector(std::vector<Double_t> &values)=0;
   
  virtual void Copy(VQwSubsystem *source) = 0;
  virtual VQwSubsystem* Copy()=0;

  virtual VQwSubsystem&  operator=  (VQwSubsystem *value)=0;
  virtual VQwSubsystem&  operator+= (VQwSubsystem *value)=0;
  virtual VQwSubsystem&  operator-= (VQwSubsystem *value)=0;
  virtual void Sum(VQwSubsystem  *value1, VQwSubsystem  *value2)=0;
  virtual void Difference(VQwSubsystem  *value1, VQwSubsystem  *value2)=0;
  virtual void Ratio(VQwSubsystem *numer, VQwSubsystem *denom)=0;

  virtual Int_t LoadEventCuts(TString filename)=0;//Will load the event cut file.
  virtual Bool_t ApplySingleEventCuts()=0;//impose single event cuts
  virtual Int_t GetEventcutErrorCounters()=0;// report number of events falied due to HW and event cut faliure
  virtual Bool_t CheckRunningAverages(Bool_t )=0; //check the running averages of sub systems and passing argument decide print AVG or not.

  private:
  VQwSubsystemParity(){};  //  Private constructor.
};


/*
 *  Define the functions.
 */

#endif
