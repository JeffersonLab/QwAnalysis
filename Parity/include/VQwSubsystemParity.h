/**********************************************************\
* File: VQwSubsystemParity.h                               *
*                                                          *
* Author: P. M. King, Rakitha Beminiwattha                 *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/


#ifndef __VQWSUBSYSTEMPARITY__
#define __VQWSUBSYSTEMPARITY__


#include "VQwSubsystem.h"

#include "TTree.h"


#include "QwDatabase.h"

///
/// \ingroup QwAnalysis_ADC
///
/// \ingroup QwAnalysis_BL
class VQwSubsystemParity : virtual public VQwSubsystem {
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
  virtual void FillDB(QwDatabase *db)=0;
   
  virtual VQwSubsystem&  operator= (VQwSubsystem *value)=0;//VQwSubsystem routine is overridden. Call it at the beginning by VQwSubsystem::operator=(value)
  virtual VQwSubsystem&  operator+= (VQwSubsystem *value)=0;
  virtual VQwSubsystem&  operator-= (VQwSubsystem *value)=0;
  virtual void Sum(VQwSubsystem  *value1, VQwSubsystem  *value2)=0;
  virtual void Difference(VQwSubsystem  *value1, VQwSubsystem  *value2)=0;
  virtual void Ratio(VQwSubsystem *numer, VQwSubsystem *denom)=0;
  virtual void Calculate_Running_Average()=0;
  virtual void Do_RunningSum()=0;//update the running sums for devices

  virtual Int_t LoadEventCuts(TString filename)=0;//Will load the event cut file.
  virtual Bool_t ApplySingleEventCuts()=0;//impose single event cuts
  virtual Int_t GetEventcutErrorCounters()=0;// report number of events falied due to HW and event cut faliure
  virtual Int_t GetEventcutErrorFlag()=0;//return the error flag to the main routine

  virtual VQwSubsystem* Copy()=0;

  private:
  VQwSubsystemParity(){};  //  Private constructor.
};


/*
 *  Define the functions.
 */

#endif
