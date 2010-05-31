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
#include "QwSSQLS.h"


class QwEventBuffer;

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
    QwDatabase(QwOptions &options); //!< Constructor with QwOptions object

    ~QwDatabase(); //!< Destructor

    void         SetAccessLevel(string accesslevel);  //!< Sets the access level flag based on string labels:  "off", "ro", "rw".

    Bool_t       AllowsReadAccess(){return (fAccessLevel==kQwDatabaseReadOnly || fAccessLevel==kQwDatabaseReadWrite);};
    Bool_t       AllowsWriteAccess(){return (fAccessLevel==kQwDatabaseReadWrite);};

    Bool_t       Connect();                    //!< Open a connection to the database using the predefined parameters.
    void         Disconnect() {disconnect();}; //<! Close an open database connection
    const string GetServerVersion() {return server_version();}; //<! Get database server version
    static void  DefineOptions(QwOptions& options); //!< Defines available class options for QwOptions
    void ProcessOptions(QwOptions &options); //!< Processes the options contained in the QwOptions object.

    mysqlpp::Query Query(const char *qstr=0     ) {return query(qstr);} //<! Generate a query to the database.
    mysqlpp::Query Query(const std::string &qstr) {return query(qstr);} //<! Generate a query to the database.

    const UInt_t GetMonitorID(const string& name);         //<! Get monitor_id for beam monitor name
    const UInt_t GetMainDetectorID(const string& name);    //<! Get main_detector_id for main detector name
    const UInt_t GetLumiDetectorID(const string& name);    //<! Get lumi_detector_id for lumi detector name
    const string GetMeasurementID(const Int_t index);
    const UInt_t GetSlowControlDetectorID(const string& name);         //<! Get slow_controls_data_id for epics name
    
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
    enum EQwDBAccessLevel{kQwDatabaseOff, kQwDatabaseReadOnly, kQwDatabaseReadWrite};
 private:

    Bool_t       ValidateConnection();                  //!< Checks that given connection parameters result in a valid connection
    const UInt_t SetRunID(QwEventBuffer& qwevt);        //<! Set fRunID using data from CODA event buffer
    const UInt_t SetRunletID(QwEventBuffer& qwevt);        //<! Set fRunletID using data from CODA event buffer
    const UInt_t SetAnalysisID(QwEventBuffer& qwevt);   //<! Set fAnalysisID using data from CODA event buffer
    void StoreMonitorIDs();                             //<! Retrieve monitor IDs from database and populate fMonitorIDs
    void StoreMainDetectorIDs();                        //<! Retrieve main detector IDs from database and populate fMainDetectorIDs
    void StoreLumiDetectorIDs();                        //<! Retrieve LUMI monitor IDs from database and populate fLumiDetectorIDs
    void StoreMeasurementIDs();
    void StoreSlowControlDetectorIDs();                  //<! Retrieve slow controls data IDs from database and populate fSlow_Controls_DataIDs
    const bool StoreDBVersion();  //!< Retrieve database schema version information from database

    EQwDBAccessLevel fAccessLevel;  //!< Access level of the database instance

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
    static std::map<string, unsigned int> fSlowControlDetectorIDs; //!< Associative array of slow controls data IDs.  This declaration will be a problem if QwDatabase is used to connect to two databases simultaneously.
    static std::vector<string>            fMeasurementIDs; 

    friend class StoreMonitorID;
    friend class StoreMainDetectorID;
    friend class StoreLumiDetectorID;
    friend class StoreMeasurementID;
    friend class StoreSlowControlDetectorID;
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


class StoreSlowControlDetectorID {
  public:
    void operator() (QwParityDB::sc_detector elem) {
      QwDebug << "StoreSlowControlDetectorID: sc_detector_id = " << elem.sc_detector_id << " name = " << elem.name << QwLog::endl;
      QwDatabase::fSlowControlDetectorIDs.insert(std::make_pair(elem.name, elem.sc_detector_id));
    }
};



// QwDBInterface  GetDBEntry(TString subname);

// I extend a DB interface for QwIntegrationPMT to all subsystem,
// because the table structure are the same in the lumi_data table,
// the md_data table, and the beam table of MySQL database.
// Now every device-specified action will be done in
// the FillDB(QwDatabase *db, TString datatype) of QwBeamLine, 
// QwMainCerenkovDetector, and QwLumi class.


