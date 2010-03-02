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

// Third Party Headers
#include <mysql++.h>

// ROOT headers
#include "TString.h"

// Qweak headers
#include "QwTypes.h"
#include "QwLog.h"
#include "QwColor.h"
#include "QwOptions.h"
#include "QwEventBuffer.h"
//#include "QwSSQLS.h"

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
    Bool_t SetConnect(TString dbname, TString dbserver, TString dbuser, TString dbpasswd, UInt_t dbport = 0);
    void Disconnect() {disconnect();}; //<! Close an open database connection
    const string GetServerVersion() {return server_version();}; //<! Get database server version
    static void DefineOptions(QwOptions& options); //!< Defines available class options for QwOptions
    mysqlpp::Query Query(const char *qstr=0) {return query(qstr);} //<! Generate a query to the database.
    mysqlpp::Query Query(const std::string &qstr) {return query(qstr);} //<! Generate a query to the database.
    const UInt_t GetRunNumber() {return fRunNumber;} //<! Run number getter
//    const UInt_t GetRunID() {return fRunID;} //<! Run ID getter
    const UInt_t GetRunID(QwEventBuffer& qwevt); //<! Get run ID using data from CODA event buffer
    const UInt_t GetAnalysisID(QwEventBuffer& qwevt); //<! Get analysis ID using data from CODA event buffer
    bool SetRunNumber(const UInt_t runnum); //<! Run number setter

    void PrintServerInfo();

  private:

    bool ValidateConnection(); //!< Checks that given connection parameters result in a valid connection
    const UInt_t SetRunID(QwEventBuffer& qwevt); //<! Set fRunID using data from CODA event buffer
    const UInt_t SetAnalysisID(QwEventBuffer& qwevt); //<! Set fAnalysisID using data from CODA event buffer

    string fDatabase; //!< Name of database to connect to
    string fDBServer; //!< Name of server carrying DB to connect to
    string fDBUsername; //!< Name of account to connect to DB server with
    string fDBPassword; //!< DB account password
    UInt_t fDBPortNumber; //!< Port number to connect to on server (mysql default port is 3306)
    bool fValidConnection; //!< True if a valid connection was established using defined connection information

    UInt_t fRunNumber; //!< Run number of current run
    UInt_t fRunID; //!< run_id of current run
    UInt_t fAnalysisID; //!< analysis_id of current analysis pass

};

extern QwDatabase gQwDatabase;

#endif
