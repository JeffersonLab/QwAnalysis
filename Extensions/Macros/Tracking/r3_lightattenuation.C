//
// authors: Siyuan Yang, David Armstrong, Eric Henderson
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
//   
//    Results are output in several .txt files, which I need to 
//      document... soon...  
//
//     There are two other functions included that are not directly 
//      related to light attenuation studies, but are kept here for now:
//
//         Angle(package, octant, run, events);
//       (for studying main detector pulse height vs, incident track angle)
//      and
//         CleanTrack(package, octant, run, events);
//       (for studying how many Region3 partial tracks have pulses in
//        main detector)
//      
#include<iostream>
#include<fstream>
#include <string>
#include <TF1.h>
#include <stdio.h>
#include <stdlib.h>

const double pe_convert[]={14.0141,16.6726,21.9799,38.5315,20.4254,20.3896,22.1042,22.3945,22.7986,30.0517,28.7274,25.3396,24.6273,16.0718,27.8305,12.3251};

TCanvas *c;


void project_root(int package=1,int md_number=1,int run_number=6327,int max_events=-1,TString file_suffix="Qweak_",string command="")
{
//this z_pos was the original used. We believe it is 5 cm off now. 2012-05-09 JAM
//  Double_t md_zpos[9] = {0.0, 581.665,  576.705, 577.020, 577.425, 582.515,  577.955, 577.885, 577.060};
  Double_t md_zpos[9] = {0.0, 576.665,  571.705, 572.020, 572.425, 577.515,  572.955, 572.885, 572.060};

  TString file_name = "";
  string command = "MD_";
  file_name += gSystem->Getenv ( "QW_ROOTFILES" );
  file_name +="/";
  file_name += file_suffix;
  file_name += run_number;
  file_name +=".root";
  
  TFile *file = new TFile ( file_name );

  TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
  QwEvent* fEvent=0;
  QwPartialTrack* pt=0;
  QwTreeLine* tl=0;
  // QwHit* hits=0;

  //check if you put some crazy numbers here
  if (package<=0 || package >4 || md_number <=0 || md_number > 8)
    {
      cout << "put the wrong package or main detector number, please check agian! " << endl;
      return;
    }
  // section to process the command

  size_t type_found;
  string type;
  type_found=command.find_first_of("_");
  if (type_found==string::npos)
    {
      cerr << "bad things happens! You need to type a underscore _" << endl;
      return;
    }
  else
    {
      type.assign(command,0,type_found);
    }

  bool IsProfile=false;
  if (command.find("profile")==string::npos)
    IsProfile=false;
  else IsProfile=true;

  bool pe=false;
  if (command.find("photon")==string::npos)
    pe=false;
  else pe=true;

  int mode=0;   //p,m,m+p,p-m;starting from 1
  size_t found=0;
  if ((found=command.find("+"))!=string::npos)
    mode=3;
  else if ((found=command.find("-"))!=string::npos)
    mode=4;
  else if (found=command.find_first_of("p")!=string::npos)
    {
      if (command.at(++found)!="r" || command.at(++found)!="e")
	mode=1;
    }
  else if (command.find("m")!=string::npos)
    mode=2;

  string w_title;
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
    hp_2d=new TProfile2D(Form("hp_2d %s profile",w_title.c_str()),"h_2d ",240,0,0,480,0,0);

  TH1D* h_1f=new TH1D("project run","project run",240,280,400);
  TH1D* adcph = new TH1D("ADCP", "ADCP data", 1650, 0, 3500);
  TH1D* adcmh = new TH1D("ADCM", "ADCM data", 1650, 0, 3500);
  TH1D* tdcph = new TH1D("TDCP", "TDCP data", 1650, 0, 0);
  TH1D* tdcmh = new TH1D("TDCM", "TDCM data", 1650, 0, 0);
  TH1D* adcpped = new TH1D("ADCPPED", "ADCP pedestal", 50, 0, 0);
  TH1D* adcmped = new TH1D("ADCMPED", "ADCM pedestal", 50, 0, 0);

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
  fulladcpp = new TProfile("FULLADCPP", Form("Exponential Run %d Octant %d positive PMT", run_number, md_number), 150, -105, 105);
  fulladcmp = new TProfile("FULLADCMP", Form("Exponential Run %d Octant %d minus PMT", run_number, md_number), 150, -105, 105);
  linadcpp = new TProfile("LINADCPP", Form("Linear Run %d Octant %d plus PMT", run_number, md_number), 150, -105, 105);
  linadcmp = new TProfile("LINADCMP", Form("Linear Run %d Octant %d minus PMT", run_number, md_number), 150, -105, 105);
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


  for (int i=0;i<1000;i++)
    {

      branch_event->GetEntry(i);
      branch->GetEntry(i);

      for(int num_p=0; num_p < fEvent->GetNumberOfPartialTracks(); num_p++)
	{
	  Double_t pdataplus = mdp_t->GetValue();
	  Double_t pdataminus = mdm_t->GetValue();
	  Double_t padcp = mdp->GetValue();
	  Double_t padcm = mdm->GetValue();

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
	      pt=fEvent->GetPartialTrack(num_p);
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

  fprintf(mean, "%f\n", effmean);

  
  fclose(mean);



  c = new TCanvas("c","Region 3 Projections",10, 10, 800,800);
  c->Divide(2,2);
  c->cd(1);
  if (IsProfile==false)
    {
      string title="track projection on " + type + "  " +  w_title + ": not profile";
      h_2d->GetXaxis()->SetTitle("position x:cm");
      h_2d->GetYaxis()->SetTitle("position y:cm");
      h_2d->SetTitle(title.c_str());
      h_2d->Draw("colz");
    }
  else
    {
      string title="track projection on " + type + "  " +  w_title + ": profile";
      hp_2d->GetXaxis()->SetTitle("position x:cm");
      hp_2d->GetYaxis()->SetTitle("position y:cm");
      hp_2d->SetTitle(title.c_str());
      hp_2d->Draw("colz");
    }
    
  c->cd(2);
  if(IsProfile==false){
    h_2d->ProjectionX()->Draw();
  }
  else if(IsProfile==true){
    hp_2d->ProjectionX()->Draw();
  }
  c->cd(3);
  if(IsProfile==false){
    h_2d->ProjectionY()->Draw();
  }
  else if(IsProfile==true){
    hp_2d->ProjectionY()->Draw();
  }
  c->cd(4);
  adcph->Draw();


  pedestals = new TCanvas("pedestals", "Pedestal Data", 10, 10, 800, 800);
  pedestals->Divide(1,2);

  pedestals->cd(1);
  adcpped->Draw();

  pedestals->cd(2);
  adcmped->Draw();


  quad = new TCanvas("quad", "ADC and TDC data", 10, 10, 800, 800);
  quad->Divide(2,2);

  quad->cd(1);
  adcph->Draw();

  quad->cd(2);
  adcmh->Draw();

  quad->cd(3);
  tdcph->Draw();

  quad->cd(4);
  tdcmh->Draw();


  hitandmiss = new TCanvas("hitandmiss", "Hit and Miss Charts", 10, 10, 800, 800);
  hitandmiss->Divide(1,2);

  hitandmiss->cd(1);
  //hits->SetContour(20,[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]);
  hits->Draw("COLZ");

  hitandmiss->cd(2);
  //misses->SetContour(20,[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]);
  misses->Draw("COLZ");


  profiles = new TCanvas("profiles", "ADC profiles", 10, 10, 800, 800);
  profiles->Divide(2,2);

  profiles->cd(1);
  adcpp1->Draw();
  //gStyle->SetOptFit(1111);
  //adcpp1->Fit("pol1");
  adcpp1->GetXaxis()->SetTitle("Position on MD (cm)");

  profiles->cd(2);
  adcpp2->Draw();
  //gStyle->SetOptFit(1111);
  //adcpp2->Fit("pol1");
  adcpp2->GetXaxis()->SetTitle("Position on MD (cm)");

  profiles->cd(3);
  adcmp1->Draw();
  //gStyle->SetOptFit(1111);
  //adcmp1->Fit("pol1");
  adcmp1->GetXaxis()->SetTitle("Position on MD (cm)");

  profiles->cd(4);
  adcmp2->Draw();
  //gStyle->SetOptFit(1111);
  //adcmp2->Fit("pol1");
  adcmp2->GetXaxis()->SetTitle("Position on MD (cm)");


  TF1* fit1 = new TF1("FIT1", "pol1", -95, -5);
  TF1* fit2 = new TF1("FIT2", "pol1", 5, 95);
  TF1* fit3 = new TF1("FIT3", "pol1", -3, 3);
  TF1* fit4 = new TF1("FIT4", "expo", -95, -5);
  TF1* fit5 = new TF1("FIT5", "expo", 5, 95);


  sections = new TCanvas("Sections", "MD_Sections", 10, 10, 800, 800);
  sections->Divide(2,3);

  Double_t e = 2.71828;
  Double_t x1pint1;
  Double_t x1pint2;
  Double_t x2pint1;
  Double_t x2pint2;
  Double_t x3pint1;
  Double_t x3pint2;
  Double_t x1mint1;
  Double_t x1mint2;
  Double_t x2mint1;
  Double_t x2mint2;
  Double_t x3mint1;
  Double_t x3mint3;

  sections->cd(1);
  x1adcpp->Draw();

  x1adcpp->Fit(fit4, "R");
  x1pint1 = e**fit4->GetParameter(0);

  x1adcpp->Fit(fit5, "R+");
  x1pint2 = e**fit5->GetParameter(0);

  sections->cd(2);
  x1adcmp->Draw();

  x1adcmp->Fit(fit4, "R");
  x1mint1 = e**fit4->GetParameter(0);

  x1adcmp->Fit(fit5, "R+");
  x1mint2 = e**fit5->GetParameter(0);

  sections->cd(3);
  x2adcpp->Draw();

  x2adcpp->Fit(fit4, "R");
  x2pint1 = e**fit4->GetParameter(0);

  x2adcpp->Fit(fit5, "R+");
  x2pint2 = e**fit5->GetParameter(0);

  sections->cd(4);
  x2adcmp->Draw();

  x2adcmp->Fit(fit4, "R");
  x2mint1 = e**fit4->GetParameter(0);

  x2adcmp->Fit(fit5, "R+");
  x2mint2 = e**fit5->GetParameter(0);

  sections->cd(5);
  x3adcpp->Draw();

  x3adcpp->Fit(fit4, "R");
  x3pint1 = e**fit4->GetParameter(0);

  x3adcpp->Fit(fit5, "R+");
  x3pint2 = e**fit5->GetParameter(0);

  sections->cd(6);
  x3adcmp->Draw();

  x3adcmp->Fit(fit4, "R");
  x3mint1 = e**fit4->GetParameter(0);

  x3adcmp->Fit(fit5, "R+");
  x3mint2 = e**fit5->GetParameter(0);


  Double_t ratio1;
  Double_t ratio2;
  Double_t ratio3;
  Double_t ratio4;

  ratio1 = x1pint1 / x2pint1;
  ratio2 = x1mint1 / x2mint1;
  ratio3 = x2pint2 / x3pint2;
  ratio4 = x2mint2 / x3mint2;

  FILE* ratios;

  ratios = fopen("ratios.txt","a");

  if (ratios == NULL)
    {
      cout << "ratios did not open" << endl;
    }

  fprintf(ratios, "%d \t %d \t %d \t %f \t %f \t %f \t %f\n", run_number, package, md_number, ratio1, ratio2, ratio3, ratio4);

  fclose(ratios);




  fulls = new TCanvas("FULLS","Full Profiles", 10, 10, 800, 800);
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



  EFF = new TCanvas("EFF", "Efficiency Plot", 10, 10, 800, 800);
  effplot = new TGraph(size, xpos, efficiency);
  effplot->SetMarkerStyle(20);
  effplot->SetMarkerColor(4);
  effplot->Draw("AP");



  //Double_t mm = -1*pslope3 + pslope1;
  //Double_t bb = -1* pintercept3 + pintercept1;
  //Double_t pintersection1 = bb / (-1*mm);

  //mm = -1*pslope3 + pslope2;
  //bb = -1*pintercept3 + pintercept2;
  //Double_t pintersection2 = bb / (-1*mm);

  //Double_t gluejoint1 = (pintersection1 + pintersection2) / 2;

  //Double_t nn = -1*mslope3 + mslope1;
  //Double_t cc = -1*mintercept3 + mintercept1;
  //Double_t mintersection1 = cc / (-1*nn);

  //nn = -1*mslope3  + mslope2;
  //cc = -1*mintercept3 + mintercept2;
  //Double_t mintersection2 = cc /(-1*nn);

  //Double_t gluejoint2 = (mintersection1 + mintersection2) / 2;


  Double_t gjap;
  Double_t gjam;

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

  Double_t nslopep1;
  Double_t nslopep2;
  Double_t nslopem1;
  Double_t nslopem2;
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



  FILE *pedestal_mean;
  FILE *slope_intercept;
  FILE *nslope_gja;

  pedestal_mean = fopen("pedestal_mean.txt", "a");
  slope_intercept = fopen("slope_intercept.txt", "a");
  nslope_gja = fopen("nslope_gja.txt", "a");

  if (pedestal_mean == NULL)
    {
      cout << "pedestal_mean did not open" << endl;
    }

  if (slope_intercept == NULL)
    {
      cout << "slope_intercept did not open" << endl;
    }

  if (nslope_gja == NULL);
    {
      cout << "nslope_gja did not open" << endl;
    }

  fprintf(pedestal_mean, "%d \t %d \t %d \t %f \t %f\n", run_number, package, md_number, adcpmean, adcmmean);
  
  fprintf(slope_intercept, "%d \t %d \t %d \t %f \t %f \t %f \t %f \t %f \t %f \t %f \t %f\n", run_number, package, md_number, pintercept1, pslope1, pintercept2, pslope2, mintercept1, mslope1, mintercept2, mslope2);

  fprintf(nslope_gja, "%d \t %d \t %d \t %f \t %f \t %f \t %f \t %f \t %f\n",run_number, package, md_number, gjap, gjam, nslopep1, nslopep2, nslopem1, nslopem2);



  fclose(pedestal_mean);
  fclose(slope_intercept);
  fclose(nslope_gja);


  return;
};


// default: md,p+m mode
void CleanTrack(int package=1, int md_number=5,int run=6327, int max_events=-1)
{
    gStyle->SetPalette(1);
    string file_name= Form ( "%s/Qweak_%d.root",gSystem->Getenv ( "QW_ROOTFILES" ),run );
    TFile *file = new TFile ( file_name.c_str() );

    TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
    QwEvent* fEvent=0;
    QwPartialTrack* pt=0;
    QwTreeLine* tl=0;

    TBranch* branch_event=event_tree->GetBranch("events");
    TBranch* branch=event_tree->GetBranch("maindet");
    branch_event->SetAddress(&fEvent);
    TLeaf* mdp=branch->GetLeaf(Form("md%dp_adc",md_number));
    TLeaf* mdm=branch->GetLeaf(Form("md%dm_adc",md_number));

    string w_title="weighted light yield";
    TH1F* h_f=new TH1F(Form("h_f %s profile",w_title.c_str()),"h_f ",200,0,8000);
    TH1F* h_fg=new TH1F(Form("h_f %s profile",w_title.c_str()),"h_fg ",200,0,8000);
    TH2F* h_2f=new TH2F(Form("h_2f %s profile",w_title.c_str()),"h_2f ",200,0,8000,100,0,0.1);
    
    int nevents=event_tree->GetEntries();
    if (max_events == -1)max_events = nevents; 


    for (int i=0;i<max_events;i++)
    {
        if(i%1000==0) cout << "events process so far: " << i << endl;
        branch_event->GetEntry(i);
        branch->GetEntry(i);

        int npt=fEvent->GetNumberOfPartialTracks();
        bool valid=false;
        for (int j=0;j<npt;j++)
        {
            pt=fEvent->GetPartialTrack(j);
            if (pt->GetRegion()==3 && pt->GetPackage()==package && valid!=true) {valid=true;}
        }

        //retrieve the residual
        double residual=0;
        if (valid)
        {
            int ntl=fEvent->GetNumberOfTreeLines();
            for (int j=0;j<ntl;j++)
            {
                tl=fEvent->GetTreeLine(j);
                if (tl->GetRegion()==3 && tl->GetPackage()==package && tl->GetPlane()==0)
                    residual+=tl->GetAverageResidual();
            }
        }
        
        residual/=2;
        if(valid==false) h_f->Fill(mdp->GetValue()+mdm->GetValue());
	if(valid){
	  h_2f->Fill((mdp->GetValue()+mdm->GetValue()),residual);
	  h_fg->Fill(mdp->GetValue()+mdm->GetValue());
	}
    }
        TCanvas* c=new TCanvas("c","R3 partial track residuals and MD pulseheights",700,700);
        c->Divide(1,3);
        c->cd(1);
	c_1->SetLogy();
        h_f->Draw();
	h_f->SetTitle("MD pulse height for events with no Region 3 partial track");
        c->cd(2);
	c_2->SetLogy();
        h_fg->Draw();
	h_fg->SetTitle("MD pulse height for events with Region 3 partial track");
        c->cd(3);
	h_2f->SetTitle("Region 3 partial track residual vs. MD pulseheight sum");
        h_2f->Draw("colz");
  }

void Angle(int package=1, int md_number=5,int run=6327, int max_events=-1)
{
    gStyle->SetPalette(1);
    string file_name= Form ( "%s/Qweak_%d.root",gSystem->Getenv ( "QW_ROOTFILES" ),run );
    TFile *file = new TFile ( file_name.c_str() );

    TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
    QwEvent* fEvent=0;
    QwPartialTrack* pt=0;
    QwTreeLine* tl=0;

    TBranch* branch_event=event_tree->GetBranch("events");
    TBranch* branch=event_tree->GetBranch("maindet");
    branch_event->SetAddress(&fEvent);
    TLeaf* mdp=branch->GetLeaf(Form("md%dp_adc",md_number));
    TLeaf* mdm=branch->GetLeaf(Form("md%dm_adc",md_number));

    string w_title="weighted light yield";
    //    TProfile* h_x=new TProfile(Form("h_x %s profile",w_title.c_str()),"h_2f ",200,-1,1);
    TProfile* h_x=new TProfile(Form("h_x %s profile",w_title.c_str()),"h_2f ",200,0.2,0.6);
    //    TProfile* h_y=new TProfile(Form("h_y %s profile",w_title.c_str()),"h_2f ",200,-1,1);
    TProfile* h_y=new TProfile(Form("h_y %s profile",w_title.c_str()),"h_2f ",200,-0.3,0.3);
    //    TProfile2D* h_2f=new TProfile2D(Form("h_2f %s profile",w_title.c_str()),"h_2f ",200,-1,1,200,-1,1);
    TProfile2D* h_2f=new TProfile2D(Form("h_2f %s profile",w_title.c_str()),"h_2f ",200,-0.25,0.25,200,0.2,0.6);
    
    int nevents=event_tree->GetEntries();
    if (max_events == -1)max_events = nevents; 

    for (int i=0;i<max_events;i++)
    {
        if(i%1000==0) cout << "events process so far: " << i << endl;
        branch_event->GetEntry(i);
        branch->GetEntry(i);

        int npt=fEvent->GetNumberOfPartialTracks();
        for (int j=0;j<npt;j++)
        {
            pt=fEvent->GetPartialTrack(j);
            if (pt->GetRegion()==3 && pt->GetPackage()==package){
             h_x->Fill(pt->fSlopeX,(mdp->GetValue()+mdm->GetValue()));
             h_y->Fill(pt->fSlopeY,(mdp->GetValue()+mdm->GetValue()));
             h_2f->Fill(pt->fSlopeY,pt->fSlopeX,(mdp->GetValue()+mdm->GetValue()));
             }
        }
    }
    TCanvas* c=new TCanvas("c","c",800,800);
    c->Divide(2,2);
    c->cd(1);
    h_x->SetTitle("MD pulseheightsum vs. Region 3 PartialTrack slope X");
    h_x->Draw();
    c->cd(2);
    h_y->SetTitle("MD pulseheightsum vs. Region 3 PartialTrack slope Y");
    h_y->Draw();
    c->cd(3);
    h_2f->SetTitle("MD pulseheightsum vs. Region 3 PartialTrack slope X,Y");
    h_2f->Draw("colz");
  }
