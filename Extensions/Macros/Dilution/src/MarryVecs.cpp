/*
 * f1marriage.cpp
 * Patches together f1adc data by time, "marrying" them.
 *
 * Written by Josh Magee
 * magee@jlab.org
 * November 19, 2013
 */

#include <stdio.h>
#include <cstdlib>
#include "MarryVecs.h"

using namespace std;

//default constructor
MarryVecs::MarryVecs(void) {
  num_entries_groom=0;
  num_entries_bride=0;
  isMarried = false;
}

//default destructor
MarryVecs::~MarryVecs(void) {
  reset();
}

//add entries to groom/bride arrays
void MarryVecs::add_entry_groom(double entry) {
  if (num_entries_groom==max_size) {
    printf("There are too many hits in this event...\n");
  }
  groom[num_entries_groom] = entry;
  num_entries_groom++;
}

void MarryVecs::add_entry_bride(double entry) {
  if (num_entries_bride==max_size) {
    printf("There are too many hits in this event...\n");
  }
  bride[num_entries_bride] = entry;
  num_entries_bride++;
}

void MarryVecs::add_entry(double groomEntry, double brideEntry) {
  add_entry_groom(groomEntry);
  add_entry_bride(brideEntry);
}

//return vector-reference to vector of final matches
vector<double> * MarryVecs::get_final_marriage(void) {
  return &matched;
}

//clear all vectors
void MarryVecs::reset(void) {
  for (int j=0; j<max_size; j++) {
    groom[j] = dummy_entry;
    bride[j] = dummy_entry;
  }
  num_entries_groom=0;
  num_entries_bride=0;
  matched.clear();
}

double MarryVecs::get_groom_entry(int entry) {
  if (entry<=max_size)
    return groom[entry];
  else
    return dummy_entry;
}

double MarryVecs::get_bride_entry(int entry) {
  if (entry<=max_size)
    return bride[entry];
  else
    return dummy_entry;
}

void MarryVecs::check_size(void) {
  if (num_entries_groom > max_size) {
    printf("One of your vectors has too much data! (groom)\n");
    exit(1);
  }
  if (num_entries_bride> max_size) {
    printf("One of your vectors has too much data! (bride)\n");
    exit(1);
  }
}

void MarryVecs::print_vectors(void) {
  printf("Index \tGroom \tBride\n");
  for (int j=0; j<max_size; j++) {
    printf("%i \t%f \t%f\n",j,groom[j],bride[j]);
  }

  if (matched.size() == 0) {
    printf("No matched pairs yet!\n");
  } else {
    printf("Matched pairs: \n");
    for (unsigned int j=0; j<matched.size(); j++){
      printf("%i \t%f \n",j,matched[j]);
    }
  }
}


//marriage algorithm still needs to be written
//CHEESY POOFS
void MarryVecs::marry_arrays(void) {
//  printf("In marry_arrays function. This section not implemented!\n");
}


