/* Implementation file for SlugCalcVec class
 *
 * SlugCalcVecs are vector objects where elements are SlugCalc objects.
 * SlugCalc objects process data sets into chunks called "slugs." Often,
 * one wants to manipulate multiple slugs for plotting and analysis purposes.
 * The SlugCalcVec class makes this possible and less head-ache prone.
 *
 * Joshua Magee
 * joshuamagee@gmail.com
 * February 14, 2013
 * SlugCalcVec.h
 */

#include <iostream>
#include<math.h>
#include<vector>
#include<stdlib.h>
#include<map>

#include"SlugCalc.h"
#include"SlugCalcVec.h"

using namespace std;

SlugCalcVec::SlugCalcVec() //default constructor
{
  fVal  = 0;
  fErr  = 0;
  fSlug = 0;
} //end constructor

SlugCalcVec::~SlugCalcVec() //default destructor
{   } //end destructor

void SlugCalcVec::push_back(float slug, float runlet, float val, float err)
{
  map<float,int>::iterator iter = index.find(slug);
  if ( iter == index.end() ) {
    index[slug] = slugvec.size();
    slugvec.push_back(SlugCalc(slug));
  }

  int i = index[slug];
  slugvec[i].Push(runlet,val,err);

} //end push_back def

const float* SlugCalcVec::GetErrorArray()
{
  int length = slugvec.size();
  float * old = fErr;
  fErr = new float (length+1);

  for( int i=0; i<length; i++ ) {
    fErr[i]=slugvec[i].GetError();
  } //end for loop
  delete old;

  return fErr;
} //end GetErrorArray function


const float* SlugCalcVec::GetAvgArray()
{
  int length = slugvec.size();
  float * old = fVal;
  fVal = new float (length+1);

  for( int i=0; i<length; i++ ) {
    fVal[i]=slugvec[i].GetAvg();
  } //end for loop
  delete old;

  return fVal;
} //end GetErrorArray function

const float* SlugCalcVec::GetSlugsArray()
{
  int length = slugvec.size();
  float * old = fSlug;
  fSlug = new float (length+1);

  for( int i=0; i<length; i++ ) {
    fSlug[i]=slugvec[i].GetSlug();
  } //end for loop
  delete old;

  return fSlug;
} //end GetErrorArray function

const float *SlugCalcVec::GetChiArray()
{
  int length = slugvec.size();
  float * old = fChi;
  fChi = new float (length+1);

  for( int i=0; i<length; i++ ) {
    fChi[i]=slugvec[i].GetChi();
  } //end for loop
  delete old;

  return fChi;
}

const float *SlugCalcVec::GetReducedChiArray()
{
  int length = slugvec.size();
  float * old = fRedChi;
  fRedChi = new float (length+1);

  for( int i=0; i<length; i++ ) {
    fRedChi[i]=slugvec[i].GetReducedChi();
  } //end for loop
  delete old;

  return fRedChi;
}

const float *SlugCalcVec::GetDofArray()
{
  int length = slugvec.size();
  float * old = fDof;
  fDof = new float (length+1);

  for( int i=0; i<length; i++ ) {
    fDof[i]=slugvec[i].GetDof();
  } //end for loop
  delete old;

  return fDof;
}


void SlugCalcVec::PrintSlugVec()
{
  int length = slugvec.size();
  const float *slugArray  = GetSlugsArray();
  const float *avgArray   = GetAvgArray();
  const float *errorArray = GetErrorArray();

//  std::cout <<"Size: \t" <<length <<std::endl;
  std::cout <<"\tSlug \tAvg \tError\n";
  for (int i=0; i<length; i++) {
    std::cout <<" \t" <<slugArray[i] <<" \t" <<avgArray[i]
      <<" \t" <<errorArray[i] <<std::endl;
  } //end for loop

} //end PrintSlugVec function


int SlugCalcVec::Size()
{
  return (int) slugvec.size();
}

SlugCalc& SlugCalcVec::operator[] (int index)
{
  return slugvec[index];
}









