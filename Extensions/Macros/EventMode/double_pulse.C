/*
 * Short ROOT script to look at multihit structures in the Q-weak Event Mode
 * analysis.
 *
 * Joshua Magee * magee@jlab.org
 * Feb 11, 2015
 */

//non-ROOT, c++ libraries
#include <string>
#include <cctype> //isdigit

//ROOT based libraries
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TAxis.h>
#include <TPaveStats.h>
#include <TPaveLabel.h>
#include <TLegend.h>

#include <QwEvent.h>
#include <QwHit.h>

using namespace std;
using std::string;
using std::vector;


//now for for plotting functions
void placeAxis(TString, TString, TString, TCanvas*, TH1F*);
void greenHisto(TH1F*);
void setup_stats(TPaveStats *, float, float, float, float);
void placeLabel(TString, float, float, float, float);

void double_pulse(int runNumber, int lowEvent=0, int highEvent=4000000, int octant=1, int tsoctant=2,bool savePlot=true)
{
  gROOT->Reset();
  gROOT->SetStyle("Modern");
  gStyle->SetOptStat(1);

  //open file

  //open rootfile
  char filename[100];
  sprintf(filename,"Qweak_%i.root",runNumber);
  TFile *file = new TFile(filename);
  if ( file->IsOpen() ) {
    std::cout <<"Successfully opened ROOTFILE " <<file->GetName() <<std::endl;
  } else {
    std::cout <<"Error opening ROOTFILE " << file->GetName() <<std::endl;
    return;
  }


  //since the file is open we need to get everything set up to walk through the
  //tree, event-by-event.
  TTree*   event_tree   = (TTree*) file ->Get("event_tree");
  TBranch* event_branch = event_tree->GetBranch("events");
  QwEvent* qwevent     = 0; //create a pointer to an individual QwEvent
  event_branch->SetAddress(&qwevent); //set the branch to the correct address

  long n_entries = event_tree->GetEntries(); //number of events to loop over

   /* We need to choose which detectors we are looking at.
  * The following nomenclature is used:
  *
  * fRegion  [1,5]: 4 is TS, 5 is MD
  * fPlane   [1,2]: for TS, package 1 or 2
  *          [1,8]: for MD, this is octant number
  * fElement [1,3]: 1 is positive tube
  *                 2 is negative tube
  *                 3 is tube average
  */

  const int tsRegion = 4;
  const int mdRegion = 5;
  const int mdPlane = octant;
  const int tsPlane = tsoctant;

  //define data structures we care about
  vector<double> mdhits_temp; //vector to store individual hits from each event
  //vector<double> mdhits_diff1; //stores hit 1 - hit 0
  //vector<double> mdhits_diff2; //stores hit 2 - hit 1

  //create histogram to store number of hits per event
  const int Nbins = 250;
  const int xmax  = 2000;
  //const int Nbins = 320;
  //const int xmax  = 1600;
  const int xmin  = 0;
  const int resolution = (xmax-xmin)/Nbins;
  TH1F *hDiff1 = new TH1F("hDiff1","title",Nbins,xmin,xmax);

  //loop over entries
  for(long j=lowEvent; j<n_entries; j++) {
    if (j<lowEvent) {continue;} //only look at events we care about
    if (j>highEvent) {break;}
    if (j % 10000==0) {
      printf("Processing event #%i out of %i\n",j,n_entries);
    }
    event_tree->GetEntry(j);

    //variable to determine whether we have a true hit, as defined by the TDC
    bool isGoodHit = false;

    //loop over individual hits
    int n_hit = qwevent->GetNumberOfHits();
    for(int k=0; k<n_hit; k++) {
      const QwHit* qwhit = qwevent->GetHit(k);
      const int fRegion  = qwhit  -> GetRegion();
      const int fElement = qwhit  -> GetElement();
      const int fPlane   = qwhit  -> GetPlane();

      //check - if the TS fired in the appropriate timing window, count this
      //as a good event
      if (fRegion == tsRegion && fPlane==tsPlane && fElement==3) {
        if (qwhit->GetTimeNs()>-186 && qwhit->GetTimeNs()<-178) {
          isGoodHit = true;
        }
      }

      if (fRegion == mdRegion && fPlane==mdPlane && fElement==3) {
        mdhits_temp.push_back(qwhit->GetTimeNs());
      }

    } //end loop over individual hits

    //if this is a good hit, we want to process this information
    if (isGoodHit) {
      //look at difference between hits 1 - hit 0
      if(mdhits_temp.size()>=2) {
        hDiff1->Fill(mdhits_temp[1] - mdhits_temp[0]);
      }
    }
    mdhits_temp.clear();
  } //end loop over entries

  //generic dummy variables
  TPaveStats *stats;
  string title, xaxis, yaxis;
  char subtitle[100];
  sprintf(subtitle,"(Run %i,%i,%i)",runNumber,lowEvent,highEvent);

  //now plot histograms and all that good stuff
//start plotting hit differences
  TCanvas *canvas2 = new TCanvas("canvas2","title");
  canvas2->cd();

  greenHisto(hDiff1);
  hDiff1->Draw();

  title = "Time Difference Between Successive Hits (MDall)";
  xaxis = "Time difference (ns)";
  yaxis = "";
  placeAxis(title.data(),xaxis.data(),yaxis.data(),canvas2,hDiff1);
  placeLabel(subtitle,0.36,0.8,0.64,0.89);

  stats = (TPaveStats*) hDiff1->GetListOfFunctions()->FindObject("stats");
  if (stats) {
    setup_stats(stats,0.7,0.75,0.9,0.9);
    stats->SetTextColor(kGreen+3);
    stats->Draw("sames");
  }

  gPad->Update();
  gPad->Modified();

  TF1 *fAvg = new TF1("fAvg","pol0");
  fAvg->SetRange(300,570);
  fAvg->SetLineColor(kRed);
  fAvg->SetLineStyle(7);
  hDiff1->Fit("fAvg","R");

  const double diff1max = hDiff1->GetMaximum();
  const double diff1maxbin = hDiff1->GetMaximumBin(); 
  const double plateau = fAvg->GetParameter(0);
//  const double plateau = 10;
  std::cout <<"Plateau's value: \t" <<plateau <<std::endl;

  //start by walking *up* from bin 0
  double diff1halfmax = 0;
  double diff1halfplateau = 0;
  for(int j=0; j<diff1maxbin; j++) {
    if(hDiff1->GetBinContent(j) >= (diff1max/2) && diff1halfmax==0) {
      diff1halfmax = hDiff1->GetBin(j);
    }
    if(hDiff1->GetBinContent(j) >= (plateau/2) && diff1halfplateau==0) {
      diff1halfplateau = hDiff1->GetBin(j);
    }
    if(diff1halfmax!=0 && diff1halfplateau!=0)
      break;
  }

  std::cout <<"From below..." <<std::endl;
  std::cout <<"Average [300:570] \tResolution \tHalfMax \tHalfPlateau"
    <<" \tTau_max \tTau_plat" <<std::endl;
  std::cout <<plateau <<" \t\t\t" <<resolution <<" \t\t" <<diff1halfmax
    <<" \t\t" <<diff1halfplateau <<" \t\t" <<diff1halfmax*resolution
    <<" \t\t" <<diff1halfplateau*resolution <<std::endl;

/*  const double diff1halfplateauBelow = diff1halfplateau;
  //start by walking *down* from bin 0
  diff1halfmax = 0;
  diff1halfplateau = 0;
  for(int j=diff1maxbin; j>0; j--) {
    if(hDiff1->GetBinContent(j) <= (diff1max/2) && diff1halfmax==0) {
      diff1halfmax = hDiff1->GetBin(j);
    }
    if(hDiff1->GetBinContent(j) <= (plateau/2) && diff1halfplateau==0) {
      diff1halfplateau = hDiff1->GetBin(j);
    }
    if(diff1halfmax!=0 && diff1halfplateau!=0)
      break;
  }

  std::cout <<"From above..." <<std::endl;
  std::cout <<plateau <<" \t" <<resolution <<" \t" <<diff1halfmax
    <<" \t" <<diff1halfplateau <<" \t" <<diff1halfmax*resolution
    <<" \t" <<diff1halfplateau*resolution <<std::endl;

  const double diff1halfplateauAbove = diff1halfplateau;
  std::cout <<resolution <<" \t"
    <<diff1halfplateauBelow <<" \t" <<diff1halfplateauAbove <<" \t"
    <<diff1halfplateauBelow*resolution <<"\t "
    <<diff1halfplateauAbove*resolution <<std::endl;
*/
  if(savePlot==true) {
    //save canvases
    //canvas2->SaveAs(Form("plots/mdall/hitdiff_%i_%i.eps",runNum,octant));
    canvas2->SaveAs(Form("hitdiff_%i_%i.png",runNumber,octant));
  }

} //end double_pulse function

