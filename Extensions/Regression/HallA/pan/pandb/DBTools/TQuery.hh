///////////////////////////////////////////////////////////
//
//  File :  TQuery.hh  include file for TQuery Class
//
//  R. suleiman
//
///////////////////////////////////////////////////////////

#ifndef __TQUERY__
#define __TQUERY__


#include <iostream.h>
#include <stdlib.h>
#include <unistd.h>

#include <TSQLServer.h>
#include <TSQLResult.h>
#include <TSQLRow.h>

// ROOT includes

#include "TROOT.h"
#include "TObject.h"
#include "TString.h"

class TQuery{
  
public:
  
  TString fQuery; // String with query
  
  TQuery();
  
  virtual ~TQuery();
  
  const char *Data();
  void  Preview();
  void  Reset();
  
  TQuery & operator<<( int x_int); 
  TQuery & operator<<( float x_float);
  TQuery & TQuery::operator<<(const char *n_char);
  
  float  FloatOf(const char *chr_float="0.0");
  Int_t  IntOf(const char *chr_int="0");
  
  ClassDef(TQuery,1) // Class to construct MySQL Queries
    
    };

#endif

