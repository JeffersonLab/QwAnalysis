#include <TCanvas.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TH1F.h>
#include <TChain.h>
#include <TMath.h>
#include <TGraph.h>
#include <QwSIS3320_Samples.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <exception>

#define _VAR_LED_   0
#define _BOTH_LED_  2
#define _OFF_LED_   3
#define _DELTA_LED_ 1

#define _NUM_DIFFS_ 7
#define _NUM_PERCENTS_ 4
#define _NUM_SLOPES_ 2

//const Double_t ledOffValue[4] = {20000,25000,-1e5,-1300};
const Double_t ledOffValue[4] = {-1e4,-1e4,-1e4,-1e4};
const Double_t errorValue = -99999999;
Int_t DAC_SETTINGS = 0;
std::vector<Int_t> DAC_VALUES;
Int_t intLow = (Int_t)0;
Int_t intHigh = (Int_t)255;
const Int_t intRange = intHigh-intLow;

enum AnalysisType {
  SUM_WINDOW = 0,
  SUM_PEAK,
  PEAK,
  TIMING
};

Int_t getDACIndex(Int_t value)
{
  //std::cout << "Looking for DAC Index: " << value <<std::endl;
  try {
    for(Int_t kk = 0; kk < DAC_SETTINGS; kk++ ) {
      if(value == DAC_VALUES[kk])
        return kk;
    }
  } catch ( const std::exception &e ) {
  }
  return -1;
}

Double_t minimum(Double_t one, Double_t two)
{
  if(one == errorValue)
    return two;
  return one > two ? two : one;
}

Double_t maximum(Double_t one, Double_t two)
{
  if(one == errorValue)
    return two;
  return one > two ? one : two;
}

Double_t getAverage(Int_t n, Double_t *array)
{
  if(array == 0 ) {
    std::cout << "Empty array!!\n";
    throw("Null array");
  }

  Double_t sum = 0;
  for(Int_t j = 0; j < n; j++ )
    sum+=array[j];

  return sum/n;
}

