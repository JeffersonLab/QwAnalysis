// Name  :   DrawHits.C
// Author:   Jeong Han Lee
// email :   jhlee@jlab.org
// Date:     Tue Nov 10 16:55:06 EST 2009
// Version:  0.0.0
//
//  An example script that uses a "generic ROOT way" in order to draw
//  some useful information from "hit-based" ROOT output file created 
//  by the QwAnalysis
//
//
// History:
// 0.0.0     2009/11/10   created
//
// 0.0.1     2009/11/11   introduce a "premature" access_hit()
//                        

// Examples
//
// root [0] .L DrawHits.C                                                               
// root [1] load_libraries()
// root [2] Int_t run_number=398
// root [3]  TFile file(Form("%s/Qweak_%d.root", getenv("QW_ROOTFILES_DIR"),run_number));
// root [4]  TTree* tree = (TTree*) file.Get("event_tree");
// root [5]   tree->Draw("events.fQwHits.fRawTime", "events.fQwHits.fChannel==16");
// <TCanvas::MakeDefCanvas>: created default TCanvas with name c1
// root [6] 

// or 

// root [0] .L DrawHits.C
// root [1] load_libraries()
// root [2] draw_hit()



void 
load_libraries()
{
  gSystem->Load("./libQwHit.so");
  gSystem->Load("./libQwHitContainer.so");
  gSystem->Load("./libQwHitRootContainer.so");
}



// This is a small example to draw what you want to see in the "hit-based" ROOT file
// Actually, you don't need to load "libraries" before. But, if you load the 
// "libraries" before you type some commands, you can access all member functions 
// of three classes: QwHit, QwHitContainer, and QwHitRootContainer.
// (see the Tracking/include/ directory)



void 
draw_hit(Int_t run_number=398)
{
  TFile file(Form("%s/Qweak_%d.root", getenv("QW_ROOTFILES_DIR"),run_number));
  TTree* tree = (TTree*) file.Get("event_tree");


  /*---------------------    First Example Begin  ------------------------*/
  /*                                                                      */
  /* Histogram of the raw time value of all hits on a given TDC channel   */

  tree->Draw("events.fQwHits.fRawTime", "events.fQwHits.fChannel==16")  ;

  /*                                                                      */
  /*---------------------    First Example End    ------------------------*/



  /*---------------------    Second Example Begin  -----------------------*/
  /*                                                                      */
  /* Histogram of the raw time value of all hits that are assigned to a   */
  /* given drift chamber wire.                                            */ 
  
  // tree->Draw("events.fQwHits.fRawTime", "events.fQwHits.fElement==4")  ;

  /*                                                                      */
  /*---------------------    First Example End    ------------------------*/


  // In principle, you can use the following variables that are defined in
  // QwHit.h (Tracking/include/ directory) according to what you desire to
  // see 
  //

  //   Int_t fCrate;     ///< ROC number
  //   Int_t fModule;    ///< F1TDC slot number, or module index
  //   Int_t fChannel;   ///< Channel number
  //   Int_t fHitNumber; ///< Index for multiple hits in a single channel on the left
  //   Int_t fHitNumber_R;    ///< Index for multiple hits in a single channel on the right
  
  //   Int_t fRegion;    ///< Region 1, 2, 3, trigger scint., or cerenkov
  //   Int_t fPackage;   ///< Which arm of the rotator, or octant number
  //   Int_t fDirection; ///< Direction of the plane:  X, Y, U, V; R, theta; etc.
  //   Int_t fPlane;     ///< R or theta index for R1; plane index for R2 & R3
  //   Int_t fElement;   ///< Trace # for R1; wire # for R2 & R3; PMT # for others

  //   Bool_t fAmbiguousElement;  ///< TRUE if this hit could come from two different elements (used by Region 3 tracking only)
  //   Bool_t fLRAmbiguity;       ///< TRUE if the other element is 8 wires "right"; FALSE if the other element is 8 wires "left" (used by Region 3 tracking only)
  
  
  //   Double_t fRawTime;    ///< Time as reported by TDC; it is UNSUBTRACTED
  //   Double_t fTime;       ///< Start corrected time, may also be further modified
  //   Double_t fTimeRes;    ///< Resolution of time (if appropriate)
  
  //   Double_t fDistance;   ///< Perpendicular distance from the wire to the track,
  //                         ///  as reconstructed from the drift time
  //   Double_t fPosition;   ///< Reconstructed position of the hit in real x, u, v
  //                         ///  coordinates perpendicular to the wires
  //   Double_t fResidual;   ///< Residual of this hit (difference between the drift
  //                         ///  distance and the distance to the fitted track)
  //   Double_t fZPos;       ///< Lontigudinal position of the hit (this is mainly
  //                         ///  used in region 3 where the z coordinate is taken
  //                         ///  in the wire plane instead of perpendicular to it)
  
  //   Double_t fSpatialResolution;	/// Spatial resolution
  //   Double_t fTrackResolution;	/// Track resolution
  
  //     Double_t rResultPos;	/// Resulting hit position
  //     Double_t rPos;		/// Position from level I track finding
  //     Double_t rPos2;		/// Position from level II decoding
  
}




void 
access_hit(Int_t run_number=398)
{

  TStopwatch timer;
  timer.Start();

  TFile file(Form("%s/Qweak_%d.root", getenv("QW_ROOTFILES_DIR"),run_number));
  if (file.IsZombie()) 
    {
      printf("Error opening file\n");  
      exit(-1);
    }
  else
    {
      TTree* tree = (TTree*) file.Get("event_tree");
      QwHitRootContainer* hitContainer = NULL;
      tree->SetBranchAddress("events",&hitContainer);
      tree->Draw(">>TrawList", "fQwHits.fRawTime");

      TEventList* TrawList=(TEventList*)gDirectory->Get("TrawList");
      Int_t    nevent      = 0;
      Int_t    nhit        = 0;
      Double_t tdc_rawtime = 0.0;

      nevent = TrawList->GetN();

      printf("total event %d\n", nevent);

      for (Int_t i=0;i<nevent;i++) 
	{
	  tree->GetEntry(TrawList->GetEntry(i));
	  nhit = hitContainer->GetSize();
	  if( i%10000 == 0) printf(" nhit %12d %8d\n", i, nhit);

	}
    }

  file.Close();

  timer.Stop();

  std::cout << "CPU time used:  " << timer.CpuTime() << " s" << std::endl
	    << "Real time used: " << timer.RealTime() << " s" << std::endl;
}
