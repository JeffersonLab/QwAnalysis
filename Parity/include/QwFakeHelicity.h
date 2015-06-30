/**********************************************************\
* File: QwFakeHelicity.h                                  *
*                                                         *
* Author: B.Waidyawansa                                   *
* Time-stamp:03-06-2010                                   *
\**********************************************************/
/**
  The QwFakeHelicity class uses a pregenerated random seed to generate
  the fake helicity signal that then can be used to perfrom helicity
  related calculations.
*/


#ifndef __QwFAKEHELICITY__
#define __QwFAKEHELICITY__

#include "QwHelicity.h"

class QwFakeHelicity: public QwHelicityBase , public MQwSubsystemCloneable<QwFakeHelicity> {
 public:
  /// Constructor with name
 QwFakeHelicity(const TString& name):VQwSubsystem(name),QwHelicityBase(name){
    fMinPatternPhase = 1;
    fMaxPatternPhase = 4;
  };
  /// Copy constructor
 QwFakeHelicity(const QwFakeHelicity& source):VQwSubsystem(source),QwHelicityBase(source){};
  /// Virtual destructor
  virtual ~QwFakeHelicity() { };

    void    ClearEventData();
    Bool_t  IsGoodHelicity();
    void    ProcessEvent();

 protected:
    Bool_t CollectRandBits();
    UInt_t GetRandbit(UInt_t& ranseed);

 private:
    /// Private default constructor (not implemented, will throw linker error on use)
    QwFakeHelicity();

};

#endif
