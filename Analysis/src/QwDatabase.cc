/*!
 * \file   QwDatabase.cc
 * \brief  A class for handling connections to the Qweak database.
 *
 * \author Damon Spayde
 * \date   2010-01-07
 */


#define EXPAND_MY_SSQLS_STATICS
#include "QwDatabase.h"

// System headers

// Qweak headers
#include "QwEventBuffer.h"
#include "QwSSQLS.h"
using namespace QwParityDB;

//
//  Template definitions for the QwDBInterface class.
//
//

template <class T>
void QwDBInterface::AddThisEntryToList(std::vector<T> &list){
  Bool_t okay = kTRUE;
  if (fAnalysisId == 0){
    QwError << "AddDBEntryToList:  Analysis ID invalid; entry dropped"
	    << QwLog::endl;
    okay = kFALSE;
  }
  if (fDeviceId == 0){
    QwError << "AddDBEntryToList:  Device ID invalid; entry dropped"
	    << QwLog::endl;
    okay = kFALSE;
  }
  if (okay){
    T row = TypedDBClone<T>();
    if (row.analysis_id == 0){
      QwError << "AddDBEntryToList:  Unknown list type; entry dropped"
	      << QwLog::endl;
      okay = kFALSE;
    } else {
      list.push_back(row);
    }
  }
  if (okay == kFALSE){
    PrintStatus(kTRUE);
  };
};

template void QwDBInterface::AddThisEntryToList<QwParityDB::md_data>(std::vector<QwParityDB::md_data> &list);
template void QwDBInterface::AddThisEntryToList<QwParityDB::lumi_data>(std::vector<QwParityDB::lumi_data> &list);
template void QwDBInterface::AddThisEntryToList<QwParityDB::beam>(std::vector<QwParityDB::beam> &list);




// Definition of static class members in QwDatabase
std::map<string, unsigned int> QwDatabase::fMonitorIDs;
std::map<string, unsigned int> QwDatabase::fMainDetectorIDs;
std::map<string, unsigned int> QwDatabase::fLumiDetectorIDs;
std::vector<string>            QwDatabase::fMeasurementIDs;
std::map<string, unsigned int> QwDatabase::fSlowControlDetectorIDs;

/*! The simple constructor initializes member fields.  This class is not
 * used to establish the database connection.  It sets up a
 * mysqlpp::Connection() object that has exception throwing disabled.
 */
//QwDatabase::QwDatabase() : Connection(false)
QwDatabase::QwDatabase() : Connection(), kValidVersionMajor("01"), kValidVersionMinor("00"), kValidVersionPoint("0000")
{
  // Initialize member fields
  fDatabase=fDBServer=fDBUsername=fDBPassword="";
  fVersionMajor=fVersionMinor=fVersionPoint="";

  fAccessLevel = kQwDatabaseOff;

  fDBPortNumber      = 0;
  fValidConnection   = false;
  fRunNumber         = 0;
  fRunID             = 0;
  fRunletID          = 0;
  fAnalysisID        = 0;
  fSegmentNumber     = -1;
}

/*! The constructor initializes member fields using the values in
 *  the QwOptions object.
 * @param options  The QwOptions object.
 */
QwDatabase::QwDatabase(QwOptions &options) : Connection(), kValidVersionMajor("01"), kValidVersionMinor("00"), kValidVersionPoint("0000")
{
  // Initialize member fields
  fDatabase=fDBServer=fDBUsername=fDBPassword="";
  fVersionMajor=fVersionMinor=fVersionPoint="";

  fAccessLevel = kQwDatabaseOff;

  fDBPortNumber      = 0;
  fValidConnection   = false;
  fRunNumber         = 0;
  fRunID             = 0;
  fAnalysisID        = 0;

  ProcessOptions(options);
}

/*! The destructor says "Good-bye World!"
 */
QwDatabase::~QwDatabase()
{
  if( connected() ) Disconnect();
  QwDebug << "QwDatabase::~QwDatabase() : Good-bye World from QwDatabase destructor!" << QwLog::endl;
}

