/* Header file for SlugCalcVec class
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

#ifndef __SlugCalcVecDef___
#define __SlugCalcVecDef___
#include <iostream>
#include<math.h>
#include<vector>
#include<stdlib.h>
#include<map>

#include"SlugCalc.h"

using namespace std;

class SlugCalcVec   //creates a vector of slugs
{
  public:
    SlugCalcVec();  //default constructor
    ~SlugCalcVec(); //default destructor
    SlugCalc& operator[] (int); //define [] operator
    void push_back(float,float,float,float); //slug,run,value,error
    int Size(void);   //return size of slugvec vector
    const float* GetErrorArray();
    const float* GetAvgArray();
    const float* GetSlugsArray();
    const float* GetChiArray();
    const float* GetReducedChiArray();
    const float* GetDofArray();
    void PrintSlugVec();

  private:
    map <float,int> index;
    float *fVal;
    float *fErr;
    float *fSlug;
    float *fChi;
    float *fRedChi;
    float *fDof;
    vector <SlugCalc> slugvec;

}; //end class definition

#endif
