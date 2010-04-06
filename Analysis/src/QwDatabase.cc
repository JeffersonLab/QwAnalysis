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
#include "QwSSQLS.h"
using namespace QwParityDB;
// Is there any method put QwSSQLS.h in QwDatabase.h? (jhlee)

// Definition of static class members in QwDatabase
std::map<string, unsigned int> QwDatabase::fMonitorIDs;

/*! The simple constructor initializes member fields.  This class is not
 * used to establish the database connection.  It sets up a
 * mysqlpp::Connection() object that has exception throwing disabled.
 */
//QwDatabase::QwDatabase() : Connection(false)
QwDatabase::QwDatabase() : Connection()
{
  // Initialize member fields
  fDatabase=fDBServer=fDBUsername=fDBPassword="";

  fDBPortNumber      = 0;
  fValidConnection   = false;
  fRunNumber         = 0;
  fRunID             = 0;
  fAnalysisID        = 0;

}

/*! The destructor says "Good-bye World!"
 */
QwDatabase::~QwDatabase()
{
  Disconnect();
  QwDebug << "QwDatabase::~QwDatabase() : Good-bye World from QwDatabase destructor!" << QwLog::endl;
}

/*! This function is used to load the connection information for the
 * database.  It tests the connection to make sure it is valid and causes a
 * program exit if no valid connection can be formed.
 *
 * It is called the first time Connect() is called.
 */
bool QwDatabase::ValidateConnection() {

  //
  // Retrieve options
  //
  string dbname, server, username, password;
  Int_t port = gQwOptions.GetValue<int>("QwDatabase.dbport");

  if (gQwOptions.HasValue("QwDatabase.dbname")) {
    dbname = gQwOptions.GetValue<string>("QwDatabase.dbname");
  } else {
    QwError << "QwDatabase::ValidateConnection() : No database supplied.  Unable to connect." << QwLog::endl;
    return false;
  }

  if (gQwOptions.HasValue("QwDatabase.dbusername")) {
    username = gQwOptions.GetValue<string>("QwDatabase.dbusername");
  } else {
    QwError << "QwDatabase::ValidateConnection() : No database username supplied.  Unable to connect." << QwLog::endl;
    return false;
  }

  if (gQwOptions.HasValue("QwDatabase.dbpassword")) {
    password = gQwOptions.GetValue<string>("QwDatabase.dbpassword");
  } else {
    QwError << "QwDatabase::ValidateConnection() : No database password supplied.  Unable to connect." << QwLog::endl;
    return false;
  }

  if (gQwOptions.HasValue("QwDatabase.dbserver")) {
    server = gQwOptions.GetValue<string>("QwDatabase.dbserver");
  } else {
    QwMessage << "QwDatabase::ValidateConnection() : No database server supplied.  Attempting localhost." << QwLog::endl;
    server = "localhost";
  }

  //
  // Try to connect with given information
  //
  try {
    connect(dbname.c_str(), server.c_str(), username.c_str(), password.c_str(), (unsigned int) port);
    //    connect(dbname.Data(), server.Data(), username.Data(), password.Data(), (unsigned int) port);
  } catch (std::exception const& e) {
    QwError << "QwDatabase::ValidateConnection() : " << QwLog::endl;
    QwError << e.what() << " while validating connection" << QwLog::endl;
    QwError << "Database name = " << dbname <<QwLog::endl;
    QwError << "Database server = " << server <<QwLog::endl;
    QwError << "Database username = " << username <<QwLog::endl;
    QwError << "Database port = " << port <<QwLog::endl;
    QwError << "Exiting." << QwLog::endl;
    exit(1);
  }

  // Success!
  QwMessage << "QwDatabase::ValidateConnection() : Successfully connected to requested database." << QwLog::endl;

  // Connection was good so update member variables and disconnect from
  // database
  fDatabase=dbname;
  fDBServer=server;
  fDBUsername=username;
  fDBPassword=password;
  fDBPortNumber=port;
  fValidConnection=true;
  disconnect();

  return true;

}


/*! This function is used to initiate a database connection.
 */
bool QwDatabase::Connect()
{
  /* Open a connection to the database using the predefined parameters.
   * Must call QwDatabase::ConnectionInfo() first.
   */

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
  options.AddOptions()("QwDatabase.dbname", po::value<string>(), "database name");
  options.AddOptions()("QwDatabase.dbserver", po::value<string>(), "database server name");
  options.AddOptions()("QwDatabase.dbusername", po::value<string>(), "database username");
  options.AddOptions()("QwDatabase.dbpassword", po::value<string>(), "database password");
  options.AddOptions()("QwDatabase.dbport", po::value<int>()->default_value(0), "database server port number (defaults to standard mysql port)");

}

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
    vector<run> res;
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
      vector<run> res;
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
      run row(0, qwevt.GetRunNumber());