Double_t getSum(Int_t low, Int_t high, Double_t *array )
{
  if(array == 0 ) {
    std::cout << "Empty array!!\n";
    throw("Null array");
  }
  Double_t sum = 0;
  for(Int_t m = low+1; m < high+1; m++ ) {
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
  for(Int_t m = low+1; m < high+1; m++ ) {
    if (array[m] > result[0] ) {
      result[0] = array[m];
      result[1] = m;
    }
  }
}

void phdet_linearity(Int_t runnum = 23388,
    Int_t variable_at = 10,
    AnalysisType anType = SUM_WINDOW,
    Int_t start = 0, Int_t end = -1)
{
  // First, select which is the variable in this case
  Int_t foundAt = start+variable_at;

  TString type;
  switch(anType) {
    case SUM_WINDOW:
      type = "sum_window";
      break;
    case SUM_PEAK:
      type = "sum_peak";
      intLow = 153;
      intHigh = 170;
      break;
    case PEAK:
      type = "peak";
      break;
    case TIMING:
      type = "timing";
      break;
  }

  TChain *chain = new TChain("Mps_Tree");
  //chain->Add(Form("$QW_ROOTFILES/Compton_first100k_%d.root",runnum));
  //chain->Add(Form("$QW_ROOTFILES/LED_test_%d*.*.root",runnum));
  //chain->Add(Form("$QW_ROOTFILES/LED_test_%d*.*.root",runnum));
  chain->Add(Form("$QW_ROOTFILES/Compton_Pass1_%d*.*.root",runnum));

  Int_t entries;
  if(end < 0)
    entries = chain->GetEntries();
  else
    entries = end;

  // Now find all the DAC values
  chain->ResetBranchAddresses();
  Double_t dac = 0;
  chain->SetBranchStatus("*",0);
  chain->SetBranchStatus("sca_haptb_time_int",1);
  //chain->SetBranchAddress("sca_haptb_time_int",&dac);
  for(Int_t i = 10; i < 500000 && i <entries; i++ ) {
    chain->GetEntry(i);
    dac = ((TLeaf*)((TBranch*)chain->GetBranch("sca_haptb_time_int"))->GetLeaf("value"))->GetValue();
    if(i%100000==0)
      std::cout << "Looking at entry: " << i << std::endl;
    if(getDACIndex((Int_t)dac)==-1&&dac>0) {
      std::cout << "Found new DAC value: " << dac << std::endl;
      DAC_SETTINGS++;
      DAC_VALUES.push_back((Int_t)dac);
    }
  }

  chain->ResetBranchAddresses();
  std::vector<QwSIS3320_Samples>* sampled_events = 0;
  chain->SetBranchStatus("*",0);
  chain->SetBranchStatus("fadc_compton_samples*",1);
  chain->SetBranchStatus("sca_haptb_time_int",1);
  chain->SetBranchAddress("fadc_compton_samples", &sampled_events);
  //chain->SetBranchAddress("sca_haptb_time_int",&dac);

  std::cout << "Total entries: " << entries << std::endl;
  std::vector<Double_t*> max;
  std::vector<Double_t*> min;
  std::vector<Double_t> minDiff[_NUM_DIFFS_];
  std::vector<Double_t> maxDiff[_NUM_DIFFS_];
  std::vector<Double_t> minPercent[_NUM_PERCENTS_];
  std::vector<Double_t> maxPercent[_NUM_PERCENTS_];
  std::vector<Double_t> minSlope[_NUM_SLOPES_];
  std::vector<Double_t> maxSlope[_NUM_SLOPES_];

  std::vector<Bool_t*> first;
  for(Int_t k=0; k < DAC_SETTINGS+1; k++ ) {
    first.push_back(new Bool_t[4]);
    max.push_back(new Double_t[4]);
    min.push_back(new Double_t[4]);
  }
  for(Int_t k=0; k < _NUM_DIFFS_; k++ ) {
    minDiff[k].resize(DAC_SETTINGS);
    maxDiff[k].resize(DAC_SETTINGS);
  }
  for(Int_t k=0; k < _NUM_PERCENTS_; k++ ) {
    minPercent[k].resize(DAC_SETTINGS);
    maxPercent[k].resize(DAC_SETTINGS);
  }
  for(Int_t k=0; k < _NUM_SLOPES_; k++ ) {
    minSlope[k].resize(DAC_SETTINGS);
    maxSlope[k].resize(DAC_SETTINGS);
  }

  /* No longer needed, no sync bit present on the board anymore
  Bool_t found = kFALSE;
  Int_t bit = 0;
  for( Int_t i = start; i < entries && !found; i++ ) {
    chain->GetEntry(i);
    if(input>30000) {
      found = kTRUE;
      foundAt = i+3+syncOffset;
      std::cout << "Variable found at entry " << i+3+syncOffset << std::endl;
      bit = (i+3+syncOffset)%4;
    }
  }
  */

  std::vector<Double_t> values[4];
  const Int_t quartets = (entries-foundAt)/4;
  std::cout << "Total quartets: " << quartets << std::endl;
  for(Int_t j = 0; j < 4; j++ ) {
    values[j].resize(quartets);
  }
  std::vector<Double_t> diff[_NUM_DIFFS_];
  std::vector<Double_t> percent[_NUM_PERCENTS_];
  std::vector<Double_t> slope[_NUM_SLOPES_];
  for(Int_t k=0; k < _NUM_DIFFS_; k++ ) {
    diff[k].resize(quartets);
  }
  for(Int_t k=0; k < _NUM_PERCENTS_; k++ ) {
    percent[k].resize(quartets);
  }
  for(Int_t k=0; k < _NUM_SLOPES_; k++ ) {
    slope[k].resize(quartets);
  }

  std::vector<Int_t> dacValue;
  dacValue.resize(quartets);

  for( Int_t j = 0; j < 5; j++ ) {
    for(Int_t setting = 0 ; setting < DAC_SETTINGS; setting++)
      first[setting][j] = kTRUE;
  }

  Double_t ped = 0;

  Bool_t good;
  Int_t ledOffCount = 0;
  Int_t dac_index = -1;
  Double_t value = errorValue;
  for( Int_t i = 0; i < quartets-1; i++ ) {
    good = kTRUE;
    Double_t lastDAC = -1;
    for(Int_t led = 0; led < 4; led++ ) {
      dac = values[led][i] = value = errorValue;
      chain->GetEntry(foundAt+i*4+led);
      dac = ((TLeaf*)((TBranch*)chain->GetBranch("sca_haptb_time_int"))->GetLeaf("value"))->GetValue();
      if( dac <= 0 )
        good = kFALSE;
      dac_index = getDACIndex(dac);
      //std::cout << "DAC: " << dac << " \tIndex: " << dac_index << std::endl;
      try{
        TGraph* graph = sampled_events->at(0).GetGraph();
        //sum = sampled_events->at(0).GetSum(intLow,intHigh);
        value = getSum(intLow,intHigh,graph->GetY());
        ped = getAverage(110,graph->GetY());
        //if(led!=3909809)
        //  ped = 0;
        //if(ped>-4)
        //  good = kFALSE
        if( i%Int_t(.05*entries/4) == 0 )
        std::cout << "Entry (" << led << "): " << foundAt+i*4+led << " \t Int: "
                  << value
                  << " \tPed: " << ped << " \tIntN: "
                  << value-intRange*ped << " \tDAC: (" << dac << "," << dac_index << ")" << std::endl;
        values[led][i] = value -intRange*ped;
        /*if(values[led][i]>4000||values[led][i]<-250) {
          good = kFALSE;
        } else if ( values[2][i]>250||values[3][i]>250 ) {
          good = kFALSE;
        }*/
        if(values[led][i]<ledOffValue[led]) {
          std::cout << "LED(" << led << ") SUM: " << value << std::endl;
          ledOffCount++;
          if(ledOffCount%10==0)
            std::cout << "Have counted " << ledOffCount
              << " LED off counts so far!" << std::endl;
          //throw(new exception());// GeneralException("LED Value off"));
          //std::cout << "LED Off!\n";
          good = kFALSE;
          values[led][i] = errorValue;
        }
      } catch (const std::exception & e) {
        good = kFALSE;
        std::cout <<  "Exception caught  at entry (" << led << "): " << i <<  ". Message: " << e.what() << std::endl;
        values[led][i] = errorValue;
      }
      if(led == 0 ) {
        lastDAC = dac;
      } else if( lastDAC != dac ) {
        //std::cout << "DAC: " << dac << std::endl;
        good = kFALSE;
        values[led][i] = errorValue;
      }
      if(values[led][i] < -50000 && good )
        std::cout << "Int(" << led << "," << i << "): "
          << values[led][i] << " \tSum: " << value
             << " \tPed: " << ped  << " \tG: " << good << std::endl;
      if(!good&&kFALSE) {
        std::cout << "Int(" << led << "," << i << "): "
          << values[led][i] << " \tSum: " << value
             << " \tPed: " << ped  << " \tDAC: " << dac << std::endl;
      }


      if(dac_index >= 0 && first[dac_index][led]) {
        if(values[led][i] != errorValue && good ) {
          min[dac_index][led] =
            max[dac_index][led] = values[led][i];
          first[dac_index][led] = kFALSE;
          std::cout << "Processed first of: (" << led << "," << dac << ")" << std::endl;
        }
      } else if (good && dac_index>=0) {
        min[dac_index][led] = minimum(values[led][i],
            min[dac_index][led]);
        max[dac_index][led] = maximum(values[led][i],
            max[dac_index][led]);
        //std::cout << "Min(" << dac_index << "," << led << ") = " << min[dac_index][led] << " \t";
        //std::cout << "Max(" << dac_index << "," << led << ") = " << max[dac_index][led] << std::endl;
      }
    }
    if(good) {
      // Window - off
      diff[0][i] = values[_VAR_LED_][i]-values[_OFF_LED_][i];
      diff[1][i] = values[_DELTA_LED_][i]-values[_OFF_LED_][i];
      diff[2][i] = values[_BOTH_LED_][i]-values[_OFF_LED_][i];

      // Useful differences
      diff[3][i] = values[_VAR_LED_][i]-values[_DELTA_LED_][i];
      diff[4][i] = values[_BOTH_LED_][i]-values[_VAR_LED_][i];
      diff[5][i] = values[_BOTH_LED_][i]-values[_DELTA_LED_][i];
      diff[6][i] = values[_BOTH_LED_][i]-values[_VAR_LED_][i]-values[_DELTA_LED_][i]-values[_OFF_LED_][i];

      // Percents/Scales
      percent[0][i] = 100*(values[_BOTH_LED_][i]-values[_VAR_LED_][i])/values[_VAR_LED_][i];
      percent[1][i] = 100*(values[_BOTH_LED_][i]-values[_DELTA_LED_][i])/values[_DELTA_LED_][i];
      percent[2][i] = 100*(values[_BOTH_LED_][i]-values[_VAR_LED_][i])/(values[_VAR_LED_][i]-values[_OFF_LED_][i]);
      percent[3][i] = 100*(values[_BOTH_LED_][i]-values[_DELTA_LED_][i])/values[_DELTA_LED_][i];

      // Slopes
      slope[0][i] = (values[_BOTH_LED_][i]-values[_VAR_LED_][i])/values[_DELTA_LED_][i];
      slope[1][i] = (values[_BOTH_LED_][i]-values[_DELTA_LED_][i])/(values[_DELTA_LED_][i]-values[_OFF_LED_][i]);

      dacValue[i] = (Int_t)dac;
    } else {
      diff[0][i] = diff[1][i] = diff[2][i] = diff[3][i] = diff[4][i] = diff[5][i] = diff[6][i] = errorValue;
      percent[0][i] = percent[1][i] = percent[2][i] = percent[3][i] = errorValue;
      slope[0][i] = slope[1][i] = errorValue;
      dacValue[i] = (Int_t)errorValue;
      //std::cout << "Stored in quarted " << i << " error value!!\n";
    }
    if(good) {
      if(first[dac_index][4]) {
        for(Int_t a = 0; a<_NUM_DIFFS_; a++ ) {
          Double_t mult = diff[a][i]<0?0.99:1.01;
          minDiff[a][dac_index] = diff[a][i];
          maxDiff[a][dac_index] = diff[a][i]*mult;
          std::cout << "For Diff: " << a << " \tDAC: " << dac_index
            << " \tMin: " << minDiff[a][dac_index]
            << " \tMax: " << maxDiff[a][dac_index] << std::endl;
        }
        for(Int_t a = 0; a<_NUM_PERCENTS_; a++ ) {
          Double_t mult = percent[a][i]<0?0.99:1.01;
          minPercent[a][dac_index] = percent[a][i];
          maxPercent[a][dac_index] = percent[a][i]*mult;
          std::cout << "For Percent: " << a << " \tDAC: " << dac_index
            << " \tMin: " << minPercent[a][dac_index]
            << " \tMax: " << maxPercent[a][dac_index] << std::endl;

        }
        for(Int_t a = 0; a<_NUM_SLOPES_; a++ ) {
          Double_t mult = slope[a][i]<0?0.99:1.01;
          minSlope[a][dac_index] = slope[a][i];
          maxSlope[a][dac_index] = slope[a][i]*mult;
          std::cout << "For Slope: " << a << " \tDAC: " << dac_index
            << " \tMin: " << minSlope[a][dac_index]
            << " \tMax: " << maxSlope[a][dac_index] << std::endl;

        }
        first[dac_index][4] = kFALSE;

      } else {
        for(Int_t a = 0; a<_NUM_DIFFS_; a++ ) {
          minDiff[a][dac_index] = minimum(diff[a][i],minDiff[a][dac_index]);
          maxDiff[a][dac_index] = maximum(diff[a][i],maxDiff[a][dac_index]);
        }
        for(Int_t a = 0; a<_NUM_PERCENTS_; a++ ) {
          minPercent[a][dac_index] = minimum(percent[a][i],minPercent[a][dac_index]);
          maxPercent[a][dac_index] = maximum(percent[a][i],maxPercent[a][dac_index]);
        }
        for(Int_t a = 0; a<_NUM_SLOPES_; a++ ) {
          minSlope[a][dac_index] = minimum(slope[a][i],minSlope[a][dac_index]);
          maxSlope[a][dac_index] = maximum(slope[a][i],maxSlope[a][dac_index]);
        }

      }
      //std::cout << diff[3][i] << " \t" << minDiff[3][0] << std::endl;
    }
  }

  Int_t n = DAC_SETTINGS;

  Double_t *diffValue[_NUM_DIFFS_];
  Double_t *diffError[_NUM_DIFFS_];
  for(Int_t f=0; f<_NUM_DIFFS_; f++ ) {
    diffValue[f] = new Double_t[n];
    diffError[f] = new Double_t[n];
  }
  Double_t *percentValue[_NUM_PERCENTS_];
  Double_t *percentError[_NUM_PERCENTS_];
  for(Int_t f=0; f<_NUM_PERCENTS_; f++ ) {
    percentValue[f] = new Double_t[n];
    percentError[f] = new Double_t[n];
  }
  Double_t *slopeValue[_NUM_SLOPES_];
  Double_t *slopeError[_NUM_SLOPES_];
  for(Int_t f=0; f<_NUM_SLOPES_; f++ ) {
    slopeValue[f] = new Double_t[n];
    slopeError[f] = new Double_t[n];
  }

  Double_t diffSetting[n];
  Double_t varValue[n];
  Double_t varError[n];
  Double_t bothValue[n];
  Double_t bothError[n];
  Double_t offValue[n];
  Double_t offError[n];
  Double_t deltaValue[n];
  Double_t deltaError[n];
  for(Int_t setting = 0; setting < n; setting++ ) {
    TString titles[4] = {"Variable","Delta","Both On","Both Off"};
    TH1F *hists[4];
    TString names[4] = {
      Form("VariableHist_%d",setting),
      Form("BothHist_%d",setting),
      Form("NoneHist_%d",setting),
      Form("DeltaHist_%d",setting)
    };
    for (Int_t led = 0; led < 4; led++ ) {
      hists[led] = new TH1F(names[led],titles[led]+";Pulse Integral",100,
          min[setting][led],max[setting][led]);
      //std::cout << "Will create: " << min[setting][led] << "," << max[setting][led] << std::endl;
    }
    TH1F *diffH[_NUM_DIFFS_];
    diffH[0] = new TH1F(Form("DiffHist0_%d",setting),
        "Variable - Off",100,minDiff[0][setting],maxDiff[0][setting]);
    diffH[1] = new TH1F(Form("DiffHist1_%d",setting),
        "Delta - Off",100,minDiff[1][setting],maxDiff[1][setting]);
    diffH[2] = new TH1F(Form("DiffHist2_%d",setting),
        "Both - Off",100,minDiff[2][setting],maxDiff[2][setting]);
    diffH[3] = new TH1F(Form("DiffHist3_%d",setting),
        "Variable - Delta",100,minDiff[3][setting],maxDiff[3][setting]);
    diffH[4] = new TH1F(Form("DiffHist4_%d",setting),
        "Both - Variable",100,minDiff[4][setting],maxDiff[4][setting]);
    diffH[5] = new TH1F(Form("DiffHist5_%d",setting),
        "Both-Delta",100,minDiff[5][setting],maxDiff[5][setting]);
    diffH[6] = new TH1F(Form("DiffHist6_%d",setting),
        "Both-Variable-Delta-Off",100,minDiff[6][setting],maxDiff[6][setting]);

    TH1F *percentH[_NUM_PERCENTS_];
    percentH[0] = new TH1F(Form("PercentHist0_%d",setting),
        "(Both-Variable)/Variable",100,minPercent[0][setting],maxPercent[0][setting]);
    percentH[1] = new TH1F(Form("PercentHist1_%d",setting),
        "(Both-Delta)/Delta",100,minPercent[1][setting],maxPercent[1][setting]);
    percentH[2] = new TH1F(Form("PercentHist2_%d",setting),
        "(Both-Variable)/(Variable-Off)",100,minPercent[2][setting],maxPercent[2][setting]);
    percentH[3] = new TH1F(Form("PercentHist3_%d",setting),
        "(Both-Delta)/(Delta-Off)",100,minPercent[3][setting],maxPercent[3][setting]);

    TH1F *slopeH[_NUM_SLOPES_];
    slopeH[0] = new TH1F(Form("SlopeHist0_%d",setting),
        "(Both-Variable)/Delta",100,minSlope[0][setting],maxPercent[0][setting]);
    slopeH[1] = new TH1F(Form("SlopeHist1_%d",setting),
        "(Both-Variable)/(Delta-Off)",100,minSlope[1][setting],maxPercent[1][setting]);




    for( Int_t i = 0; i < quartets; i++ ) {
      //std::cout << "DAC_VALUE: " << DAC_VALUES[setting] << " \tdacValue: " << dacValue[i] << " \ti: " << i << std::endl;
      if(DAC_VALUES[setting] == dacValue[i]) {
        for( Int_t led = 0; led < 4; led++ ) {
          hists[led]->Fill(values[led][i]);
          //std::cout << "Fill(" << led << "," << dacValue[i] << ") = " << values[led][i]  << std::endl;
        }
        for(Int_t d = 0 ; d < _NUM_DIFFS_; d++ ) {
          diffH[d]->Fill(diff[d][i]);
        }
        for(Int_t d = 0 ; d < _NUM_PERCENTS_; d++ ) {
          percentH[d]->Fill(percent[d][i]);
        }
        for(Int_t d = 0 ; d < _NUM_SLOPES_; d++ ) {
          slopeH[d]->Fill(slope[d][i]);
        }
      }
    }


    gSystem->mkdir(Form("www/run_%d",runnum));
    TCanvas *canvas = new  TCanvas(Form("canvas_%d",setting),Form("canvas_%d",setting),1500,3000);
    canvas->Divide(2,2);
    for( Int_t led = 0; led < 4; led++ ) {
      canvas->cd(led+1);
      hists[led]->Draw();
        switch(led) {
          case _VAR_LED_ :
            varValue[setting] = hists[led]->GetMean();
            varError[setting] = hists[led]->GetRMS()/TMath::Sqrt(hists[led]->GetEntries());
            break;
          case _DELTA_LED_:
            deltaValue[setting] = hists[led]->GetMean();
            deltaError[setting] = hists[led]->GetRMS()/TMath::Sqrt(hists[led]->GetEntries());
            break;
          case _OFF_LED_:
            offValue[setting] = hists[led]->GetMean();
            offError[setting] = hists[led]->GetRMS()/TMath::Sqrt(hists[led]->GetEntries());
            break;
          case _BOTH_LED_:
            bothValue[setting] = hists[led]->GetMean();
            bothError[setting] = hists[led]->GetRMS()/TMath::Sqrt(hists[led]->GetEntries());
            break;
        }
    }
    canvas->SaveAs(Form("www/run_%d/led_%d_%d_%s_leds.png",runnum,runnum,setting,type.Data()));
    canvas->Clear();
    canvas->Divide(2,4);
    for(Int_t e=0; e<_NUM_DIFFS_; e++ ) {
      canvas->cd(e+1);
      diffH[e]->Draw();
      diffValue[e][setting] = diffH[e]->GetMean();
      diffError[e][setting] = diffH[e]->GetRMS()/TMath::Sqrt(diffH[e]->GetEntries());
    }
    canvas->SaveAs(Form("www/run_%d/led_%d_%d_%s_differences.png",runnum,runnum,setting,type.Data()));
    canvas->Clear();
    canvas->Divide(2,2);
    for(Int_t e=0; e<_NUM_PERCENTS_; e++ ) {
      canvas->cd(e+1);
      percentH[e]->Draw();
      percentValue[e][setting] = percentH[e]->GetMean();
      percentError[e][setting] = percentH[e]->GetRMS()/TMath::Sqrt(percentH[e]->GetEntries());
    }
    /*
    canvas->SaveAs(Form("www/run_%d/led_%d_%d_%s_percentages.png",runnum,runnum,setting,type.Data()));
    canvas->Clear();
    canvas->Divide(1,2);
    for(Int_t e=0; e<_NUM_SLOPES_; e++ ) {
      canvas->cd(e+1);
      slopeH[e]->Draw();
      slopeValue[e][setting] = slopeH[e]->GetMean();
      slopeError[e][setting] = slopeH[e]->GetRMS()/TMath::Sqrt(slopeH[e]->GetEntries());
    }
    canvas->SaveAs(Form("www/run_%d/led_%d_%d_%s_slopes.png",runnum,runnum,setting,type.Data()));

    */
    diffSetting[setting] = DAC_VALUES[setting];
    std::cout
      << DAC_VALUES[setting] << " \t"
      << varValue[setting]  << " \t" << varError[setting] << " \t"
      << deltaValue[setting]  << " \t" << deltaError[setting] << " \t"
      << bothValue[setting]  << " \t" << bothError[setting] << " \t"
      << offValue[setting]  << " \t" << offError[setting] << " \t";
    for(Int_t e=0; e<_NUM_DIFFS_; e++ ) {
      std::cout <<  diffValue[e][setting] << " \t" << diffError[e][setting] << " \t";
    }
    /*
    for(Int_t e=0; e<_NUM_PERCENTS_; e++ ) {
      std::cout <<  percentValue[e][setting] << " \t" << percentError[e][setting] << " \t";
    }
    for(Int_t e=0; e<_NUM_SLOPES_; e++ ) {
      std::cout <<  slopeValue[e][setting] << " \t" << slopeError[e][setting] << " \t";
    }
    */
    std::cout << std::endl;

//    canvas->cd(11);
//    Int_t sample = (entries-foundAt)/2;
//    while((foundAt+sample)%4!=(foundAt+1)%4) {
//      sample++;
//    }
//    chain->GetEntry(foundAt+sample);
//    TGraph *graph = sampled_events->at(0).GetGraph();
//    graph->SetTitle("Sample pulse (both ON)");
//    graph->Draw("AL");
//    gSystem->mkdir(Form("www/run_%d",runnum));
//    canvas->SaveAs(Form("www/run_%d/led_%d_%d_%s.png",runnum,runnum,setting,type.Data()));
    canvas->Clear();
  }
  //std::cout << 6648.15 << "DAC: " <<  dacValue[11000] << "Integral: " << values[0][11000] << std::endl;
}
