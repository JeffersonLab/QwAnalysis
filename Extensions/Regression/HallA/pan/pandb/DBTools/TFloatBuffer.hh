
#ifndef __TFLOATBUFFER__
#define __TFLOATBUFFER__

#include <iostream.h>

#include <stdlib.h>
#include <unistd.h>
#include <iomanip.h>

// ROOT includes

#include "TROOT.h"
#include "TObject.h"
#include "TString.h"

class TFloatBuffer{
private:
  
  Float_t *lBuffer;
  Int_t    lBuffLength;
  
public:
  
  TFloatBuffer();
  //  TFloatBuffer(int gLength);
  
  virtual ~TFloatBuffer();
  
  void   SetSize(int gSize);
  Int_t  GetSize();
  void   AllocateBuffer(int gLength);
  void   ResetBuffer();
  int    BoundCheck(int gPos);
  
  void     Set(int gPos, float gValue);
  Float_t  Get(int gPos);
  
  void   Print();
  
  ClassDef(TFloatBuffer,1) // Class for keeping Array of Floats (with bound check)
    };

#endif

