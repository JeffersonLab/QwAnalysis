/*
 * MarryVecs.cpp
 * This class combines two vectors through "marriage".
 * It accepts values for both vectors (the bride/groom)
 * and then combines them appropriately.
 * This class does not collect the data directly, just processes it.
 *
 * Written by Josh Magee
 * magee@jlab.org
 * November 19, 2013
 */

#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <cmath>

#include <TMath.h>

#include "MarryVecs.h"

using namespace std;

//default constructor
MarryVecs::MarryVecs(void) {
  resetAll();
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
  raw_grooms.push_back(entry);
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

void MarryVecs::add_matched_entry(double groom_entry, double bride_entry) {
    matched_grooms.push_back(groom_entry);
    matched_brides.push_back(bride_entry);
    add_differences_entry(groom_entry, bride_entry);
    marriage_status = true;
}

void MarryVecs::add_matched_peak_entry(double entry) {
  matched_peaks.push_back(entry);
}

void MarryVecs::add_matched_accidentals_entry(double entry) {
    matched_accidentals.push_back(entry);
}

void MarryVecs::add_differences_entry(double entry1, double entry2) {
  if (entry1==dummy_entry || entry2==dummy_entry)
    { return; }
  differences.push_back(entry1-entry2);
}


//return vector-reference to vector of final matches
vector<double> * MarryVecs::get_final_marriage_brides(void) {
  return &matched_brides;
}

//return vector-reference to vector of final matches
vector<double> * MarryVecs::get_final_marriage_grooms(void) {
  return &matched_grooms;
}

//return vector-reference to vector of differences
vector<double> *MarryVecs::get_differences(void) {
  return &differences;
}

vector<double> *MarryVecs::get_matched_peak(void) {
  return &matched_peaks;
}

//return vector-reference to vector of accidentals
vector<double> *MarryVecs::get_accidentals(void) {
  return &matched_accidentals;
}

//return vector of raw groom data
vector<double> *MarryVecs::get_raw_grooms(void) {
  return &raw_grooms;
}

//return vector-reference to vector holding tube hit time differences
vector<double> *MarryVecs::get_groom_hit_timediff(void) {
  return &groom_hit_timediff;
}

//return vector-reference to vector holding tube hit time differences
vector<double> *MarryVecs::get_bride_hit_timediff(void) {
  return &bride_hit_timediff;
}

//clear all vectors but married ones
void MarryVecs::reset(void) {
  for (int j=0; j<max_size; j++) {
    groom[j] = dummy_entry;
    bride[j] = dummy_entry;
  }
  num_entries_groom=0;
  num_entries_bride=0;
}

//clear all vectors
void MarryVecs::resetAll(void) {
  reset();
  matched_grooms.clear();
  matched_brides.clear();
  differences.clear();
  marriage_status=false;
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
    if (groom[j]==dummy_entry && bride[j]==dummy_entry) continue;
    printf("%i \t%f \t%f\n",j,groom[j],bride[j]);
  }
  print_marriage_vectors();
}

void MarryVecs::print_marriage_vectors(void) {
  if ( marriage_status == false ) {
    printf("No matched pairs yet!\n");
  } else {
    printf("Matched pairs: \n");
    printf("Hit \tGrooms \t\tBrides \n");
    for (unsigned int j=0; j<matched_grooms.size(); j++){
      if (matched_grooms[j]!=dummy_entry && matched_brides[j]!=dummy_entry)
        printf(" %i \t%f \t%f\n",j,matched_grooms[j],matched_brides[j]);
    }
    printf("Differences of the two vectors: \n");
    printf("Hit \tGroom - Bride \n");
      for (unsigned int j=0; j<differences.size(); j++) {
        printf(" %i \t%f \n",j,differences[j]);
      }
  }
}

/* To determine time differences between hits in a single tube,
 * one must be careful. We have 3 kinds of entries:
 *    0:  dummy entries (ie, no hit recorded)
 *    1:  accidentals (hit recorded, but not married)
 *    2:  true hit (married with pulse from other tube)
 *  These arrays then contain information for a given tube and hit.
 *  Example:
 *    groom_hit_matches[5] = { 1, 2, 2, 1, 0};
 *  This says hits 0 and 3 are accidentals, hits 1 and 2
 *  are true hits, and hit 5 was filled with a dummy entry
 *  because no signal was present.
 */
