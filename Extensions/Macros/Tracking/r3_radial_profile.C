//  r3_radial_profile.C
//
// author: David Armstrong
//
//   based on script (r3_light_attenuation.C) by Siyuan Yang and Eric Henderson
//
//  Look at projected partial tracks from Region 3
//     and 
//     1) calculate fraction that miss the bar 
//     2) find average radial location of all tracks (regardless of
//        hitting the bar)
//     3) find average radial location of all tracks that hit bar
//     4) find fraction that miss bar at inner edge (super-elastic side) 
//     5) look at summed MD ADC pulseheights
//     6) estimate discrim threshold values
//     7) get summed ADC in central regon (radially) of MD bar
// 
//
//    For now:
//     - use all R3 partial tracks; no check on succesful bridging (should fix this?)
//     - should we only use first (best)? R3 partial track?
//     - checks from prompt TS meantime hit in appropriate package
//     - defines a "miss" as an event where the TS fired, but neither of the 
//       appropriate MD PMT's fired their TDCs in the prompt time window.
//     - also looks at "single tube hits" (TS is prompt, one prompt MD, the other tube no prompt hit
//         and no ADC above pedestal
//     - need to "tweak" upper pedestal values for MD ADCs for each detector
//
//  Usage: first you must put the rootfile which contains 
//   the data under the $QW_ROOTFILES directory and then type:
//   project_root(package, octant, runnumber,events)
//   where "package" = package number (1 or 2)
//         "octant" = main detector octant number (1 to 8)
//         "runnumber" = run number
//         "events" is number of events to analyze (default is whole run)
//   
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
//  Double_t md_zpos[9] = {0.0, 576.665,  571.705, 572.020, 572.425, 577.515,  572.955, 572.885, 572.060};
//    below is version from Feb. 26 2014; matches what is in:  qweak_new.geo
  Double_t md_zpos[9] = {0.0, 576.665,  576.705, 577.020, 577.425, 577.515,  577.955, 577.885, 577.060};

