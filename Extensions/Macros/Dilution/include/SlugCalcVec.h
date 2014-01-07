//Header file for slug vec class
//J. Magee
//February 14, 2013
//slugvec.h

#ifndef __SlugCalcVecDef___
#define __SlugCalcVecDef___
#include <iostream>
#include<math.h>
#include<vector>
#include<stdlib.h>
#include<map>

#include"SlugCalc.h"

using namespace std;

class SlugCalcVec   //creates a vector of slugs, 
{
  public:
    SlugCalcVec();  //default constructor
    ~SlugCalcVec(); //default destructor
    void push_back(int,int,float,float); //slug,run,value,error
    int size();
    const float* GetErrorArray();
    const float* GetAvgArray();
    const float* GetSlugsArray();
    void PrintSlugVec();

  private:
    map <int,int> index;
    float *fVal;
    float *fErr;
    float *fSlug;
    vector <SlugCalc> slugvec;

}; //end class definition

#endif