/*! This function is used to load the connection information for the
 * database.  It tests the connection to make sure it is valid and causes a
 * program exit if no valid connection can be formed.
 *
 * It is called the first time Connect() is called.
 */
Bool_t QwDatabase::ValidateConnection() 
{
  // Bool_t status;
  //
  // Retrieve options if they haven't already been filled.
  //
  //   if (fDatabase.empty()){
  //     status = ProcessOptions(gQwOptions);
  //     if (!status) return status;
  //   }
  
  //  Check values.
  if (fAccessLevel!=kQwDatabaseOff){
    if (fDatabase.empty()){
      QwError << "QwDatabase::ValidateConnection() : No database supplied.  Unable to connect." << QwLog::endl;
      fValidConnection=false;    
    }
    if (fDBUsername.empty()){
      QwError << "QwDatabase::ValidateConnection() : No database username supplied.  Unable to connect." << QwLog::endl;
      fValidConnection=false;    
    }
    if (fDBPassword.empty()){
      QwError << "QwDatabase::ValidateConnection() : No database password supplied.  Unable to connect." << QwLog::endl;
      fValidConnection=false;
    }
    if (fDBServer.empty()){
      QwMessage << "QwDatabase::ValidateConnection() : No database server supplied.  Attempting localhost." << QwLog::endl;
      fDBServer = "localhost";
    }
    //
    // Try to connect with given information
    //
    try {
      connect(fDatabase.c_str(), fDBServer.c_str(), fDBUsername.c_str(),
	      fDBPassword.c_str(), (unsigned int) fDBPortNumber);
      //    connect(dbname.Data(), server.Data(), username.Data(), password.Data(), (unsigned int) port);
    } catch (std::exception const& e) {
      QwError << "QwDatabase::ValidateConnection() : " << QwLog::endl;
      QwError << e.what() << " while validating connection" << QwLog::endl;
      QwError << "Database name = " << fDatabase <<QwLog::endl;
      QwError << "Database server = " << fDBServer <<QwLog::endl;
      QwError << "Database username = " << fDBUsername <<QwLog::endl;
      QwError << "Database port = " << fDBPortNumber <<QwLog::endl;
      QwError << "Exiting." << QwLog::endl;
      exit(1);
    }

    // Get database schema version information
    if (StoreDBVersion()) {
      fValidConnection=true;
      // Success!
      QwMessage << "QwDatabase::ValidateConnection() : Successfully connected to requested database." << QwLog::endl;
    } else {
      QwError << "QwDatabase::ValidateConnection() : Unsuccessfully connected to requested database." << QwLog::endl;
      // Connection was bad so clear the member variables
      fValidConnection=false;
      fDatabase.clear();
      fDBServer.clear();
      fDBUsername.clear();
      fDBPassword.clear();
      fDBPortNumber=0;
    }
    disconnect();
  }

  // Check to make sure database and QwDatabase schema versions match up.
  if (fVersionMajor != kValidVersionMajor ||
      fVersionMinor != kValidVersionMinor ||
      fVersionPoint < kValidVersionPoint) {
    fValidConnection = false;
    QwError << "QwDatabase::ValidConnection() : Connected database schema inconsistent with current version of analyzer." << QwLog::endl;
    QwError << "  Database version is " << this->GetVersion() << QwLog::endl;
    QwError << "  Required database version is " << this->GetValidVersion() << QwLog::endl;
    QwError << "  Please connect to a database supporting the required schema version." << QwLog::endl;
    exit(1);
  }

  return fValidConnection;
}


/*! This function is used to initiate a database connection.
 */
bool QwDatabase::Connect()
{
  /* Open a connection to the database using the predefined parameters.
   * Must call QwDatabase::ConnectionInfo() first.
   */

  //  Return flase, if we're not using the DB.
  if (fAccessLevel==kQwDatabaseOff) return false;

  // Make sure not already connected
  if (connected()) return true;

  // If never connected before, then make sure connection parameters form
  // valid connection
  if (!fValidConnection) {
    ValidateConnection();
  }

  if (fValidConnection) {
    return connect(fDatabase.c_str(), fDBServer.c_str(), fDBUsername.c_str(), fDBPassword.c_str(), (unsigned int) fDBPortNumber);
    //   return connect(fDatabase.Data(), fDBServer.Data(), fDBUsername.Data(), fDBPassword.Data(), (unsigned int) fDBPortNumber);
  } else {
    QwError << "QwDatabase::Connect() : Must establish valid connection to database." << QwLog::endl;
    return false;
  }
}

