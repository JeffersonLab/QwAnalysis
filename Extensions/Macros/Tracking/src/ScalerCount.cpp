/* * ScalerCount.h
 * This class quickly runs through a given branch and grabs
 * the scaler data for a given detector. At the moment
 * it really only supports Qweak Main Detector scalers, because
 * the array sizes are set by default to 8. This will be a quick
 * change to make to expand it to trigger scintillators.
 *
 * Written by Josh Magee
 * magee@jlab.org
 * May 20, 2014
 */

#include <iostream>
#include <cstdlib>
#include <cmath>

#include <TBranch.h>
#include <TLeaf.h>
#include "ScalerCount.h"

using std::cout;
using std::vector;

//default constructor
ScalerCount::ScalerCount(void) {
  resetAll();
  setEventRange(0, 4000000);
  branch = NULL;
  haveBranch = false;
  haveAverages = false;
  haveCorrections = false;
}

//constructor if given a branch
ScalerCount::ScalerCount(TBranch *dummyBranch) {
  resetAll();
  setEventRange(0, 4000000);
  setBranch(dummyBranch);
  haveBranch = true;
  haveAverages = false;
  haveCorrections = false;
}

ScalerCount::ScalerCount(TBranch *dummyBranch, long low, long high) {
  resetAll();
  setBranch(dummyBranch);
  setEventRange(low, high);
  haveBranch = true;
  haveAverages = false;
  haveCorrections = false;
}

//default destructor
ScalerCount::~ScalerCount(void) {
  resetAll();
  branch = NULL;
}

//setter for eventLow
void ScalerCount::setEventLow(long low) {
  eventLow = low;
}

//setter for eventHigh
void ScalerCount::setEventHigh(long high) {
  eventHigh = high;
}

//setter for both eventLow and eventHigh
void ScalerCount::setEventRange(long low, long high) {
  eventLow  = low;
  eventHigh = high;
}

//setter for TBranch for walk through
void ScalerCount::setBranch(TBranch *dummyBranch) {
  branch = dummyBranch;
}

void ScalerCount::reset(void) {
  for(int j=0; j<num; j++) {
    all_means[j] = 0;
    all_error[j] = 0;
    pos_means[j] = 0;
    pos_error[j] = 0;
    neg_means[j] = 0;
    neg_error[j] = 0;

    pos_uncorrelated_values[j] = 0;
    pos_uncorrelated_errors[j] = 0;
    neg_uncorrelated_values[j] = 0;
    neg_uncorrelated_errors[j] = 0;

    livetime[j] = 0;
    livetime_error[j] = 0;
    accidentals[j] = 0;
    accidentals_error[j] = 0;
  }
}

void ScalerCount::resetAll(void) {
  //lets clean up our memory management
  for(int j=0; j<num; j++) {
    all_values[j].clear();
    pos_values[j].clear();
    neg_values[j].clear();
  }
  for(int j=0; j<num; j++) {
    lAll[j] = NULL;
    lPos[j] = NULL;
    lNeg[j] = NULL;
  }
  reset();
}

/* The following methods return a pointer to the vector of
 * values for the mdallbars, or positive/negative tubes.
 * I'm assuming people will request BY OCTANT (ie, somebody
 * would call this saying "return the address of Octant 1".
 * Since arrays start at 0, I return the vector to
 * j-1.
 */
std::vector<double>* ScalerCount::get_all_values(int j) {
  return &all_values[j-1];
}

std::vector<double>* ScalerCount::get_pos_values(int j) {
  return &pos_values[j-1];
}

std::vector<double>* ScalerCount::get_neg_values(int j) {
  return &neg_values[j-1];
}

void ScalerCount::processEvents(void) {
  if (haveBranch==false) {
    printf("You have not specified a branch yet...\n");
    return;
  }

  for(int j=0; j<num; j++) {
    lAll[j] = (TLeaf*) branch->GetLeaf(Form("md%ibar_sca",j+1));
    lPos[j] = (TLeaf*) branch->GetLeaf(Form("md%ip_sca",j+1));
    lNeg[j] = (TLeaf*) branch->GetLeaf(Form("md%im_sca",j+1));
  }

  long n_entries = branch->GetEntries();

  for (long j=0; j<n_entries; j++) {
    //process events only within desired event number range
    if (!(j>=eventLow && j<=eventHigh)) {continue;}
    branch->GetEntry(j);

    if(j % 10000 == 0) {
      printf(" Total events %ld. events process so far: %ld\n", n_entries, j);
    }

    for (int j=0; j<num; j++) {
      if (lAll[j]->GetValue() !=0)
        all_values[j].push_back( .01*lAll[j]->GetValue() );
      if (lPos[j]->GetValue() !=0)
        pos_values[j].push_back( .01*lPos[j]->GetValue() );
      if (lNeg[j]->GetValue() !=0)
        neg_values[j].push_back( .01*lNeg[j]->GetValue() );
    }
  } //end loop over events

  return;
}

/*short function to calculate the straight average of the values
 */
double ScalerCount::calculateAverage(std::vector<double> *vec) {
  double sum=0;
  for(size_t j=0; j<vec->size(); j++)
    { sum += vec->at(j); }
  return sum/double(vec->size());
}

/*short function to calculate distribution RMS
 * RMS = sqrt ( sum(x_i^2) / (number of x_i) )
 */
double ScalerCount::calculateRMS(std::vector<double> *vec) {
  double sum=0;
  for(size_t j=0; j<vec->size(); j++)
    { sum += vec->at(j)*vec->at(j); }
  return sqrt ( sum/double(vec->size()) );
}

