/*
 * MarryVecs.h
 * Patches together f1adc data by time, "marrying" them.
 *
 * Written by Josh Magee
 * magee@jlab.org
 * November 19, 2013
 */

#ifndef _MARRYVECS_H_
#define _MARRYVECS_H_
#include <vector>
#include <TTree.h>
using std::vector;

class MarryVecs {
  public:
    MarryVecs(); //default constructor
    ~MarryVecs(); //defailt destructor

    //methods to add new data to private arrays
    void add_entry_groom(double);
    void add_entry_bride(double);
    void add_entry(double, double); //add data to both arrays

    //methods to retrieve specific entrys from private arrays
    double get_groom_entry(int);
    double get_bride_entry(int);

    vector<double> * get_final_marriage(void);  //get final marriages
    void marry_arrays(void);  //marry bride/groom arrays

    void reset(void);
    void check_size(void);
    void print_vectors(void);

  private:
    int num_entries_groom;
    int num_entries_bride;
    static const int max_size = 7;
    static const double dummy_entry = -1e7;
    bool isMarried;

    double groom[max_size];   //"male" or "positive"
    double bride[max_size];   //"woman" or "negative"
    vector<double> matched; //vector of matches (marriages)
}; //end of class MarryVecs definition

#endif

