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
#include <map>
#include <vector>
#include <string>

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
#include "QwSSQLS.h"


/**
 *  \class QwDatabase
 *  \ingroup QwAnalysis
 *  \brief A database interface class
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

    Bool_t       Connect();                    //!< Open a connection to the database using the predefined parameters.
    void         Disconnect() {disconnect();}; //<! Close an open database connection
    const string GetServerVersion() {return server_version();}; //<! Get database server version
    static void  DefineOptions(QwOptions& options); //!< Defines available class options for QwOptions

    mysqlpp::Query Query(const char *qstr=0     ) {return query(qstr);} //<! Generate a query to the database.
    mysqlpp::Query Query(const std::string &qstr) {return query(qstr);} //<! Generate a query to the database.

    const UInt_t GetMonitorID(const string& name);         //<! Get monitor_id for beam monitor name
    const UInt_t GetMainDetectorID(const string& name);    //<! Get main_detector_id for main detector name
    const UInt_t GetLumiDetectorID(const string& name);    //<! Get lumi_detector_id for lumi detector name
    const UInt_t GetDetectorID(const string& name);
    const string GetMeasurementID(const Int_t index);
    
    const UInt_t GetRunNumber() {return fRunNumber;}       //<! Run number getter
    const UInt_t GetSegmentNumber() {return fSegmentNumber;}       //<! CODA File segment number getter
    const UInt_t GetRunID()     {return fRunID;}           //<! Run ID getter
    const UInt_t GetRunletID()     {return fRunletID;}           //<! Runlet ID getter
    const UInt_t GetAnalysisID() {return fAnalysisID;};    //<! Get analysis ID


    const UInt_t GetRunID(QwEventBuffer& qwevt);           //<! Get run ID using data from CODA event buffer
    const UInt_t GetRunletID(QwEventBuffer& qwevt);      //<! Get runlet ID using data from CODA event buffer
    const UInt_t GetAnalysisID(QwEventBuffer& qwevt);      //<! Get analysis ID using data from CODA event buffer
    Bool_t       SetRunNumber(const UInt_t runnum);        //<! Run number setter
    Bool_t       SetSegmentNumber(const UInt_t segment);        //<! CODA file segment number setter
    const string GetVersion();                             //! Return a full version string for the DB schema
    const string GetVersionMajor() {return fVersionMajor;} //<! fVersionMajor getter
    const string GetVersionMinor() {return fVersionMinor;} //<! fVersionMinor getter
    const string GetVersionPoint() {return fVersionPoint;} //<! fVersionPoint getter
    const string GetValidVersion();                        //<! Return a full required version string.

    void         PrintServerInfo();                        //<! Print Server Information
    
 private:

    Bool_t       ValidateConnection();                  //!< Checks that given connection parameters result in a valid connection
    const UInt_t SetRunID(QwEventBuffer& qwevt);        //<! Set fRunID using data from CODA event buffer
    const UInt_t SetRunletID(QwEventBuffer& qwevt);        //<! Set fRunletID using data from CODA event buffer
    const UInt_t SetAnalysisID(QwEventBuffer& qwevt);   //<! Set fAnalysisID using data from CODA event buffer
    void StoreMonitorIDs();                             //<! Retrieve monitor IDs from database and populate fMonitorIDs
    void StoreMainDetectorIDs();                        //<! Retrieve main detector IDs from database and populate fMainDetectorIDs
    void StoreLumiDetectorIDs();                        //<! Retrieve LUMI monitor IDs from database and populate fLumiDetectorIDs
    void StoreMeasurementIDs();
    const bool StoreDBVersion();  //!< Retrieve database schema version information from database

    string fDatabase;        //!< Name of database to connect to
    string fDBServer;        //!< Name of server carrying DB to connect to
    string fDBUsername;      //!< Name of account to connect to DB server with
    string fDBPassword;      //!< DB account password
    UInt_t fDBPortNumber;    //!< Port number to connect to on server (mysql default port is 3306)
    Bool_t fValidConnection; //!< True if a valid connection was established using defined connection information

    UInt_t fRunNumber;       //!< Run number of current run
    UInt_t fSegmentNumber;   //!< CODA file segment number of current run
    UInt_t fRunID;           //!< run_id of current run
    UInt_t fRunletID;        //!< runlet_id of current run
    UInt_t fAnalysisID;      //!< analysis_id of current analysis pass
    string fVersionMajor;    //!< Major version number of current DB schema
    string fVersionMinor;    //!< Minor version number of current DB schema
    string fVersionPoint;    //!< Point version number of current DB schema
    const string kValidVersionMajor;
    const string kValidVersionMinor;
    const string kValidVersionPoint;

    static std::map<string, unsigned int> fMonitorIDs; //!< Associative array of beam monitor IDs.  This declaration will be a problem if QwDatabase is used to connect to two databases simultaneously.
    static std::map<string, unsigned int> fMainDetectorIDs; //!< Associative array of main detector IDs.  This declaration will be a problem if QwDatabase is used to connect to two databases simultaneously.
    static std::map<string, unsigned int> fLumiDetectorIDs; //!< Associative array of LUMI detector IDs.  This declaration will be a problem if QwDatabase is used to connect to two databases simultaneously.
    static std::vector<string>            fMeasurementIDs; 

    friend class StoreMonitorID;
    friend class StoreMainDetectorID;
    friend class StoreLumiDetectorID;
    friend class StoreMeasurementID;
};

class StoreMonitorID {
  public:
    void operator() (QwParityDB::monitor elem) {
      QwDebug << "StoreMonitorID:  monitor_id = " << elem.monitor_id << " quantity = " << elem.quantity << QwLog::endl;
      QwDatabase::fMonitorIDs.insert(std::make_pair(elem.quantity, elem.monitor_id));
    }
};

class StoreMainDetectorID {
  public:
    void operator() (QwParityDB::main_detector elem) {
      QwDebug << "StoreMainDetectorID:  main_detector_id = " << elem.main_detector_id << " quantity = " << elem.quantity << QwLog::endl;
      QwDatabase::fMainDetectorIDs.insert(std::make_pair(elem.quantity, elem.main_detector_id));
    }
};

class StoreLumiDetectorID {
  public:
    void operator() (QwParityDB::lumi_detector elem) {
      QwDebug << "StoreLumiDetectorID:  lumi_detector_id = " << elem.lumi_detector_id << " quantity = " << elem.quantity << QwLog::endl;
      QwDatabase::fLumiDetectorIDs.insert(std::make_pair(elem.quantity, elem.lumi_detector_id));
    }
};

class StoreMeasurementID {
 public:
  void operator() (QwParityDB::measurement_type elem) {
      QwDebug << "StoreMeasurementID:  measurement_type = " << elem.measurement_type_id << QwLog::endl;
      QwDatabase::fMeasurementIDs.push_back( (std::string) elem.measurement_type_id );
    }
};


#endif
