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
#include <TH1F.h>
#include <TCanvas.h>
#include <TPaveText.h>

#include "ScalerCount.h"

using std::cout;
using std::vector;

//can't declare private static array when declaring
const int ScalerCount::plot_sequence[num] = { 4, 1, 2, 3, 6, 9, 8, 7 };

//default constructor
ScalerCount::ScalerCount(void) {
  resetAll();
  setEventRange(0, 4000000);
  setName("");
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
  setName("");
  haveBranch = true;
  haveAverages = false;
  haveCorrections = false;
}

ScalerCount::ScalerCount(TBranch *dummyBranch, long low, long high) {
  resetAll();
  setBranch(dummyBranch);
  setEventRange(low, high);
  setName("");
  haveBranch = true;
  haveAverages = false;
  haveCorrections = false;
}


ScalerCount::ScalerCount(TBranch *dummyBranch, 
    long low,
    long high,
    std::string name) {
  resetAll();
  setBranch(dummyBranch);
  setEventRange(low, high);
  setName(name.data());
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

void ScalerCount::setName(std::string name) {
  myName = name;
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
    lAll_values[j] = NULL;
    lPos_values[j] = NULL;
    lNeg_values[j] = NULL;

    lPos_adc[j] = NULL;
    lNeg_adc[j] = NULL;
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
    lAll_values[j] = (TLeaf*) branch->GetLeaf(Form("md%ibar_sca",j+1));
    lPos_values[j] = (TLeaf*) branch->GetLeaf(Form("md%ip_sca",j+1));
    lNeg_values[j] = (TLeaf*) branch->GetLeaf(Form("md%im_sca",j+1));
  }

  for(int j=0; j<num; j++) {
    lPos_adc[j] = (TLeaf*) branch->GetLeaf(Form("md%ip_adc",j+1));
    lNeg_adc[j] = (TLeaf*) branch->GetLeaf(Form("md%im_adc",j+1));
  }

  long n_entries = branch->GetEntries();

  for (long j=0; j<n_entries; j++) {
    //process events only within desired event number range
    if (!(j>=eventLow && j<=eventHigh)) {continue;}
    branch->GetEntry(j);

    if(j % 100000 == 0) {
      printf(" Total events %ld. events process so far: %ld\n", n_entries, j);
    }

    for (int j=0; j<num; j++) {
      if (lAll_values[j]->GetValue() !=0)
        all_values[j].push_back( .01*lAll_values[j]->GetValue() );
      if (lPos_values[j]->GetValue() !=0)
        pos_values[j].push_back( .01*lPos_values[j]->GetValue() );
      if (lNeg_values[j]->GetValue() !=0)
        neg_values[j].push_back( .01*lNeg_values[j]->GetValue() );

      if (lPos_adc[j]->GetValue() !=0)
        pos_adc[j].push_back( lPos_adc[j]->GetValue() );
      if (lNeg_adc[j]->GetValue() !=0)
        neg_adc[j].push_back( lNeg_adc[j]->GetValue() );
      if (lPos_adc[j]->GetValue() !=0 && 
          lNeg_adc[j]->GetValue() !=0)
        all_adc[j].push_back( 
            (lPos_adc[j]->GetValue() + lNeg_adc[j]->GetValue())/2 );
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

void ScalerCount::createCanvas(void) {
  //if canvas points to an already existing canvas
  //delete it and start fresh
//  if (canvas != NULL) {
//    delete canvas;
//    canvas = NULL;
//  }

  char buffer[10];
  for(int j=0; j<3; j++) {
    sprintf(buffer,"canvas_%i",j);
    canvas_adc[j] = new TCanvas(buffer,"title",1200,900);
    canvas_adc[j]->Divide(3,3);
  }
}

/*
void ScalerCount::resetHistograms(void) {
  for(int j=0; j<num; j++) {
    if (h_allbars != NULL) {
      delete h_allbars[j];
      h_allbars[j] = NULL;
    }
  }
}
*/

void ScalerCount::createHistograms(void) {
  char name_pos[20], name_neg[20], name_all[20];
  char title_pos[30],title_neg[30],title_all[30];
  int Nbins = 4000;
  int xlow  = 0;
  int xhigh = 4000;
  for(int j=0; j<num; j++) {
    sprintf(name_pos,"h_adcpos_%i",j);
    sprintf(name_neg,"h_adcneg_%i",j);
    sprintf(name_all,"h_adcall_%i",j);
    sprintf(title_pos,"MD #%i Pos PMT ADC",j);
    sprintf(title_neg,"MD #%i Neg PMT ADC",j);
    sprintf(title_all,"MD #%i All Bars ADC",j);
    h_adcpos[j] = new TH1F(name_pos,title_pos,Nbins,xlow,xhigh);
    h_adcneg[j] = new TH1F(name_neg,title_neg,Nbins,xlow,xhigh);
    h_adcall[j] = new TH1F(name_all,title_all,Nbins,xlow,xhigh);
  }
}

void ScalerCount::fillHistograms(void ){
//  resetHistograms();

  for(int j=0; j<num; j++) {
    h_adcpos[j]->FillN(pos_adc[j].size(),pos_adc[j].data(),NULL);
    h_adcneg[j]->FillN(neg_adc[j].size(),neg_adc[j].data(),NULL);
    h_adcall[j]->FillN(all_adc[j].size(),all_adc[j].data(),NULL);
  }
}

void ScalerCount::plotHistograms(void) {
  TH1F *tmp;
  char label[10];
  TPaveText *txt;
  for(int i=0; i<3; i++) {
    for(int j=0; j<num; j++) {
      if(i==0) {
        tmp = h_adcpos[j];
        sprintf(label,"Pos PMT");
      } else if (i==1) {
        tmp = h_adcneg[j];
        sprintf(label,"Neg PMT");
      } else {
        tmp = h_adcall[j];
        sprintf(label,"All Bars");
      }
      canvas_adc[i]->cd(plot_sequence[j]);

      tmp->Draw("");

      //now let's create the axis titles, etc
      char buffer[50];
      std::string title, xaxis, yaxis;
      sprintf(buffer,"MD #%i %s ADC",j+1,label);
      title = buffer;
      xaxis = "ADC Channel";
      yaxis = "Counts";

      //this function does all the details
      makeHistoPretty(
          tmp,canvas_adc[i],i,title.data(),xaxis.data(),yaxis.data());
      canvas_adc[i]->cd(5);
      txt = new TPaveText(0.05,0.10,0.95,0.8);
      txt->AddText(myName.data());
      txt->AddText(label);
      txt->Draw();
      txt->SetFillColor(0);
      txt->SetBorderSize(0);
    }
    delete txt;
  }
  txt = NULL;
}


//this method just makes the graphs look sexy
//because, let's face it, ROOT isn't sexy
void ScalerCount::makeHistoPretty(
    TH1F *hist,
    TCanvas *canvas,
    int i,
    std::string title,
    std::string xaxis,
    std::string yaxis) {

  int hist_color;
  if(i == 0) {
    hist_color = kBlue+3;
  } else if (i == 1) {
    hist_color = kYellow+3;
  } else {
    hist_color = kGreen+3;
  }

  hist->SetLineColor(hist_color);
  hist->SetFillColor(hist_color);

  float size=0.05;
//  float titlesize=0.05;
  float labelsize = 0.04;
  hist->SetTitle(title.data());
  hist->GetXaxis()->SetTitle(xaxis.data());
  hist->GetYaxis()->SetTitle(yaxis.data());

  canvas->Modified();
  canvas->Update();

  hist->GetYaxis()->SetTitleOffset(0.95);
  hist->GetXaxis()->SetTitleOffset(1.00);
  hist->GetYaxis()->SetTitleSize(size);
  hist->GetXaxis()->SetTitleSize(size);
  hist->GetYaxis()->SetLabelSize(labelsize);
  hist->GetXaxis()->SetLabelSize(labelsize);

  TPaveText *stats = (TPaveText*) gPad->GetPrimitive("stats");
  stats->SetFillColor(0);
  stats->SetX1NDC(0.60);
  stats->SetY1NDC(0.58);
  stats->SetX2NDC(0.90);
  stats->SetY2NDC(0.90);

  gPad->SetGrid();
  gPad->SetLogy();
  canvas->Modified();
  canvas->Update();
}








