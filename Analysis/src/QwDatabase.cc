/*!
 * \file   QwDatabase.cc
 * \brief  A class for handling connections to the Qweak database.
 *
 * \author Damon Spayde
 * \date   2010-01-07
 */

#include "QwDatabase.h"

// System headers
//#include <fstream>

// Qweak headers

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


}

/*! The destructor says "Good-bye World!"
 */
QwDatabase::~QwDatabase()
{
  QwDebug << "QwDatabase::~QwDatabase() : Good-bye World from G0Database destructor!" << QwLog::endl;
  Disconnect();
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
  Int_t port = gQwOptions.GetValue<int>("dbport");

  if (gQwOptions.HasValue("dbname")) {
    dbname = gQwOptions.GetValue<string>("dbname");
  } else {
    QwError << "QwDatabase::ValidateConnection() : No database supplied.  Unable to connect." << QwLog::endl;
    return false;
  }

  if (gQwOptions.HasValue("dbusername")) {
    username = gQwOptions.GetValue<string>("dbusername");
  } else {
    QwError << "QwDatabase::ValidateConnection() : No database username supplied.  Unable to connect." << QwLog::endl;
    return false;
  }

  if (gQwOptions.HasValue("dbpassword")) {
    password = gQwOptions.GetValue<string>("dbpassword");
  } else {
    QwError << "QwDatabase::ValidateConnection() : No database password supplied.  Unable to connect." << QwLog::endl;
    return false;
  }

  if (gQwOptions.HasValue("dbserver")) {
    server = gQwOptions.GetValue<string>("dbserver");
  } else {
    QwMessage << "QwDatabase::ValidateConnection() : No database username supplied.  Attempting localhost." << QwLog::endl;
    server = "localhost";
  }

  //
  // Try to connect with given information
  //
  try {
    connect(dbname.c_str(), server.c_str(), username.c_str(), password.c_str(), (unsigned int) port);
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
void QwDatabase::DefineOptions()
{  
  // Specify command line options for use by QwDatabase
  gQwOptions.AddOptions()("dbname", po::value<string>(), "database name");
  gQwOptions.AddOptions()("dbserver", po::value<string>(), "database server name");
  gQwOptions.AddOptions()("dbusername", po::value<string>(), "database username");
  gQwOptions.AddOptions()("dbpassword", po::value<string>(), "database password");
  gQwOptions.AddOptions()("dbport", po::value<int>()->default_value(0), "database server port number (defaults to standard mysql port)");

}

