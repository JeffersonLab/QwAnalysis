#include <TCanvas.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TH1F.h>
#include <TChain.h>
#include <TFile.h>
#include <TMath.h>
#include <TGraph.h>
#include <QwSIS3320_Samples.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <exception>
#include <fstream>

#define _VAR_LED_   0
#define _BOTH_LED_  2
#define _OFF_LED_   3
#define _DELTA_LED_ 1

#define _N_SAMPLES_ 256
#define _N_PED_SAMPLES_ 125
#define _SUM_LOW_ 126
#define _SUM_HIGH_ 255

const Double_t ledOffValue[4] = {-1e4,-1e4,-1e4,-1e4};
const Double_t errorValue = -99999999;

void copyArray(Int_t n, Double_t *from, Double_t *to)
{
  for(Int_t i = 0; i < n; i++ ) {
    to[i] = from[i];
  }
}

void getAverage(Int_t n, Double_t *array, Double_t *average, Double_t *sigma)
{
  if(array == 0 ) {
    std::cout << "Empty array!!\n";
    throw("Null array");
  }

  Double_t sum = 0;
  for(Int_t j = 0; j < n; j++ ) {
    sum+=array[j];
  }

  *average = sum/n;
  sum = 0;
  for(Int_t j = 0; j < n; j++ ) {
    sum+=(*average-array[j]) * (*average-array[j]);
  }
  *sigma = TMath::Sqrt(sum/(n-1));
}

Double_t getSum(Int_t low, Int_t high, Double_t *array)
{
  if(array == 0 ) {
    std::cout << "Empty array!!\n";
    throw("Null array");
  }
  Double_t sum = 0;
  for(Int_t m = low; m <= high; m++ ) {
    sum+=array[m];
  }
  return sum;
}

void getPeak(Int_t low, Int_t high, Double_t *array, Double_t *result )
{
  if(array == 0 ) {
    std::cout << "Empty array!!\n";
    throw("Null array");
  }
  result[0] = -1e9;
  result[1] = 0;
  for(Int_t m = low; m <= high; m++ ) {
    if (array[m] > result[0] ) {
      result[0] = array[m];
      result[1] = m;
    }
  }
}

