//Implementation file for slug calc class.
//J. Magee
//February 14, 2013
//slugcalc.cpp

#include<iostream>
#include<math.h>
#include<vector>
#include<stdlib.h>

#include"SlugCalc.h"

using namespace std;

SlugCalc::SlugCalc( ) //define general constructor
{ dataPresent=0; } //end constructor def


SlugCalc::SlugCalc(int x ) //define general constructor
{
  SetSlug(x);
  dataPresent=0;
} //end constructor def

SlugCalc::~SlugCalc() //define general destructor
{ } //end destructor def

float SlugCalc::GetError()
{
  float sum=0;
  for (int i=0; i<(int)err.size(); i++ ) 
    { sum += 1/pow(err[i],2); }

  return sqrt(1/sum);
} //end GetError def

float SlugCalc::GetAvg()
{
  float sum=0;
  for (int i=0; i<(int)val.size(); i++)
    { sum += val[i]/pow(err[i],2); }

  return sum*pow(GetError(),2);
} //end GetAvg def

void SlugCalc::Push( int x, float value, float error )
{
  runlet.push_back(x);
  val.push_back(value);
  err.push_back(error);
  dataPresent=1;
} //end push def


void SlugCalc::Push( int slugNumber, int x, float value, float error )
{
  if ( CheckSlug(slugNumber) != 1 ) return;

  SetSlug(slugNumber);
  runlet.push_back(x);
  val.push_back(value);
  err.push_back(error);
  dataPresent=1;
} //end push def

void SlugCalc::SetSlug( int x )
{
  slug=x;
} //end SetSlug def

int SlugCalc::GetSlug( )
{
  return slug;
} //end GetSlug def

//CheckIfFilled will check to see if the vectors are properly implemented
bool SlugCalc::CheckIfFilled( )
{
  if (val.size() != err.size() ) 
  {
    std::cout <<"There is a problem - vector val size != vector err size!\n";
      return 0;
  } else if (val.size() != runlet.size() ) {
    std::cout <<"There is a problem - vector val size != vector runlet size!\n";
      return 0;
  } else if (val.size()==0) {
    std::cout <<"There is no data in your vectors!\n";
    return 0;
  } else {dataPresent=1; return 1;}
} //end CheckIfFilled def

void SlugCalc::PrintError( )
{
  if (dataPresent==0) {CheckIfFilled();}
  std::cout <<"The errors used to calculate the average are:\n";
  for (int i=0;i<(int)err.size(); i++)
  {
    std::cout <<" \t" <<err[i];
    if (i%5) {std::cout <<std::endl;}
  } //end for loop
} //end PrintError function

void SlugCalc::PrintVal( )
{
  if (dataPresent==0) {CheckIfFilled();}
  std::cout <<"The values used to calculate the average are:\n";
  for (int i=0;i<(int)val.size(); i++)
  {
    std::cout <<" \t" <<val[i];
    if (i%5) {std::cout <<std::endl;}
  } //end for loop
} //end PrintError function

void SlugCalc::PrintRunlet( )
{
  if (dataPresent==0) {CheckIfFilled();}
  std::cout <<"The runlets used to calculate the average are:\n";
  for (int i=0;i<(int)runlet.size(); i++)
  {
    std::cout <<" \t" <<runlet[i];
    if (i%5) {std::cout <<std::endl;}
  } //end for loop
} //end PrintError function

bool SlugCalc::CheckSlug( int testSlug )
{
  if ( testSlug == slug ) { return 1; }
  else {

    std::cout <<"There is a problem. Slug " <<testSlug 
      <<" is trying to put data into a SlugCalc object "
     <<" for slug #" <<slug <<std::endl;
    return 0;
  }

} //end CheckSlug function








