//////////////////////////////////////////////////////////////////////////
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TDBTools.cc
//
//    Authors :  R. Suleiman
//
//  TDBTools is set of tools to get or put
//  data into pandb (MySQL DataBase)
//
/////////////////////////////////////////////////////////////////////////

#include "TDBTools.hh"

ClassImp(TDBTools)


TDBTools::TDBTools(){
  // Default Destructor
  // 
  fPASSWORD = "";
  SetDbVersion("DEV");

}

TDBTools::TDBTools(const char *dbDATABASE){
  // 
  // Constructor initializes Database name only
  fHOSTNAME = "alquds.jlab.org";
  fDATABASE = dbDATABASE;
  fUSERNAME = "dbmanager";
  fPASSWORD = "parity";
  SetDbVersion("DEV");
  SetDebugMode(0x0);
}

TDBTools::TDBTools(const char *dbHOSTNAME, const char *dbDATABASE,const char *dbUSERNAME){
  // 
  // Constructor initializes Database host
  // Database and user name

  fHOSTNAME = dbHOSTNAME;
  fDATABASE = dbDATABASE;
  fUSERNAME = dbUSERNAME;
  fPASSWORD = "parity";
  SetDbVersion("DEV");
}

TDBTools::~TDBTools(){
  // Default destructor
  //
  //
  fDebugMode = 0;
}

void TDBTools::SetHost(const char *dbHOSTNAME){
  // Sets mySQL Server host name to dbHOSTNAME
  //
  fHOSTNAME = dbHOSTNAME;
}

void TDBTools::SetDBParams(const char *dbHOSTNAME,const char *dbDATABASE, const char *dbUSERNAME){
  // Sets Database Information i.e.
  // HOSTNAME, DATABASE, USERNAME
  //
  fHOSTNAME = dbHOSTNAME;
  fUSERNAME = dbUSERNAME;
  fDATABASE = dbDATABASE;
}

void TDBTools::PrintInfo(){
  // Prints information about Database settings
  // HOSTNAME, DATABASE, USERNAME 
  //
  //
  cout << setiosflags(ios::left);
  cout << endl << " Pan TDB Tools : Database Information " << endl;
  cout << "+-----------------+---------------------------------+"<<endl;
  cout << "| " << setw(16) << " Database  Host " << "| " << setw(32) << fHOSTNAME.Data() << "|" << endl;
  cout << "+-----------------+---------------------------------+"<<endl;
  cout << "| " << setw(16) << " DataBase " << "| " << setw(32) << fDATABASE.Data() << "|" << endl;
  cout << "+-----------------+---------------------------------+"<<endl;
  cout << "| " << setw(16) << " Database User " << "| " << setw(32) << fUSERNAME.Data() << "|" << endl;
  cout << "+-----------------+---------------------------------+"<<endl;
  cout << "| " << setw(16) << " Password " << "| " << setw(32) << fPASSWORD.Data() << "|" << endl;
  cout << "+-----------------+---------------------------------+"<<endl<<endl;
}

void  TDBTools::ScanDBs(){
  //  Connectos to DATBASE on HOSTNAME and Prints out System Names from
  //  table System. They correcspond to MAP names in old version of DBManager
  //
 TString serverHost;
 serverHost += "mysql://";
 serverHost += fHOSTNAME;

 TSQLServer *db = TSQLServer::Connect(serverHost.Data(),fUSERNAME.Data(),fPASSWORD.Data());
 if(db->SelectDataBase(fDATABASE.Data()) != 0){
    cout << " ScanDBs: Error selecting DataBase " << fDATABASE.Data() << endl;
    return;
 }

 TSQLRow     *fRow;
 TSQLResult  *fResult;
 TQuery       fQuery;

 cout << setiosflags(ios::left);

 if(db->IsConnected()){

   fQuery << "select * from System";
   fResult = db->Query(fQuery.Data());
   
   cout << endl << endl << "+--------+-------------------------+" << endl;
   cout << "|  SysID |" << setw(25) << "  System Name  " << "|" << endl;
   cout << "+--------+-------------------------| " << endl;
   while((fRow=fResult->Next())){
     cout << "|  " << setw(4) << fRow->GetField(0) << "  " << setw(1) << "|" << "    " << setw(21) << fRow->GetField(1) << "|" << endl;
   }
   cout << "+--------+-------------------------+" << endl << endl;	
 }
 
 delete db;
}