/*!
 * Defines configuration options for QwDatabase class using QwOptions
 * functionality.
 *
 * Should apparently by called by QwOptions::DefineOptions() in
 * QwParityOptions.h
 */
void QwDatabase::DefineOptions(QwOptions& options)
{
  // Specify command line options for use by QwDatabase
  options.AddOptions()("QwDatabase.accesslevel", po::value<string>(), "database access level (OFF,RW)");
  options.AddOptions()("QwDatabase.dbname", po::value<string>(), "database name");
  options.AddOptions()("QwDatabase.dbserver", po::value<string>(), "database server name");
  options.AddOptions()("QwDatabase.dbusername", po::value<string>(), "database username");
  options.AddOptions()("QwDatabase.dbpassword", po::value<string>(), "database password");
  options.AddOptions()("QwDatabase.dbport", po::value<int>()->default_value(0), "database server port number (defaults to standard mysql port)");
}

/*!
 * Loads the configuration options for QwDatabase class into this instance of
 * QwDatabase from the QwOptions object.
 * @param options Options object
 */
void QwDatabase::ProcessOptions(QwOptions &options)
{
  if (options.HasValue("QwDatabase.accesslevel")) {
    string access = options.GetValue<string>("QwDatabase.accesslevel");
    SetAccessLevel(access);
  } 
  else {
    QwWarning << "QwDatabase::ProcessOptions : No access level specified; database access is OFF" << QwLog::endl;
    fAccessLevel = kQwDatabaseOff;
  }
  if (options.HasValue("QwDatabase.dbport")) {
    fDBPortNumber = options.GetValue<int>("QwDatabase.dbport");
  }
  if (options.HasValue("QwDatabase.dbname")) {
    fDatabase = options.GetValue<string>("QwDatabase.dbname");
  }
  if (options.HasValue("QwDatabase.dbusername")) {
    fDBUsername = options.GetValue<string>("QwDatabase.dbusername");
  }
  if (options.HasValue("QwDatabase.dbpassword")) {
    fDBPassword = options.GetValue<string>("QwDatabase.dbpassword");
  }
  if (options.HasValue("QwDatabase.dbserver")) {
    fDBServer = options.GetValue<string>("QwDatabase.dbserver");
  }
  
  return;
};

void QwDatabase::ProcessOptions(const TString& dbname, const TString& username, const TString& passwd, const TString& dbhost, const Int_t dbport, const TString& accesslevel)
{
  SetAccessLevel(static_cast<string>(accesslevel));
  fDatabase = dbname;
  fDBUsername = username;
  fDBPassword = passwd;
  fDBServer = dbhost;
  fDBPortNumber = dbport;
}

void QwDatabase::SetAccessLevel(string accesslevel)
{
  TString level = accesslevel.c_str();
  level.ToLower();
  if (level=="off")     fAccessLevel = kQwDatabaseOff;
  else if (level=="ro") fAccessLevel = kQwDatabaseReadOnly;
  else if (level=="rw") fAccessLevel = kQwDatabaseReadWrite;
  else{
    QwWarning << "QwDatabase::SetAccessLevel  : Unrecognized access level \""
	      << accesslevel << "\"; setting database access OFF"
	      << QwLog::endl;
    fAccessLevel = kQwDatabaseOff;
  }
  return;
};


/*!
 * Sets run number for subsequent database interactions.  Makes sure correct
 * entry exists in run table and retrieves run_id.
 */
