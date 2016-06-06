// auto_MD_image_pack2.C 
//
// Version 1.0
// May 27 2014
// author: David Armstrong
//
//   This script is for light attenuation studies in the Main Detector using
// projected tracks from Region 3, and for checking Region 3 track 
// reconstruction by "imaging" the Main Detector (hence the name of the script)
//
//  It is only for the MD "imaged" by Region 3 package 2.
//  An essentially identical script, auto_MD_image_pack2.C, does the same
//  thing for package 1.
//
//  Yes, this is inelegant; but the original script did one package
//   at a time, and I don't have the energy to rewrite the code to 
//   have everything as two-element vector,s so that both packages could
//   be handled in the same script - you, dear reader, are welcome to 
//   take this task on, should you be so inclined. 
//
//  The script calculates the mean radial position of the MD (as a diagnostic 
//  of Region 3 tracking), by looking at the "hits" and "misses" of the MD.
//   
//  It also calculates, for each MD, a "slope" of the MD PMT ADC amplitude
//  vs. long-axis location of the R3 partial track (linear fits), on 
//  either side of the  mid-detector glue joint, so one can look at light 
//  attenuation in the MD.
//
//  It also calculates a "glue joint attenuation factor", which is the ratio
//  of the light seen in a given PMT for tracks on either side of the glue joint
//  (by extrapolating fits to the attenuation data to the glue joint location).
//  This is calculated for both of the PMTs on the main detector.
//
//  A series of plots are made and saved as .png files, some of which can
//  be added to the tracking "auto-plots" WWW pages. Some we can leave to 
//  be saved, but not by default written to the WWW pages (to keep the WWW 
//  pages manageable).
//
// Numerical results are  written in txt form to the file (run#)_md_image.txt
// 
//   Originally based on:  r3_lightattenuation.C (Authors: Siyuan Yang, 
//                         Eric Henderson, David Armstrong)
//      
 #include "auto_shared_includes.h"
 #include "TSystem.h"
 #include "TF1.h"
 #include "TH2.h"
 #include "TProfile2D.h"
 #include "TGraph.h"
 #include "TPaveText.h"
 #include "TLeaf.h"
 #include "QwEvent.h"
 #include "QwTrack.h"
 #include "QwHit.h"
 #include <fstream>
 #include <iostream>
 #include <iomanip>

//define a functions 
int DetermineOctantRegion3(TChain* event_tree, int package);

// this below just for octant determination purposes (i.e. not great pedestal)
double adc_pedestal[9] = {0.0, 550.000, 550.000, 550.000, 550.000, 550.000, 550.000
, 550.000, 550.000}; 

const double pe_convert[]={14.0141,16.6726,21.9799,38.5315,20.4254,20.3896,22.1042,22.3945,22.7986,30.0517,28.7274,25.3396,24.6273,16.0718,27.8305,12.3251};