void TDBTools::ScanSystem(const char *tSystem){
  //
  // Prints out Subsystems and Items for Given System
  //
  //
  TString serverHost;
  serverHost += "mysql://";
  serverHost += fHOSTNAME;
  
  TSQLServer *db = TSQLServer::Connect(serverHost.Data(),fUSERNAME.Data(),fPASSWORD.Data());
  
  if(db->SelectDataBase(fDATABASE.Data()) != 0){
    cout << " ScanDBs: Error selecting DataBase " << fDATABASE.Data() << endl;
    return;
  }
  
 TSQLRow     *fRow;
 TSQLRow     *fItemRow; 
 TSQLResult  *fResult;
 TSQLResult  *fItemResult;	
 TQuery       fQuery;
 TQuery       fItemQuery;
 Int_t        tSysID;
 // TItemClass   tmpIndex;
 
 if(db->IsConnected()){
   
   GetSystemID(db,tSystem,&tSysID);
   cout << setiosflags(ios::left);
   
   fQuery << "select subsystemId,subsystemName,description from SubSystem where systemId=" << tSysID;
   fResult = db->Query(fQuery.Data());
   cout << endl << endl << "    ScanDB For System : " << tSystem << endl << endl;
   
   while((fRow=fResult->Next())){
     Int_t  tSubSysID = fQuery.IntOf(fRow->GetField(0));
     cout  << " SubSystem : " << fRow->GetField(1) 
	   << " (ID=" << tSubSysID  << ")  Description : " << fRow->GetField(2)  << endl ; 
     
     fQuery.Reset();
     fQuery << "select * from Item where subsystemId=" << tSubSysID;
     
     if((fDebugMode&0x20)>0) fQuery.Preview();
     
     fItemResult = db->Query(fQuery.Data());
     while((fItemRow=fItemResult->Next())){
       cout << setw(15) << "   " 
	    << setw(12) << fItemRow->GetField(1) << " : " 
	    << setw(7) << fItemRow->GetField(0) <<" : " 
	    << setw(7) << fItemRow->GetField(3) << "  :  " 
	    << setw(7) << fItemRow->GetField(4) << "  :  " 
	    << setw(7) << fItemRow->GetField(5) << "  :  " 
	    << endl;
     }
     cout << endl << endl;
   }
  }
 
 delete db;
}

void  TDBTools::PrintItemTable(TSQLServer *fbConn,TItem tDBItem, Int_t fcRunNum){
  //   
  //
  //
  //
  TSQLRow     *fRow;
  TSQLResult  *fResult;  
  TQuery       fQuery;
  
  cout << endl << "----------------------------------------------------------------------------------------" << endl;
  fQuery << "select * from " << fItemValueVersion.Data() << " where itemId=" << tDBItem.fItemId 
	 << " AND RunNumber=" << fcRunNum << " ORDER BY time" ;
  fResult = fbConn->Query(fQuery.Data());
  for(int j = 0; j < fResult->GetRowCount(); j++){
    fRow = fResult->Next();
    for(int i = 0 ; i < fResult->GetFieldCount(); i++)
      cout << "  " << fRow->GetField(i);
    cout << endl;
  }
  cout << endl << "----------------------------------------------------------------------------------------" << endl;
}

