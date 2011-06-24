
#ifndef __TINTBUFFER__
#define __TINTBUFFER__

#include <iostream.h>

#include <stdlib.h>
#include <unistd.h>
#include <iomanip.h>

// ROOT includes

#include "TROOT.h"
#include "TObject.h"
#include "TString.h"


class TIntBuffer{
  
private:
  
  Int_t    *lBuffer;
  Int_t     lBuffLength;
  
public:
  
  TIntBuffer();
  //  TIntBuffer(int gLength);
  
  virtual ~TIntBuffer();
  
  void   SetSize(int gSize);
  Int_t  GetSize();
  void   AllocateBuffer(int gLength);
  void   ResetBuffer();
  int    BoundCheck(int gPos);
  
  void     Set(int gPos, int gValue);
  Int_t    Get(int gPos);
  
  void   Print();
  
  ClassDef(TIntBuffer,1) // Class for keeping Array Integers (with bound check)
    };

#endif
