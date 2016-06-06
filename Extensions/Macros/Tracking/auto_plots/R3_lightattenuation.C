//
// authors: Siyuan Yang, David Armstrong, Eric Henderson
//  modified by Rachel Taverner
//
//   this script is for light attenuatuion studies in the Main Detector using
// projected tracks from Region 3.
//
//  based originally on  r3_projection.C
//
//  Usage: first you must put the rootfile which contains
//   the data under the $QW_ROOTFILES directory and then type:
//   project_root(package, octant, runnumber,events)
//   where "package" = package number (1 or 2)
//         "octant" = main detector octant number (1 to 8)
//         "runnumber" = run number
//         "events" is number of events to analyze (default is whole run)


#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include <TF1.h>
#include <TH1F.h>
#include <TH1D.h>
#include <TH2F.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TChain.h>
#include <TLeaf.h>
#include <TGraph.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TFile.h>
#include <TPaveStats.h>
#include <TPaveText.h>

#include "QwEvent.h"

const double pe_convert[]={14.0141,16.6726,21.9799,38.5315,20.4254,20.3896,22.1042,22.3945,22.7986,30.0517,28.7274,25.3396,24.6273,16.0718,27.8305,12.3251};

TCanvas *c;

TString PREFIX;

int getOctNumber(TChain* event_tree)
{
  //get a histogram (called h) of the octant cutting on region 3 and pacakge 2 - only one octant will be return - the one we want

  TH1F* h = new TH1F("h","Region 3, Package 2 Octant number",9,-0.5,8.5);

  event_tree->Draw("events.fQwPartialTracks.fOctant>>h","events.fQwPartialTracks.fRegion==3&&events.fQwPartialTracks.fPackage==2","GOFF",100000);

  //get the mean of histgram h made above, this returns a double value that is the trunkated to interger which is the region 3 pacakge 2 octant number

  int j = int (h->GetMean());

  delete h;

  return (j);
}