void  TDBTools::PrintItem(const char *tSystem, const char *tSubSystem,const char *tItem, Int_t tRun){
  // 
  //  This function Prints content of ValueDescription table for Given Item
  // specified by (System,SubSystem,Item) for Run 
  //
  // NOTE. This will print all entries independent of author entry in the database
  //

  TString serverHost;
  serverHost += "mysql://";
  serverHost += fHOSTNAME;
  
  TSQLRow     *fRow;
  TSQLResult  *fResult;
  TQuery       fQuery;
  TItem       flDBItem;

  TSQLServer *db = TSQLServer::Connect(serverHost.Data(),fUSERNAME.Data(),fPASSWORD.Data());
  
  if(db->SelectDataBase(fDATABASE.Data()) != 0){
    cout << " PrintTable: Error selecting DataBase " << fDATABASE.Data() << endl;
    return;
  }
  
  if(GetItem(db,tSystem,tSubSystem,tItem,&flDBItem)==0){
    
    cout << "--------------------- System found " << endl;
    fQuery << "select * from " << fItemValueVersion.Data() << " where itemId=" << flDBItem.fItemId 
	   << " AND RunNumber=" << tRun;
    if((fDebugMode&0x20)>0) fQuery.Preview();
    fResult = db->Query(fQuery.Data());
    while((fRow=fResult->Next())){
      cout << setw(4) << "   "
	   << setw(12) << fRow->GetField(0) << " : " 
	   << setw(7) << fRow->GetField(2) <<" : " 
	   << setw(7) << fRow->GetField(3) << "  :  " 
	   << setw(7) << fRow->GetField(4) << "  :  " 
	   << setw(7) << fRow->GetField(5) << "  :  " 
	   << setw(7) << fRow->GetField(6) << "  :  " 
	   << endl;		
    }
  }
  delete db;
}

TSQLResult & TDBTools::SelectValues(const char *fcSystem, const char *fcSubSystem, const char *fcItem, 
				    Int_t fcRun_no, const char *fcAuthor, const char *fcValueType){
  //
  // Selects Values from Database for given Item , Run Number and author and returns 
  // TSQLResult to calling function. Also makes check on type of Item.
  //
  // NOTE. If calling this function one must call it from "try"-"catch" block.
  // If error accures function will trow an exception. 
  //

  TQuery  tmpQuery;
  TSQLResult *flResult;
  TItem flDBItem;
  TItemValue flItemValue;
  
  TString serverHost;
  serverHost += "mysql://";
  serverHost += fHOSTNAME;
  
  TSQLServer *dbConn = TSQLServer::Connect(serverHost.Data(),fUSERNAME.Data(),fPASSWORD.Data());
  
  if(dbConn->IsConnected()){
   
    if(dbConn->SelectDataBase(fDATABASE.Data()) != 0){
      delete dbConn;
      TQuery localErrMsg;
      localErrMsg << " SelectValues: Error selecting DataBase " << fDATABASE.Data();
      throw localErrMsg;
    }
    
    if(GetItem(dbConn,fcSystem,fcSubSystem,fcItem,&flDBItem) != 0){
      delete dbConn;
      TQuery localErrMsg;
      localErrMsg << "SelectValues: Item Not Found ";
      throw localErrMsg;
    }
    
    if(GetItemValue(dbConn,flDBItem,fcRun_no,fcAuthor,&flItemValue) != 0){
      delete dbConn;
      TQuery localErrMsg;
      localErrMsg << "SelectValues: Item Not Found ";
      throw localErrMsg;
    }
    
    if(flDBItem.fType != fcValueType){
      delete dbConn;
      TQuery localErrMsg;
      localErrMsg << "SelectValues: Type Is not what was expected   " << fcValueType;
      throw localErrMsg;
    }
   
    tmpQuery.Reset(); 
    TQuery  flTblName;
    flTblName << fcSystem << "_" << fcSubSystem << "_" << fcItem << "_VALUE";
    
    tmpQuery << "select * from " <<  flTblName.Data() << " where itemValueId=" << flItemValue.fItemValueId;
    if((fDebugMode&0x20)>0) tmpQuery.Preview();
    
    flResult = dbConn->Query(tmpQuery.Data());

    // Close the connection (Revisit again)
    if(flDBItem.fType != "char") delete dbConn; 

    if(flResult->GetRowCount() != 1){
      delete dbConn;
      TQuery localErrMsg;
      localErrMsg << "SelectValues: Got unexpected count of Rows " << flResult->GetRowCount();
      throw localErrMsg;    
    }
    
    if(flResult->GetFieldCount()-4 != flDBItem.fLength){
      delete dbConn;
      TQuery localErrMsg;
      localErrMsg << "SelectValues: Wrong Item Length. expected : " << flDBItem.fLength 
		  << "  got : " << flResult->GetFieldCount()-4;
      throw localErrMsg;
    }     
    
  } else {
    
    TQuery localErrMsg;
    localErrMsg << "SelectValues: Database is not connected... "; 
    throw localErrMsg; 
  } 
  
  return (*flResult);
}