void MarryVecs::determine_hit_type(double groom_entry, double bride_entry, int p) {
      //if groom==dummy && bride==dummy, both empty hits
      //if groom || bride ==dummy, one is an accidental, the other
      //  an empty hit
      //if neither groom nor bride are dummy_entry, they must be
      //a proper marriage.
      if (groom_entry==dummy_entry && bride_entry==dummy_entry) {
        groom_hit_matches[p] = 0;
        bride_hit_matches[p] = 0;
      } else if (groom_entry!=dummy_entry && bride_entry==dummy_entry) {
        groom_hit_matches[p] = 1;
        bride_hit_matches[p] = 0;
      } else if (groom_entry==dummy_entry && bride_entry!=dummy_entry) {
        groom_hit_matches[p] = 0;
        bride_hit_matches[p] = 1;
      } else {
        if (fabs(groom_entry-bride_entry)>max_difference) {
          groom_hit_matches[p] = 1;
          bride_hit_matches[p] = 1;
        } else {
          groom_hit_matches[p] = 2;
          bride_hit_matches[p] = 2;
        }
      }
  return;
}

void MarryVecs::calculate_hit_time_differences(void) {
  for(int j=0; j<max_size-1; j++) {
    /*Take groom channel as example. We want to calculate
     *            groom[j+1] - groom[j]
     * This gives us the time differences between pulses/signals.
     */

    //We don't want to calculate the time difference from dummy_entry
    //if(groom[j]!=dummy_entry && groom[j+1]!=dummy_entry)
    if(groom_hit_matches[j]!=0 && groom_hit_matches[j+1]!=0)
      groom_hit_timediff.push_back( groom[j+1] - groom[j] );

    if(bride_hit_matches[j]!=0 && bride_hit_matches[j+1]!=0)
      if( bride[j+1] - bride[j] != 0 )
      bride_hit_timediff.push_back( bride[j+1] - bride[j] );
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

  bool local_debug=false;

  //initialize all arrays
  for (int i=0; i<max_size; i++) {
    for (int j=0; j<max_size; j++) {
      pst[i][j]=dummy_entry;
      nst[i][j]=dummy_entry;

      p_preference_list[i][j]=0;
      n_preference_list[i][j]=0;
    }
  }

  //p_prefernce_list is the "positive" list, built from perspective
  //of groom
  //the smaller the difference, the smaller pst is, and therefore _higher_
  //the preference
  //p_preference_list must be built according to the positive channel
  for(int p=0; p<max_size; p++) {
    for(int n=0; n<max_size; n++) {
      pst[p][n] = fabs(groom[p]-bride[n]);
      //if either bride/groom is zero, fill will unreasonable number
      if (pst[p][n] == fabs(groom[p]) || pst[p][n] == fabs(bride[n]) )
        pst[p][n] = dummy_entry;
      if (local_debug) {
        if (p==0) {
          if (n==0) { printf(" p \tn \tgroom-bride=delta \tpst[p][n]\n"); }
          printf(" %i \t%i \t%f-%f=%f \t\t%f\n",p,n,groom[p],bride[n],fabs(groom[p]-bride[n]),pst[p][n]);
        }
      }
    }
    //sort p_preference_list if lowest st at the first place [p][0]
    TMath::Sort(max_size,pst[p],p_preference_list[p],false);
  }

  //n_preference_list must be built according to the negative channel
  for(int n=0; n<max_size; n++) {
    for(int p=0; p<max_size; p++) {
      nst[n][p] = fabs(groom[p]-bride[n]);

      //if either bride/groom is zero, fill will unreasonable number
      if ( nst[n][p] == fabs(groom[p]) || nst[n][p] == fabs(bride[n]) )
        nst[n][p] = dummy_entry;
    }

    //sort n_preference_list in lowest st at the first place [n][0]
    TMath::Sort(max_size,nst[n],n_preference_list[n],false);
  }

  // rank of negative channels
  // n_rank[negative channel][rank]
  // we don't need to have rank of positive channels,
  // because positive proposes to negative
  int n_rank[max_size][max_size];
  for (int n=0; n<max_size; n++) {
    for (int rank=0; rank<max_size; rank++) {
      n_rank[n][n_preference_list[n][rank]]=rank;
    }
  }

  //print lots of output if debugging
  if (local_debug) {
    std::cout <<"Positive preference list:\n";
    for(int p=0; p<max_size; p++) {
      for(int n=0; n<max_size; n++) {
        printf("%i/%i %f-%f = %f\n",p,n,groom[p],bride[n],pst[p][n]);
      }
    }
    std::cout <<"Negative preference list:\n";
    for(int n=0; n<max_size; n++) {
      for(int p=0; p<max_size; p++) {
        printf("%i/%i %f-%f = %f\n",n,p,groom[p],bride[n],nst[n][p]);
      }
    }

    //print sorted preference list
    for(int p=0; p<max_size; p++) {
      for(int n=0; n<max_size; n++) {
        if (p==0 && n==0) {
          std::cout <<"Positive preference list (sorted):\n";
          std::cout <<"p groom[p] \tn pref[p][n]\n";
        }
        printf("%i %f \t %i \t%i\n",p,groom[p],n,p_preference_list[p][n]);
      }
    }

    for(int n=0; n<max_size; n++) {
      for(int p=0; p<max_size; p++) {
        if (n==0 && p==0) {
          std::cout <<"Negative preference list (sorted):\n";
          std::cout <<"n groom[n] \tp pref[n][p]\n";
        }
        printf("%i %f \t %i \t%i\n",n,groom[n],p,n_preference_list[n][p]);
      }
    }

    printf("n \tpreference  \trank\n");
    for (int n=0; n<max_size; n++) {
      for (int rank=0; rank<max_size; rank++) {
        printf("%i \t%i = \t%i\n",n,n_preference_list[n][rank],rank);
      }
    }
  }

  //p seems to be man, and n seems to be woman
  int fiancee[max_size];   //fiance[p] = n, n with viewpoint of p
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
        if (local_debug) {
          printf("%f engaged to %f\n",groom[p],bride[n]);
        }
      } else {
        //else n is currently engaged to positive suitor[n]
        //if n prefers p_old(suitor[n]) to p_new
        //nothing happens. This loop iteration starts again
        //with decrease_rank increased
        if (n_rank[n][suitor[n]] < n_rank[n][p]) {
          //nothing happens
          if (local_debug) {
            printf("%f proposed to %f, rejected\n",groom[p],bride[n]);
          }
        } else {
          //prefers f to f_old
          if (local_debug) {
            printf("%f engaged to %f. %f is now single.\n",groom[p],bride[n],groom[suitor[n]]);
          }
          fiancee[suitor[n]]=-1; //f_old becomes free
          suitor[n]=p;  //(f,p) become engaged
          fiancee[p]=n;
          if (local_debug) {
          }
        }
      } //end stacked if-else
    } //fiancee[p] is engaged
  } //everything is engaged (while getting_engaged==7)

