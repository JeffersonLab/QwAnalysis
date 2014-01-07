//Header file for slug calc class
//J. Magee
//February 14, 2013
//slugcalc.h

#ifndef __SlugCalcDef___
#define __SlugCalcDef___
#include<math.h>
#include<vector>
#include<stdlib.h>

using namespace std;

class SlugCalc
{
  public:
     SlugCalc(); //default constructor
     SlugCalc(int); //constructor
    ~SlugCalc(); //default destructor
    float GetError(); //calculate error
    float GetAvg();   //calculate weighted average of elements
    void Push(int, float, float); //function to add data
        //format: x-value,value,error
    void Push(int, int, float, float); //function to add data
        //format: slug,x-value,value,error
    void SetSlug(int);  //set which slug to process
    int GetSlug();      //return which slug to process
    bool CheckSlug(int); //checks if new slug # matches old #
    bool CheckIfFilled();  //this will check to see if vectors are dataPresent
    void PrintError();
    void PrintVal();
    void PrintRunlet();

  private:
    vector <float> val;    //value, ie f(x)
    vector <float> err;    //error bar
    vector <float> runlet; //runlet
    int slug;     //slug
    bool dataPresent;
}; //end class definition

#endif


