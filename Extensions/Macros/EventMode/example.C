/* This example script demonstrates how to use the F1TDC MultiHit features. The
 * basic idea is one must walk through each event, and for every even walk
 * through each hit. Although each TDC can only record 7 hits, the QwAnalyzer
 * lumps all hits together, and one must untangle the desired detector from the
 * others.
 *
 * Nomenclature is as follows:
 *  fRegion  [1,5]: 4 is TS, 5 is MD
 *  fPlane   [1,2]: for TS, package 1 or 2
 *           [1,8]: for MD, this is octant number
 *  fElement [1,3]: 1 is positive tube
 *                  2 is negative tube
 *                  3 is tube average
 *
 * After collecting the data into a vector, it will plot a basic histogram.
 *
 * To use, one should analyze the desired run using options
 *  -o "enable--md-software-meantime 1 --enable-ts-software-meantime 1"
 * One must also use the QwAnalysis libraries (qwroot), as shown below.
 *
 * Author: Josh Magee
 *         magee@jlab.org
 *         December 2, 2014
 */

//need to include relevant root libraries
#include <TROOT.h>
#include <TFile.h> //need to read TFiles
#include <TTree.h> //need to read TTrees
#include <TH1F.h> //want to create a histogram

//need these specific QwAnalysis libraries
#include <QwEvent.h> //class for multi-hit events
#include <QwHit.h> //class for individual multi-hits

using std::cout;
using std::endl;
using std::vector;

void example(std::string filename, int lowEvent, int highEvent) {
  gROOT->Reset(); //reset ROOT enviroment from previous work

  //open rootfile or exit
  TFile *file = new TFile(filename.data());
  if ( file->IsOpen() ) {
    std::cout <<"Your rootfile " <<filename.data() <<"is open." <<std::endl;
  } else {
    std::cout <<"Your rootfile " <<filename.data() <<"is NOT open.\n"
      <<"Exiting." <<std::endl;
    exit(EXIT_FAILURE);
  }

  //create vector to hold data
  std::vector<double> data;

  //the TDC information is in the TTree event_tree
  //the individual hits are contained in the events branch
  TTree*   event_tree   = (TTree*) file ->Get("event_tree");
  TBranch* event_branch = event_tree->GetBranch("events");
  QwEvent* qwevent     = 0; //create a pointer to an individual QwEvent
  event_branch->SetAddress(&qwevent); //set the branch to the correct address

  long n_entries = event_tree->GetEntries(); //number of events to loop over

  //loop over entries
  for(long j=0; j<n_entries; j++) {
    if (j<lowEvent) {continue;} //only look at events we care about
    if (j>highEvent) {break;}
    if (j % 10000==0) {
      printf("Processing event #%i out of %i\n",j,n_entries);
    }
    event_tree->GetEntry(j);

    int n_hit = qwevent->GetNumberOfHits();
    //loop over individual hits
    for(int k=0; k<n_hit; k++) {
      const QwHit* qwhit = qwevent->GetHit(k);
      const int fregion  = qwhit  -> GetRegion();
      const int felement = qwhit  -> GetElement();
      const int fplane   = qwhit  -> GetPlane();

      //we are looking at region 5 (MD), octant 1 tube average
      if (fregion==5 && fplane==1 && felement==3)  {
        data.push_back(qwhit->GetTimeNs());
      }
    } //end loop over individual hits
  } //end loop over entries

  std::cout <<"Plotting histogram of hit times in ns...\n";
  //the time window to record events is 2 micro-seconds long
  //there is a short ~200 ns reset window which isn't used
  TH1F *hist = new TH1F("hist","MD1 Tube Avg Hit Times in ns",1800,-400,1400);
  hist->FillN(data.size(),data.data(),NULL); //fill histogram with data
  hist->Draw(); //draw histogram

  std::cout <<"Script successfully completed." <<std::endl;
} //end example script