//  print_vectors();
  for(int p=0; p<max_size; p++) {
    //this is a temporary kludge to store hit times
    determine_hit_type(groom[p],bride[fiancee[p]],p);

    //if our match is two dummy entries, we don't want to keep it
    //if however our match has ONE dummy entry, we still have a valid
    //accidental to analyze
    if (groom_hit_matches[p]==0 && bride_hit_matches[p]==0)
      continue;

    if (fabs(groom[p]-bride[fiancee[p]])>max_difference)
    { continue; }

    //if our match is GOOD (ie, doesn't have a dummy entry) //we should add it to our matched vector
//    add_matched_entry(groom[p],bride[fiancee[p]]);
   // printf("Adding to matched: groom: %f \tbride: %f\n",groom[p],bride[fiancee[p]]);
    //if our entries are within the F1TDC bounds selected,
    //put them into matched vector
    if (groom[p]!=0 && bride[fiancee[p]]!=0) {
      double tsentry = groom[p];
      double mdentry = bride[fiancee[p]];
      if (tsentry != 0 && tsentry>-186 && tsentry<-178 &&
//      if (tsentry != 0 && tsentry>-180 && tsentry<-160 &&
          mdentry!=0) {
         add_matched_entry(tsentry,mdentry);
         if (mdentry>=-220 && mdentry<=-150)
           add_matched_peak_entry(mdentry);
         if (mdentry != 0 && mdentry>=-149 && mdentry<=-79)
           add_matched_accidentals_entry(mdentry);
      }
    }
  }
  //now calculate the actual hit time differences for a given tube
  calculate_hit_time_differences();
}//end MarryVecs::marry_arrays

bool MarryVecs::get_marriage_status(void) {
  return marriage_status;
}