void project_root(int package=1,int md_number=1,int run_number=6327,int max_events=-1,TString file_suffix="Qweak_",std::string command="MD_")
{
//this z_pos was the original used. We believe it is 5 cm off now. 2012-05-09 JAM
//  Double_t md_zpos[9] = {0.0, 581.665,  576.705, 577.020, 577.425, 582.515,  577.955, 577.885, 577.060};
//  Double_t md_zpos[9] = {0.0, 576.665,  571.705, 572.020, 572.425, 577.515,  572.955, 572.885, 572.060};
//    below is version from Feb. 26 2014; matches what is in:  qweak_new.geo
  Double_t md_zpos[9] = {0.0, 576.665,  576.705, 577.020, 577.425, 577.515,  577.955, 577.885, 577.060};

  TString file_name = "";
  file_name += gSystem->Getenv ( "QW_ROOTFILES" );
  file_name +="/";
  file_name += file_suffix;
  file_name += run_number;
  file_name +=".root";
  
  TFile *file = new TFile ( file_name );

  TTree* event_tree = ( TTree* ) file->Get ( "event_tree" );
  QwEvent* fEvent = 0;

  //check if you put some crazy numbers here
  if (package<=0 || package >4 || md_number <=0 || md_number > 8)
    {
      cout << "put the wrong package or main detector number, please check agian! " << endl;
      return;
    }
  // section to process the command

  size_t type_found;
  std::string type;
  type_found=command.find_first_of("_");
  if (type_found==std::string::npos)
    {
      cerr << "bad things happens! You need to type a underscore _" << endl;
      return;
    }
  else
    {
      type.assign(command,0,type_found);
    }

  bool IsProfile=false;
  if (command.find("profile")==std::string::npos)
    IsProfile=false;
  else IsProfile=true;

  bool pe=false;
  if (command.find("photon")==std::string::npos)
    pe=false;
  else pe=true;

  int mode=0;   //p,m,m+p,p-m;starting from 1
  size_t found=0;
  if ((found=command.find("+"))!=std::string::npos)
    mode=3;
  else if ((found=command.find("-"))!=std::string::npos)
    mode=4;
  else if (found=command.find_first_of("p")!=std::string::npos)
    {
//      if (command.at(++found)!="r" || command.at(++found)!="e")
//	mode=1;
    }
  else if (command.find("m")!=std::string::npos)
    mode=2;

  std::string w_title;
  cout << "mode: " << mode << endl;
  if (mode==0)
    w_title="not weighted";
  else
    w_title="weighted";



  TH2F* h_2d;
  TProfile2D* hp_2d;
  if (IsProfile==false)
    h_2d=new TH2F(Form("h_2d %s not profile",w_title.c_str()),"h_2d ",240,0,0,480,0,0);
  else
    hp_2d=new TProfile2D(Form("hp_2d %s profile",w_title.c_str()),"h_2d ",240,0,0,480,0,0,"");

  TH1D* h_1f = new TH1D("project run","project run",240,280,400);
  TH1D* adcph = new TH1D("ADCP", "ADCP data", 1650, 0, 3500);
  TH1D* adcmh = new TH1D("ADCM", "ADCM data", 1650, 0, 3500);
  TH1D* tdcph = new TH1D("TDCP", "TDCP data", 1650, 0, 0);
  TH1D* tdcmh = new TH1D("TDCM", "TDCM data", 1650, 0, 0);
  TH1D* adcpped = new TH1D("ADCPPED", "ADCP pedestal", 50, 0, 400);
  TH1D* adcmped = new TH1D("ADCMPED", "ADCM pedestal", 50, 0, 400);

  TProfile* adcpp1;
  TProfile* adcpp2;
  TProfile* adcmp1;
  TProfile* adcmp2;
  TProfile* fulladcpp;
  TProfile* fulladcmp;
  TProfile* linadcpp;
  TProfile* linadcmp;
  TProfile* x1adcpp;
  TProfile* x1adcmp;
  TProfile* x2adcpp;
  TProfile* x2adcmp;
  TProfile* x3adcpp;
  TProfile* x3adcmp;


  adcpp1 = new TProfile("ADCPP1", "ADCP1 profile", 50, -100, 0);
  adcpp2 = new TProfile("ADCPP2", "ADCP2 profile", 50, 0, 100);
  adcmp1 = new TProfile("ADCMP1", "ADCM1 profile", 50, -100, 0);
  adcmp2 = new TProfile("ADCMP2", "ADCM2 profile", 50, 0, 100);
  fulladcpp = new TProfile("FULLADCPP", Form("Exponential Run %d Octant %d positive PMT", run_number, md_number), 500, -95, 95);
  fulladcmp = new TProfile("FULLADCMP", Form("Exponential Run %d Octant %d minus PMT", run_number, md_number), 500, -95, 95);
  linadcpp = new TProfile("LINADCPP", Form("Linear Run %d Octant %d plus PMT", run_number, md_number), 500, -95, 95);
  linadcmp = new TProfile("LINADCMP", Form("Linear Run %d Octant %d minus PMT", run_number, md_number), 500, -95, 95);
  x1adcpp = new TProfile("X1P", "X1 ADCP profile", 200, -110, 110);
  x1adcmp = new TProfile("X1M", "X1 ADCM profile", 200, -110, 110);
  x2adcpp = new TProfile("X2P", "X2 ADCP profile", 200, -110, 110);
  x2adcmp = new TProfile("X2M", "X2 ADCM profile", 200, -110, 110);
  x3adcpp = new TProfile("X3P", "X3 ADCP profile", 200, -110, 110);
  x3adcmp = new TProfile("X3M", "X3 ADCM profile", 200, -110, 110);


  TH2F* hits = new TH2F("hits", Form("Run %d Octant %d Hit Chart", run_number, md_number), 400, -130, 130, 300, 320, 360);
  TH2F* misses = new TH2F("misses", Form("Run %d Octant %d Miss Chart", run_number, md_number), 400, -130, 130, 300, 320, 360);

  TGraph* effplot;

  TBranch* branch_event=event_tree->GetBranch("events");
  TBranch* branch     = event_tree->GetBranch("maindet");

  branch_event->SetAddress(&fEvent);

  TLeaf* mdp=branch->GetLeaf(Form("md%dp_adc",md_number));
  TLeaf* mdm=branch->GetLeaf(Form("md%dm_adc",md_number));

  TLeaf* mdp_t=branch->GetLeaf(Form("md%dp_f1",md_number));
  TLeaf* mdm_t=branch->GetLeaf(Form("md%dm_f1",md_number));


  Int_t nevents=event_tree->GetEntries();
  Int_t r3_events =0;

  if (max_events == -1)max_events = nevents; 

  Double_t dz = 0.0;
  
  if      (type=="QTOR") dz = 0.0;
  else if (type=="TS")   dz = 539.74;
  else if (type=="MD")   dz = md_zpos[md_number];
  else if (type=="SC")   dz = 591.515;
  else {
    cerr << "no such plane!" << endl;
    return;
  }


  Double_t pdataplus, pdataminus, padcp, padcm;
  for (int i=0;i<10000;i++)
    {

      branch_event->GetEntry(i);
      branch->GetEntry(i);

      for(int num_p=0; num_p < fEvent->GetNumberOfPartialTracks(); num_p++)
	{
	  r3_events++;
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

  Int_t size = 125;
  Double_t hit[125];
  Double_t trial[125];
  Int_t bin;
  Int_t xmin = 323;
  Double_t width = 0.2;
  Double_t xpos[125];
  Double_t efficiency[125];

  for (Int_t j = 0;j < size; j = j + 1)
    {
      xpos[j] = (j * width) + xmin;
      hit[j] = 0;
      trial[j] = 0;
    }

  Double_t adcpmean = adcpped->GetMean();
  Double_t adcmmean = adcmped->GetMean();

  for(int i = 0; i < max_events; i++)
	{
	  if(i % 1000 == 0) cout <<"events processed so far:" << i << endl;
	  branch_event->GetEntry(i);
	  branch->GetEntry(i);

	  Double_t xoffset, yoffset, xslope, yslope, x, y;
	  //    weight = 0;

	 for (int num_p=0; num_p< fEvent->GetNumberOfPartialTracks();num_p++)
	    {
	      const QwPartialTrack* pt = fEvent->GetPartialTrack(num_p);
              if (pt->GetRegion()==3 && pt->GetPackage()==package)
		{
	      xoffset = pt->fOffsetX;
	      yoffset = pt->fOffsetY;
	      xslope  = pt->fSlopeX;
	      yslope  = pt->fSlopeY;
	      x       = xoffset+xslope*dz;
	      y       = yoffset+yslope*dz;
	      
	      //if(package == 1)
	      //md_number = (md_number + 4) % 8;

	      Double_t theta = -45.0*3.14159/180.0*(md_number - 1);
	      Double_t xx = x*cos(theta) - y*sin(theta);
	      Double_t yy = x*sin(theta) + y*cos(theta);
	      x = xx;
	      y = yy;

	      Double_t m = padcm;
	      Double_t p = padcp;
	      Double_t weight = 1.0;
	      Double_t adcpdata = mdp->GetValue();
	      Double_t adcmdata = mdm->GetValue();
	      Double_t tdcpdata = mdp_t->GetValue();
	      Double_t tdcmdata = mdm_t->GetValue();
	      int p_t = pdataplus;
	      int m_t = pdataminus;
	      if (pe==true) {
		p = p/pe_convert[2*md_number-1];
		m = m/pe_convert[2*(md_number-1)];
	      }
	      
	      switch (mode)
                {
                case 1:
		  weight = p;
		  break;
                case 2:
		  weight = m;
		  break;
                case 3:
		  weight = m+p;
		  break;
                case 4:
		  weight = p-m;
		  break;
                default:
		  weight = 1.0;
                }

	      //  printf("x %10.2f y %10.2f weight %10.2f mdp %10.2f mdm %10.2f \n", x, y, weight, p, m);
	      if (p!=0.0 && m!=0.0 && p_t<-10 && m_t <-10)
                {
		  if (IsProfile==false)
		    h_2d->Fill(x,y);
		  else
		    hp_2d->Fill(x,y,weight);
                }

	      if (tdcpdata < -150 && tdcpdata > -250)
		{
		  adcph->Fill(adcpdata);
		  adcpp1->Fill(y,adcpdata-adcpmean);
		  adcpp2->Fill(y,adcpdata-adcpmean);
		  fulladcpp->Fill(y,adcpdata-adcpmean);
		  linadcpp->Fill(y,adcpdata-adcpmean);
		}

	      if (tdcmdata < -150 && tdcmdata > -250)
		{
		  adcmh->Fill(adcmdata);
		  adcmp1->Fill(y,adcmdata-adcmmean);
		  adcmp2->Fill(y,adcmdata-adcmmean);
		  fulladcmp->Fill(y,adcmdata-adcmmean);
		  linadcmp->Fill(y,adcmdata-adcmmean);

		  if(tdcpdata < -150 && tdcpdata > -250)
		    {
		      hits->Fill(y,x);
		    }
		}

	      if (tdcpdata == 0 && tdcmdata == 0)
		{
		  misses->Fill(y,x);
		}

	      if (x >= 330 && x < 335)
		{
		  x1adcpp->Fill(y,adcpdata-adcpmean);
		  x1adcmp->Fill(y,adcmdata-adcmmean);
		}

	      if (x >= 335 && x < 340)
		{
		  x2adcpp->Fill(y,adcpdata-adcpmean);
		  x2adcmp->Fill(y,adcmdata-adcmmean);
		}

	      if (x >= 340 && x <= 345)
		{
		  x3adcpp->Fill(y,adcpdata-adcpmean);
		  x3adcmp->Fill(y,adcmdata-adcmmean);
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

  //  FILE* hit_trial;
  //
  //hit_trial = fopen("hit_trial.txt","w");
  //
  //for (Int_t v = 0;v < size;v = v + 1)
  //{
  //  fprintf(hit_trial, "%d \t %d\n", hit[v], trial[v]);
  //}
  //
  //fclose(hit_trial);


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

  FILE* mean;

  cout << "mean radial position = " << effmean << "   (cm)  " << endl;

  mean = fopen("mean.txt","a");

  fprintf(mean, "%d \t %d \t %d \t %f\n", run_number, package, md_number, effmean);

  
  fclose(mean);


//////////////////////////////////////////////////////////////////////////
//IT'S HERE///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

  TF1* fit1 = new TF1("FIT1", "pol1", -95, -5);
  TF1* fit2 = new TF1("FIT2", "pol1", 5, 95);

  TCanvas* fulls = new TCanvas("FULLS","Full Profiles", 10, 10, 800, 800);
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

TCanvas* diff = new TCanvas("diff","Difference Profile",200,10,700,500);
diff->cd();

Int_t nbins = 500;
Int_t length = 190;
Double_t lengthperbin = 190. / 500.;

Double_t pvalue_at_pmt = pintercept2 + 90.0 * pslope2;
Double_t mvalue_at_pmt = mintercept1 + (-90.0) * mslope1;
Double_t ratio = pvalue_at_pmt / mvalue_at_pmt;

TH1D* fulladc_diff = new TH1D("fulladc_diff","Difference Profile",500,-95,95);
fulladc_diff->Add(fulladcpp,fulladcmp,1,-ratio);


fulladc_diff->Draw("e");
fulladc_diff->SetTitle(Form("Difference Profile, Run %d, Octant %d", run_number, md_number));
diff->Update();

///////////////////////////////////////////////////////////////////////////
//fitting range at the glue joint will be different depending on the octant

TF1* fit3;
if ( md_number == 1 )
  {
      fit3 = new TF1("FIT3", "pol1", 1.75, 3.25); //done
  }
else if (md_number == 2 )
  {
      fit3 = new TF1("FIT3", "pol1", .5, 2); //done
  }
else if (md_number == 3 )
  {
      fit3 = new TF1("FIT3", "pol1", -1, 1.5);  //done
  }
else if (md_number == 4 )
  {
      fit3 = new TF1("FIT3", "pol1", -1.5, .5); //done
  }
else if (md_number == 5 )
  {
      fit3 = new TF1("FIT3", "pol1", -1.25, 1.25); //done
  }
else if (md_number == 6 )
  {
      fit3 = new TF1("FIT3", "pol1", .5, 2); //done
  }
else if (md_number == 7 )
  {
      fit3 = new TF1("FIT3", "pol1", 2, 5); //done
  }
else if (md_number == 8 )
  {
      fit3 = new TF1("FIT3", "pol1", -1, 1.5); //done
  }

fulladc_diff->Fit(fit3, "R+");
Double_t intercept = fit3->GetParameter(0);
Double_t slope = fit3->GetParameter(1);

Double_t x_intercept = -intercept / slope;
cout<< "Intercept: " <<x_intercept << " cm." << endl;


TPaveText *myText= new TPaveText(0.30,0.12,0.7,0.19, "NDC");
myText->SetTextSize(0.025); 
myText->SetFillColor(0);
myText->SetTextAlign(12);
myText->AddText(Form("Intercept = %0.2f  cm", x_intercept));
myText->Draw();


TPaveStats *st1 = (TPaveStats *) fulladc_diff->GetListOfFunctions()->FindObject("stats");
st1->SetX1NDC(.15);
st1->SetX2NDC(.4);
st1->SetY1NDC(.90);
st1->SetY2NDC(.70);

diff->SaveAs(PREFIX+Form("adcdiff_center_pkg%d.png",package));
diff->SaveAs(PREFIX+Form("adcdiff_center_pkg%d.C",package));

  return;
};



void R3_lightattenuation (int runnum, bool is100k)
{
  // Output prefix
  //PREFIX = Form(TString(gSystem->Getenv("QWSCRATCH"))+"/tracking/www/run_%d/%d_",runnum,runnum);

  PREFIX = Form(
         TString(gSystem->Getenv("WEBDIR")) + "/run_%d/%d_",
         runnum, runnum);

  // Create and load the chain
  TChain *event_tree = new TChain("event_tree");
  event_tree->Add(Form("$QW_ROOTFILES/Qweak_%d.root",runnum));

  // Try to get the octant number from the run number
  int oct_pkg2 = getOctNumber(event_tree);
  int oct_pkg1 = (oct_pkg2 + 3) % 8 + 1;

  // package 1
  project_root(1,oct_pkg1,runnum,-1,"Qweak_","MD_profile");
  // package 2
  project_root(2,oct_pkg2,runnum,-1,"Qweak_","MD_profile");
}