void auto_MD_image(Int_t runnum, Bool_t isFirst100K = kFALSE, int event_start=-1, int event_end=-1, TString stem="Qweak_", TString suffix=""){


  Double_t pdataplus = 0;
  Double_t pdataminus = 0;
  Double_t padcp = 0;
  Double_t padcm = 0;


  Double_t md_zpos[9] = {0.0, 576.665,  576.705, 577.020, 577.425, 577.515,  577.955, 577.885, 577.060};

  //TString outputPrefix(Form(TString(gSystem->Getenv("QWSCRATCH"))+"/tracking/www/run_%d/md_image_%d_",runnum,runnum));

  TString outputPrefix = Form(
      TString(gSystem->Getenv("WEBDIR")) + "/run_%d/%d_",
      runnum, runnum);

// Create and load the chain
  TChain *event_tree = new TChain("event_tree");
 
  event_tree->Add(Form("$QW_ROOTFILES/%s%d%s.root",stem.Data(),runnum,suffix.Data()));
//  event_tree->Add("$QW_ROOTFILES/Qweak_10638.root");

   std::ofstream fout;
   std::ofstream fout2;

  fout.open(outputPrefix+"md_image_p2.txt");
  fout2.open(outputPrefix+"md_attenuation_p2.txt");

  QwEvent* fEvent=0;

// How many events are in this rootfile?

  //get octant number for package two
  int package =2;
  int md_number = DetermineOctantRegion3(event_tree, package);
  cout << "octant number = " << md_number << "  package = 2" << endl; 

  Int_t nevents=event_tree->GetEntries();
  cout << "total events: " << nevents << endl;

// 

  TProfile2D *hp_2d = new TProfile2D("hp_2d profile","h_2d ",130,300.,360.,240,-150.,150.);

  TH1D* adcph = new TH1D("ADCP", Form("ADCP data  MD %d",md_number), 400, 0, 4000);
  TH1D* adcmh = new TH1D("ADCM", Form("ADCM data  MD %d",md_number), 400, 0, 4000);
  TH1D* tdcph = new TH1D("TDCP", Form("TDCP data  MD %d",md_number), 400, -300, 100);
  TH1D* tdcmh = new TH1D("TDCM", Form("TDCM data  MD %d",md_number), 400, -300, 100);
  TH1D* adcpped = new TH1D("ADCPPED", Form("ADCP pedestal  MD %d",md_number), 50, 0, 400);
  TH1D* adcmped = new TH1D("ADCMPED", Form("ADCM pedestal  MD %d",md_number), 50, 0, 400);

  TProfile* adcpp1;
  TProfile* adcpp2;
  TProfile* adcmp1;
  TProfile* adcmp2;
  TProfile* fulladcpp;
  TProfile* fulladcmp;

  adcpp1 = new TProfile("ADCPP1", "ADCP1 profile", 50, -100, 0);
  adcpp2 = new TProfile("ADCPP2", "ADCP2 profile", 50, 0, 100);
  adcmp1 = new TProfile("ADCMP1", "ADCM1 profile", 50, -100, 0);
  adcmp2 = new TProfile("ADCMP2", "ADCM2 profile", 50, 0, 100);
  fulladcpp = new TProfile("FULLADCPP", Form("Linear Fits   Run %d Octant %d positive PMT", runnum, md_number), 150, -105, 105);
  fulladcmp = new TProfile("FULLADCMP", Form("Linear Fits   Run %d Octant %d minus PMT", runnum, md_number), 150, -105, 105);


  TH2F* hits = new TH2F("hits", Form("Run %d Octant %d  Package %d  MD Hits ", runnum, md_number, package), 400, -130, 130, 300, 320, 360);
  TH2F* misses = new TH2F("misses", Form("Run %d Octant %d  Package %d MD Misses", runnum, md_number, package), 400, -130, 130, 300, 320, 360);

  TBranch* branch_event=event_tree->GetBranch("events");
  TBranch* branch     = event_tree->GetBranch("maindet");

  branch_event->SetAddress(&fEvent);

  TLeaf* mdp=branch->GetLeaf(Form("md%dp_adc",md_number));
  TLeaf* mdm=branch->GetLeaf(Form("md%dm_adc",md_number));
  TLeaf* mdp_t=branch->GetLeaf(Form("md%dp_f1",md_number));
  TLeaf* mdm_t=branch->GetLeaf(Form("md%dm_f1",md_number));

  Int_t r3_events =0;
  Double_t dz = 0.0;
  Int_t size = 125;
  Double_t hit[125];
  Double_t trial[125];
  Int_t bin;
  Int_t xmin = 323;
  Double_t width = 0.2;
  Double_t xpos[125];
  Double_t efficiency[125];
 
  dz = md_zpos[md_number];

  // look at a subset of events in our to determine MD ADC pedestals

  for (int i=0;i<10000;i++)
    {

      branch_event->GetEntry(i);
      branch->GetEntry(i);

      for(int num_p=0; num_p < fEvent->GetNumberOfPartialTracks(); num_p++)
	{
	  pdataplus = mdp_t->GetValue();
	  pdataminus = mdm_t->GetValue();
	  padcp = mdp->GetValue();
	  padcm = mdm->GetValue();

	  if(pdataplus == 0 && pdataminus == 0)
	    {
	      adcpped->Fill(padcp);
	      adcmped->Fill(padcm);
	    }
	}
    }

  for (Int_t j = 0;j < size; j = j + 1)
    {
      xpos[j] = (j * width) + xmin;
      hit[j] = 0;
      trial[j] = 0;
    }

  Double_t adcpmean = adcpped->GetMean();
  Double_t adcmmean = adcmped->GetMean();

  //  for short-test purposes only 
  //   for(int i = 0; i < 105000; i++)
  for(int i = 0; i < nevents; i++)
	{
	  if(i % 100000 == 0) cout <<"events processed so far:" << i << endl;
	  branch_event->GetEntry(i);
	  branch->GetEntry(i);

	  Double_t xoffset, yoffset, xslope, yslope, x, y;

	 for (int num_p=0; num_p< fEvent->GetNumberOfPartialTracks();num_p++)
	    {
	      r3_events++;
	      const QwPartialTrack* pt=fEvent->GetPartialTrack(num_p);
              if (pt->GetRegion()==3 && pt->GetPackage()==package)
		{
	      xoffset = pt->fOffsetX;
	      yoffset = pt->fOffsetY;
	      xslope  = pt->fSlopeX;
	      yslope  = pt->fSlopeY;
	      x       = xoffset+xslope*dz;
	      y       = yoffset+yslope*dz;
	      
	      Double_t theta = -45.0*3.14159/180.0*(md_number - 1);
	      Double_t xx = x*cos(theta) - y*sin(theta);
	      Double_t yy = x*sin(theta) + y*cos(theta);
	      x = xx;
	      y = yy;

	      Double_t adcpdata = mdp->GetValue();
	      Double_t adcmdata = mdm->GetValue();
	      Double_t tdcpdata = mdp_t->GetValue();
	      Double_t tdcmdata = mdm_t->GetValue();
	      Double_t weight = adcpdata + adcmdata;

	      if (adcpdata!=0.0 && adcmdata!=0.0 && tdcpdata<-10 && tdcmdata <-10)
                {
		    hp_2d->Fill(x,y,weight);
                }

	      if (tdcpdata < -150 && tdcpdata > -250)
		{
		  adcph->Fill(adcpdata);
		  adcpp1->Fill(y,adcpdata-adcpmean);
		  adcpp2->Fill(y,adcpdata-adcpmean);
		  fulladcpp->Fill(y,adcpdata-adcpmean);
		}

	      if (tdcmdata < -150 && tdcmdata > -250)
		{
		  adcmh->Fill(adcmdata);
		  adcmp1->Fill(y,adcmdata-adcmmean);
		  adcmp2->Fill(y,adcmdata-adcmmean);
		  fulladcmp->Fill(y,adcmdata-adcmmean);

		  if(tdcpdata < -150 && tdcpdata > -250)
		    {
		      hits->Fill(y,x);
		    }
		}

	      if (tdcpdata == 0 && tdcmdata == 0)
		{
		  misses->Fill(y,x);
		}


	      tdcph->Fill(tdcpdata);
	      tdcmh->Fill(tdcmdata);

	      bin = (x - xmin)/width;

	      if (bin < 0 || bin > size - 1)
		{
		  //		  cout << "bin not in x range" << endl;
		}

	      else
		{
		  //		  cout << "the bin is " << bin << endl;
		  trial[bin] = trial[bin] + 1;

		  if (tdcpdata > -250 && tdcpdata < -150)
		    {
		      if (tdcmdata > -250 && tdcmdata < -150)
			{
			  hit[bin] = hit[bin] + 1;
			}
		    }
		}

            }
        }
     }
  
  for (Int_t ii = 0;ii < size;ii = ii + 1)
    {
      if (trial[ii] == 0)
	{
	  efficiency[ii] = 0;
	}

      else
	{
	  efficiency[ii] = hit[ii] / trial[ii];
	}
    }

  Double_t dummyvalue = 0;
  Double_t product;
  Double_t effvalue = 0;
  Double_t effmean;

  for (Int_t w = 0;w < size;w = w + 1)
    {
      effvalue = effvalue + efficiency[w];
      product = xpos[w] * efficiency[w];
      dummyvalue = dummyvalue + product;
    }

  effmean = dummyvalue / effvalue;

  fout << "Run \t Pack. \tOctant\t #R3 PT\t mean r (cm) " << endl;
  fout << runnum << " \t " << package << " \t " << md_number << " \t " << r3_events << " \t " << effmean << endl;

  //  (x,y) plot of where R3 partial tracks project to MD, weighted by summed PMT signals, and
  // their x,y projections as profiles. 

TCanvas  *c = new TCanvas("c","Region 3 Projections",10, 10, 800,800);
  c->Divide(2,2);
  c->cd(2);

  string title="track projection: profile";
  hp_2d->GetXaxis()->SetTitle("position x (cm)");
  hp_2d->GetYaxis()->SetTitle("position y (cm)");
  hp_2d->SetTitle(title.c_str());
  hp_2d->Draw("colz");

  c->cd(1);
  hp_2d->ProjectionX()->Draw();

  c->cd(3);
  hp_2d->ProjectionY()->Draw();

  c->cd(4);
  adcph->Draw();

  c->SaveAs(outputPrefix+"MD_projections_p2.png");
  c->SaveAs(outputPrefix+"MD_projections_p2.C");


  //  plots of MD ADC data for pedestals (no corresponding TDC hit)

TCanvas  *pedestals = new TCanvas("pedestals", "Pedestal Data", 10, 10, 800, 800);
  pedestals->Divide(1,2);

  pedestals->cd(1);
  adcpped->Draw();

  pedestals->cd(2);
  adcmped->Draw();

  pedestals->SaveAs(outputPrefix+"MD_ADC_pedestals_p2.png");
  pedestals->SaveAs(outputPrefix+"MD_ADC_pedestals_p2.C");

  //  plots of MD ADC and TDC data for both PMTs
  //  ADC data are for "prompt TDC hits"

TCanvas  *quad = new TCanvas("quad", "ADC and TDC data", 10, 10, 800, 800);
  quad->Divide(2,2);

  quad->cd(1);
  adcph->Draw();

  quad->cd(2);
  adcmh->Draw();

  quad->cd(3);
  tdcph->Draw();

  quad->cd(4);
  tdcmh->Draw();

  quad->SaveAs(outputPrefix+"MD_ADC_and_TDC_p2.png");
  quad->SaveAs(outputPrefix+"MD_ADC_and_TDC_p2.C");


  // x,y plots of where R3 partial tracks project to MD, for events that fire the MD ("hits") and 
  //   those for which the MD did not fire ("misses")

TCanvas  *hitandmiss = new TCanvas("hitandmiss", "Hit and Miss Charts", 10, 10, 800, 800);
  hitandmiss->Divide(1,2);

  hitandmiss->cd(1);
  //hits->SetContour(20,[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]);
  hits->Draw("COLZ");

  hitandmiss->cd(2);
  //misses->SetContour(20,[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]);
  misses->Draw("COLZ");

  hitandmiss->SaveAs(outputPrefix+"MD_hit_and_miss_p2.png");
  hitandmiss->SaveAs(outputPrefix+"MD_hit_and_miss_p2.C");


// now calculate slopes and intercepts of light vs. long-axis location; linear fits to ADC amplitude vs. location

  TF1* fit1 = new TF1("FIT1", "pol1", -95, -5);
  TF1* fit2 = new TF1("FIT2", "pol1", 5, 95);

TCanvas  *fulls = new TCanvas("FULLS","Full Profiles", 10, 10, 800, 800);
  fulls->Divide(1,2);

  fulls->cd(1);
  fulladcpp->Draw();
  gStyle->SetOptFit(1111);

  fulladcpp->Fit(fit1, "R");
  Double_t pintercept1 = fit1->GetParameter(0);
  Double_t pslope1 = fit1->GetParameter(1);

  fulladcpp->Fit(fit2, "R+");
  Double_t pintercept2 = fit2->GetParameter(0);
  Double_t pslope2 = fit2->GetParameter(1);

  fulls->cd(2);
  fulladcmp->Draw();
  gStyle->SetOptFit(1111);

  fulladcmp->Fit(fit1, "R");
  Double_t mintercept1 = fit1->GetParameter(0);
  Double_t mslope1 = fit1->GetParameter(1);

  fulladcmp->Fit(fit2, "R+");
  Double_t mintercept2 = fit2->GetParameter(0);
  Double_t mslope2 = fit2->GetParameter(1);

  fulls->SaveAs(outputPrefix+"MD_full_profiles_p2.png");
  fulls->SaveAs(outputPrefix+"MD_full_profiles_p2.C");

// end of calculation of slopes and intercepts of light vs. location


// Make the efficiency plots used for the mean radial location calculation

  TCanvas *EFF = new TCanvas("EFF", "Efficiency Plot", 10, 10, 600, 600);

  TGraph *effplot = new TGraph(size, xpos, efficiency);
  effplot->SetMarkerStyle(20);
  effplot->SetMarkerColor(4);
  effplot->GetXaxis()->SetTitle("MD Radial location (cm)");
  effplot->SetTitle(Form("MD %d efficiency vs. radial track location",md_number));
  effplot->Draw("AP");
  
  TPaveText *myText= new TPaveText(0.30,0.2,0.7,0.27, "NDC");
  myText->SetTextSize(0.025); 
  myText->SetFillColor(0);
  myText->SetTextAlign(12);
  TText *myTextEntry = myText->AddText(Form("Mean Location = %0.2f  cm",effmean));
  myText->Draw();
  TPaveText *RunNumText= new TPaveText(0.30,0.28,0.52,0.33, "NDC");
  RunNumText->SetTextSize(0.025); 
  RunNumText->SetFillColor(42);
  RunNumText->SetTextAlign(12);
  TText  *RunNumTextEntry = RunNumText->AddText(Form("Run Number %d",runnum));
  RunNumText->Draw();

  EFF->SaveAs(outputPrefix+"MD_radial_projection_p2.png");
  EFF->SaveAs(outputPrefix+"MD_radial_projection_p2.C");

  // done mean radial location calculation

  // now, calculate "glue joint attenuation factors" gjap, gjam from positive and negative PMTs
  Double_t gjap=0;
  Double_t gjam=0;

  if (pintercept1 > pintercept2)
    {
      gjap = pintercept1 / pintercept2;
      gjam = mintercept2 / mintercept1;
    }

  if (pintercept2 > pintercept1)
    {
      gjap = pintercept2 / pintercept1;
      gjam = mintercept1 / mintercept2;
    }

  Double_t nslopep1=10;
  Double_t nslopep2=10;
  Double_t nslopem1=10;
  Double_t nslopem2=10;
  Double_t q;
  Double_t p;

  if (pintercept1 > pintercept2)
    {
      nslopep2 = pslope2 / pintercept2;
      q = -100*pslope1 + pintercept1;
      nslopep1 = pslope1 / q;
    }

  if (pintercept2 > pintercept1)
    {
      nslopep1 = pslope1 / pintercept1;
      q = 100*pslope2 + pintercept2;
      nslopep2 = pslope2 / q;
    }

  if (mintercept1 > mintercept2)
    {
      nslopem2 = mslope2 / mintercept2;
      p = -100*mslope1 + mintercept1;
      nslopem1 = mslope1 / p;
    }

  if (mintercept2 > mintercept1)
    {
      nslopem1 = mslope1 / mintercept1;
      p = 100*mslope2 + mintercept2;
      nslopem2 = mslope2 / p;
    }

  fout2 << "Run \t Pack. \t Octant\t gja_p    \t gja_m    \t slope_p1 \t slope_p2 \t slope_m1 \t slope_m2" << endl;

  fout2 << runnum << " \t " << package << " \t " << md_number << " \t " <<  gjap << " \t " << gjam << " \t " << nslopep1 << " \t " << nslopep2 << " \t " << nslopem1 << " \t " << nslopem2 << endl; 

  fout.close();
  fout2.close();

  return;
}

/*********************************************************** 
Function: DetermineOctantRegion3
Purpose: To determinae the octant for a certain package in Region 3
Entry Conditions: TChain - event_tree
                                int - package
Global:
Return: int - octant 
Called By: LightWeighting
Date: 09-14-2012
Modified:
*********************************************************/

int DetermineOctantRegion3(TChain* event_tree, int package)
{
	  TH1F* h = new TH1F("h",Form("Region3, Package %d Octant",package),9,-0.5,8.5);
	  event_tree->Draw("events.fQwPartialTracks.fOctant>>h",Form("events.fQwPartialTracks.fRegion==3&&events.fQwPartialTracks.fPackage==%d",package),"GOFF",100000);
	  int oct = int (h->GetMean());
	  delete h;
	  return oct;
}
