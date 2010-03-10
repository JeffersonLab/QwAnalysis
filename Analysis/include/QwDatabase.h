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

#include <vector>
using std::vector;
#include <string>
using std::string;
#include <map>
using std::map;
#include <algorithm>

// Should the following defines be placed in
// other header file?

#define QWK_UNDEFINED  74  // clk
#define QWK_BATTERY   184
#define QWK_BCM0102   186  // qwk_bcm0102
#define QWK_QPD1      216 
#define QWK_QPD2      217 
#define QWK_QPD3      218
#define QWK_QPD4      219  // monitor table in the qw_test "quantity is qpd3" ***
#define QWK_XQPD      220  // qwk_qpdRelX
#define QWK_YQPD      221  // qwk_qpdRelY
#define QWK_1I02X     188  // qwk_1i02RelX
#define QWK_1I04X     189  // qwk_1i04RelX
#define QWK_1I06X     190  // qwk_1i06RelX
#define QWK_0I02X     191  // qwk_0i02RelX
#define QWK_0I02AX    192  // qwk_0i02aRelX
#define QWK_0i05X     193  // qwk_0i05RelX
#define QWK_0i07X     194  // qwk_0i07RelX
#define QWK_0L01X     195  // qwk_0l01RelX
#define QWK_0L02X     196  // qwk_0l02RelX
#define QWK_0L03X     197  // qwk_0l03RelX
#define QWK_0L04X     198  // qwk_0l04RelX
#define QWK_0L05X     199  // qwk_0l05RelX
#define QWK_0L06X     200  // qwk_0l06RelX
#define QWK_0L07X     QWK_UNDEFINED
#define QWK_0L08X     QWK_UNDEFINED
#define QWK_0L09X     QWK_UNDEFINED
#define QWK_0L10X     QWK_UNDEFINED
#define QWK_0R01X     QWK_UNDEFINED
#define QWK_0R02X     QWK_UNDEFINED
#define QWK_0R05X     201  //qwk_0r05RelX
#define QWK_0R06X     QWK_UNDEFINED

#define QWK_1I02Y     202  // qwk_1i02RelY
#define QWK_1I04Y     203  // qwk_1i04RelY
#define QWK_1I06Y     204  // qwk_1i06RelY
#define QWK_0I02Y     205  // qwk_0i02RelY
#define QWK_0I02AY    206  // qwk_0i02aRelY
#define QWK_0i05Y     207  // qwk_0i05RelY
#define QWK_0i07Y     208  // qwk_0i07RelY
#define QWK_0L01Y     209  // qwk_0l01RelY
#define QWK_0L02Y     210  // qwk_0l02RelY
#define QWK_0L03Y     211  // qwk_0l03RelY
#define QWK_0L04Y     212  // qwk_0l04RelY
#define QWK_0L05Y     213  // qwk_0l05RelY
#define QWK_0L06Y     214  // qwk_0l06RelY
#define QWK_0L07Y     QWK_UNDEFINED
#define QWK_0L08Y     QWK_UNDEFINED
#define QWK_0L09Y     QWK_UNDEFINED
#define QWK_0L10Y     QWK_UNDEFINED
#define QWK_0R01Y     QWK_UNDEFINED
#define QWK_0R02Y     QWK_UNDEFINED
#define QWK_0R05Y     215  //qwk_0r05RelY
#define QWK_0R06Y     QWK_UNDEFINED


#define QWK_1I02Y   202 // qwk_1i02RelY 
#define QWK_1I04Y   203 // qwk_1i04RelY


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

    Bool_t       Connect();                    //!< Open a connection to the database using the predefined parameters.
    void         Disconnect() {disconnect();}; //<! Close an open database connection
    const string GetServerVersion() {return server_version();}; //<! Get database server version
    static void  DefineOptions(QwOptions& options); //!< Defines available class options for QwOptions

    mysqlpp::Query Query(const char *qstr=0     ) {return query(qstr);} //<! Generate a query to the database.
    mysqlpp::Query Query(const std::string &qstr) {return query(qstr);} //<! Generate a query to the database.

    const UInt_t GetRunNumber() {return fRunNumber;}   //<! Run number getter
    const UInt_t GetRunID()     {return fRunID;}       //<! Run ID getter
    const UInt_t GetAnalysisID() {return fAnalysisID;};//<! Get analysis ID
 
    const UInt_t GetRunID(QwEventBuffer& qwevt);       //<! Get run ID using data from CODA event buffer
    const UInt_t GetAnalysisID(QwEventBuffer& qwevt);  //<! Get analysis ID using data from CODA event buffer
    Bool_t       SetRunNumber(const UInt_t runnum);    //<! Run number setter

    void         PrintServerInfo();//<! Print Server Information
    
    const Int_t  LookupMonitorID(TString element_name);

    Bool_t       GetStaticTypes();
 
  private:

    Bool_t       ValidateConnection(); //!< Checks that given connection parameters result in a valid connection
    const UInt_t SetRunID(QwEventBuffer& qwevt); //<! Set fRunID using data from CODA event buffer
    const UInt_t SetAnalysisID(QwEventBuffer& qwevt); //<! Set fAnalysisID using data from CODA event buffer

    vector <TString> fMonitorTable;
/*     vector <TString> fMeasurementTypeTable; */

    string fDatabase;        //!< Name of database to connect to
    string fDBServer;        //!< Name of server carrying DB to connect to
    string fDBUsername;      //!< Name of account to connect to DB server with
    string fDBPassword;      //!< DB account password
    UInt_t fDBPortNumber;    //!< Port number to connect to on server (mysql default port is 3306)
    Bool_t fValidConnection; //!< True if a valid connection was established using defined connection information

    UInt_t fRunNumber;       //!< Run number of current run
    UInt_t fRunID;           //!< run_id of current run
    UInt_t fAnalysisID;      //!< analysis_id of current analysis pass

    Bool_t fReadyStaticTypes; 

};

#endif