bool QwDatabase::SetRunNumber(const UInt_t runnum)
{

  QwDebug << "Made it into QwDatabase::SetRunNumber()" << QwLog::endl;

  try {

    this->Connect();

    mysqlpp::Query query= this->Query();
    query << "SELECT * FROM run WHERE run_number = " << runnum;
    std::vector<run> res;
    query.storein(res);

    QwDebug << "Number of rows returned:  " << res.size() << QwLog::endl;

    if (res.size()!=1) {
      QwError << "Unable to find unique run number " << runnum << " in database." << QwLog::endl;
      QwError << "Run number query returned " << res.size() << "rows." << QwLog::endl;
      QwError << "Please make sure that the database contains one unique entry for this run." << QwLog::endl;
      return false;
    }

    QwDebug << "Run ID = " << res.at(0).run_id << QwLog::endl;

    fRunNumber = runnum;
    fRunID = res.at(0).run_id;

    this->Disconnect();
  }
  catch (const mysqlpp::Exception& er) {
    QwError << er.what() << QwLog::endl;
    return false;
  }

  return true;

}

/*!
 * This function sets the fRunID for the run being replayed as determined by the QwEventBuffer class.
 */
const UInt_t QwDatabase::SetRunID(QwEventBuffer& qwevt)
{

  // Check to see if run is already in database.  If so retrieve run ID and exit.
  try
    {
      this->Connect();
      mysqlpp::Query query = this->Query();

      query << "SELECT * FROM run WHERE run_number = " << qwevt.GetRunNumber();
      std::vector<run> res;
      query.storein(res);
      QwDebug << "QwDatabase::SetRunID => Number of rows returned:  " << res.size() << QwLog::endl;

      // If there is more than one run in the DB with the same run number, then there will be trouble later on.  Catch and bomb out.
      if (res.size()>1)
	{
	  QwError << "Unable to find unique run number " << qwevt.GetRunNumber() << " in database." << QwLog::endl;
	  QwError << "Run number query returned " << res.size() << "rows." << QwLog::endl;
	  QwError << "Please make sure that the database contains one unique entry for this run." << QwLog::endl;
	  this->Disconnect();
	  return 0;
	}

      // Run already exists in database.  Pull run_id and move along.
      if (res.size()==1)
	{
	  QwDebug << "QwDatabase::SetRunID => Run ID = " << res.at(0).run_id << QwLog::endl;

	  fRunNumber = qwevt.GetRunNumber();
	  fRunID     = res.at(0).run_id;
	  this->Disconnect();
	  return fRunID;
	}
      this->Disconnect();
    }
  catch (const mysqlpp::Exception& er)
    {

      QwError << er.what() << QwLog::endl;
      this->Disconnect();
      return 0;
    }

  // Run is not in database so insert pertinent data and retrieve run ID
  // Right now this does not insert start/stop times or info on number of events.
  try
    {

      this->Connect();
      run row(0);
      row.run_number      = qwevt.GetRunNumber();
      row.run_type        = "good"; // qwevt.GetRunType(); RunType is the confused name because we have also a CODA run type.
      row.start_time      = mysqlpp::DateTime(qwevt.GetStartUnixTime());
      row.end_time        = mysqlpp::DateTime(qwevt.GetEndUnixTime());
      row.n_mps = 0;
      row.n_qrt	= 0;
//    row.n_mps=10; // This works
      //    row.start_time = mysqlpp::null; // This works
      //    row.start_time = qwevt.GetStartSQLTime().Data(); // This does not work
        mysqlpp::Query query=this->Query();
        query.insert(row);
       QwDebug<< "QwDatabase::SetRunID() => Run Insert Query = " << query.str() << QwLog::endl;

      query.execute();

      if (query.insert_id()!=0)
	{
	  fRunNumber = qwevt.GetRunNumber();
	  fRunID     = query.insert_id();
	}
      this->Disconnect();
      return fRunID;
    }
  catch (const mysqlpp::Exception& er)
    {
      QwError << er.what() << QwLog::endl;
      this->Disconnect();
      return 0;
    }

}

/*!
 * This is a getter for run_id in the run table.  Should be used in subsequent queries to retain key relationships between tables.
 */
