/**********************************************************\
* File: VQwSubsystemParity.h                               *
*                                                          *
* Author: P. M. King, Rakitha Beminiwattha                 *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/


#ifndef __VQWSUBSYSTEMPARITY__
#define __VQWSUBSYSTEMPARITY__


#include "VQwSubsystem.h"




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

  Int_t returnSubsystemType(){
    return 2;
};

  virtual ~VQwSubsystemParity(){};

  
  



 

 
  private:
  VQwSubsystemParity(){};  //  Private constructor.


};


/*
 *  Define the functions.
 */



#endif