/* This method doesn't technically "set" any variables directly.
 * It calculates the distribution means and errors on the means for
 * the allbars, positive tubes, and negative tube data.
 * It walks through the vectors, then plops the result in the
 * appropriate area to grab later. In this sense, it "sets"
 * the average values and errors in the arrays.
 *
 * Note also, the uncertainty on the mean of a distribution is
 * the RMS/sqrt(N), where N is the number of entries.
 */

void ScalerCount::setAverages(void) {
  for(int j=0; j<num; j++) {
    all_means[j] = calculateAverage(&all_values[j]);
    pos_means[j] = calculateAverage(&pos_values[j]);
    neg_means[j] = calculateAverage(&neg_values[j]);
  }

  for(int j=0; j<num; j++) {
    all_error[j] = ( calculateRMS(&all_values[j])/all_values[j].size() );
    pos_error[j] = ( calculateRMS(&pos_values[j])/pos_values[j].size() );
    neg_error[j] = ( calculateRMS(&neg_values[j])/neg_values[j].size() );
  }
  haveAverages=true;
}

//This method calculates the corrections for each tube.
//Corrections are for livetime and electronic accidentals
void ScalerCount::calculateCorrections(void) {
  for(int j=0; j<num; j++) {
    /* Lets start by calculating the livetime. This is simply:
     * 1-all_means*discriminator width. The 1000 is because
     * all_means is in kHz.
     * The uncertainty on livetime assumes a 10% uncertainty on the
     * discriminator width. This sounds large, but after discussion
     * with Katherine they were measured on a scope. It's easy enough
     * to change here if need be...
     * Note (10%)^2 = (.1)^2 = 0.01
     */
    livetime[j] = 1-disc*1000*all_means[j], //livetime, then livetime uncertainty
    livetime_error[j] = 
      all_means[j]*1000*disc*sqrt(pow(all_error[j]/all_means[j],2) + 0.01 );

    /* To calculate the accidentals, let's start by calculating
     * the uncorrelated pusles rates.
     * Uncorrelated pulse rates are essentially
     *    tube rate - coincidence rate
     * For the uncertainty, I just take the uncertainties of
     * the tube/coincidence rate in quadrature
     */
    neg_uncorrelated_values[j] = neg_means[j]-all_means[j];
    neg_uncorrelated_errors[j]  = sqrt(pow(all_error[j],2)+pow(neg_error[j],2));
    pos_uncorrelated_values[j] = pos_means[j]-all_means[j];
    pos_uncorrelated_errors[j]  = sqrt(pow(all_error[j],2)+pow(pos_error[j],2));

  /* Now that we have uncorrelated pulse information, we can calculate
   * the accidentals contribution. The 1000 is the conversion to kHz.
   * At the moment I'm assuming neglible contribution from the coincidence
   * module for the accidentals uncertainty.
   */
    accidentals[j]=neg_uncorrelated_values[j]*pos_uncorrelated_values[j]*coin*1000;
    accidentals_error[j] =
      accidentals[j] * sqrt(pow(all_error[j],2) + pow(neg_uncorrelated_errors[j],2) + pow(pos_uncorrelated_errors[j],2));
  }
  haveCorrections=true;
}


//this method will print the RAW rates
void ScalerCount::printRawRates(void) {
  if (haveAverages==false)
    { setAverages(); }
  std::cout <<"\n\n\tThe Raw Main Detector Scaler Data (kHz)\n";
  std::cout <<"Octant \tAllBars Error \tNeg \tError \tPos \tError\n";
  for(int j=0; j<num; j++) {
  printf("%i \t%-.2f \t%-.2f \t%-.2f \t%-.2f \t%-.2f \t%-.2f\n", j+1,
    all_means[j]  , all_error[j],
    neg_means[j]  , neg_error[j],
    pos_means[j]  , pos_error[j] );
  }
  std::cout <<std::endl;
}

//now give the lifetime, accidentals, and uncorrelated pulse rates
void ScalerCount::printCorrections(void) {
  if (haveAverages==false)
    { setAverages(); }
  calculateCorrections();
  std::cout <<"\n\n\tThe Main Detector Corrections\n";
  std::cout <<"Uncorrelated pulse rates (kHz)\n";
  std::cout <<"Octant \tNeg \tError \tPos \tError\n";
  for(int j=0; j<num; j++) {
    printf("%i \t%-.2f \t%-.2f \t%-.2f \t%-.2f\n", j+1,
        neg_uncorrelated_values[j],
        neg_uncorrelated_errors[j],
        pos_uncorrelated_values[j],
        pos_uncorrelated_errors[j]
        );
  }
  std::cout <<std::endl;
  std::cout <<"Octant \tLivetime \tError \tAccidentals(kHz) \tError\n";
  for(int j=0; j<num; j++) {
  printf("%i \t%-.2f \t%-.2f \t%-.2f \t%-.2f\n", j+1,
      livetime[j], livetime_error[j],
      accidentals[j], accidentals_error[j]);
  }
  std::cout <<std::endl;
}

//Now this method prints the mdallbars rate in kHz
void ScalerCount::printCorrectedRates(void) {
  if (haveAverages==false)
    { setAverages(); }
  if (haveCorrections==false)
    { calculateCorrections(); }
  std::cout <<"\n\n\t\tThe Corrected Main Detector Scaler Data (kHz)\n";
  std::cout <<"Octant \tAllBars \tError\n";
  for(int j=0; j<num; j++) {
  printf("%i \t%-.2f \t%-.2f\n", j+1,
      ( all_means[j]-accidentals[j] )/livetime[j], //this is rate...
      ( all_means[j]-accidentals[j] )/livetime[j]  //this is uncertainty...
        * sqrt( pow(all_error[j]/all_means[j],2)
              + pow(livetime_error[j]/livetime[j],2)
              + pow(accidentals_error[j]/accidentals[j],2)
              )
      );
  }
  std::cout <<std::endl;
}