Int_t    *TDBTools::GetDBInt(const char *fcSystem, const char *fcSubSystem, const char *fcItem, 
			     Int_t fcRun_no, Int_t *fiBuffer_length, const char *fcAuthor = "*"){
  //
  // Return Pointer to an Array of Integer Elements extracted from 
  // map for given Item. fiBuffer_length return length of array
  //
  // NOTE. The array is created with new function. user should take care of
  //       deleting it after use.
  //
  
  Int_t    *flBuffer = NULL; 
  TSQLRow  *sqlRow;
  TQuery    tmpQuery;
  
  try{
    
    TSQLResult & sqlResult = SelectValues(fcSystem,fcSubSystem,fcItem,fcRun_no,fcAuthor,"int");
    Int_t lBufferLength = sqlResult.GetFieldCount() -4;
    flBuffer = new Int_t[lBufferLength];
    *fiBuffer_length = lBufferLength;
    sqlRow = sqlResult.Next();
    for(int j=0; j< lBufferLength ; j++) flBuffer[j] = tmpQuery.IntOf(sqlRow->GetField(j+1));
    
  } catch  (TQuery & fErrMsg) {
    
    cout << endl << fErrMsg.Data() << endl << endl;
    *fiBuffer_length = 0;
    return NULL;
    
  }

  return flBuffer;
}

void    TDBTools::GetDBIntBuff(const char *fcSystem, const char *fcSubSystem, const char *fcItem, 
			       Int_t fcRun_no, TIntBuffer *fIntBuffer, const char *fcAuthor = "*"){
  
  TSQLRow  *sqlRow;
  TQuery    tmpQuery;
  
  try{
    
    TSQLResult & sqlResult = SelectValues(fcSystem,fcSubSystem,fcItem,fcRun_no,fcAuthor,"int");
    Int_t lBufferLength = sqlResult.GetFieldCount()-4;
    fIntBuffer->SetSize(lBufferLength);
    //  flBuffer = new Int_t[lBufferLength];
    //  *fiBuffer_length = lBufferLength;
    sqlRow = sqlResult.Next();
    for(int j=0; j< lBufferLength ; j++) fIntBuffer->Set(j,tmpQuery.IntOf(sqlRow->GetField(j+1)));
    
  } catch  (TQuery & fErrMsg) {
    
    cout << endl << fErrMsg.Data() << endl << endl;  
  }
}

Float_t    *TDBTools::GetDBFloat(const char *fcSystem, const char *fcSubSystem, const char *fcItem, 
				 Int_t fcRun_no, Int_t *fiBuffer_length, const char *fcAuthor = "*"){
  //
  // Return Pointer to an Array of Float Elements extracted from 
  // map for given Item. fiBuffer_length return length of array
  //
  // NOTE. The array is created with new function. user should take care of
  //       deletin it after use.

  Float_t    *flBuffer = NULL; 
  TSQLRow  *sqlRow;
  TQuery    tmpQuery;
  
  try{
    
    TSQLResult & sqlResult = SelectValues(fcSystem,fcSubSystem,fcItem,fcRun_no,fcAuthor,"float");
    Int_t lBufferLength = sqlResult.GetFieldCount()-4;
    flBuffer = new Float_t[lBufferLength];
    *fiBuffer_length = lBufferLength;
    
    sqlRow = sqlResult.Next();
    
    for(int j=0; j< lBufferLength ; j++){ 
      flBuffer[j] = tmpQuery.FloatOf(sqlRow->GetField(j+1));
    }
    
  } catch  (TQuery & fErrMsg) {
    cout << endl << fErrMsg.Data() << endl << endl;
    *fiBuffer_length = 0;
    return NULL;
  }
  
  return flBuffer;
}