const UInt_t QwDatabase::GetRunID(QwEventBuffer& qwevt)
{
  // If the stored run number does not agree with the CODA run number
  // or if fRunID is not set, then retrieve data from database and update if necessary.

  // GetRunNumber() in QwEventBuffer returns Int_t, thus
  // we should convert it to UInt_t here. I think, it is OK.

  if (fRunID == 0 || fRunNumber != (UInt_t) qwevt.GetRunNumber() ) {
     QwDebug << "QwDatabase::GetRunID() set fRunID to " << SetRunID(qwevt) << QwLog::endl;
     fRunletID = 0;
     fAnalysisID = 0;
  }

  return fRunID;

}

/*!
 * This function sets the fRunletID for the run being replayed as determined by the QwEventBuffer class.
 *
 * Runlets are differentiated by file segment number at the moment, not by event range or start/stop time.  This function will need to be altered if we opt to differentiate between runlets in a different way.
 */
const UInt_t QwDatabase::SetRunletID(QwEventBuffer& qwevt)
{

  // Make sure 'run' table has been populated and retrieve run_id
  //  UInt_t runid = this->GetRunID(qwevt);
 
  // Check to see if runlet is already in database.  If so retrieve runlet_id and exit.  
  try 
    {
      this->Connect();
      mysqlpp::Query query = this->Query();

      // Query is slightly different if file segments are being chained together for replay or not.
      if (qwevt.AreRunletsSplit()) {
        fSegmentNumber = qwevt.GetSegmentNumber();
        query << "SELECT * FROM runlet WHERE run_id = " << fRunID << " AND full_run = 'false' AND segment_number = " << fSegmentNumber;
      } else {
	query << "SELECT * FROM runlet WHERE run_id = " << fRunID << " AND full_run = 'true'";
      }

      std::vector<runlet> res;
      query.storein(res);
      QwDebug << "QwDatabase::SetRunletID => Number of rows returned:  " << res.size() << QwLog::endl;

      // If there is more than one run in the DB with the same runlet number, then there will be trouble later on.  Catch and bomb out.
      if (res.size()>1) 
	{
	  QwError << "Unable to find unique runlet number " << qwevt.GetRunNumber() << " in database." << QwLog::endl;
	  QwError << "Run number query returned " << res.size() << "rows." << QwLog::endl;
	  QwError << "Please make sure that the database contains one unique entry for this run." << QwLog::endl;
	  this->Disconnect();
	  return 0;
	}
      
      // Run already exists in database.  Pull runlet_id and move along.
      if (res.size()==1) 
	{
	  QwDebug << "QwDatabase::SetRunletID => Runlet ID = " << res.at(0).runlet_id << QwLog::endl;
	  
	  fRunletID     = res.at(0).runlet_id;
	  this->Disconnect();
	  return fRunletID;
	}
      this->Disconnect();
    }
  catch (const mysqlpp::Exception& er) 
    {
   
      QwError << er.what() << QwLog::endl;
      this->Disconnect();
      return 0;
    }

  // Runlet is not in database so insert pertinent data and retrieve run ID
  // Right now this does not insert start/stop times or info on number of events.
  try 
    {
      
      this->Connect();
      runlet row(0);
      row.run_id      = fRunID;
      row.run_number      = qwevt.GetRunNumber();
      row.start_time      = mysqlpp::null;
      row.end_time        = mysqlpp::null;
      row.first_mps = 0;
      row.last_mps	= 0;
      if (qwevt.AreRunletsSplit()) {
        row.segment_number = fSegmentNumber;
        row.full_run = "false";
      } else {
        row.segment_number  = mysqlpp::null;
        row.full_run = "true";
      }

        mysqlpp::Query query=this->Query();
        query.insert(row);
       QwDebug<< "QwDatabase::SetRunletID() => Run Insert Query = " << query.str() << QwLog::endl;
      
      query.execute();

      if (query.insert_id()!=0) 
	{
	  fRunletID     = query.insert_id();
	}
      this->Disconnect();
      return fRunletID;
    }
  catch (const mysqlpp::Exception& er) 
    {
      QwError << er.what() << QwLog::endl;
      this->Disconnect();
      return 0;
    }
  
}

/*!
 * This is a getter for runlet_id in the runlet table.  Should be used in subsequent queries to retain key relationships between tables.
 */
