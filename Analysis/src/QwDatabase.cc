/*!
 * \file   QwDatabase.cc
 * \brief  A class for handling connections to the Qweak database.
 *
 * \author Damon Spayde
 * \date   2010-01-07
 */

#include "QwDatabase.h"

// System headers

// Qweak headers
#define EXPAND_MY_SSQLS_STATICS
#include "QwSSQLS.h"

// Create the static logger object (with streams to screen and file)
QwDatabase gQwDatabase;

/*! The simple constructor initializes member fields.  This class is not
 * used to establish the database connection.  It sets up a
 * mysqlpp::Connection() object that has exception throwing disabled.
 */
//QwDatabase::QwDatabase() : Connection(false)
QwDatabase::QwDatabase() : Connection()
{
  // Initialize member fields
  fDatabase=fDBServer=fDBUsername=fDBPassword="";
  fDBPortNumber=0;
  fValidConnection=false;
  fRunNumber=fRunID=0;

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

/*! This function is used to initiate a database connection without using QwOption class
 */
Bool_t QwDatabase::SetConnect(TString dbname, TString dbserver, TString dbuser, TString dbpasswd, UInt_t dbport)
{

  /* Open a connection to the database using the predefined parameters.
   * Must call QwDatabase::ConnectionInfo() first.
   */

  // Make sure not already connected
  if (connected()) return true;

  // If never connected before, then make sure connection parameters form
  // valid connection
  if (!fValidConnection)
    {
      try
	{
	  connect(dbname, dbserver, dbuser, dbpasswd, dbport);
	}
      catch (std::exception const& e)
	{
	  QwError << "QwDatabase::ValidateConnection() : " << QwLog::endl;
	  QwError << e.what() << " while validating connection" << QwLog::endl;
	  QwError << "Database name = " << dbname <<QwLog::endl;
	  QwError << "Database server = " << dbserver <<QwLog::endl;
	  QwError << "Database username = " << dbuser <<QwLog::endl;
	  QwError << "Database port = " << dbport <<QwLog::endl;
	  QwError << "Exiting." << QwLog::endl;
	  exit(1);
	}
      // Success!
      QwMessage << "QwDatabase::ValidateConnection() : Successfully connected to requested database." << QwLog::endl;

      // Connection was good so update member variables and disconnect from
      // database
      fDatabase=dbname;
      fDBServer=dbserver;
      fDBUsername=dbuser;
      fDBPassword=dbpasswd;
      fDBPortNumber=dbport;
      fValidConnection=true;
      disconnect();
    }

  if (fValidConnection)
    {
      return connect(fDatabase.c_str(), fDBServer.c_str(), fDBUsername.c_str(), fDBPassword.c_str(), (unsigned int) fDBPortNumber);
      //   return connect(fDatabase.Data(), fDBServer.Data(), fDBUsername.Data(), fDBPassword.Data(), (unsigned int) fDBPortNumber);
    }
  else
    {
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
    gQwDatabase.Connect();

    mysqlpp::Query query=gQwDatabase.Query();
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

    gQwDatabase.Disconnect();
  }
  catch (const mysqlpp::Exception& er) {
    QwError << er.what() << QwLog::endl;
    return false;
  }

  return true;

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