void    TDBTools::GetDBFloatBuff(const char *fcSystem, const char *fcSubSystem,const char *fcItem, Int_t fcRun_no,TFloatBuffer *fFloatBuffer, const char *fcAuthor = "*"){
  
  TSQLRow  *sqlRow;
  TQuery    tmpQuery;
  
  try{
    
    TSQLResult & sqlResult = SelectValues(fcSystem,fcSubSystem,fcItem,fcRun_no,fcAuthor,"float");
    Int_t lBufferLength = sqlResult.GetFieldCount()-4;
    fFloatBuffer->SetSize(lBufferLength);
    //  flBuffer = new Float_t[lBufferLength];
    //  *fiBuffer_length = lBufferLength;
    
    sqlRow = sqlResult.Next();
    
    for(int j=0; j< lBufferLength ; j++){ 
      fFloatBuffer->Set(j,tmpQuery.FloatOf(sqlRow->GetField(j+1)));
    }
    
  } catch  (TQuery & fErrMsg) {
    cout << endl << fErrMsg.Data() << endl << endl;
  }
  
}

TString    *TDBTools::GetDBChar(const char *fcSystem, const char *fcSubSystem, const char *fcItem, 
				Int_t fcRun_no, Int_t *fiBuffer_length, const char *fcAuthor = "*"){
  //
  // Return Pointer to an Array of TString Elements extracted from 
  // map for given Item. fiBuffer_length return length of array
  //
  // NOTE. The array is created with new function. user should take care of
  //       deletin it after use.
  //
  
  TString    *flBuffer = NULL; 
  TSQLRow   *sqlRow;
  TQuery    tmpQuery;
  
  try{
    
    TSQLResult & sqlResult = SelectValues(fcSystem,fcSubSystem,fcItem,fcRun_no,fcAuthor,"char");
    Int_t lBufferLength = sqlResult.GetFieldCount()-4;
    flBuffer = new TString[lBufferLength];
    *fiBuffer_length = lBufferLength;

    sqlRow = sqlResult.Next();
    
    for(int j=0; j< lBufferLength ; j++){ 
     flBuffer[j] = sqlRow->GetField(j+1);
    }

  } catch  (TQuery & fErrMsg) {
    cout << endl << fErrMsg.Data() << endl << endl;
    *fiBuffer_length = 0;
    return NULL;
  }
  
return flBuffer;
}

Int_t    TDBTools::GetSystemID(TSQLServer *conn, const char *system_name, Int_t *pSystem_ID){
  //
  // This routine writes into pSystem_ID system ID for given System_name
  //
  // returns 0 if system was found. -1 otherwise
  //

  try{
    *pSystem_ID = GetSystemIDEx(conn,system_name);
  }catch  (TQuery & fErrMsg) {
    cout << endl << fErrMsg.Data() << endl << endl;
    return -1;
  }
  return 0;
}

Int_t    TDBTools::GetSystemIDEx(TSQLServer *conn, const char *system_name){
  Int_t System_id = -1;
  
  TSQLRow     *fRow;
  TSQLResult  *fResult;
  TQuery fQuery;
  
  fQuery << "select systemId from System where systemName=\"" << system_name << "\"";
  if((fDebugMode&0x20)>0) fQuery.Preview();
  fResult = conn->Query(fQuery.Data());
  
  if(fResult->GetRowCount()==1){
    fRow = fResult->Next();
    sscanf(fRow->GetField(0),"%d",&System_id);
  } else {
    
    TQuery ErrMsg;
    ErrMsg << "==>> ERROR(TItemClass::GetSystemID): System " << system_name << " not Found ";
    throw ErrMsg;
  }
  
  return System_id;
}