const UInt_t QwDatabase::GetRunletID(QwEventBuffer& qwevt)
{
  // If the stored run number does not agree with the CODA run number 
  // or if fRunID is not set, then retrieve data from database and update if necessary.
  
  if (fRunletID == 0 || (qwevt.AreRunletsSplit() && fSegmentNumber!=qwevt.GetSegmentNumber()) || fRunNumber != (UInt_t) qwevt.GetRunNumber() ) {
     QwDebug << "QwDatabase::GetRunletID() set fRunletID to " << SetRunletID(qwevt) << QwLog::endl;
     fAnalysisID = 0;
  }

  return fRunletID;

}

/*!
 * This is used to set the appropriate analysis_id for this run.  Must be a valid runlet_id in the runlet table before proceeding.  Will insert an entry into the analysis table if necessary.
 */
const UInt_t QwDatabase::SetAnalysisID(QwEventBuffer& qwevt)
{
  try {



    analysis analysis_row(0);

    analysis_row.runlet_id  = GetRunletID(qwevt);
    analysis_row.seed_id = 1;
    analysis_row.monitor_calibration_id = 1;
    analysis_row.cut_id  = 1;

    std::pair<UInt_t, UInt_t> event_range;
    event_range = qwevt.GetEventRange();

    analysis_row.time        = mysqlpp::DateTime::now();
    analysis_row.bf_checksum = "empty"; // we will match this as a real one later
    analysis_row.beam_mode   = "nbm";   // we will match this as a real one later
    analysis_row.n_mps       = 0;       // we will match this as a real one later
    analysis_row.n_qrt       = 4;       // we will match this as a real one later
    analysis_row.first_event = event_range.first;
    analysis_row.last_event  = event_range.second;
    analysis_row.segment     = 0;       // we will match this as a real one later
    analysis_row.slope_calculation = "off";  // we will match this as a real one later
    analysis_row.slope_correction  = "off"; // we will match this as a real one later


    this->Connect();
    mysqlpp::Query query= this->Query();
    query.insert(analysis_row);
    // I don't know why QwLog doesn't print properly, for I switched to cout
    std::cout << "QwDatabase::SetAnalysisID() => Analysis Insert Query = " << query.str() << std::endl;

    query.execute();

    if (query.insert_id()!=0)
      {
	fAnalysisID = query.insert_id();
      }

    this->Disconnect();
    return fAnalysisID;
  }
  catch (const mysqlpp::Exception& er) {
    QwError << er.what() << QwLog::endl;
    this->Disconnect();
    return 0;
  }


}

/*!
 * This is a getter for analysis_id in the analysis table.  Required by all queries on cerenkov, beam, etc. tables.  Will return 0 if fRunID has not been successfully set.  If fAnalysisID is not set, then calls code to insert into analysis table and retrieve analysis_id.
 */
const UInt_t QwDatabase::GetAnalysisID(QwEventBuffer& qwevt)
{
  // Sanity check to make sure not calling this before runlet_id has been retrieved.
  if (fRunletID == 0) {
    QwDebug << "QwDatabase::GetAnalysisID() : fRunletID must be set before proceeding.  Check to make sure run exists in database." << QwLog::endl;
    return 0;
  }

  if (fAnalysisID == 0 || fRunNumber != (UInt_t) qwevt.GetRunNumber() 
      || (qwevt.AreRunletsSplit() && fSegmentNumber!=qwevt.GetSegmentNumber())) {
    QwDebug << "QwDatabase::GetAnalysisID() set fAnalysisID to " << SetAnalysisID(qwevt) << QwLog::endl;
    if (fAnalysisID==0) {
      QwError << "QwDatabase::SetAnalysisID() unable to set valid fAnalysisID for this run.  Exiting." <<QwLog::endl;
      exit(1);
    }
  }

  return fAnalysisID;
}


/*!
 * This function prints the server information.
 */