// the below are upper edge of the MD ADC pedestals for + and - tubes
//  need to adjust on detector-by-detector (& run-by-run?) basis
  int ped_mdm = 270;
  int ped_mdp = 250;

  TString outputPrefix(Form("Radial_run%d_MD%d_",run_number,md_number));
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
  //  QwTrackingTreeLine* tl=0;
  //  QwHit* hit=0;

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
      cerr << "bad things happen! You need to type a underscore _" << endl;
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
  TH1D* md_adc = new TH1D("md_adc", Form("Run %d Summed MD%d ADC (pedestal subtracted)",run_number, md_number),1000, 0, 6000);
  TH1D* md_adc_center = new TH1D("md_adc_center", Form("Run %d Summed MD%d ADC (bar center, ped. sub.)",run_number, md_number),1000, 0, 6000);
  TH1D* md_adc_loose = new TH1D("md_adc_center", Form("Run %d Summed MD%d ADC (>= 1TDC), ped. sub.)",run_number, md_number),1000, 0, 6000);
  TH1D* adcph = new TH1D("ADCP", Form("Run %d  MD%d+ ADC (pedestal subtracted)",run_number, md_number), 1650, 0, 3500);
  TH1D* adcmh = new TH1D("ADMP", Form("Run %d  MD%d- ADC (pedestal subtracted)",run_number, md_number), 1650, 0, 3500);
  TH1D* adcphz = new TH1D("ADCP", Form("Run %d  MD%d+ ADC (pedestal subtracted, zoom)",run_number, md_number), 250, 0, 250);
  TH1D* adcmhz = new TH1D("ADMP", Form("Run %d  MD%d- ADC (pedestal subtracted, zoom)",run_number, md_number), 250, 0, 250);
  TH1D* adcpthresh = new TH1D("ADCPTHRESH", Form("Run %d MD%d ADC+, no TDC",run_number,md_number), 200, 0, 250);
  TH1D* adcmthresh = new TH1D("ADCMTHRESH", Form("Run %d MD%d ADC-, no TDC",run_number,md_number), 200, 0, 250);
  TH1D* tdcph = new TH1D("TDCP", "TDCP data", 1650, 0, 0);
  TH1D* tdcmh = new TH1D("TDCM", "TDCM data", 1650, 0, 0);
  TH1D* adcpped = new TH1D("ADCPPED", "ADCP pedestal", 50, 0, 400);
  TH1D* adcmped = new TH1D("ADCMPED", "ADCM pedestal", 50, 0, 400);

  TH1D* radial_all= new TH1D("radial_all", Form("Run %d Octant %d Radial location, all tracks",run_number, md_number), 60 , 300, 360);
 TH1D* radial_hit= new TH1D("radial_hit", Form("Run %d Octant %d Radial location, MD hits",run_number, 
					       md_number), 60 , 300, 360);
 TH1D* radial_miss= new TH1D("radial_miss", Form("Run %d Octant %d Radial location, MD misses",run_number, md_number), 60 , 300, 360);
 TH1D* radial_miss_super= new TH1D("radial_miss_super", Form("Run %d Octant %d Radial location, MD misses",run_number, md_number), 60 , 300, 330);

  TH1D* azimuth_all= new TH1D("azimuth_all", Form("Run %d Octant %d Azimuth (y) location, all tracks",run_number, md_number), 130, -130, 130);
 TH1D* azimuth_hit= new TH1D("azimuth_hit", Form("Run %d Octant %d Azimuth (y) location, MD hits",run_number, 
					       md_number), 130 , -130, 130);
 TH1D* azimuth_miss= new TH1D("azimuth_miss", Form("Run %d Octant %d Azimuth (y) location, MD misses",run_number, md_number), 130 , -130, 130);

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
  TH2F* alltracks = new TH2F("alltracks", Form("Run %d Octant %d All Tracks", run_number, md_number), 400, -130, 130, 300, 320, 360);
  TH2F* mdpmisses = new TH2F("mdpmisses", Form("Run %d Octant %d MD+ singles Chart", run_number, md_number), 400, -130, 130, 300, 320, 360);
  TH2F* mdmmisses = new TH2F("mdmmisses", Form("Run %d Octant %d MD- singles Chart", run_number, md_number), 400, -130, 130, 300, 320, 360);

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

  // this bit is to get quick ADC pedestal estimates
  for (int i=0;i<10000;i++)
    {
      branch_event->GetEntry(i);
      branch->GetEntry(i);

      //      for(int num_p=0; num_p < fEvent->GetNumberOfPartialTracks(); num_p++)
      //	{
          Double_t pdataplus = mdp_t->GetValue();
	  Double_t pdataminus = mdm_t->GetValue();
	  Double_t padcp = mdp->GetValue();
	  Double_t padcm = mdm->GetValue();

	  if(pdataplus == 0 && pdataminus == 0)
	    {
	      adcpped->Fill(padcp);
	      adcmped->Fill(padcm);
	    }
	  //	}
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
	  if(i % 100000 == 0) cout <<"events processed so far:" << i << endl;
	  branch_event->GetEntry(i);
	  branch->GetEntry(i);

          Double_t pdataplus = mdp_t->GetValue();
	  Double_t pdataminus = mdm_t->GetValue();
	  Double_t padcp = mdp->GetValue();
	  Double_t padcm = mdm->GetValue();

	  Double_t xoffset, yoffset, xslope, yslope, x, y;
	  //    weight = 0;

	  int prompt_mdp = 0;
	  int prompt_mdm = 0;
	  int prompt_ts=0;
	  int early_hit=0;
	  int nhits=fEvent->GetNumberOfHits();
	  for(int j=0;j<nhits;++j){
	    hit_d=fEvent->GetHit(j);
  // look for Main Detector hits:  MainDetector data is "Region=5"
  // recall that in QwHits the "plane" for MD hits is the octant number, and element = 1 for PMT_positive, =2 for PMT_negative
  //   enumerate prompt MD TDC hits and events with prompt TS hit in same package
  //   look for events with early hits in the TS or MD PMTs
	    if(hit_d->GetRegion()==5 && hit_d->GetPlane()==md_number && hit_d->GetElement()==1 ) {
	      if (hit_d->GetTimeNs()<-150 && hit_d->GetTimeNs()>-210) prompt_mdp++;
	      if (hit_d->GetTimeNs()<=-210) early_hit++;
	    }
	    if(hit_d->GetRegion()==5 && hit_d->GetPlane()==md_number && hit_d->GetElement()==2 ) {
	      if (hit_d->GetTimeNs()<-150 && hit_d->GetTimeNs()>-210) prompt_mdm++;
	      if (hit_d->GetTimeNs()<=-210) early_hit++;
	    }	    
	    if(hit_d->GetRegion()==4 && hit_d->GetPlane()==package && hit_d->GetElement()==0) {
	      if (hit_d->GetTimeNs()>-140 && hit_d->GetTimeNs()<-135) prompt_ts++;
	      if (hit_d->GetTimeNs()<=-140) early_hit++;
	    }
	  }

	 for (int num_p=0; num_p< fEvent->GetNumberOfPartialTracks();num_p++)
	    {
	      pt=fEvent->GetPartialTrack(num_p);
              if (pt->GetRegion()==3 && pt->GetPackage()==package)
		{
	      r3_events++;
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
     
	      if (prompt_ts>0 && early_hit==0){
		alltracks->Fill(y,x);
		radial_all->Fill(x);
		azimuth_all->Fill(y);
	      }

	      if (prompt_mdp>0 && prompt_ts>0)
		{
		  adcph->Fill(adcpdata-adcpmean);
		  adcphz->Fill(adcpdata-adcpmean);
		  adcpp1->Fill(y,adcpdata-adcpmean);
		  adcpp2->Fill(y,adcpdata-adcpmean);
		  fulladcpp->Fill(y,adcpdata-adcpmean);
		  linadcpp->Fill(y,adcpdata-adcpmean);
		}

	      if (prompt_mdm>0 && prompt_ts>0)
		{
		  adcmh->Fill(adcmdata-adcmmean);
		  adcmhz->Fill(adcmdata-adcmmean);
		  adcmp1->Fill(y,adcmdata-adcmmean);
		  adcmp2->Fill(y,adcmdata-adcmmean);
		  fulladcmp->Fill(y,adcmdata-adcmmean);
		  linadcmp->Fill(y,adcmdata-adcmmean);
		}

	      if ((prompt_mdp>0 && adcmdata > ped_mdm) || ( prompt_mdm>0 && adcpdata > ped_mdp) && prompt_ts>0 && early_hit==0)
		    {
		      hits->Fill(y,x);
		      radial_hit->Fill(x);
		      azimuth_hit->Fill(y);
		      md_adc->Fill(adcmdata-adcmmean + adcpdata-adcpmean);
		      if (x>330 && x<340) md_adc_center->Fill(adcmdata-adcmmean + adcpdata-adcpmean);
		    }
		
	      if ((prompt_mdp> 0 || prompt_mdm > 0) && prompt_ts > 0 && early_hit==0)
		{
		  md_adc_loose->Fill(adcmdata-adcmmean + adcpdata-adcpmean);
		}



	      if (prompt_mdp == 0 && prompt_mdm > 0 && prompt_ts > 0 && early_hit==0 && x>326)
		{
		  adcpthresh->Fill(adcpdata-adcpmean);
		}

	      if (prompt_mdm == 0 && prompt_mdp > 0 && prompt_ts > 0 && early_hit==0 && x>326)
		{
		  adcmthresh->Fill(adcmdata-adcmmean);
		}

	      if (prompt_mdp == 0 && prompt_mdm == 0 && prompt_ts > 0 && early_hit==0)
		{
		  misses->Fill(y,x);
		  radial_miss->Fill(x);
		  azimuth_miss->Fill(y);
		  if (x<330){
		    radial_miss_super->Fill(x);
		  }
		}

	      if (prompt_mdp > 0 && prompt_mdm == 0 && adcmdata < ped_mdm && prompt_ts > 0 && early_hit==0)
		{
		  mdpmisses->Fill(y,x);
	       	}

	      if (prompt_mdm > 0 && prompt_mdp == 0 && adcpdata < ped_mdp && prompt_ts > 0 && early_hit==0)
		{
		  mdmmisses->Fill(y,x);
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

		  if (prompt_mdp>0 && prompt_ts>0)
		    {
		      if (prompt_mdm>0)
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

  FILE* mean;

  cout << "mean radial position = " << effmean << "   (cm)  " << endl;

  mean = fopen("mean.txt","a");

  fprintf(mean, "%d \t %d \t %d \t %f\n", run_number, package, md_number, effmean);

  
  fclose(mean);


  /*
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

  */

  pedestals = new TCanvas("pedestals", "Pedestal Data", 10, 10, 800, 800);
  pedestals->Divide(1,2);

  pedestals->cd(1);
  adcpped->Draw();

  pedestals->cd(2);
  adcmped->Draw();

  pedestals->SaveAs(outputPrefix+"pedestals.png");

  thresholds = new TCanvas("thresholds", "Discrim. thresholds", 10, 10, 800, 800);
  thresholds->Divide(2,2);

  thresholds->cd(1);
  adcpthresh->Draw();

  thresholds->cd(2);
  adcmthresh->Draw();

  thresholds->cd(3);
  adcphz->Draw();

  thresholds->cd(4);
  adcmhz->Draw();

  thresholds->SaveAs(outputPrefix+"thresholds.png");

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

  quad->SaveAs(outputPrefix+"adc_tdc.png");

  mdadc =  new TCanvas("mdadc", "MD ADC (summed)", 10, 10, 800, 800);

  mdadc->Divide(2,2);

  mdadc->cd(1);
  md_adc->Draw();

  mdadc->cd(2);
  md_adc_center->Draw();

  mdadc->cd(3);
  md_adc_loose->Draw();

  mdadc->SaveAs(outputPrefix+"md_adc.png");

  hitandmiss = new TCanvas("hitandmiss", "Hit and Miss Charts", 10, 10, 800, 800);
  hitandmiss->Divide(1,3);

  hitandmiss->cd(1);
  //hits->SetContour(20,[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]);
  hits->Draw("COLZ");

  hitandmiss->cd(2);
  //misses->SetContour(20,[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]);
  misses->Draw("COLZ");

  hitandmiss->cd(3);
  alltracks->Draw("COLZ");

  hitandmiss->SaveAs(outputPrefix+"hit_and_miss.png");

  mdmiss = new TCanvas("mdmiss", "Single Tube hit Charts", 10, 10, 800, 800);
  mdmiss->Divide(1,3);

  mdmiss->cd(1);
  hits->Draw("COLZ");

  mdmiss->cd(2);
  mdpmisses->Draw("COLZ");

  mdmiss->cd(3);
  mdmmisses->Draw("COLZ");

  mdmiss->SaveAs(outputPrefix+"singleTubeHits.png");

  radial = new TCanvas("radial", "Radial Distributions", 10, 10, 800, 800);
  radial->Divide(2,2);

  radial->cd(1);
  radial_all->Draw();

  radial->cd(2);
  radial_hit->Draw();

  radial->cd(3);
  radial_miss->Draw();

  radial->cd(4);
  radial_miss_super->Draw();

  radial->SaveAs(outputPrefix+"Radial_Hists.png");

  azimuth = new TCanvas("azimuth", "Azimuth Distributions", 10, 10, 800, 800);
  azimuth->Divide(2,2);

  azimuth->cd(1);
  azimuth_all->Draw();

  azimuth->cd(2);
  azimuth_hit->Draw();

  azimuth->cd(3);
  azimuth_miss->Draw();

  azimuth->SaveAs(outputPrefix+"Azimuth_Hists.png");
  /*

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

  */


  EFF = new TCanvas("EFF", "Efficiency Plot", 10, 10, 600, 600);
  effplot = new TGraph(size, xpos, efficiency);
  effplot->SetMarkerStyle(20);
  effplot->SetMarkerColor(4);
  effplot->GetXaxis()->SetTitle("MD Radial location (cm)");
  effplot->SetTitle(Form("MD %d efficiency vs. radial track location",md_number));
  effplot->Draw("AP");
  TPaveText *myText= new TPaveText(0.30,0.2,0.7,0.27, "NDC");
  myText->SetTextSize(0.025); 
  myText->SetFillColor(0);
  myText->SetTextAlign(12);
  myTextEntry = myText->AddText(Form("Mean Location = %0.2f  cm",effmean));
  myText->Draw();
  TPaveText *RunNumText= new TPaveText(0.30,0.28,0.52,0.33, "NDC");
  RunNumText->SetTextSize(0.025); 
  RunNumText->SetFillColor(42);
  RunNumText->SetTextAlign(12);
  RunNumTextEntry = RunNumText->AddText(Form("Run Number %d",run_number));
  RunNumText->Draw();

  EFF->SaveAs(outputPrefix+"detector_image.png");

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


  /*
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

  fprintf(nslope_gja, "%d \t %d \t %d \t %d \t %f \t %f \t %f \t %f \t %f \t %f\n",run_number, package, md_number, r3_events, gjap, gjam, nslopep1, nslopep2, nslopem1, nslopem2);



  fclose(pedestal_mean);
  fclose(slope_intercept);
  fclose(nslope_gja);

  */
  return;
}