int getRunNumberFromFile(string filename) {
  string runnum;

  for(size_t j=0; j<filename.length(); j++) {
    if ( isdigit(filename[j]) ) {
      runnum += filename[j];
    }
  }

  return atoi(runnum.data());
} //end function getNumberFromFile

void placeAxis(TString title, TString xaxis, TString yaxis, TCanvas *canvas, TH1F* hist) {
  float size=0.04;
  float titlesize=0.04;
  float labelsize = 0.035;
  hist->SetTitle(title);
  hist->GetXaxis()->SetTitle(xaxis);
  hist->GetYaxis()->SetTitle(yaxis);

  canvas->Modified();
  canvas->Update();

  hist->GetYaxis()->SetTitleOffset(0.35);
  hist->GetXaxis()->SetTitleOffset(1.10);
  hist->GetYaxis()->SetTitleSize(size);
  hist->GetXaxis()->SetTitleSize(size);
  hist->GetYaxis()->SetLabelSize(labelsize);
  hist->GetXaxis()->SetLabelSize(labelsize);

  TPaveText *tit = (TPaveText*) gPad->GetPrimitive("title");
  tit->SetTextSize(titlesize);
  //gStyle->SetTitleSize(titlesize);

  gPad->SetGrid();
  canvas->Modified();
  canvas->Update();
}

void greenHisto(TH1F *hist) {
  const int color = kGreen+2;
  hist->SetLineColor(color);
  hist->SetFillColor(color);
  hist->SetFillColorAlpha(color,0.70);
}


void placeLabel(TString text, float x1, float y1, float x2, float y2) {
 TPaveLabel *subtitle = new TPaveLabel(x1,y1,x2,y2,text,"NDC");
 subtitle->SetBorderSize(0);
 subtitle->SetFillColor(0);
 subtitle->SetTextSize(0.3);
 //  subtitle->SetTextColor(46);
 subtitle->Draw();
}


void setup_stats(TPaveStats *stats, float x1, float y1, float x2, float y2) {
  stats->SetFillColor(0);
  stats->SetX1NDC(x1);
  stats->SetY1NDC(y1);
  stats->SetX2NDC(x2);
  stats->SetY2NDC(y2);
}







