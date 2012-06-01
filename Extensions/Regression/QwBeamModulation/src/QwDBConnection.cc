#define QW_DB_CONNECTION_CC
#include "../include/QwDBConnection.hh"
#include "../include/headers.h"

#ifdef QW_DB_CONNECTION_CC

QwDBConnection::QwDBConnection():Connection(false)
{

  std::cout << "Setting up standard database settings." << std::endl;

  fPortNumber = 0;
  fUser ="qweak";
  fDBaseServer ="qweakdb.jlab.org";
//   fDatabase = "qw_run2_pass1";
  fDatabase = "qw_run1_pass4b";
  fPass = "QweakQweak";


}

QwDBConnection::~QwDBConnection()
{
}

void QwDBConnection::Connect()
{
  if( !(this->connect(fDatabase.c_str(), fDBaseServer.c_str(), fUser.c_str(), fPass.c_str(), fPortNumber)) ){
    std::cout << "Error with connection." << std::endl;
  } else { 
    std::cout <<"Successfully connected to database."<<std::endl;
  }
}

void QwDBConnection::Connect(const char *db, const char *server, const char *user, const char *password, unsigned int port)
{

  fPortNumber = port;
  fUser = user;
  fDBaseServer = server;
  fDatabase = db;
  fPass = password;

  if( !(this->connect(db, server, user, password, port)) ){
    std::cout << "Error with connection." << std::endl;
  } else { 
    std::cout <<"Successfully connected to database."<<std::endl;
  }
}

void QwDBConnection::Query()
{

  mysqlpp::Query query = this->query("show tables");
  
  if( result = query.store() ){
    std::cout << "Query results stored" << std::endl;
  }
  else {
    std::cout << "Failed to get item list: " << query.error() << std::endl;
    return;
  }

}

Int_t QwDBConnection::Query(const char *request)
{

  mysqlpp::Query query = this->query(request);
  
  if( result = query.store() ){
    std::cout << "Query results stored." << std::endl;
  }
  else {
    std::cout << "Failed to get item list: " << query.error() << std::endl;
    return(0);
  }

  return((Int_t)result.num_rows());

}

void QwDBConnection::Close()
{
  this->disconnect();
}

void QwDBConnection::Print() const
{
  for(Int_t i = 0; i < (Int_t)result.num_rows(); i++){
    for(Int_t j = 0; j < this->fNumberColumns; j++){
      std::cout << result[i][j] << std::endl; 
    }
  }

}

#endif
