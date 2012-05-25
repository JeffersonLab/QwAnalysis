#ifndef QW_DB_CONNECTION_HH
#define QW_DB_CONNECTION_HH

#include <mysql++.h>
#include "headers.h"
#include <string>

class QwDBConnection: private mysqlpp::Connection {

private:
 
  //  mysqlpp::Connection database(false);
  unsigned int fPortNumber;

  std::string fUser;
  std::string fDBaseServer;
  std::string fDatabase;
  std::string fPass;

public:

  Int_t fNumberColumns;

  mysqlpp::StoreQueryResult result;

  QwDBConnection();
  ~QwDBConnection();


  void Connect();
  void Connect(const char *db, const char *server=0, const char *user=0, const char *password=0, unsigned int port=0);
  void Query();
  void Close();

  void Print() const;

  Int_t Query(const char *request=0);  

};
#endif