class QwDBInterface
{
  
 private:
  
  UInt_t fAnalysisId;
  UInt_t fDeviceId;
  UInt_t fSubblock;
  UInt_t fN;
  Double_t fValue;
  Double_t fError;
  Char_t fMeasurementTypeId[4];

  TString fDeviceName;

 private:
  template <class T> inline T TypedDBClone();


 public:

  QwDBInterface():fAnalysisId(0),fDeviceId(0),fSubblock(0),fN(0),fValue(0.0),fError(0.0)
    {std::strcpy(fMeasurementTypeId, "");fDeviceName ="";} ;
    ~QwDBInterface(){};

    void SetAnalysisID(UInt_t id) {fAnalysisId = id;};
    void SetDetectorName(TString &in) {fDeviceName = in;};
    void SetDeviceID(UInt_t id) {fDeviceId = id;};
    void SetMonitorID(QwDatabase *db) {
      fDeviceId = db->GetMonitorID(fDeviceName.Data());
      return;
    }
    void SetMainDetectorID(QwDatabase *db) {
      fDeviceId = db->GetMainDetectorID(fDeviceName.Data());
      return;
    }
    void SetLumiDetectorID(QwDatabase *db) {
      fDeviceId = db->GetLumiDetectorID(fDeviceName.Data());
      return;
    }
    void SetMeasurementTypeID(const char* in) {std::strncpy(fMeasurementTypeId, in, 3);};
    void SetSubblock(UInt_t in) {fSubblock = in;};
    void SetN(UInt_t in)        {fN = in;};
    void SetValue(Double_t in)  {fValue = in;};
    void SetError(Double_t in)  {fError = in;};
    
    TString GetDeviceName() {return fDeviceName;};

    void Reset() {
      fAnalysisId = 0;
      fDeviceId = 0;
      fSubblock = 0;
      fN = 0;
      fValue = 0.0;
      fError = 0.0;
      std::strcpy(fMeasurementTypeId,"");
      fDeviceName = "";
    };
  
    template <class T> void AddThisEntryToList(std::vector<T> &list);

      
    void PrintStatus(Bool_t print_flag) {
      if(print_flag) {
	QwMessage << std::setw(12)
		  << " AnalysisID " << fAnalysisId
		  << " Device :"    << std::setw(18) << fDeviceName 
		  << ":" << std::setw(4) << fDeviceId 
		  << " Subblock "   << fSubblock
		  << " n "          << fN
		  << " Type "       << fMeasurementTypeId
		  << " [ave, err] " 
		  << " [" << std::setw(14) << fValue
		  << ","  << std::setw(14) << fError
		  << "]"
		  << QwLog::endl;
      }
      return;
    };
};


template <class T> inline T QwDBInterface::TypedDBClone(){
  T row(0);
  return row;
};
template<> inline QwParityDB::md_data 
QwDBInterface::TypedDBClone<QwParityDB::md_data>() {
  QwParityDB::md_data row(0);
  row.analysis_id         = fAnalysisId;
  row.main_detector_id    = fDeviceId;
  row.measurement_type_id = fMeasurementTypeId;
  row.subblock            = fSubblock;
  row.n                   = fN;
  row.value               = fValue;
  row.error               = fError;
  return row;
};
template<> inline QwParityDB::lumi_data
QwDBInterface::TypedDBClone<QwParityDB::lumi_data>() {
  QwParityDB::lumi_data row(0);
  row.analysis_id         = fAnalysisId;
  row.lumi_detector_id    = fDeviceId;
  row.measurement_type_id = fMeasurementTypeId;
  row.subblock            = fSubblock;
  row.n                   = fN;
  row.value               = fValue;
  row.error               = fError;
  return row;
};
template<> inline QwParityDB::beam
QwDBInterface::TypedDBClone<QwParityDB::beam>() {
  QwParityDB::beam row(0);
  row.analysis_id         = fAnalysisId;
  row.monitor_id          = fDeviceId;
  row.measurement_type_id = fMeasurementTypeId;
  row.subblock            = fSubblock;
  row.n                   = fN;
  row.value               = fValue;
  row.error               = fError;
  return row;
};

#endif