void QwDatabase::PrintServerInfo()
{
  if (fValidConnection)
    {
      printf("\nQwDatabase MySQL ");
      printf("%s v%s %s -----------------\n", BOLD, GetServerVersion().c_str(), NORMAL);
      printf("Database server : %s%10s%s",    RED,  fDBServer.c_str(),   NORMAL);
      printf(" name   : %s%12s%s",            BLUE, fDatabase.c_str(),   NORMAL);
      printf(" user   : %s%6s%s",             RED,  fDBUsername.c_str(), NORMAL);
      printf(" port   : %s%6d%s\n",           BLUE, fDBPortNumber,       NORMAL);
      printf(" %s\n\n", server_status().c_str());
    }
  else
    {
      printf("There is no connection.\n");
    }

  return;
}

/*
 * This function retrieves the monitor table key 'monitor_id' for a given beam monitor.
 */
const UInt_t QwDatabase::GetMonitorID(const string& name)
{
  if (fMonitorIDs.size() == 0) {
    StoreMonitorIDs();
  }

  UInt_t monitor_id = fMonitorIDs[name];

  if (monitor_id==0) {
    //    monitor_id = 6; // only for QwMockDataAnalysis
    QwError << "QwDatabase::GetMonitorID() => Unable to determine valid ID for beam monitor " << name << QwLog::endl;
  }

  return monitor_id;

}

/*
 * Stores monitor table keys in an associative array indexed by monitor name.
 */
void QwDatabase::StoreMonitorIDs()
{
  try {

    this->Connect();
    mysqlpp::Query query=this->Query();
    query.for_each(monitor(), StoreMonitorID());

//    QwDebug<< "QwDatabase::SetAnalysisID() => Analysis Insert Query = " << query.str() << QwLog::endl;
    this->Disconnect();
  }

  catch (const mysqlpp::Exception& er) {
    QwError << er.what() << QwLog::endl;
    Disconnect();
    exit(1);
  }
  return;
};

/*
 * This function retrieves the main_detector table key 'main_detector_id' for a given beam main_detector.
 */
const UInt_t QwDatabase::GetMainDetectorID(const string& name)
{
  if (fMainDetectorIDs.size() == 0) {
    StoreMainDetectorIDs();
  }

  UInt_t main_detector_id = fMainDetectorIDs[name];

  if (main_detector_id==0) {
    //    main_detector_id = 19; // only for QwMockDataAnalysis
    QwError << "QwDatabase::GetMainDetectorID() => Unable to determine valid ID for beam main_detector " << name << QwLog::endl;
  }

  return main_detector_id;

};


/*
 * Stores main_detector table keys in an associative array indexed by main_detector name.
 */
void QwDatabase::StoreMainDetectorIDs()
{

  try {
    this->Connect();
    mysqlpp::Query query=this->Query();
    query.for_each(main_detector(), StoreMainDetectorID());

//    QwDebug<< "QwDatabase::SetAnalysisID() => Analysis Insert Query = " << query.str() << QwLog::endl;

    this->Disconnect();
  }
  catch (const mysqlpp::Exception& er) {
    QwError << er.what() << QwLog::endl;
    Disconnect();
    exit(1);
  }
  return;
};


/*
 * This function retrieves the slow control detector table key 'sc_detector_id' for a given epics variable.
 */
const UInt_t QwDatabase::GetSlowControlDetectorID(const string& name)
{
  if (fSlowControlDetectorIDs.size() == 0) {
    StoreSlowControlDetectorIDs();
  }

  UInt_t sc_detector_id = fSlowControlDetectorIDs[name];

  if (sc_detector_id==0) {
    QwError << "QwDatabase::GetSlowControlDetectorID() => Unable to determine valid ID for the epics variable " << name << QwLog::endl;
  }

  return sc_detector_id;

};

/*
 * Stores slow control detector table keys in an associative array indexed by slow_controls_data name.
 */
void QwDatabase::StoreSlowControlDetectorIDs()
{

  try {
    this->Connect();
    mysqlpp::Query query=this->Query();
    query.for_each(sc_detector(), StoreSlowControlDetectorID());

//    QwDebug<< "QwDatabase::SetAnalysisID() => Analysis Insert Query = " << query.str() << QwLog::endl;

    this->Disconnect();
  }
  catch (const mysqlpp::Exception& er) {
    QwError << er.what() << QwLog::endl;
    Disconnect();
    exit(1);
  }
  return;
};