Int_t    TDBTools::GetSubSystemID(TSQLServer *conn, int system_id, const char *subsystem_name, Int_t *pSubSystem_ID){
  //
  //  SubSystem in pSubSystem_ID variable
  //
  //
  
  try{
    *pSubSystem_ID = GetSubSystemIDEx(conn,system_id,subsystem_name);
  }catch  (TQuery & fErrMsg) {
    cout << endl << fErrMsg.Data() << endl << endl;
    return -1;
  }
  return 0;
}

Int_t    TDBTools::GetSubSystemIDEx(TSQLServer *conn, int system_id, const char *subsystem_name){
  //
  // Returns SubSystem id from table SubSystem corresponding to given SystemID
  //
  //
  
  Int_t SubSystem_id = -1;
  
  TSQLRow     *fRow;
  TSQLResult  *fResult;
  TQuery fQuery;
  
  fQuery << "select subsystemId from SubSystem where systemId=" 
	 << system_id << " AND subsystemName=\"" << subsystem_name <<"\"";
  
  if((fDebugMode&0x20)>0) fQuery.Preview();
  
  fResult = conn->Query(fQuery.Data());
  
  if(fResult->GetRowCount()==1){
    fRow = fResult->Next();
    sscanf(fRow->GetField(0),"%d",&SubSystem_id);
    //	   cout << " System ID = " << fRow->GetField(0) << endl;
  } else {
    TQuery ErrMsg;
    ErrMsg << "==>> ERROR(TItemClass::GetSubSystemID): SubSystem not found. System ID = " << system_id 
	   << "  SubSystem Name : " << subsystem_name;
    throw ErrMsg;
  }

  return SubSystem_id;
}

void    TDBTools::GetItemEx(TSQLServer *conn, int subsystem_id, const char *item_name, TItem *fDBItem){

  TSQLRow     *fRow;
  TSQLResult  *fResult;
  TQuery       fQuery;
  
  fQuery << "select * from Item where subsystemID=" 
	 << subsystem_id << " AND itemName=\"" << item_name << "\"";
  if((fDebugMode&0x20)>0) fQuery.Preview();
  
  fResult = conn->Query(fQuery.Data());
  
  if(fResult->GetRowCount()==1){
    fRow = fResult->Next();
    
    fDBItem->fItemId        = fQuery.IntOf(fRow->GetField(0));
    fDBItem->fItemName      = (char *) fRow->GetField(1);
    fDBItem->fSubsystemId   = fQuery.IntOf(fRow->GetField(2));
    fDBItem->fLength        = fQuery.IntOf(fRow->GetField(3));
    fDBItem->fType          = (char *) fRow->GetField(4);
    fDBItem->fDescription   = (char *) fRow->GetField(5);
    
  } else {
    TQuery ErrMsg;
    ErrMsg << "==>> ERROR(TItemClass::GetItemID): Item not found. SubSystem ID = " << subsystem_id 
	   << "  Item Name : " << item_name;
    throw ErrMsg;
  }
}


Int_t  TDBTools::GetItem(TSQLServer *fbConn, const char *fcSystem, const char *fcSubSystem,
			 const char *fcItem, TItem *sfItem){
  
  try{
    
    Int_t  tmpSystemID    = GetSystemIDEx(fbConn,fcSystem);
    Int_t  tmpSubSystemID = GetSubSystemIDEx(fbConn,tmpSystemID,fcSubSystem);
    GetItemEx(fbConn,tmpSubSystemID,fcItem,sfItem);
    
  } catch (TQuery & fErrMsg) {
    cout << endl << fErrMsg.Data() << endl << endl;
    return -1;
 } 
  
  return 0;
}

