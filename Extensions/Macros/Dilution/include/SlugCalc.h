/* Header file for slug calc class
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
 * slugcalc.h
 */

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
     SlugCalc(float); //constructor
    ~SlugCalc(); //default destructor
    void CalcError(); //calculate error
    void CalcAvg();   //calculate weighted average of elements
    void CalcChi();   //calculate chi^2 of data
    void Push(float, float, float); //function to add data
        //format: x-value,value,error
    void Push(float, float, float, float); //function to add data
        //format: slug,x-value,value,error
    void SetSlug(float);  //set which slug to process
    float GetSlug();      //return which slug to process
    float GetError(); //getter for Error
    float GetAvg();   //getter for Average
    float GetChi();   //getter for chi^2
    int   GetDof();   //getter for number of degrees of freedom
    float GetReducedChi();  //gives GetChi()/GetDoF();
    bool CheckSlug(int); //checks if new slug # matches old #
    bool CheckIfFilled();  //this will check to see if vectors are dataPresent
    void PrintError();
    void PrintVal();
    void PrintRunlet();

  private:
    vector <float> val;    //value, ie f(x)
    vector <float> err;    //error bar
    vector <float> runlet; //runlet
    float slug;     //slug
    float avg;
    float error;
    float chi;
    int dof;
    bool dataPresent;
}; //end class definition

#endif


