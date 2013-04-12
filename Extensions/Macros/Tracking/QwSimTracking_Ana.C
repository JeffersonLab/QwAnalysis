// jpan@jlab.org, March 2013
// Comparing Geant4-simulated events and qwsimtracking-analyzed events

// ============================================================================
// function: compare_R2(int start, int end, int cs)
// for comparing theta angles obtained from different methods
// theta 1: from event generator of simulation, i.e., scattering angle at vertex
// theta 2: from simulated Region 2 partial tracks
// theta 3: from reconstructed Region 2 partial tracks
//
// cs = 0 (no weight), 
// cs = 1 (with cross-section weight), 

void compare_R2(int start=1, int end=10, int cs=0)
{
  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1);
  //gStyle->SetOptStat(0);

  TString Path = "$QW_ROOTFILES/"; // qwsimtracking-analyzed events

  TH1F* theta_angle1 = new TH1F("theta_angle1","#theta_{1} from event generator (at vertex)",100,3,14);
  theta_angle1->SetLineColor(kRed);
  theta_angle1->GetXaxis()->SetTitle("#theta [deg]");

  TH1F* theta_angle2 = new TH1F("theta_angle2","#theta_{2} from simulated R2 partial track",100,3,14);
  theta_angle2->SetLineColor(kGreen);
  theta_angle2->GetXaxis()->SetTitle("#theta [deg]");

  TH1F* theta_angle3 = new TH1F("theta_angle3","#theta_{3} from reconstructed R2 partial track",100,3,14);
  theta_angle3->SetLineColor(kBlue);
  theta_angle3->GetXaxis()->SetTitle("#theta [deg]");

  TH1F* dtheta1 = new TH1F("dtheta1","#Delta #theta_{1} = #theta_{2} - #theta_{1}",100,-1.5,1.5);
  dtheta1->SetLineColor(kRed);
  dtheta1->GetXaxis()->SetTitle("#Delta #theta [deg]");

  TH1F* dtheta2 = new TH1F("dtheta2","#Delta #theta_{2} = #theta_{3} - #theta_{1}",100,-1.5,1.5);
  dtheta2->SetLineColor(kGreen);
  dtheta2->GetXaxis()->SetTitle("#Delta #theta [deg]");

  TH1F* dtheta3 = new TH1F("dtheta3","#Delta #theta_{3} = #theta_{3} - #theta_{2}",100,-1.5,1.5);
  dtheta3->SetLineColor(kBlue);
  dtheta3->GetXaxis()->SetTitle("#Delta #theta [deg]");

  TH1F* chi_3 = new TH1F("chi_3","#chi-distribution in R2",100,0,14);
  chi_3->SetLineColor(kBlue);
  chi_3->GetXaxis()->SetTitle("#chi");

  double theta1, theta2, theta3;

  QwEvent *fEvents=0;
  QwEvent *fOriginals=0;
  QwTrack* track=0;
  QwPartialTrack* pt=0;


  for (Int_t ifile = start; ifile <= end; ifile++)
  {
    TFile* rootfile = new TFile(Path+Form("QwSim_%i.root", ifile),"r");
    cout<<"\nOpened file: "<<Path+Form("QwSim_%i.root", ifile)<<endl;

    TTree* evt_tree = (TTree*)gROOT->FindObject("event_tree");
  
    TBranch* event_branch=evt_tree->GetBranch("events");
    event_branch->SetAddress(&fEvents);
  
    TBranch* original_branch=evt_tree->GetBranch("originals");
    original_branch->SetAddress(&fOriginals);


    int NumOfEntries = evt_tree->GetEntries();
    for(int i=0;i<NumOfEntries;i++)  // start to loop over all events
    {

      if(i%100000==0)
        cout << "events processed so far: " << i << endl;

      evt_tree->GetEntry(i);

      double Q2_val = fEvents->fKinElasticWithLoss.fQ2;
  
      if(Q2_val>0.01 && Q2_val<0.08)
      {
         // Get cross-section
         double xsect;
         if (cs==1)
             xsect = fOriginals->fCrossSection;             
         else
             xsect = 1.0;

         // vertex info from event generator
         theta1 = fOriginals->fScatteringAngle;
         //cout<<"\ntheta1 from event generator (at vertex): "<<theta1<<endl;

          // simulated original events branch
         double npartialtracks=fOriginals->GetNumberOfPartialTracks();
         for(int j=0;j<npartialtracks; j++)
         {

             pt=fOriginals->GetPartialTrack(j);
             if (pt->GetRegion()==2)
             {
                theta2 = pt->GetMomentumDirectionTheta()*180.0/3.1415926535897932;
                //cout<<"theta2 from simulated R2 partial track: "<<theta2<<endl;
             }
          }

         // reconstructed events branch
         double ntracks=fEvents->GetNumberOfTracks();
         for(int j=0;j<ntracks; j++)
         {
             track=fEvents->GetTrack(j);
             pt = track->fFront;

             theta3 = pt->GetMomentumDirectionTheta()*180.0/3.1415926535897932;
             //cout<<"theta3 from reconstructed R2 partial track: "<<theta3<<endl;

             double chi = pt->fChi;             
          }

          theta_angle1->Fill(theta1,xsect);
          theta_angle2->Fill(theta2,xsect);
          theta_angle3->Fill(theta3,xsect);

          dtheta1->Fill(theta2-theta1,xsect);
          dtheta2->Fill(theta3-theta1,xsect);
          dtheta3->Fill(theta3-theta2,xsect);

          chi_3->Fill(chi,xsect);
           
       } // end of if(Q2_val>0.01 && Q2_val<0.08)

     } // end of event loop

     rootfile->Close();
     cout<<"\nProcessed file: "<<Path+Form("QwSim_%i.root", ifile)<<endl;

   } // end of file loop


   TCanvas* c1 = new TCanvas("c1","theta angles obtained from different methods",1200,800);
   c1->Divide(3,2);

   c1->cd(1);
   theta_angle1->Draw();

   c1->cd(2);
   theta_angle2->Draw();

   c1->cd(3);
   theta_angle3->Draw();

   c1->cd(4);
   dtheta1->Draw();

   c1->cd(5);
   dtheta2->Draw();

   c1->cd(6);
   dtheta3->Draw();

   TCanvas* c2 = new TCanvas("c2","theta angle differences",800,400);
   c2->Divide(2,1);

   c2->cd(1);
   theta_angle3->Draw();
   theta_angle2->Draw("sames");
   theta_angle1->Draw("sames");

   c2->cd(2);
   dtheta3->Draw("");
   dtheta2->Draw("sames");
   dtheta1->Draw("sames");

   TCanvas* c3 = new TCanvas("c3","chi-distribution of Region 2",400,400);
   c3->cd();
   chi_3->Draw("");   

} // end of compare_R2