//      run row(0, qwevt.GetRunNumber(),mysqlpp::null, mysqlpp::null, mysqlpp::null, 0,0);
      row.run_type = mysqlpp::null;
      row.helicity_length = 0;
			row.start_time = mysqlpp::null;
			row.end_time = mysqlpp::null;
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
  
  if (fRunID == 0 || fRunNumber != qwevt.GetRunNumber() ) {
    QwDebug << "QwDatabase::GetRunID() set fRunID to " << SetRunID(qwevt) << QwLog::endl;
  }

  return fRunID;

}

/*!
 * This is used to set the appropriate analysis_id for this run.  Must be a valid run_id in the run table before proceeding.  Will insert an entry into the analysis table if necessary.
 */
const UInt_t QwDatabase::SetAnalysisID(QwEventBuffer& qwevt)
{
  try {

    this->Connect();

    analysis analysis_row(0);

    analysis_row.run_id  = GetRunID(qwevt);

    // seed_id, monitor_calibration_id, cut_id are necessary to "create" one analysis ID
    // thus, I selected one id (1), already be in,  from their tables and I inserted
    // one id into monitor_calibration table and used it in order to remove the 
    // several ERROR messages.

    analysis_row.seed_id = 1;

    
    //    row.n_mps=10; // This works
    //    row.start_time = mysqlpp::null; // This works
    //    row.start_time = qwevt.GetStartSQLTime().Data(); // This does not work

    // ERROR 1452 (23000): Cannot add or update a child row: a foreign key constraint fails (`qw_test/analysis`, CONSTRAINT `fk_analysis_seed_id` FOREIGN KEY (`seed_id`) REFERENCES `seed` (`seed_id`))
    // How do I know the seed id

    analysis_row.monitor_calibration_id = 1;
    //Error: Cannot add or update a child row: a foreign key constraint fails (`qw_test/analysis`, CONSTRAINT `fk_analysis_monitor_calibration_id` FOREIGN KEY (`monitor_calibration_id`) REFERENCES `monitor_calibration` (`monitor_calibration_id`))
    //Error: QwDatabase::SetAnalysisID() unable to set valid fAnalysisID for this run.  Exiting.
    //  mysql> select * from monitor_calibration;
    //  Empty set (0.00 sec)

    //  mysql> insert into monitor_calibration (comment) values ('test');

    analysis_row.cut_id = 1;
    //Error: Cannot add or update a child row: a foreign key constraint fails (`qw_test/analysis`, CONSTRAINT `fk_analysis_cut_id` FOREIGN KEY (`cut_id`) REFERENCES `cut` (`cut_id`))
    //Error: QwDatabase::SetAnalysisID() unable to set valid fAnalysisID for this run.  Exiting.


    //
    // 
    //  analysis_row.time = mysqlpp::null; 
    //    analysis_row.bf_checksum = "";
    //    analysis_row.beam_mode = "nbm";

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
  // Sanity check to make sure not calling this before run_id has been retrieved.
  if (fRunID == 0) {
    QwDebug << "QwDatabase::GetAnalysisID() : fRunID must be set before proceeding.  Check to make sure run exists in database." << QwLog::endl;
    return 0;
  }

  if (fAnalysisID == 0 ) {
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
      printf("\n");
      printf("QwDatabase MySQL ");
      printf("%s v%s %s -----------------\n", BOLD, GetServerVersion().c_str(), NORMAL);

      printf("Database server : %s%10s%s",  RED,  fDBServer.c_str(), NORMAL);
      printf(" name   : %s%12s%s",  BLUE, fDatabase.c_str(), NORMAL);
      printf(" user   : %s%6s%s",  RED, fDBUsername.c_str(), NORMAL);
      printf(" port   : %s%6d%s\n",  BLUE, fDBPortNumber, NORMAL);
      printf(" %s\n", server_status().c_str());
      printf("\n");
    }
  else
    {
      printf("There is no connection\n");

    }

  return;
}

/*
 * This function retrieves the monitor table key 'monitor_id' for a given beam monitor.
 */
const UInt_t QwDatabase::GetMonitorID(const string& monitor)
{
  if (fMonitorIDs.size() == 0) {
    StoreMonitorIDs();
  }

  UInt_t monitor_id = fMonitorIDs[monitor];

  if (monitor_id==0) {
    QwError << "QwDatabase::GetMonitorID() => Unable to determine valid ID for beam monitor " << monitor << QwLog::endl;
  }

  return monitor_id;

}

void QwDatabase::StoreMonitorIDs()
{

  try {
    Connect();

    mysqlpp::Query query=this->Query();
    query.for_each(monitor(), StoreMonitorID());

//    QwDebug<< "QwDatabase::SetAnalysisID() => Analysis Insert Query = " << query.str() << QwLog::endl;

    Disconnect();

  }
  catch (const mysqlpp::Exception& er) {
    QwError << er.what() << QwLog::endl;
    Disconnect();
    exit(1);
  }

}
