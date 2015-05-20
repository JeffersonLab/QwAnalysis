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

#include <stdio.h>
#include <cstdlib>

#include "MarryVecs.h"
#include "DetectorMarriage.h"

using namespace std;

//default constructor
DetectorMarriage::DetectorMarriage(void) {
  marriageVectors.reset();

  tree = NULL;
  haveTree = false;
}

//constructor if given a TTree*
DetectorMarriage::DetectorMarriage(TTree *dummyTree) {
  marriageVectors.reset();

  setTree(dummyTree);
}

//default destructor
DetectorMarriage::~DetectorMarriage(void) {
}

void DetectorMarriage::setTree(TTree *dummyTree) {
  tree = dummyTree;
  haveTree=true;
}

void DetectorMarriage::resetMarriages(void) {
  marriageVectors.reset();
}

void DetectorMarriage::setTsPlane(int plane) {
  tsPlane=plane;
}

void DetectorMarriage::setMdPlane(int plane) {
  mdPlane=plane;
}

void DetectorMarriage::setEventRange(long low, long high) {
  eventLow=low;
  eventHigh=high;
}

int DetectorMarriage::getTsPlane(void) {
  return tsPlane;
}

int DetectorMarriage::getMdPlane(void) {
  return mdPlane;
}

void DetectorMarriage::processEvents(void) {
  if (haveTree==false) {
    printf("You have not specified a tree yet...\n");
    return;
  }
  TBranch *branch = tree->GetBranch("events");
  QwEvent *qwevent =0;

  long n_entries = tree->GetEntries();
  long localEntry=0;

  branch->SetAddress(&qwevent);

  for (long j=0; j<n_entries; j++) {
    //process events only within desired event number range
    if (!(j>=eventLow && j<=eventHigh)) {continue;}
    localEntry = tree->LoadTree(j);
    tree->GetEntry(localEntry);

    if(j % 10000 == 0) {
      printf(" Total events %ld. events process so far: %ld\n", n_entries, j);
    }

    marriageVectors.reset();
    int nhit = qwevent->GetNumberOfHits();

    for (int k=0; k<nhit; k++) {
      const QwHit* qwhit = qwevent->GetHit(k);
      const int fregion  = qwhit  -> GetRegion();
      const int felement = qwhit  -> GetElement();
      const int fplane   = qwhit  -> GetPlane();

      //region==4 is trigger scintilator
      //Since TS proposes to MD, TS is the groom
      if (fregion==4 && fplane==tsPlane && felement==3)  {
        //groom[hitnumber] = qwhit -> GetTimeNs();
        marriageVectors.add_entry_groom(qwhit->GetTimeNs());
      }
      //region==5 is main detector
      //Main detector is the bride
      if (fregion==5 && fplane==mdPlane && felement==3) {
        //bride[hitnumber] = qwhit -> GetTimeNs();
        marriageVectors.add_entry_bride(qwhit->GetTimeNs());
      }
    } //end loop through individual hits

    double tsentry = marriageVectors.get_groom_entry(0);
    double mdentry = marriageVectors.get_bride_entry(0);
//    if (tsentry != 0 && tsentry>-186 && tsentry<-178 &&
//    if (tsentry != 0 && tsentry>-180 && tsentry<-160 &&
    if (tsentry != 0 && tsentry>-200 && tsentry<-150 &&
        mdentry != 0 )//&& mdentry>-210 && mdentry<-150 )
    {
      first_marriage.push_back(mdentry);
    }
    //now that the groom and bride arrays have been filled, we can marry them
    marriageVectors.marry_arrays();
  } //end loop through events
} //end processEvents() declaration

vector<double>* DetectorMarriage::getDetectorMarriage(void) {
  if (marriageVectors.get_marriage_status()==false) {
    printf("There is no data in this marriage vector. Make sure you process the file first!\n");
  }
  return marriageVectors.get_final_marriage_brides();
}

vector<double>* DetectorMarriage::getDetectorMarriage_unused(void) {
  if (marriageVectors.get_marriage_status()==false) {
    printf("There is no data in this marriage vector. Make sure you process the file first!\n");
  }
  return marriageVectors.get_final_marriage_grooms();
}

vector<double>* DetectorMarriage::getDetectorMarriageDifferences(void) {
  if (marriageVectors.get_marriage_status()==false) {
    printf("There is no data in this differences vector. Make sure you process the file first!\n");
  }
  return marriageVectors.get_differences();
}

vector<double>* DetectorMarriage::getDetectorMarriagePeak(void) {
  return marriageVectors.get_matched_peak();
}

vector<double>* DetectorMarriage::getDetectorMarriageAccidentals(void) {
  if (marriageVectors.get_marriage_status()==false) {
    printf("There is no data in this differences vector. Make sure you process the file first!\n");
  }
  return marriageVectors.get_accidentals();
}

vector<double>* DetectorMarriage::getFirstMarriage(void) {
//  marriageVectors.print_vectors();
  return &first_marriage;
}

vector<double>* DetectorMarriage::get_groom_hit_time_diff(void) {
  return marriageVectors.get_groom_hit_timediff();
}

vector<double>* DetectorMarriage::get_bride_hit_time_diff(void) {
  return marriageVectors.get_bride_hit_timediff();
}

//this will return all the groom data, both married and un-married
vector<double>* DetectorMarriage::get_raw_grooms(void) {
  return marriageVectors.get_raw_grooms();
}





