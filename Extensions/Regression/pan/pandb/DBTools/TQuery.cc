///////////////////////////////////////////////////////////
//
//  File :  TQuery.cc
//
//  Query class to construct mySQL queries 
//  
//  R. Suleiman 
//
///////////////////////////////////////////////////////////

#include "TQuery.hh"

ClassImp(TQuery)
  
TQuery::TQuery(){
  // Default constructor
  //
}

TQuery::~TQuery(){
  // Default destructor
  //
}

TQuery & TQuery::operator<<( int x_int){
  // Converts integer into string and adds to
  // query string
  //
  char num[15];
  sprintf(num,"%d",x_int);
  this->fQuery += num;
  return *this;
}

TQuery & TQuery::operator<<( float x_float){
  // Converts float into string and adds to
  // query string
  //
  char num[15];
  sprintf(num,"%f",x_float);
  this->fQuery += num;
  return *this;
}

TQuery & TQuery::operator<<(const char *n_char){
  // adds character string to
  // query string
  //
  this->fQuery += n_char;
  return *this;
}

void TQuery::Preview(){
  // Displays query
  //
  cout << "QUERY.PREVIEW: " << fQuery.Data() << endl;
}

const char *TQuery::Data(){
  // returns pointer to query string
  //
  return fQuery.Data();
}

void TQuery::Reset(){
  // reset query string
  //  
  fQuery = "";
}

Float_t  TQuery::FloatOf(const char *chr_float){
  // Converts character string into Float and return float value
  //
  Float_t  float_value;
  sscanf(chr_float,"%f",&float_value);
  return float_value;
}

Int_t    TQuery::IntOf(const char *chr_int){
  // Converts character string into Int and return int value
  //
  Int_t  int_value;
  sscanf(chr_int,"%d",&int_value);
  return int_value;
}

