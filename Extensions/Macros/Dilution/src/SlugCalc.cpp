/* Implementation file for slug calc class
 *
 * SlugCalcs are objects of grouped data, called "slugs".
 * For instance, let's say you have informations from two
 * distinct time periods: time A, and time B. You could
 * separate all of information from A and B into two SlugCalc objects,
 * slugA, slugB. These objects would keep a running tally of information
 * and properly compute the weighted mean and error of the data sets.
 *
 * SlugCalc objects just take and process the data. A "cousin" class,
 * SlugCalcVec makes vectors of these objects for easier manipulation
 * of multi-slug data sets.
 *
 * Josh Magee
 * joshuamagee@gmail.com
 * February 14, 2013
 * slugcalc.cpp
 */


#include<iostream>
#include<math.h>
#include<vector>
#include<stdlib.h>

#include"SlugCalc.h"

using namespace std;

SlugCalc::SlugCalc( ) //define general constructor
{
  dataPresent=0;
  avg = -1;
  error = -1;
  chi = -1;
  dof = -1;
} //end constructor def


SlugCalc::SlugCalc(float x ) //define general constructor
{
  SetSlug(x);
  dataPresent=0;
  avg = -1;
  error = -1;
  chi = -1;
  dof = -1;
} //end constructor def

SlugCalc::~SlugCalc() //define general destructor
{ } //end destructor def

void SlugCalc::CalcError()
{
  float sum=0;
  for (int i=0; i<(int)err.size(); i++ ) 
    { sum += 1/pow(err[i],2); }
  error = sqrt(1/sum);
} //end GetError def

void SlugCalc::CalcAvg()
{
  float sum=0;
  for (int i=0; i<(int)val.size(); i++)
    { sum += val[i]/pow(err[i],2); }

  avg = sum*pow(GetError(),2);
} //end GetAvg def

void SlugCalc::CalcChi()
{
  //to properly calculate chi^2, we need to make sure
  //we have a properly calculated weighted average
  CalcAvg();

  //reset chi
  chi = 0;

  std::vector<float>::iterator itval;
  std::vector<float>::iterator iterr;

  //chi is a sum of: the difference between an individual
  //value and the expected, or avg value, /error, squared
  for(itval = val.begin(), iterr = err.begin();
      itval != val.end() && iterr != err.end();
      ++itval, ++iterr) {
    chi += pow( ((*itval-avg) / *iterr),2);
  }

  //number of degrees of freedom is number of values -1
  dof = val.size()-1;

  return;
}


float SlugCalc::GetError()
{
  CalcError();
  return error;
} //end GetError def

float SlugCalc::GetAvg()
{
  CalcAvg();
  return avg;
} //end GetAvg def

//get chi^2
float SlugCalc::GetChi()
{
  CalcChi();
  return chi;
}

//return number of degrees of freedom
int SlugCalc::GetDof()
{
  if (dof<=0) {
    std::cout <<"You need to put data into your SlugCalc object "
      <<"before you can return number of Dof." <<std::endl;
    return 1;
  }

  return dof;
}

float SlugCalc::GetReducedChi()
{
  CalcChi();
  return  chi/float(dof);
}

void SlugCalc::Push( float x, float value, float error )
{
  runlet.push_back(x);
  val.push_back(value);
  err.push_back(error);
  dataPresent=1;
} //end push def


void SlugCalc::Push( float slugNumber, float x, float value, float error )
{
  if ( CheckSlug(slugNumber) != 1 ) return;

  SetSlug(slugNumber);
  runlet.push_back(x);
  val.push_back(value);
  err.push_back(error);
  dataPresent=1;
} //end push def

void SlugCalc::SetSlug( float x )
{
  slug=x;
} //end SetSlug def

float SlugCalc::GetSlug( )
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








