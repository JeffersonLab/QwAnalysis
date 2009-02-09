
/**********************************************************\
* File: QwSubsystemArrayParity.h                           *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2009-02-04 10:30>                           *
\**********************************************************/

#ifndef __QWSUBSYSTEMARRAYPARITY__
#define __QWSUBSYSTEMARRAYPARITY__


#include "QwSubsystemArray.h"

class QwSubsystemArrayParity:  public QwSubsystemArray {
 
 public:
  QwSubsystemArrayParity() {};
  ~QwSubsystemArrayParity(){
  };
  
  void Copy(QwSubsystemArrayParity *source);

  QwSubsystemArrayParity& operator=  (const QwSubsystemArrayParity &value);
  QwSubsystemArrayParity& operator+= (const QwSubsystemArrayParity &value);
  QwSubsystemArrayParity& operator-= (const QwSubsystemArrayParity &value);
  void Sum(QwSubsystemArrayParity &value1, QwSubsystemArrayParity &value2);
  void Difference(QwSubsystemArrayParity &value1, QwSubsystemArrayParity &value2);
  void Ratio(QwSubsystemArrayParity &numer, QwSubsystemArrayParity &denom);

};
  
#endif
