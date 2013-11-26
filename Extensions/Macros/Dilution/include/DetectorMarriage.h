/*
 * f1marriage.h
 * Patches together f1adc data by time, "marrying" them.
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

    void setTree(TTree *);
    void resetMarriages(void);

    void setTsPlane(int);
    void setMdPlane(int);
    void setEventRange(long, long);

    int getTsPlane(void);
    int getMdPlane(void);

    void processEvents(void);

  private:
    TTree *tree;
    int tsPlane;
    int mdPlane;
    long eventLow;    //event number minimum (ie, process only events> eventLow)
    long eventHigh;   //event number maximum (ie, process only events< eventHigh)
    MarryVecs marriageVectors;  //marriage class for final marriage of MD to TS

    bool haveTree; //variable to determine if DetectorMarriage instance has a tree
};


#endif


