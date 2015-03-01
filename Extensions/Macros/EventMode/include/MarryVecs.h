/*
 * MarryVecs.h
 * This class combines two vectors through "marriage".
 * It accepts values for both vectors (the bride/groom)
 * and then combines them appropriately.
 * This class does not collect the data directly, just processes it.
 *
 * Written by Josh Magee
 * magee@jlab.org
 * November 19, 2013
 */

#ifndef _MARRYVECS_H_
#define _MARRYVECS_H_
#include <vector>
#include <cmath>

#include <TMath.h>
#include <TTree.h>

using std::vector;

class MarryVecs {
  public:
    MarryVecs(); //default constructor
    ~MarryVecs(); //defailt destructor

    /* methods to add new data to private arrays
     * The marriage algorithm requires two data sets to match.
     * groom's propose to brides. For Qweak, trigger scintillator
     * is typically the groom, as the TS is the event trigger.
     *
     * Vectors (defined below)
     * groom  -   vector of ALL groom entries for a given EVENT
     * bride  -   vector of ALL bride entries for a given EVENT
     * matched grooms/brides  - ONLY married grooms/brides (ALL events)
     * matched differences    - difference between a groom/bride value
     *                          for a given marriage (ie, groom-bride)
     * matched peak           - "prompt peak" of spectra of matches
     * matched accidentals    - accidental count from a married spectra
     *
     * Methods
     * matched_entry() adds entries to groom/bride/difference vectors
     */
    void add_entry_groom(double);
    void add_entry_bride(double);
    void add_entry(double, double); //add data to both arrays
    void add_matched_entry(double,double);
    void add_matched_peak_entry(double);
    void add_matched_accidentals_entry(double);
    void add_differences_entry(double,double);
    void calculate_channel_time_differences(void);  //calculate time diff for
                                                    //only the bride OR groom
                                                    //channel. Discussed below.
    void find_channel_time_diffs(void);

    //methods to retrieve specific entries from private arrays
    double get_groom_entry(int);
    double get_bride_entry(int);

    vector<double> * get_final_marriage_brides(void);  //get final brides
    vector<double> * get_final_marriage_grooms(void);  //get finalgrooms
    vector<double> * get_matched_peak(void);     //get vector of coincidence peak
    vector<double> * get_differences(void);     //get vector of time differences
    vector<double> * get_accidentals(void);     //get vector of time differences
    vector<double> * get_raw_grooms(void); //get vector of initial grooms

    void marry_arrays(void);  //marry bride/groom arrays
    void reset(void);         //resets all non-married vectors
    void resetAll(void);      //resets ALL ectors
    void check_size(void);
    void print_vectors(void);         //prints ALL vectors in object
    void print_marriage_vectors(void);//prints only properly married hits

    bool get_marriage_status(void);   //are there any proper marriages?

    void determine_hit_type(double, double, int); //method to determine type
                                      //of hits in an array
    void calculate_hit_time_differences(void);
    vector<double> * get_groom_hit_timediff(void);
    vector<double> * get_bride_hit_timediff(void);

  private:
    /* For each event all possible hits are sorted into groom/bride
     * arrays. These hits are then married, and if appropriate, are
     * stored in the vectors of a married brides/grooms.
     *
     * After each event is processed, the groom/bride ARRAYS are
     * reset, but the vectors typically remain. The ARRAYS process individual
     * hits; the VECTORS hold good married data.
     */
    int num_entries_groom;
    int num_entries_bride;
    static const int max_size = 7;  //number if possible hits per event
    static const int max_difference = 1000000;  //ABS(TS-MD)<max_difference
    static const double dummy_entry = 1000000;  //empty array values are filled
                                                //with this ridiculous value
    bool marriage_status;     //true = there is AT LEAST ONE proper marriage

    double groom[max_size];   //"male" or "positive" ARRAY
    double bride[max_size];   //"woman" or "negative" ARRAY
    vector<double> matched_grooms;   //vector of ALL married grooms
    vector<double> matched_brides;   //vector of ALL married brides
    vector<double> matched_peaks;   //vector of marriages UNDER promp peak
    vector<double> matched_accidentals;   //vector of marriages NOT under peak
    vector<double> differences; //vector of time differences (TS-MD)<max_diff
    vector<double> raw_grooms; //vector to hold ALL groom values. Useful to
                              //determine appropriate cuts

    /* These are vectors that store the time differences between
     * successive hits in each detector. For instance,
     *      groom_hit_timediff[1] = groom_hit_2 - groom_hit-1
     * This is opposed to the previously defined differences
     * which is the difference between groom and bride channels:
     *      differences.push_back(groom_entry - bride_entry)
     * Note because we are calculating differences, array size
     * is max_size-1
     */
    vector<double> groom_hit_timediff;
    vector<double> bride_hit_timediff;
    int groom_hit_matches[max_size];
    int bride_hit_matches[max_size];
}; //end of class MarryVecs definitionl
#endif

