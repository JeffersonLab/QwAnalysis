/*!
 * \file   QwDatabase.h
 * \brief  A class for handling connections to the Qweak database.
 *
 * \author Damon Spayde
 * \date   2010-01-07
 */

#ifndef QWDATABASE_HH
#define QWDATABASE_HH 

// System headers
//#include <iostream>
//#include <iomanip>
#include <string>
using std::string;

//Third Party Headers
#include <mysql++.h>

// Qweak headers
#include "QwTypes.h"
#include "QwLog.h"
#include "QwOptions.h"


/**
 *  \class QwDatabase
 *  \ingroup QwAnalysis
 *  \brief A logfile class
 *
 * This class provides the connection to the Qweak database to other objects
 * in the Qweak analyzer.  A static global object gQwDatabase is used to
 * provide these services.
 *
 */
class QwDatabase: private mysqlpp::Connection {

  public:

    QwDatabase(); //!< Simple constructor

    ~QwDatabase(); //!< Destructor

    bool Connect(); //!< Open a connection to the database using the predefined parameters.  
    void Disconnect() {disconnect();}; //<! Close an open database connection
    const string GetServerVersion() {return server_version();}; //<! Get database server version
    void DefineOptions(); //!< Defines available class options for QwOptions

  private:

    bool ValidateConnection(); //!< Checks that given connection parameters result in a valid connection

    string fDatabase; //!< Name of database to connect to
    string fDBServer; //!< Name of server carrying DB to connect to
    string fDBUsername; //!< Name of account to connect to DB server with
    string fDBPassword; //!< DB account password
    UInt_t fDBPortNumber; //!< Port number to connect to on server (mysql default port is 3306)
    bool fValidConnection; //!< True if a valid connection was established using defined connection information

};

extern QwDatabase gQwDatabase;

#endif
