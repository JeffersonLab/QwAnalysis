/*
 * DetectorMarriage.h
 * This class scans a rootfile directly, separating events
 * based on detector type (TS or MD). To enable a marrying
 * algorithm, it contains a MarryVec object. Instead, it
 * probably should have been some inheritance structure.
 *
 * Written by Josh Magee
 * magee@jlab.org
 * November 19, 2013
 */

#ifndef _DETECTOR_MARRIAGE_H_
#define _DETECTOR_MARRIAGE_H_
#include <vector>
#include <TTree.h>
#include <TBranch.h>

#include "QwHit.h"
#include "QwHitContainer.h"
#include "QwEvent.h"

using std::vector;

class DetectorMarriage {
  public:
    DetectorMarriage(); //default constructor
    DetectorMarriage(TTree *); //constructor if given a TTree*
    ~DetectorMarriage(); //default destructor

    void setTree(TTree *);      //method to set Tree to walk
    void resetMarriages(void);  //reset MarryVec object for next event

    void setTsPlane(int);   //set TS plane (1 or 2)
    void setMdPlane(int);   //set MD plane (1-8)
    void setEventRange(long, long); //set range of event numbers to process

    int getTsPlane(void);   //return TS plane of interest
    int getMdPlane(void);   //return MD plane of interest

    void processEvents(void);   //process all events within range

    vector<double>* getDetectorMarriage(void);
    vector<double>* getDetectorMarriage_unused(void);     //gets other half
    vector<double>* getDetectorMarriageDifferences(void);
    vector<double>* getDetectorMarriagePeak(void);
    vector<double>* getDetectorMarriageAccidentals(void);
    vector<double>* getFirstMarriage(void);
    vector<double>* get_groom_hit_time_diff(void);
    vector<double>* get_bride_hit_time_diff(void);
    vector<double>* get_raw_grooms(void);

  private:
    TTree *tree;
    int tsPlane;
    int mdPlane;
    long eventLow;    //event number minimum (ie, process only events> eventLow)
    long eventHigh;   //event number maximum (ie, process only events< eventHigh)
    vector<double> first_marriage;  //vector of first TS/MD hit in each event
                                    //"lazy man's marriage"
    MarryVecs marriageVectors;  //marriage class for final marriage of MD to TS

    bool haveTree; //variable to determine if DetectorMarriage instance has a tree
};


#endif


