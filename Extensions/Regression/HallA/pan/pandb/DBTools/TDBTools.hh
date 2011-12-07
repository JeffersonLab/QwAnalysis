//////////////////////////////////////////////////////////////////////////
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TDBTools.hh   (header file)
//           ^^^^^^^^^^^^
//
//    Authors :  R. Suleiman
//
//  TDBTools is set of tools to get or put
//  data into pandb (MySQL DataBase)
//
//////////////////////////////////////////////////////////////////////////

#ifndef __TDBTOOLS__
#define __TDBTOOLS__

#include <iostream.h>
#include <iomanip.h>
#include <stdlib.h>
#include <unistd.h>

// SQL Includes
#include <TMySQLServer.h>
#include <TMySQLResult.h>
#include <TMySQLRow.h>

// ROOT includes
#include "TROOT.h"
#include "TObject.h"
#include "TString.h"

#include "TQuery.hh"
#include "TItemValue.hh"
#include "TItem.hh"
#include "TIntBuffer.hh"
#include "TFloatBuffer.hh"
#include "TStringBuffer.hh"



#define debugQUERYPREVIEW     0x20
#define debugEXCEPTIONSPRINT  0x80

class TDBTools {
  
  TString  fHOSTNAME;  // Database HOST name
  TString  fDATABASE;  // Database to use
  TString  fUSERNAME;  // Database User name 
  TString  fPASSWORD;  // Database User Password
  
  Int_t    fDebugMode; // Debugmode controls printouts given from various methods

  // This contains Velue table version Development or Production
  TString  fItemValueVersion; 
  //
  //
  //
  
  
public:
  
  
  TDBTools(); // Default Constructor
  TDBTools(const char *dbDATABASE); // Constructor with initializing Database Name

  // Constructor with initializing Database Parameters
  TDBTools(const char *dbHOSTNAME, const char *dbDATABASE,const char *dbUSERNAME); 

  virtual  ~TDBTools(); // Default Destructor.
  
  // Set Database Parameters
  void  SetDBParams(const char *dbHOSTNAME,const char *dbDATABASE, const char *dbUSERNAME); 

  void  SetHost(const char *dbHOSTNAME);          // Set Database Server name
  //  void  SetDatabase(const char *dbDATABASE);      // Set Database name
  //  void  SetPassword(const char *dbPASSWORD);      // Set Database user Passowrd
  
  //
  //
  TString GetTableName(const char *dSystem,const char *dSubSystem, const char *dItem);
  void    ScanDBs();   // Prints out on Standart output  all Systems (DBs)
  void    ScanSystem(const char *tSystem);  // Prints on Standard output SusSystems  of System tSystem
  void    PrintItem(const char *tSystem, const char *tSubSystem,const char *tItem, Int_t tRun);
  void    PrintItemTable(TSQLServer *fbConn,TItem tDBItem, Int_t fcRunNum);
  //
  
  Int_t   GetSystemIDEx(TSQLServer *conn, const char *system_name);
  Int_t   GetSystemID(TSQLServer *conn, const char *system_name,Int_t *pSystem_ID);
  Int_t   GetSubSystemIDEx(TSQLServer *conn, int system_id, const char *subsystem_name);
  Int_t   GetSubSystemID(TSQLServer *conn, int system_id, const char *subsystem_name, 
			 Int_t *pSubSystem_ID);
  void    GetItemEx(TSQLServer *conn, int subsystem_id, 
		    const char *item_name, TItem *fDBItem);
  Int_t   GetItem(TSQLServer *fbConn,const char *tSystem, const char *tSubSystem,
		  const char *tItem,TItem *tDBItem);
  void    GetItemValueEx(TSQLServer *fbConn, TItem tDBItem, Int_t fcRunNum, 
			 const char *fcAuthor, TItemValue *tItemValue);
  Int_t   GetItemValue(TSQLServer *fbConn, TItem tDBItem, Int_t fcRunNum, 
		       const char *fcAuthor, TItemValue *tItemValue);
  //
  //
  //
  
  TSQLResult & SelectValues(const char *fcSystem, const char *fcSubSystem, const char *fcItem, 
			    Int_t fcRun_no, const char *fcAuthor, const char *fcValueType);
  
  Int_t   *GetDBInt(const char *fcSystem, const char *fcSubSystem, const char *fcItem, 
		    Int_t fcRun_no, Int_t *fiBuffer_length, const char *fcAuthor = "*");  
  
  void   GetDBIntBuff(const char *fcSystem, const char *fcSubSystem, const char *fcItem, 
		      Int_t fcRun_no, TIntBuffer *fIntBuffer, const char *fcAuthor = "*"); 
  
  Float_t   *GetDBFloat(const char *fcSystem, const char *fcSubSystem, const char *fcItem, 
			Int_t fcRun_no, Int_t *fiBuffer_length, const char *fcAuthor = "*");
  
  void   GetDBFloatBuff(const char *fcSystem, const char *fcSubSystem, const char *fcItem, 
			Int_t fcRun_no, TFloatBuffer *fFloatBuffer, const char *fcAuthor = "*");
  
  TString   *GetDBChar(const char *fcSystem, const char *fcSubSystem, const char *fcItem, 
		       Int_t fcRun_no, Int_t *fiBuffer_length, const char *fcAuthor = "*");
  
  //
  
  void    PrintInfo();
  void    SetDebugMode(Int_t DbgMode);
  void    SetDbVersion(const char *fVersion);
  
  //
  //
  //
  
  ClassDef(TDBTools,1) // Class of Tools for pan DB
    
    };

#endif