void make_phdet_linearity_rootfile(Int_t runnum = 25305,
    Int_t variable_at = 10,
    Int_t windowMultiplier = 1,
    Int_t end = -1, Bool_t useCache = kFALSE)
{
  struct led_t {
    Double_t window_sum;
    Double_t peak_sum;
    Double_t peak;
    Double_t timing;
    Int_t samples;
    Double_t pedestal;
    Double_t pedestal_sigma;
    Int_t pedestal_samples;
    Double_t data[_N_SAMPLES_];
    Double_t getSum(Int_t low, Int_t high) {
      Double_t sum = 0;
      for(Int_t j = low; j < high; j++ ) {
        sum+=data[j];
      }
      return sum;
    }
  };

  // Where is the Variable at?
  Int_t foundAt = variable_at;

  // Load utility functions
  //gROOT->LoadMacro("QwComptonPhotonLED.cc++g");

  Double_t dac;
  Double_t intRange = _SUM_HIGH_ - _SUM_LOW_ + 1;
  led_t tree_led[4];

  TString inFile;
  if(!useCache)
    inFile = Form("$QW_ROOTFILES/Compton_Pass1_%d*.*.root",runnum);
  else
    inFile = Form("$QW_ROOTFILES_CACHE/Compton_Pass1_%d*.*.root",runnum);
  //TString inFile = Form("$QW_ROOTFILES/Compton_first100k_%d*.root",runnum);
  TString outFile = Form("$QW_ROOTFILES/Compton_LED_%d.root",runnum);

  TChain *chain = new TChain("Mps_Tree");
  std::cout << "Added " << chain->Add(inFile) << " rootfiles: " << inFile << std::endl;
  //chain->Add(inFile);
  TFile file(outFile,"RECREATE");
  std::cout << outFile.Data() << std::endl;
  TTree *led_tree = new TTree("LED_Tree","LED Tree");
  //TBranch *led_branch = led_tree->Branch("led",new QwComptonPhotonLED());
  led_tree->Branch("variable",&tree_led[_VAR_LED_].window_sum,
      "window_sum/D:peak_sum:peak:timing:samples/I:pedestal/D:pedestal_sigma:pedestal_samples/I:data[256]/D");
  led_tree->Branch("delta",&tree_led[_DELTA_LED_].window_sum,
      "window_sum/D:peak_sum:peak:timing:samples/I:pedestal/D:pedestal_sigma:pedestal_samples/I:data[256]/D");
  led_tree->Branch("both",&tree_led[_BOTH_LED_].window_sum,
      "window_sum/D:peak_sum:peak:timing:samples/I:pedestal/D:pedestal_sigma:pedestal_samples/I:data[256]/D");
  led_tree->Branch("off",&tree_led[_OFF_LED_].window_sum,
      "window_sum/D:peak_sum:peak:timing:samples/I:pedestal/D:pedestal_sigma:pedestal_samples/I:data[256]/D");
/*  led_tree->Branch("window_sum_variable",&tree_window_sum[_VAR_LED_]);
  led_tree->Branch("window_sum_delta",&tree_window_sum[_DELTA_LED_]);
  led_tree->Branch("window_sum_both",&tree_window_sum[_BOTH_LED_]);
  led_tree->Branch("window_sum_off",&tree_window_sum[_OFF_LED_]);

  led_tree->Branch("pedestal_variable",&tree_pedestal[_VAR_LED_]);
  led_tree->Branch("pedestal_delta",&tree_pedestal[_DELTA_LED_]);
  led_tree->Branch("pedestal_both",&tree_pedestal[_BOTH_LED_]);
  led_tree->Branch("pedestal_off",&tree_pedestal[_OFF_LED_]);

  led_tree->Branch("pedestal_error_variable",&tree_pedestal_error[_VAR_LED_]);
  led_tree->Branch("pedestal_error_delta",&tree_pedestal_error[_DELTA_LED_]);
  led_tree->Branch("pedestal_error_both",&tree_pedestal_error[_BOTH_LED_]);
  led_tree->Branch("pedestal_error_off",&tree_pedestal_error[_OFF_LED_]);

  led_tree->Branch("peak_sum_variable",&tree_peak_sum[_VAR_LED_]);
  led_tree->Branch("peak_sum_delta",&tree_peak_sum[_DELTA_LED_]);
  led_tree->Branch("peak_sum_both",&tree_peak_sum[_BOTH_LED_]);
  led_tree->Branch("peak_sum_off",&tree_peak_sum[_OFF_LED_]);

  led_tree->Branch("timing_variable",&tree_timing[_VAR_LED_]);
  led_tree->Branch("timing_delta",&tree_timing[_DELTA_LED_]);
  led_tree->Branch("timing_both",&tree_timing[_BOTH_LED_]);
  led_tree->Branch("timing_off",&tree_timing[_OFF_LED_]);

  led_tree->Branch("peak_variable",&tree_peak[_VAR_LED_]);
  led_tree->Branch("peak_delta",&tree_peak[_DELTA_LED_]);
  led_tree->Branch("peak_both",&tree_peak[_BOTH_LED_]);
  led_tree->Branch("peak_off",&tree_peak[_OFF_LED_]);

  led_tree->Branch("data_variable",(tree_data[_VAR_LED_]));
  led_tree->Branch("data_delta",(tree_data[_DELTA_LED_]));
  led_tree->Branch("data_both",(tree_data[_BOTH_LED_]));
  led_tree->Branch("data_off",(tree_data[_OFF_LED_]));
*/

  led_tree->Branch("dac",&dac);
  Int_t entries;
  if(end < 0)
    entries = chain->GetEntries();
  else
    entries = end;

  chain->ResetBranchAddresses();
  std::vector<QwSIS3320_Samples>* sampled_events = 0;
  chain->SetBranchStatus("*",0);
  chain->SetBranchStatus("fadc_compton_samples*",1);
  chain->SetBranchStatus("sca_haptb_time_int",1);
  chain->SetBranchAddress("fadc_compton_samples", &sampled_events);

  std::cout << "Total entries: " << entries << std::endl;
  const Int_t quartets = (entries-foundAt)/4/windowMultiplier;
  std::cout << "Total quartets: " << quartets << std::endl;
  Double_t ped = 0;
  Double_t pedErr = 0;

  Bool_t good;
  Int_t ledOffCount = 0;
  Double_t value = errorValue;
  Double_t peak_result[2] = {1e-9,0};
  for( Int_t i = 0; i < quartets-1; i++ ) {
    good = kTRUE;
    Double_t lastDAC = -1;
    for(Int_t led = 0; led < 4; led++ ) {
      chain->GetEntry(foundAt+(i*4+led)*windowMultiplier);
      dac = ((TLeaf*)((TBranch*)chain->GetBranch("sca_haptb_time_int"))->GetLeaf("value"))->GetValue();
      if( dac <= 0 )
        good = kFALSE;
      try{
        TGraph* graph = sampled_events->at(0).GetGraph();
        getAverage(_N_PED_SAMPLES_,graph->GetY(),&ped,&pedErr);

        copyArray(_N_SAMPLES_, graph->GetY(),tree_led[led].data);
        tree_led[led].window_sum = getSum(0,_N_SAMPLES_,graph->GetY());
        tree_led[led].pedestal = ped;
        tree_led[led].pedestal_sigma = pedErr;
        getPeak(0,_N_SAMPLES_-1,tree_led[led].data,peak_result);
        tree_led[led].peak = peak_result[0];
        tree_led[led].timing = peak_result[1];
        tree_led[led].peak_sum = getSum(_SUM_LOW_,_SUM_HIGH_,
            graph->GetY());
        if( i%Int_t(.05*entries/4/windowMultiplier) == 0 )
            std::cout << "Entry (" << led << "): " << foundAt+(i*4+led)*windowMultiplier << " \t Int: "
                      << tree_led[led].window_sum
                      << " \tPed: " << tree_led[led].pedestal << " \tIntN: "
                      << tree_led[led].window_sum-intRange*tree_led[led].pedestal << " \tDAC: (" << dac << ")" << std::endl;
        if(tree_led[led].window_sum<ledOffValue[led]) {
          std::cout << "LED(" << led << ") SUM: " << value << std::endl;
          ledOffCount++;
          if(ledOffCount%10==0)
            std::cout << "Have counted " << ledOffCount
              << " LED off counts so far!" << std::endl;
          //throw(new exception());// GeneralException("LED Value off"));
          //std::cout << "LED Off!\n";
          good = kFALSE;
        }
      } catch (const std::exception & e) {
        good = kFALSE;
        std::cout <<  "Exception caught  at entry (" << led << "): " << i <<  ". Message: " << e.what() << std::endl;
      }
      if(led == _VAR_LED_ ) {
        lastDAC = dac;
      } else if( lastDAC != dac ) {
        good = kFALSE;
      }
    }
    if(good) {
      led_tree->Fill();
    }
  }
  led_tree->Write("", TObject::kOverwrite);
  led_tree->Print();
  file.Write();
}
