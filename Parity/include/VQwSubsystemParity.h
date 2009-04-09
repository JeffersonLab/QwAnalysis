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

  private:
  VQwSubsystemParity(){};  //  Private constructor.
};


/*
 *  Define the functions.
 */

#endif