/*
 * This function retrieves the lumi_detector table key 'lumi_detector_id' for a given beam lumi_detector.
 */
const UInt_t QwDatabase::GetLumiDetectorID(const string& name)
{
  if (fLumiDetectorIDs.size() == 0) {
    StoreLumiDetectorIDs();
  }

  UInt_t lumi_detector_id = fLumiDetectorIDs[name];

  if (lumi_detector_id==0) {
     QwError << "QwDatabase::GetLumiDetectorID() => Unable to determine valid ID for beam lumi_detector " << name << QwLog::endl;
  }

  return lumi_detector_id;
};

/*
 * Stores lumi_detector table keys in an associative array indexed by lumi_detector name.
 */
void QwDatabase::StoreLumiDetectorIDs()
{

  try {
    this->Connect();

    mysqlpp::Query query=this->Query();
    query.for_each(lumi_detector(), StoreLumiDetectorID());

//    QwDebug<< "QwDatabase::SetAnalysisID() => Analysis Insert Query = " << query.str() << QwLog::endl;

    this->Disconnect();

  }
  catch (const mysqlpp::Exception& er) {
    QwError << er.what() << QwLog::endl;
    Disconnect();
    exit(1);
  }
  return;
};




const  string QwDatabase::GetMeasurementID(const Int_t index)
{
  if (fMeasurementIDs.size() == 0) {
    StoreMeasurementIDs();
  }

  string measurement_type = fMeasurementIDs[index];

  if (measurement_type.empty()) {
    QwError << "QwDatabase::GetMeasurementID() => Unable to determine valid ID for measurement type with " << index << QwLog::endl;
  }

  return measurement_type;
};

void QwDatabase::StoreMeasurementIDs()
{

  try {
    this->Connect();

    mysqlpp::Query query=this->Query();
    query.for_each(measurement_type(), StoreMeasurementID());

    // QwDebug<< "QwDatabase::SetAnalysisID() => Analysis Insert Query = " << query.str() << QwLog::endl;

    this->Disconnect();

  }
  catch (const mysqlpp::Exception& er) {
    QwError << er.what() << QwLog::endl;
    Disconnect();
    exit(1);
  }
  return;
};

const string QwDatabase::GetVersion(){
  string version = fVersionMajor + "." + fVersionMinor + "." + fVersionPoint;
  return version;
}

const string QwDatabase::GetValidVersion(){
  string version = kValidVersionMajor + "." + kValidVersionMinor + "." + kValidVersionPoint;
  return version;
}

/*
 * Retrieves database schema version information from database.
 * Returns true if successful, false otherwise.
 */
const bool QwDatabase::StoreDBVersion()
{
  try
    {
      mysqlpp::Query query = this->Query();

      query << "SELECT * FROM db_schema";
      std::vector<db_schema> res;
      query.storein(res);
      QwDebug << "QwDatabase::StoreDBVersion => Number of rows returned:  " << res.size() << QwLog::endl;

      // If there is more than one run in the DB with the same run number, then there will be trouble later on.  Catch and bomb out.
      if (res.size()>1)
	{
	  QwError << "Unable to find unique schema version in database." << QwLog::endl;
	  QwError << "Schema query returned " << res.size() << "rows." << QwLog::endl;
	  QwError << "Please make sure that db_schema contains one unique." << QwLog::endl;
	  this->Disconnect();
	  return false;
	}

      // Run already exists in database.  Pull run_id and move along.
      if (res.size()==1)
	{
	  QwDebug << "QwDatabase::StoreDBVersion => db_schema_id = " << res.at(0).db_schema_id << QwLog::endl;

	  fVersionMajor=res.at(0).major_release_number;
	  fVersionMinor=res.at(0).minor_release_number;
	  fVersionPoint=res.at(0).point_release_number;
	  this->Disconnect();
	}
    }
  catch (const mysqlpp::Exception& er)
    {
      QwError << er.what() << QwLog::endl;
      disconnect();
      return false;
    }

  return true;

};