Int_t   TDBTools::GetItemValue(TSQLServer *fbConn, TItem tDBItem, Int_t fcRunNum, 
			       const char *fcAuthor, TItemValue *tItemValue){
  try{
    GetItemValueEx(fbConn,tDBItem,fcRunNum,fcAuthor,tItemValue);
  } catch (TQuery & fErrMsg) {
    cout << endl << fErrMsg.Data() << endl << endl;
    return -1;
}
  return 0;
  
}
void   TDBTools::GetItemValueEx(TSQLServer *fbConn, TItem tDBItem, Int_t fcRunNum, 
				const char *fcAuthor, TItemValue *tItemValue){
  
  TSQLRow     *fRow;
  TSQLResult  *fResult;
  TQuery       fQuery;
  
  fQuery.Reset();
  if(strstr(fcAuthor,"*") != NULL){
    //    cout << "Getting Thigs Ordered by time " << endl;
    fQuery << "select * from " << fItemValueVersion.Data() << " where itemId=" << tDBItem.fItemId 
	   << " AND RunNumber=" << fcRunNum
	   << " ORDER BY time" ;
  } else {
    
    fQuery << "select * from " << fItemValueVersion.Data() << " where itemId=" << tDBItem.fItemId
	   << " AND author=\""<< fcAuthor <<"\" AND RunNumber=" << fcRunNum 
	   << " ORDER BY time";
  }
  
  if((fDebugMode&0x20)>0) fQuery.Preview();
  
  fResult = fbConn->Query(fQuery.Data());
  
  if(fResult->GetRowCount()<=0){
    cout << "GetDBValues: No record where found. Possible candidates are: " << endl;
    PrintItemTable(fbConn,tDBItem,fcRunNum);
    TQuery ErrMsg;
    ErrMsg << "==>> ERROR(TDBTools::GetItemValueID): Item Value not found. Item ID = " << tDBItem.fItemId 
	    << " Run : " << fcRunNum << "  Author : " << fcAuthor;
    throw ErrMsg;   
  } else {
    if(fResult->GetRowCount() > 1){
      cout << endl << endl << "\t\t ==>> W A R N I N G <<== " << endl << endl
	   << "GetItemValues: " << fResult->GetRowCount() << " records were found. Using most recent one. "
	   << endl << endl;
    }
    for(int j = 0; j < fResult->GetRowCount(); j++) fRow = fResult->Next();
    //	   fRow = fResult->Next();
    tItemValue->fRun     = fQuery.IntOf(fRow->GetField(1));
    tItemValue->fItemId  = fQuery.IntOf(fRow->GetField(2));
    tItemValue->fItemValueId = fQuery.IntOf(fRow->GetField(3));
    tItemValue->fAuthor  = fRow->GetField(4);
    tItemValue->fTime    = fRow->GetField(5);
    tItemValue->fComment = fRow->GetField(6);
  }
  
}

TString TDBTools::GetTableName(const char *dSystem,const char *dSubSystem, const char *dItem){
  
  TString  tblName;
  tblName = dSystem;
  tblName += "_" ;
  tblName = dSubSystem;
  tblName += "_" ;
  tblName = dItem;
  tblName += "_VALUE" ;
  
  return tblName;
}

void TDBTools::SetDebugMode(Int_t DbgMode){
  fDebugMode = DbgMode;
}

void    TDBTools::SetDbVersion(const char *fVersion){
  //
  //
  //
  //
  if(strcmp(fVersion,"DEV") == 0){
    fItemValueVersion = "RunIndex";
    //cout << "SetGbVersion: Set Database version to Development. Table \"" 
    // << fItemValueVersion.Data() <<"\" will be used" << endl;
  }
  if(strcmp(fVersion,"PROD") == 0){
    fItemValueVersion = "RunIndex";
    //    cout << "SetGbVersion: Set Database version to Production. Table \"" 
    // << fItemValueVersion.Data() <<"\" will be used" << endl;
  }
}
