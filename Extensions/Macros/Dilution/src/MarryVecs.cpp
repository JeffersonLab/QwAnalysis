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
#include <cmath>

#include <TMath.h>

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

void MarryVecs::add_matched_entry(double entry) {
  if (matched.size()<max_size) {
    matched.push_back(entry);
  } else {
    printf("Matched vector has too many entries!\n");
  }
}

void MarryVecs::add_differences_entry(double entry1, double entry2) {
  if (differences.size()<max_size) {
    differences.push_back(entry1-entry2);
  } else {
    printf("Differences vector has too many entries!\n");
  }
}
//return vector-reference to vector of final matches
vector<double> * MarryVecs::get_final_marriage(void) {
  return &matched;
}

//return vector-reference to vector of differences
vector<double> *MarryVecs::get_differences(void) {
  return &differences;
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
  differences.clear();
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


void MarryVecs::marry_arrays(void) {
  /*This code essentially taken verbatim from QwSoftwareMeantime,
   *a class written by Han Lee. I have tried to clean up and simplify
   *the code where possible, but it's an ongoing project...
   */

  //I'm not going to lie, I have no idea what "st" stands for...
  //p seems to be man, n seems to be woman (groom/bride respectively)
  double pst[max_size][max_size]; //[positive][negative]
  double nst[max_size][max_size]; //[negative][positive]

  int p_preference_list[max_size][max_size];
  int n_preference_list[max_size][max_size];

  //initialize all arrays
  for (int i=0; i<max_size; i++) {
    for (int j=0; j<max_size; j++) {
      pst[i][j]=dummy_entry;
      nst[i][j]=dummy_entry;

      p_preference_list[i][j]=0;
      n_preference_list[i][j]=0;
    }
  }

  //p_preference_list must be built accordipg to the negative channel
  for(int p=0; p<max_size; p++) {
    for(int n=0; n<max_size; n++) {
      pst[p][n] = fabs(groom[n]-bride[p]);

      //if either bride/groom is zero, fill will unreasonable number
      if (pst[p][n] == fabs(groom[n]) || pst[p][n] == fabs(bride[p]) )
        pst[p][n] = dummy_entry;
    }

    //sort p_preferepce_list if lowest st at the first place [p][0]
    TMath::Sort(max_size,pst[p],p_preference_list[p],false);
  }

  //n_preference_list must be built according to the negative channel
  for(int n=0; n<max_size; n++) {
    for(int p=0; p<max_size; p++) {
      nst[n][p] = fabs(groom[p]-bride[n]);

      //if either bride/groom is zero, fill will unreasonable number
      if (nst[n][p] == fabs(groom[p]) || nst[n][p] == fabs(bride[n]) )
        nst[n][p] = dummy_entry;
    }

    //sort n_preference_list in lowest st at the first place [n][0]
    TMath::Sort(max_size,nst[n],n_preference_list[n],false);
  }

  // rank of negative channels
  // n_rank[negative channel][rank]
  // we don't need to have rank of positive channels,
  // because positive propose to negative

  int n_rank[max_size][max_size];
  for (int n=0; n<max_size; n++) {
    for (int rank=0; rank<max_size; rank++) {
      n_rank[n][n_preference_list[n][rank]]=rank;
    }
  }

  //p seems to be man, and n seems to be woman
  int fiancee[max_size];   //fiance[[p] = n, n with viewpoint of p
  int suitor[max_size];    //suitor[n]  = p, p with viewpoint of n
  int decrease_rank[max_size];
  int getting_engaged=0;

  //Initially all p's and n's are free
  //fiancee[0]=-1 means p0 is solo
  //suitor[0]=-1 means n0 is solo

  for(int j=0; j<max_size; j++) {
    decrease_rank[j]=0;
    fiancee[j]=-1;
    suitor[j]=-1;
  }

  int p=0;
  int n=0;
  while (getting_engaged < max_size) {
    //this is a terrible way to pick a free p
    //need to correct this later
    for(p=0; fiancee[p]!=-1; p++) {
      if (p>=max_size) break;
    }

    while (fiancee[p]==-1) {
      //let n be the highest-ranked negative in the preference list of positive
      //In the preference list, the first place is the highest-ranked negative number

      //decrease_rank[p] = 0 means the highest-ranked negative number
      n = p_preference_list[p][decrease_rank[p]];

      //decrease the rank number, which means preparing the next highest-ranked negative number
      //if highest-ranked negative, will reject the proposal of this positive p
      decrease_rank[p]++;

      //if n is free then
      if (suitor[n]==-1) {
        suitor[n]=p;
        fiancee[p]=n;
        getting_engaged++;
      } else {
        //else n is currently engaged to positive suitor[n]
        //if n prefers p_old(suitor[n]) to p_new
        //greater rank means lowest-ranked pixel
        if (n_rank[n][suitor[n]] < n_rank[n][p]) {
          //nothing happens
        } else {
          //prefers f to f_old
          fiancee[suitor[n]]=-1; //f_old becomes free
          suitor[n]=p;  //(f,p) become engaged
          fiancee[p]=n;
        }
      } //end stacked if-else
    } //fiancee[p] is engaged
  } //everything is engaged (while getting_engaged==7)

  for(int p=0; p<max_size; p++) {
    if (groom[p]!=0 && bride[fiancee[p]]!=0) {
      //add_matched_entry(groom[p],bride[fiancee[p]]);
      add_matched_entry(groom[p]);
      add_differences_entry(groom[p],bride[fiancee[p]]);
    }
  }
}//end MarryVecs::marry_arrays






