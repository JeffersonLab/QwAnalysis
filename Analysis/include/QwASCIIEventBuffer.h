/**********************************************************\
* File: QwASCIIEventBuffer.h                                    *
*                                                          *
* Author: Rakitha Beminiwattha                             *
* Time-stamp: <2008-07-22 15:40>                           *
\**********************************************************/

#ifndef __QWEVENTASCIIBUFFER__
#define __QWEVENTASCIIBUFFER__

#include "QwEventBuffer.h"
#include "QwParameterFile.h"

#include "Det.h"
#include "Hit.h"
#include "enum.h"


#include "QwHitContainer.h"




#define NDetMax 1010
#define NEventMax 1000


class QwASCIIEventBuffer : public QwEventBuffer
{

  

 public:
  QwASCIIEventBuffer(){};

  ~QwASCIIEventBuffer(){
  };

  Int_t OpenDataFile(const TString filename,const TString rw);

  Int_t LoadChannelMap(const char *geomname);
  
  Int_t GetEvent();
  Int_t GetEventNumber(){return CurrentEvent;}

  Int_t ProcessHitContainer(QwHitContainer &);

  
  void  GetHitList(QwHitContainer & grandHitContainer){
    grandHitContainer.Append(fASCIIHits);
  }

 public:

  

  enum EUppLow up_low;
  enum ERegion region;
  enum Edir dir;
  enum Etype type;

  
 
  
  QwParameterFile *eventf ;
  Int_t CurrentEvent; 


  
 

  QwHit * currentHit; 
  std::vector< QwHit > fASCIIHits;
  


  
};

#endif
