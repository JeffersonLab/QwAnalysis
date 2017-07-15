//  r3_lightmap_3pass.C
//
// authors: Siyuan Yang, David Armstrong, Eric Henderson
//
//
//    Modified for very special purposes for 3-pass data (no valid
//  MD ADCs, thus just useful for Q^2, W distributions on MDs, and also
//  the "efficiency plot" (TDC based).
//
//   This script is for light map studies in the Main Detector using
// projected tracks from Region 3. Looks at radial (x) and azimuthal (y)
// dimensions.
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
//    Results are output in several .txt files:
//           mean.txt: 
//                 mean value of the radial location of the MD
//                 based on where R3 projected tracks "hit" detector
//
//           pedestal_mean: 
//                  MD ADC pedestals
//
//           slope_intercept.txt:
//             intercepts and slopes of linear fits to PMT ADC pulseheight
//             vs. y (position on long axis of detector). Separate fits 
//             are done for "+" and "-" tube on each side of the glue joint
//             (y=0). Also, the number of "trials" (hits+misses) output.
//
//           nslope_gja.txt:
//              normalized slopes (i.e. slope/intercept, i.e. fractional
//              pulseheight loss vs y (fraction/cm); again, for both
//             "+" and "-" tube on each side of the glue joint (y=0).
//             Also, "glue joint attentuation factor": ratio of pulseheight
//             on either side of glue joint (using y=0 intercepts of fits)
//             as seen by the "+" and +-" tubes separately. 
//             Also, the number of "trials" (hits+misses) output.
//              
//
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


void project_root(int package=1,int md_number=1,int run_number=6327,int max_events=-1,TString file_suffix="Qweak_",string command="MD_")
{

  gROOT->SetBatch(1);  // don't write canvasses to screen

  Double_t md_zpos[9] = {0.0, 576.665,  576.705, 577.020, 577.425, 577.515,  577.955, 577.885, 577.060};

  TString outputPrefix(Form("LightMap_run%d_MD%d_",run_number,md_number));
  TString runPrefix(Form("run %d MD%d",run_number,md_number));
  TString file_name = "";
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

  bool pe=false;
  if (command.find("photon")==string::npos)
    pe=false;
  else pe=true;

  //  int mode=0;   //p,m,m+p,p-m;starting from 1
  int mode=3;   // make mode=3 the default
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

  TProfile2D* hQ2_2d;
 hQ2_2d=new TProfile2D(Form("hQ2_2d %s profile",w_title.c_str()),"hQ2_2d ",20,-110,110,10,320,360);

  TProfile2D* hW_2d;
 hW_2d=new TProfile2D(Form("hW_2d %s profile",w_title.c_str()),"hW_2d ",10,-110,110,10,320,360);

  TProfile2D* hp_2d;
  hp_2d=new TProfile2D(Form("hp_2d %s profile",w_title.c_str()),"hp_2d ",240,0,0,480,0,0);

  TProfile2D* hL_2d;
  hL_2d=new TProfile2D(Form("hL_2d %s profile",w_title.c_str()),"hL_2d ",110,-110,110,40,320,360);

  TProfile2D* hL_2d_ratio;
  hL_2d_ratio=new TProfile2D(Form("hL_2d_ratio %s profile",w_title.c_str()),"hL_2d_ratio ",110,-110,110,40,320,360);

  TProfile2D* hL_2d_pos;
  hL_2d_pos=new TProfile2D(Form("hL_2d_pos %s profile",w_title.c_str()),"hL_2d_pos ",110,-110,110,40,320,360);

  TProfile2D* hL_2d_neg;
  hL_2d_neg=new TProfile2D(Form("hL_2d_neg %s profile",w_title.c_str()),"hL_2d_neg ",110,-110,110,40,320,360);

  TH1D* h_1f=new TH1D("project run","project run",240,280,400);
  TH1D* adcph = new TH1D("ADCP", "ADCP data", 400, 0, 4000);
  TH1D* adcmh = new TH1D("ADCM", "ADCM data", 400, 0, 4000);
  TH1D* tdcph = new TH1D("TDCP", "TDCP data", 1650, 0, 0);
  TH1D* tdcmh = new TH1D("TDCM", "TDCM data", 1650, 0, 0);
  TH1D* adcpped = new TH1D("ADCPPED", "ADCP pedestal", 50, 0, 400);
  TH1D* adcmped = new TH1D("ADCMPED", "ADCM pedestal", 50, 0, 400);

  TH1D* adc_sum = new TH1D("adc_sum", "summed ADC spectrum", 500, 0, 5000);
  TH1D* adc_sum_tight = new TH1D("adc_sum_tight", "summed ADC spectrum, y= 10-20cm", 500, 0, 5000);

  TH1D* hits_x = new TH1D("hits_x", runPrefix+"  x location of hits ", 40, 320,360);
  TH1D* hits_y = new TH1D("hits_y", runPrefix+"  y location of hits ", 120, -120, 120);
  TH1D* hits_yangle = new TH1D("hits_xangle", runPrefix+"  theta_x of hits ", 60, 0.20, 0.50);

  TProfile* adcpp1;
  TProfile* adcpp2;
  TProfile* adcmp1;
  TProfile* adcmp2;
  TProfile* fulladcpp;
  TProfile* fulladcmp;
  TProfile* x1adcpp;
  TProfile* x1adcmp;
  TProfile* x2adcpp;
  TProfile* x2adcmp;
  TProfile* x3adcpp;
  TProfile* xdistribution;
  TProfile* ydistribution;

  TProfile* xdist_total;
  TProfile* xdist_pos;
  TProfile* xdist_neg;
  TProfile* xangle_total;
  TProfile* xangle_pos;
  TProfile* xangle_neg;
  TProfile* xdist_pos_1;
  TProfile* xdist_neg_1;
  TProfile* xdist_pos_2;
  TProfile* xdist_neg_2;
  TProfile* xdist_pos_3;
  TProfile* xdist_neg_3;
  TProfile* xdist_pos_4;
  TProfile* xdist_neg_4;
  TProfile* xdist_pos_5;
  TProfile* xdist_neg_5;
  TProfile* xdist_pos_6;
  TProfile* xdist_neg_6;

  TProfile* xangle_pos_1;
  TProfile* xangle_neg_1;
  TProfile* xangle_pos_2;
  TProfile* xangle_neg_2;
  TProfile* xangle_pos_3;
  TProfile* xangle_neg_3;
  TProfile* xangle_pos_4;
  TProfile* xangle_neg_4;
  TProfile* xangle_pos_5;
  TProfile* xangle_neg_5;
  TProfile* xangle_pos_6;
  TProfile* xangle_neg_6;

  adcpp1 = new TProfile("ADCPP1", Form("Run %d  Oct. %d +PMT far end", run_number, md_number), 50, -100, 0);
  adcpp2 = new TProfile("ADCPP2", Form("Run %d  Oct. %d +PMT near end", run_number, md_number), 50, 0, 100);
  adcmp1 = new TProfile("ADCMP1", Form("Run %d  Oct. %d -PMT near end", run_number, md_number), 50, -100, 0);
  adcmp2 = new TProfile("ADCMP2", Form("Run %d  Oct. %d -PMT far end", run_number, md_number), 50, 0, 100);
  fulladcpp = new TProfile("FULLADCPP", Form("Linear Fit Run %d Octant %d positive PMT", run_number, md_number), 150, -105, 105);
  fulladcmp = new TProfile("FULLADCMP", Form("LInear Fit Run %d Octant %d minus PMT", run_number, md_number), 150, -105, 105);
  x1adcpp = new TProfile("X1P", "X1 ADCP profile", 200, -110, 110);
  x1adcmp = new TProfile("X1M", "X1 ADCM profile", 200, -110, 110);
  x2adcpp = new TProfile("X2P", "X2 ADCP profile", 200, -110, 110);
  x2adcmp = new TProfile("X2M", "X2 ADCM profile", 200, -110, 110);
  x3adcpp = new TProfile("X3P", "X3 ADCP profile", 200, -110, 110);
  x3adcmp = new TProfile("X3M", "X3 ADCM profile", 200, -110, 110);

  xdistribution = new TProfile("xdistribution",runPrefix+"  total ADC pulse height vs. x", 140, 320, 355);
  ydistribution = new TProfile("ydistribution",runPrefix+"  total ADC pulse height vs. y", 200, -110, 110);

  xdist_total = new TProfile("xdistribution",runPrefix+"   Summed PMT ADC vs. x", 24, 324, 348);
  xdist_pos = new TProfile("xdist_pos",runPrefix+"   PMT+ ADC vs. x", 24, 324, 348);
  xangle_total = new TProfile("xangle_total",runPrefix+" Summed PMT ADC vs. theta_x", 24, 0.32, 0.48);
  xangle_pos = new TProfile("xangle_pos",runPrefix+"   PMT+ ADC vs. theta_x", 24, 0.32, 0.48);
  xangle_neg = new TProfile("xangle_neg",runPrefix+"   PMT- ADC vs. theta_x", 24, 0.32, 0.48);
  xdist_pos_1 = new TProfile("xdist_pos_1","PMT+ ADC vs. x, y = -10 to -20", 24, 324, 348);
  xdist_pos_2 = new TProfile("xdist_pos_2","PMT+ ADC vs. x, y = -40 to -50", 24, 324, 348);
  xdist_pos_3 = new TProfile("xdist_pos_3","PMT+ ADC vs. x, y = -80 to -90", 24, 324, 348);
  xdist_pos_4 = new TProfile("xdist_pos_4","PMT+ ADC vs. x, y = +10 to +20", 24, 324, 348);
  xdist_pos_5 = new TProfile("xdist_pos_5","PMT+ ADC vs. x, y = +40 to +50", 24, 324, 348);
  xdist_pos_6 = new TProfile("xdist_pos_6","PMT+ ADC vs. x, y = +80 to +90", 24, 324, 348);
  xdist_neg = new TProfile("xdist_neg",runPrefix+"  PMT- ADC vs. x", 24, 324, 348);
  xdist_neg_1 = new TProfile("xdist_neg_1","PMT- ADC vs. x, y = -10 to -20", 24, 324, 348);
  xdist_neg_2 = new TProfile("xdist_neg_2","PMT- ADC vs. x, y = -40 to -50", 24, 324, 348);
  xdist_neg_3 = new TProfile("xdist_neg_3","PMT- ADC vs. x, y = -80 to -90", 24, 324, 348);
  xdist_neg_4 = new TProfile("xdist_neg_4","PMT- ADC vs. x, y = +10 to +20", 24, 324, 348);
  xdist_neg_5 = new TProfile("xdist_neg_5","PMT- ADC vs. x, y = +40 to +50", 24, 324, 348);
  xdist_neg_6 = new TProfile("xdist_neg_6","PMT- ADC vs. x, y = +80 to +90", 24, 324, 348);

  xangle_pos_1 = new TProfile("xangle_pos_1","PMT+ ADC vs. theta_x, y = -10 to -20", 24, 0.32, 0.48);
  xangle_pos_2 = new TProfile("xangle_pos_2","PMT+ ADC vs. theta_x, y = -40 to -50", 24, 0.32, 0.48);
  xangle_pos_3 = new TProfile("xangle_pos_3","PMT+ ADC vs. theta_x, y = -80 to -90", 24, 0.32, 0.48);
  xangle_pos_4 = new TProfile("xangle_pos_4","PMT+ ADC vs. theta_x, y = +10 to +20", 24, 0.32, 0.48);
  xangle_pos_5 = new TProfile("xangle_pos_5","PMT+ ADC vs. theta_x, y = +40 to +50", 24, 0.32, 0.48);
  xangle_pos_6 = new TProfile("xangle_pos_6","PMT+ ADC vs. theta_x, y = +80 to +90", 24, 0.32, 0.48);
  xangle_neg_1 = new TProfile("xangle_neg_1","PMT- ADC vs. theta_x, y = -10 to -20", 24, 0.32, 0.48);
  xangle_neg_2 = new TProfile("xangle_neg_2","PMT- ADC vs. theta_x, y = -40 to -50", 24, 0.32, 0.48);
  xangle_neg_3 = new TProfile("xangle_neg_3","PMT- ADC vs. theta_x, y = -80 to -90", 24, 0.32, 0.48);
  xangle_neg_4 = new TProfile("xangle_neg_4","PMT- ADC vs. theta_x, y = +10 to +20", 24, 0.32, 0.48);
  xangle_neg_5 = new TProfile("xangle_neg_5","PMT- ADC vs. theta_x, y = +40 to +50", 24, 0.32, 0.48);
  xangle_neg_6 = new TProfile("xangle_neg_6","PMT- ADC vs. theta_x, y = +80 to +90", 24, 0.32, 0.48);

  TH2F* hits = new TH2F("hits", Form("Run %d Octant %d Hit Chart", run_number, md_number), 130, -130, 130, 80, 280, 360);
  TH2F* misses = new TH2F("misses", Form("Run %d Octant %d Miss Chart", run_number, md_number), 130, -130, 130, 80, 280, 360);

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


  for (int i=0;i<10000;i++)
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
  Int_t trial_total=0;
  Int_t bin;
  Int_t xmin = 323;
  Double_t width = 0.2;
  Double_t xpos[125];
  Double_t efficiency[125];
  Double_t Q2_val = 0;
  Double_t W2_val = 0;
  Double_t W_val = 0;

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
	  if(i % 10000 == 0) cout <<"events processed so far:" << i << endl;
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

	      Double_t xxslope = xslope*cos(theta) - yslope*sin(theta);
	      xslope = xxslope;

	      //	      Double_t m = padcm;
	      //	      Double_t p = padcp;

	      Double_t weight = 1.0;
	      Double_t adcpdata = mdp->GetValue();
	      Double_t adcmdata = mdm->GetValue();
	      Double_t tdcpdata = mdp_t->GetValue();
	      Double_t tdcmdata = mdm_t->GetValue();
	      int p_t = tdcpdata;
	      int m_t = tdcmdata;
	      //	      int p_t = pdataplus;
	      //	      int m_t = pdataminus;
	      if (pe==true) {
		adcpdata = adcpdata/pe_convert[2*md_number-1];
		adcmdata = adcmdata/pe_convert[2*(md_number-1)];
	      }
	      
	      switch (mode)
                {
                case 1:
		  weight = adcpdata - adcpmean;
		  break;
                case 2:
		  weight = adcmdata - adcmmean;
		  break;
                case 3:
		  weight = (adcpdata-adcpmean) + (adcmdata-adcmmean);
		  break;
                case 4:
		  weight = adcpdata-adcmdata - adcpmean + adcmmean;
		  break;
                default:
		  weight = 1.0;
                }

	      //  printf("x %10.2f y %10.2f weight %10.2f mdp %10.2f mdm %10.2f \n", x, y, weight, p, m);

	      if (p_t < -150 && p_t > -250 && m_t <-150 && p_t > -250 && fEvent->GetNumberOfTracks()>0)
                {
	  Q2_val=fEvent->fKinWithLoss.fQ2;
	  W2_val=fEvent->fKinWithLoss.fW2;
	  W_val = sqrt(W2_val);
	  if (W_val < 1.9) cout << "event  = " << i << "   W_val = "  << W_val << endl;
	  //	  cout << "event = " << i << "   W_val = " << W_val << endl;
	  //	  cout << "event = " << i << "   Q2_val = " << Q2_val << endl;
	  //	  double ntracks=fEvent->GetNumberOfTracks();
	  //	  cout << "event = " << i << "  number of tracks = " << ntracks << endl;
		    hp_2d->Fill(x,y,weight);
		    hL_2d->Fill(y,x,weight);
		    (if W_val>0.9){
		      hW_2d->Fill(y,x,W_val);
		      hQ2_2d->Fill(y,x,Q2_val*1000);
		    }
		    hL_2d_ratio->Fill(y,x,2.0+log10(((adcpdata-adcpmean)/(adcmdata-adcmmean+.001))));
		    //		    cout << adcpdata << "  " << adcmdata << " " << 2.0+log10(((adcpdata-adcpmean)/(adcmdata-adcmmean+.001))) << "  " << weight << "  " << mode << "  " << adcpmean << "  " << adcmmean << "   "  << log10(((adcpdata-adcpmean)/(adcmdata-adcmmean+.001))) << endl;
		    hL_2d_pos->Fill(y,x,adcpdata-adcpmean);
		    hL_2d_neg->Fill(y,x,adcmdata-adcmmean);
                }

	      if (tdcpdata < -150 && tdcpdata > -250)
		{
		  adcph->Fill(adcpdata);
		  //		  if(x>330 && x<335){
		    adcpp1->Fill(y,adcpdata-adcpmean);
		    adcpp2->Fill(y,adcpdata-adcpmean);
		    fulladcpp->Fill(y,adcpdata-adcpmean);
		    //		  }
		}

	      if (tdcmdata < -150 && tdcmdata > -250)
		{
		  adcmh->Fill(adcmdata);
		  adcmp1->Fill(y,adcmdata-adcmmean);
		  adcmp2->Fill(y,adcmdata-adcmmean);
		  fulladcmp->Fill(y,adcmdata-adcmmean);

		  if(tdcpdata < -150 && tdcpdata > -250 && fEvent->GetNumberOfTracks()>0)
		    {
		      hits->Fill(y,x);
		      hits_x->Fill(x);
		      hits_y->Fill(y);
		      hits_xangle->Fill(xslope);

		      xdistribution->Fill(x,adcmdata-adcmmean+adcpdata-adcpmean);
		      ydistribution->Fill(y,adcmdata-adcmmean+adcpdata-adcpmean);
		      if(y<95 && y> -95){   // avoid very ends of bars (PMT shield hits)
			xdist_pos->Fill(x,adcpdata-adcpmean);
			xdist_neg->Fill(x,adcmdata-adcmmean);
			xangle_pos->Fill(xslope,adcpdata-adcpmean);
			xangle_neg->Fill(xslope,adcmdata-adcmmean);
			xangle_total->Fill(xslope,adcpdata-adcpmean+adcmdata-adcmmean);
			xangle_neg->Fill(xslope,adcmdata-adcmmean);

			if (y>-20 && y<-10){
			  xdist_pos_1->Fill(x,adcpdata-adcpmean);
			  xdist_neg_1->Fill(x,adcmdata-adcmmean);
			  xangle_pos_1->Fill(xslope,adcpdata-adcpmean);
			  xangle_neg_1->Fill(xslope,adcmdata-adcmmean);
			}
			if (y>-50 && y<-40){
			  xdist_pos_2->Fill(x,adcpdata-adcpmean);
			  xdist_neg_2->Fill(x,adcmdata-adcmmean);
			  xangle_pos_2->Fill(xslope,adcpdata-adcpmean);
			  xangle_neg_2->Fill(xslope,adcmdata-adcmmean);
			}
			if (y>-90 && y<-80){
			  xdist_pos_3->Fill(x,adcpdata-adcpmean);
			  xdist_neg_3->Fill(x,adcmdata-adcmmean);
			  xangle_pos_3->Fill(xslope,adcpdata-adcpmean);
			  xangle_neg_3->Fill(xslope,adcmdata-adcmmean);
			}
			if (y>10 && y<20){
			  xdist_pos_4->Fill(x,adcpdata-adcpmean);
			  xdist_neg_4->Fill(x,adcmdata-adcmmean);
			  xangle_pos_4->Fill(xslope,adcpdata-adcpmean);
			  xangle_neg_4->Fill(xslope,adcmdata-adcmmean);
			}
			if (y>40 && y<50){
			  xdist_pos_5->Fill(x,adcpdata-adcpmean);
			  xdist_neg_5->Fill(x,adcmdata-adcmmean);
			  xangle_pos_5->Fill(xslope,adcpdata-adcpmean);
			  xangle_neg_5->Fill(xslope,adcmdata-adcmmean);
			}
			if (y>80 && y<90){
			  xdist_pos_6->Fill(x,adcpdata-adcpmean);
			  xdist_neg_6->Fill(x,adcmdata-adcmmean);
			  xangle_pos_6->Fill(xslope,adcpdata-adcpmean);
			  xangle_neg_6->Fill(xslope,adcmdata-adcmmean);
			}
		      }
		      adc_sum->Fill(adcmdata-adcmmean + adcpdata-adcpmean);
		      if (y>10 && y<20){
			adc_sum_tight->Fill(adcmdata-adcmmean+ adcpdata-adcpmean);
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
		    }
		}
	      if( fEvent->GetNumberOfTracks()>0){
	      if (tdcpdata == 0 && tdcmdata == 0 && fEvent->GetNumberOfTracks()>0)
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
		  trial_total++;

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


  c = new TCanvas("c","Region 3 Projections",10, 10, 900,500);
  c->Divide(2,2);
  c->cd(1);
    {
      hL_2d->GetXaxis()->SetTitle("position x:cm");
      hL_2d->GetYaxis()->SetTitle("position y:cm");
      hL_2d->SetTitle(runPrefix+"  Light Map, summed PMTs");
      hL_2d->Draw("colz");
    }

  c->cd(2);
    {
      hL_2d_ratio->GetXaxis()->SetTitle("position x:cm");
      hL_2d_ratio->GetYaxis()->SetTitle("position y:cm");
      hL_2d_ratio->SetTitle("Light Map, 2.0 + log10(PMT+/PMT-)");
      hL_2d_ratio->Draw("colz");
    }
    
  c->cd(4);
    {
      hL_2d_pos->GetXaxis()->SetTitle("position x:cm");
      hL_2d_pos->GetYaxis()->SetTitle("position y:cm");
      hL_2d_pos->SetTitle("Light Map, positive PMT");
      hL_2d_pos->Draw("colz");
    }
  c->cd(3);  
    {
      hL_2d_neg->GetXaxis()->SetTitle("position x:cm");
      hL_2d_neg->GetYaxis()->SetTitle("position y:cm");
      hL_2d_neg->SetTitle("Light Map, negative PMT");
      hL_2d_neg->Draw("colz");
    }

  c->SaveAs(outputPrefix+"R3_projections.png");
  c->SaveAs(outputPrefix+"R3_projections.pdf");
  c->SaveAs(outputPrefix+"R3_projections.C");


  c2 = new TCanvas("c2","Q2 map",10, 10, 600,300);
  string titleq="Q^{2} distribution on Main Detector (mGeV^{2})";
  gStyle->SetStatX(0.2); 
  hQ2_2d->GetXaxis()->SetTitle("position y:cm");
  hQ2_2d->GetYaxis()->SetTitle("position x:cm");
  hQ2_2d->SetTitle(titleq.c_str());
  hQ2_2d->Draw("colz");
  c2->SaveAs(outputPrefix+"Q2_map.pdf");

  cw = new TCanvas("cw","W map",10, 10, 600,300);
  string titleq="W distribution on Main Detector (GeV)";
  gStyle->SetStatX(0.2); 
  hW_2d->GetXaxis()->SetTitle("position y:cm");
  hW_2d->GetYaxis()->SetTitle("position x:cm");
  hW_2d->SetTitle(titleq.c_str());
  hW_2d->SetMaximum(2.4);
  hW_2d->SetMinimum(1.9);
  hW_2d->Draw("colz");
  cw->SaveAs(outputPrefix+"W_map.pdf");

  /*  
  c = new TCanvas("c","Region 3 Projections",10, 10, 500,500);
  c->Divide(2,2);
  c->cd(1);
    {
      string title="track projection on " + type + "  " +  w_title + ": profile";
      hp_2d->GetXaxis()->SetTitle("position x:cm");
      hp_2d->GetYaxis()->SetTitle("position y:cm");
      hp_2d->SetTitle(title.c_str());
      hp_2d->Draw("colz");
    }
    
  c->cd(2);
  hp_2d->ProjectionX()->Draw();
  c->cd(3);
  hp_2d->ProjectionY()->Draw();
  c->cd(4);
  adc_sum->Draw();

  c->SaveAs(outputPrefix+"R3_projections.pdf");
  c->SaveAs(outputPrefix+"R3_projections.");

  pedestals = new TCanvas("pedestals", "Pedestal Data", 10, 10, 700, 700);
  pedestals->Divide(2,2);

  pedestals->cd(1);
  adcpped->Draw();

  pedestals->cd(2);
  adcmped->Draw();

  pedestals->cd(3);
  adc_sum->Draw();

  pedestals->cd(4);
  adc_sum_tight->Draw();

  pedestals->SaveAs(outputPrefix+"adc_pedestals_sums.pdf");
  pedestals->SaveAs(outputPrefix+"adc_pedestals_sums.C");

  quad = new TCanvas("quad", "ADC and TDC data", 10, 10, 600, 600);
  quad->Divide(2,2);

  quad->cd(1);
  adcph->Draw();

  quad->cd(2);
  adcmh->Draw();

  quad->cd(3);
  tdcph->Draw();

  quad->cd(4);
  tdcmh->Draw();
  
  quad->SaveAs(outputPrefix+"adc_tdc.pdf");
  */
  hitandmiss = new TCanvas("hitandmiss", "Hit and Miss Charts", 10, 10, 800, 800);

  hitandmiss->Divide(1,2);

  hitandmiss->cd(1);
  //hits->SetContour(20,[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]);
  hits->Draw("COLZ");

  hitandmiss->cd(2);
  //misses->SetContour(20,[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]);
  misses->Draw("COLZ");
  hitandmiss->SaveAs(outputPrefix+"hit_and_miss.pdf");
  hitandmiss->SaveAs(outputPrefix+"hit_and_miss.png");

  hits_c = new TCanvas("hits_c", runPrefix+"  Hit distributions", 10, 10, 800, 800);

  hits_c->Divide(2,2);
  hits_c->cd(1);
  hits_y->Draw();
  hits_c->cd(2);
  hits_x->Draw();
  hits_c->cd(3);
  hits_xangle->Draw();

  hits_c->SaveAs(outputPrefix+"hits.pdf");
  hits_c->SaveAs(outputPrefix+"hits.png");


  xangles = new TCanvas("xangles", "PMT ADC vs theta_x", 10, 10, 1000, 1600);
  xangles->Divide(2,2);
  xangle_pos->SetLineColor(2);
  xangle_neg->SetLineColor(4);
  xangles->cd(1);
  xangle_pos->GetXaxis()->SetTitle("theta_x (rad)");
  xangle_neg->GetXaxis()->SetTitle("theta_x (rad)");
  xangle_pos->GetYaxis()->SetTitle("ADC");
  xangle_neg->GetYaxis()->SetTitle("ADC");
  xangle_pos->Draw();
  xangles->cd(2);
  xangle_neg->Draw();
  xangles->cd(3);
  xangle_pos->Draw();
  xangle_neg->Draw("same");
  xangles->cd(4);
  xangle_total->Draw();
  xangles->SaveAs(outputPrefix+"xangles.pdf");
  xangles->SaveAs(outputPrefix+"xangles.C");
  xangles->SaveAs(outputPrefix+"xangles.png");
 

  xdists3 = new TCanvas("xdists3", "PMT ADC vs x.", 10, 10, 1000, 1600);
  xdists3->Divide(2,4);
  xdists3->cd(1);
  xdist_pos->SetLineColor(2);
  xdist_neg->SetLineColor(4);
  xdist_pos->DrawNormalized("",1.0);
  xdist_pos->GetXaxis()->SetTitle("x (cm)");
  xdist_pos->GetYaxis()->SetTitle("ADC");
  xdists3->cd(2);
  xdist_neg->DrawNormalized("",1.0);
  xdist_neg->GetXaxis()->SetTitle("x (cm)");
  xdist_neg->GetYaxis()->SetTitle("ADC");

  xdists3->cd(3);
  xdist_pos_1->SetLineColor(2);
  xdist_neg_1->SetLineColor(4);
  xdist_pos_1->DrawNormalized("",1.0);
  xdist_pos_1->GetXaxis()->SetTitle(runPrefix+"   x (cm)");
  xdist_pos_1->GetYaxis()->SetTitle("ADC");
  xdist_neg_1->DrawNormalized("same",1.0);
  xdist_neg_1->GetXaxis()->SetTitle(runPrefix+"    x(cm)");
  xdist_neg_1->GetYaxis()->SetTitle("ADC");

  xdists3->cd(4);
  xdist_pos_2->SetLineColor(2);
  xdist_neg_2->SetLineColor(4);
  xdist_pos_2->DrawNormalized("",1.0);
  xdist_pos_2->GetXaxis()->SetTitle(runPrefix+"    x(cm)");
  xdist_pos_2->GetYaxis()->SetTitle("ADC");
  xdist_neg_2->DrawNormalized("same",1.0);
  xdist_neg_2->GetXaxis()->SetTitle(runPrefix+"    x(cm)");
  xdist_neg_2->GetYaxis()->SetTitle("ADC");

  xdists3->cd(5);
  xdist_pos_3->SetLineColor(2);
  xdist_neg_3->SetLineColor(4);
  xdist_pos_3->DrawNormalized("",1.0);
  xdist_pos_3->GetXaxis()->SetTitle(runPrefix+"    x(cm)");
  xdist_pos_3->GetYaxis()->SetTitle("ADC");
  xdist_neg_3->DrawNormalized("same",1.0);
  xdist_neg_3->GetXaxis()->SetTitle(runPrefix+"    x(cm)");
  xdist_neg_3->GetYaxis()->SetTitle("ADC");

  xdists3->cd(6);
  xdist_pos_4->SetLineColor(2);
  xdist_neg_4->SetLineColor(4);
  xdist_pos_4->DrawNormalized("",1.0);
  xdist_pos_4->GetXaxis()->SetTitle(runPrefix+"    x(cm)");
  xdist_pos_4->GetYaxis()->SetTitle("ADC");
  xdist_neg_4->DrawNormalized("same",1.0);
  xdist_neg_4->GetXaxis()->SetTitle(runPrefix+"    x(cm)");
  xdist_neg_4->GetYaxis()->SetTitle("ADC");

  xdists3->cd(7);
  xdist_pos_5->SetLineColor(2);
  xdist_neg_5->SetLineColor(4);
  xdist_pos_5->DrawNormalized("",1.0);
  xdist_pos_5->GetXaxis()->SetTitle(runPrefix+"    x(cm)");
  xdist_pos_5->GetYaxis()->SetTitle("ADC");
  xdist_neg_5->DrawNormalized("same",1.0);
  xdist_neg_5->GetXaxis()->SetTitle(runPrefix+"    x(cm)");
  xdist_neg_5->GetYaxis()->SetTitle("ADC");

  xdists3->cd(8);
  xdist_pos_6->SetLineColor(2);
  xdist_neg_6->SetLineColor(4);
  xdist_pos_6->DrawNormalized("",1.0);
  xdist_pos_6->GetXaxis()->SetTitle(runPrefix+"    x(cm)");
  xdist_pos_6->GetYaxis()->SetTitle("ADC");
  xdist_neg_6->DrawNormalized("same",1.0);
  xdist_neg_6->GetXaxis()->SetTitle(runPrefix+"    x(cm)");
  xdist_neg_6->GetYaxis()->SetTitle("ADC");

  xdists3->SaveAs(outputPrefix+"xdists3.pdf");
  xdists3->SaveAs(outputPrefix+"xdists3.C");
  xdists3->SaveAs(outputPrefix+"xdists3.png");


  xangles3 = new TCanvas("xangles3", "PMT ADC vs theta_x", 10, 10, 1000, 1600);
  xangles3->Divide(2,4);
  xangles3->cd(1);
  xangle_pos->SetLineColor(2);
  xangle_neg->SetLineColor(4);
  xangle_pos->DrawNormalized("",1.0);
  xangle_pos->GetXaxis()->SetTitle("theta_x  (rad)");
  xangle_pos->GetYaxis()->SetTitle("ADC");
  xangles3->cd(2);
  xangle_neg->DrawNormalized("",1.0);
  xangle_neg->GetXaxis()->SetTitle("theta_x  (rad)");
  xangle_neg->GetYaxis()->SetTitle("ADC");

  xangles3->cd(3);
  xangle_pos_1->SetLineColor(2);
  xangle_neg_1->SetLineColor(4);
  xangle_pos_1->DrawNormalized("",1.0);
  xangle_pos_1->GetXaxis()->SetTitle(runPrefix+"   theta_x  (rad)");
  xangle_pos_1->GetYaxis()->SetTitle("ADC");
  xangle_neg_1->DrawNormalized("same",1.0);
  xangle_neg_1->GetXaxis()->SetTitle(runPrefix+"    theta_x  (rad)");
  xangle_neg_1->GetYaxis()->SetTitle("ADC");

  xangles3->cd(4);
  xangle_pos_2->SetLineColor(2);
  xangle_neg_2->SetLineColor(4);
  xangle_pos_2->DrawNormalized("",1.0);
  xangle_pos_2->GetXaxis()->SetTitle(runPrefix+"    theta_x  (rad)");
  xangle_pos_2->GetYaxis()->SetTitle("ADC");
  xangle_neg_2->DrawNormalized("same",1.0);
  xangle_neg_2->GetXaxis()->SetTitle(runPrefix+"    theta_x  (rad)");
  xangle_neg_2->GetYaxis()->SetTitle("ADC");

  xangles3->cd(5);
  xangle_pos_3->SetLineColor(2);
  xangle_neg_3->SetLineColor(4);
  xangle_pos_3->DrawNormalized("",1.0);
  xangle_pos_3->GetXaxis()->SetTitle(runPrefix+"    theta_x  (rad)");
  xangle_pos_3->GetYaxis()->SetTitle("ADC");
  xangle_neg_3->DrawNormalized("same",1.0);
  xangle_neg_3->GetXaxis()->SetTitle(runPrefix+"    theta_x  (rad)");
  xangle_neg_3->GetYaxis()->SetTitle("ADC");

  xangles3->cd(6);
  xangle_pos_4->SetLineColor(2);
  xangle_neg_4->SetLineColor(4);
  xangle_pos_4->DrawNormalized("",1.0);
  xangle_pos_4->GetXaxis()->SetTitle(runPrefix+"    theta_x  (rad)");
  xangle_pos_4->GetYaxis()->SetTitle("ADC");
  xangle_neg_4->DrawNormalized("same",1.0);
  xangle_neg_4->GetXaxis()->SetTitle(runPrefix+"    theta_x  (rad)");
  xangle_neg_4->GetYaxis()->SetTitle("ADC");

  xangles3->cd(7);
  xangle_pos_5->SetLineColor(2);
  xangle_neg_5->SetLineColor(4);
  xangle_pos_5->DrawNormalized("",1.0);
  xangle_pos_5->GetXaxis()->SetTitle(runPrefix+"    theta_x  (rad)");
  xangle_pos_5->GetYaxis()->SetTitle("ADC");
  xangle_neg_5->DrawNormalized("same",1.0);
  xangle_neg_5->GetXaxis()->SetTitle(runPrefix+"    theta_x  (rad)");
  xangle_neg_5->GetYaxis()->SetTitle("ADC");

  xangles3->cd(8);
  xangle_pos_6->SetLineColor(2);
  xangle_neg_6->SetLineColor(4);
  xangle_pos_6->DrawNormalized("",1.0);
  xangle_pos_6->GetXaxis()->SetTitle(runPrefix+"    theta_x  (rad)");
  xangle_pos_6->GetYaxis()->SetTitle("ADC");
  xangle_neg_6->DrawNormalized("same",1.0);
  xangle_neg_6->GetXaxis()->SetTitle(runPrefix+"    theta_x  (rad)");
  xangle_neg_6->GetYaxis()->SetTitle("ADC");

  xangles3->SaveAs(outputPrefix+"xangles3.pdf");
  xangles3->SaveAs(outputPrefix+"xangles3.C");
  xangles3->SaveAs(outputPrefix+"xangles3.png");
 
  xdists = new TCanvas("xdists", "PMT ADC vs x.", 10, 10, 1000, 1600);
  xdists->Divide(2,7);
  xdists->cd(1);
  //  xdist_pos->DrawNormalized("",1.0);
  xdist_pos->Draw();
  xdist_pos->GetXaxis()->SetTitle(runPrefix+"    x(cm)");
  xdist_pos->GetYaxis()->SetTitle("ADC");
  xdists->cd(2);
  xdist_neg->Draw();
  xdist_neg->GetXaxis()->SetTitle(runPrefix+"    x(cm)");
  xdist_neg->GetYaxis()->SetTitle("ADC");
  xdists->cd(3);
  xdist_pos_1->Draw();
  xdist_pos_1->GetXaxis()->SetTitle(runPrefix+"    x(cm)");
  xdist_pos_1->GetYaxis()->SetTitle("ADC");
  xdists->cd(4);
  xdist_neg_1->Draw();
  xdist_neg_1->GetXaxis()->SetTitle(runPrefix+"    x(cm)");
  xdist_neg_1->GetYaxis()->SetTitle("ADC");
  xdists->cd(5);
  xdist_pos_2->Draw();
  xdist_pos_2->GetXaxis()->SetTitle(runPrefix+"    x(cm)");
  xdist_pos_2->GetYaxis()->SetTitle("ADC");
  xdists->cd(6);
  xdist_neg_2->Draw();
  xdist_neg_2->GetXaxis()->SetTitle(runPrefix+"    x(cm)");
  xdist_neg_2->GetYaxis()->SetTitle("ADC");
  xdists->cd(7);
  xdist_pos_3->Draw();
  xdist_pos_3->GetXaxis()->SetTitle(runPrefix+"    x(cm)");
  xdist_pos_3->GetYaxis()->SetTitle("ADC");
  xdists->cd(8);
  xdist_neg_3->Draw();
  xdist_neg_3->GetXaxis()->SetTitle(runPrefix+"    x(cm)");
  xdist_neg_3->GetYaxis()->SetTitle("ADC");
  xdists->cd(9);
  xdist_pos_4->Draw();
  xdist_pos_4->GetXaxis()->SetTitle(runPrefix+"    x(cm)");
  xdist_pos_4->GetYaxis()->SetTitle("ADC");
  xdists->cd(10);
  xdist_neg_4->Draw();
  xdist_neg_4->GetXaxis()->SetTitle(runPrefix+"    x(cm)");
  xdist_neg_4->GetYaxis()->SetTitle("ADC");
  xdists->cd(11);
  xdist_pos_5->Draw();
  xdist_pos_5->GetXaxis()->SetTitle(runPrefix+"    x(cm)");
  xdist_pos_5->GetYaxis()->SetTitle("ADC");
  xdists->cd(12);
  xdist_neg_5->Draw();
  xdist_neg_5->GetXaxis()->SetTitle(runPrefix+"    x(cm)");
  xdist_neg_5->GetYaxis()->SetTitle("ADC");
  xdists->cd(13);
  xdist_pos_6->Draw();
  xdist_pos_6->GetXaxis()->SetTitle(runPrefix+"    x(cm)");
  xdist_pos_6->GetYaxis()->SetTitle("ADC");
  xdists->cd(14);
  xdist_neg_6->Draw();
  xdist_neg_6->GetXaxis()->SetTitle(runPrefix+"    x(cm)");
  xdist_neg_6->GetYaxis()->SetTitle("ADC");

  xdists->SaveAs(outputPrefix+"xdists.pdf");
  xdists->SaveAs(outputPrefix+"xdists.C");
  xdists->SaveAs(outputPrefix+"xdists.png");
 
 
  xdists2 = new TCanvas("xdists2", "PMT ADC vs x. normalized", 10, 10, 1000, 1600);

  xdists2->Divide(2,2);

  xdists2->cd(1);
  xdist_pos->SetLineColor(2);
  xdist_neg->SetLineColor(4);
  xdist_pos->SetTitle("Positive (red) and Negative (blue) PMT");
  xdist_pos->DrawNormalized("",1.0);
  xdist_neg->DrawNormalized("same",1.0);
 
  xdists2->cd(3);
  xdist_pos_1->SetLineColor(5);
  xdist_pos_2->SetLineColor(6);
  xdist_pos_3->SetLineColor(7);
  xdist_pos_4->SetLineColor(8);
  xdist_pos_5->SetLineColor(9);
  xdist_pos_6->SetLineColor(11);
  xdist_pos_1->SetTitle("Positive PMT, various y slices");
  xdist_pos_1->DrawNormalized("",1.0);
  xdist_pos_2->DrawNormalized("same",1.0);
  xdist_pos_3->DrawNormalized("same",1.0);
  xdist_pos_4->DrawNormalized("same",1.0);
  xdist_pos_5->DrawNormalized("same",1.0);
  xdist_pos_6->DrawNormalized("same",1.0);
  xdists2->cd(4);
  xdist_neg_1->SetLineColor(5);
  xdist_neg_2->SetLineColor(6);
  xdist_neg_3->SetLineColor(7);
  xdist_neg_4->SetLineColor(8);
  xdist_neg_5->SetLineColor(9);
  xdist_neg_6->SetLineColor(11);
  xdist_neg_1->SetTitle("Negative PMT, various y slices");
  xdist_neg_1->DrawNormalized("",1.0);
  xdist_neg_2->DrawNormalized("same",1.0);
  xdist_neg_3->DrawNormalized("same",1.0);
  xdist_neg_4->DrawNormalized("same",1.0);
  xdist_neg_5->DrawNormalized("same",1.0);
  xdist_neg_6->DrawNormalized("same",1.0);

  xdists2->cd(2);
  xdist_neg_1->SetLineColor(45);
  xdist_neg_2->SetLineColor(46);
  xdist_neg_3->SetLineColor(29);
  xdist_neg_4->SetLineColor(30);
  xdist_neg_5->SetLineColor(38);
  xdist_neg_6->SetLineColor(41);
  xdist_neg_1->SetTitle("Positive and Negative PMT, various y slices");
  xdist_neg_1->DrawNormalized("",1.0);
  xdist_neg_2->DrawNormalized("same",1.0);
  xdist_neg_3->DrawNormalized("same",1.0);
  xdist_neg_4->DrawNormalized("same",1.0);
  xdist_neg_5->DrawNormalized("same",1.0);
  xdist_neg_6->DrawNormalized("same",1.0);
  xdist_pos_1->DrawNormalized("same",1.0);
  xdist_pos_2->DrawNormalized("same",1.0);
  xdist_pos_3->DrawNormalized("same",1.0);
  xdist_pos_4->DrawNormalized("same",1.0);
  xdist_pos_5->DrawNormalized("same",1.0);
  xdist_pos_6->DrawNormalized("same",1.0);

  xdists2->SaveAs(outputPrefix+"xdists2.pdf");
  xdists2->SaveAs(outputPrefix+"xdists2.C");
  xdists2->SaveAs(outputPrefix+"xdists2.png");

  distributions = new TCanvas("distributions", "Total Pulse Height distributions", 10, 10, 800, 800);
  distributions->Divide(1,2);

  distributions->cd(1);
  xdistribution->Draw();
  //  adcpp1->GetXaxis()->SetTitle("Position on MD (cm)");
  xdistribution->GetXaxis()->SetTitle("Position on MD (cm)");

  distributions->cd(2);
  ydistribution->Draw();
  //adcpp1->GetXaxis()->SetTitle("Position on MD (cm)");
  xdistribution->GetXaxis()->SetTitle("Position on MD (cm)");

  distributions->SaveAs(outputPrefix+"total_pulse.png");
  distributions->SaveAs(outputPrefix+"total_pulse.pdf");
  distributions->SaveAs(outputPrefix+"total_pulse.C");


  TF1* fit1 = new TF1("FIT1", "pol1", -95, -5);
  TF1* fit2 = new TF1("FIT2", "pol1", 5, 95);
  TF1* fit3 = new TF1("FIT3", "pol1", -3, 3);
  TF1* fit4 = new TF1("FIT4", "pol1", -95, -5);
  TF1* fit5 = new TF1("FIT5", "pol1", 5, 95);

  profiles = new TCanvas("profiles", "ADC profiles", 10, 10, 800, 800);
  profiles->Divide(2,2);

  profiles->cd(1);
  adcpp1->Draw();
  gStyle->SetOptFit(1111);
  adcpp1->Fit(fit1, "R");
  adcpp1->GetXaxis()->SetTitle("Position on MD (cm)");

  profiles->cd(2);
  adcpp2->Draw();
  gStyle->SetOptFit(1111);
  adcpp2->Fit(fit2, "R");
  adcpp2->GetXaxis()->SetTitle("Position on MD (cm)");

  profiles->cd(3);
  adcmp1->Draw();
  gStyle->SetOptFit(1111);
  adcmp1->Fit(fit1, "R");
  adcmp1->GetXaxis()->SetTitle("Position on MD (cm)");

  profiles->cd(4);
  adcmp2->Draw();
  gStyle->SetOptFit(1111);
  adcmp2->Fit(fit2, "R");
  adcmp2->GetXaxis()->SetTitle("Position on MD (cm)");

  profiles->SaveAs(outputPrefix+"profiles.pdf");
  profiles->SaveAs(outputPrefix+"profiles.C");



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

  /*
  ratios = fopen("ratios.txt","a");

  if (ratios == NULL)
    {
      cout << "ratios did not open" << endl;
    }

  fprintf(ratios, "%d \t %d \t %d \t %f \t %f \t %f \t %f\n", run_number, package, md_number, ratio1, ratio2, ratio3, ratio4);

  fclose(ratios);
  */



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

  fulls->SaveAs(outputPrefix+"FullProfiles.pdf");
  fulls->SaveAs(outputPrefix+"FullProfiles.C");


  EFF = new TCanvas("EFF", "Efficiency Plot", 10, 10, 800, 800);
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

  EFF->SaveAs(outputPrefix+"efficiency.pdf");


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

  Double_t nslopep1 = 0;
  Double_t nslopep2 = 0;
  Double_t nslopem1 = 0;
  Double_t nslopem2 = 0;
  Double_t q = 0;
  Double_t p = 0;
  Double_t qq = 0;
  Double_t pp = 0;
  Double_t dynamic_range_p = 0;
  Double_t dynamic_range_m = 0;
  Double_t corr_dynamic_range_p = 0;
  Double_t corr_dynamic_range_m = 0;

  if (pintercept1 > pintercept2)
    {
      nslopep2 = pslope2 / pintercept2;
      q = -100*pslope1 + pintercept1;
      qq = -100*pslope2 + pintercept2;
      nslopep1 = pslope1 / q;
      dynamic_range_p = q/qq;
      corr_dynamic_range_p = q/(-200*pslope1+pintercept1);
      corr_dynamic_range_p = q/qq;
    }

  if (pintercept2 > pintercept1)
    {
      nslopep1 = pslope1 / pintercept1;
      q = 100*pslope2 + pintercept2;
      qq = -100*pslope1 + pintercept1;
      nslopep2 = pslope2 / q;
      dynamic_range_p = q/qq;
      corr_dynamic_range_p = q/(-200*pslope2+pintercept2);
      cout << "corr p " << corr_dynamic_range_p << endl;
    }

  if (mintercept1 > mintercept2)
    {
      nslopem2 = mslope2 / mintercept2;
      p = -100*mslope1 + mintercept1;
      pp = 100*mslope2 + mintercept2;
      nslopem1 = mslope1 / p;
      dynamic_range_m = p/pp;
      corr_dynamic_range_m = p/(-200*mslope1+mintercept1);
      cout << "corr m " << corr_dynamic_range_m << endl;
    }

  if (mintercept2 > mintercept1)
    {
      nslopem1 = mslope1 / mintercept1;
      p = 100*mslope2 + mintercept2;
      pp = -100*mslope1 + mintercept1;
      nslopem2 = mslope2 / p;
      dynamic_range_m = p/pp;
      corr_dynamic_range_m = p/(200*mslope2+mintercept2);
    }


  FILE *pedestal_mean;
  FILE *slope_intercept;
  FILE *nslope_gja;
  FILE *dynamic_range;

  pedestal_mean = fopen("pedestal_mean.txt", "a");
  slope_intercept = fopen("slope_intercept.txt", "a");
  nslope_gja = fopen("nslope_gja.txt", "a");
  dynamic_range = fopen("dynamic_range.txt", "a");

  if (pedestal_mean == NULL)
    {
      cout << "pedestal_mean did not open" << endl;
    }

  if (slope_intercept == NULL)
    {
      cout << "slope_intercept did not open" << endl;
    }

  if (nslope_gja == NULL)
    {
      cout << "nslope_gja did not open" << endl;
    }

  if (dynamic_range == NULL)
    {
      cout << "dynamic_range did not open" << endl;
    }

   fprintf(pedestal_mean, "%d \t %d \t %d \t %f \t %f \t %f \t %d\n", run_number, package, md_number, adcpmean, adcmmean, effmean, trial_total);


  fprintf(slope_intercept, "%d \t %d \t %d \t %f \t %f \t %f \t %f \t %f \t %f \t %f \t %f\n", run_number, package, md_number, pintercept1, pslope1, pintercept2, pslope2, mintercept1, mslope1, mintercept2, mslope2);

  fprintf(nslope_gja, "%d \t %d \t %d \t %f \t %f \t %f \t %f \t %f \t %f \t %f \t %f \t %f \t %f \t %d \n",run_number, package, md_number, gjap, gjam, nslopep1, nslopep2, nslopem1, nslopem2, q, p, dynamic_range_p, dynamic_range_m, trial_total);

  fprintf(dynamic_range, "%d \t %d \t %d \t %f \t %f \t %f \t %f \t %f \t %f \t %f \t %f \t %d \n",run_number, package, md_number, gjap, gjam, q, p, dynamic_range_p, dynamic_range_m, corr_dynamic_range_p, corr_dynamic_range_m, trial_total);



  fclose(pedestal_mean);
  fclose(slope_intercept);
  fclose(nslope_gja);
  fclose(dynamic_range);


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
    
    TH1F* h_resid=new TH1F("h_resid","Average VDC residual",200,0,1000);

    TH1F* h_resid1=new TH1F("h_resid1","Average VDC residual plane 1",200,0,1000);
    TH1F* h_resid2=new TH1F("h_resid2","Average VDC residual plane 2",200,0,1000);
    TH1F* h_resid3=new TH1F("h_resid3","Average VDC residual plane 3",200,0,1000);
    TH1F* h_resid4=new TH1F("h_resid4","Average VDC residual plane 4",200,0,1000);

    int nevents=event_tree->GetEntries();
    if (max_events == -1)max_events = nevents; 


    for (int i=0;i<max_events;i++)
    {
        if(i%1000==0) cout << "events processed so far: " << i << endl;
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
        double residual1=0;
        double residual2=0;
        double residual3=0;
        double residual4=0;
	int r3_tl=0;
        if (valid)
        {
	    int ntl=fEvent->GetNumberOfTreeLines();
            for (int j=0;j<ntl;j++)
            {
                tl=fEvent->GetTreeLine(j);

		//                if (tl->GetRegion()==3 && tl->GetPackage()==package)
		//		  cout << "plane   " <<  tl->GetPlane() << endl;

                if (tl->GetRegion()==3 && tl->GetPackage()==package)
		  {
		  if (tl->GetPlane()==0)
		    {
		      residual+=tl->GetAverageResidual();
		      r3_tl++;
		    }    
		  if (tl->GetPlane()==1) residual1+=tl->GetAverageResidual();
		  if (tl->GetPlane()==2) residual2+=tl->GetAverageResidual();
		  if (tl->GetPlane()==3) residual3+=tl->GetAverageResidual();
		  if (tl->GetPlane()==4) residual4+=tl->GetAverageResidual();
		  }
	    }
	}

	if (valid) residual = residual/r3_tl;   // average over the expected 2 treelines in the partial track

        if(valid==false) h_f->Fill(mdp->GetValue()+mdm->GetValue());
	if(valid){
	  h_2f->Fill((mdp->GetValue()+mdm->GetValue()),residual);
	  h_fg->Fill(mdp->GetValue()+mdm->GetValue());
	  h_resid->Fill(10000.*residual);

	  h_resid1->Fill(10000.*residual1);
	  h_resid2->Fill(10000.*residual2);
	  h_resid3->Fill(10000.*residual3);
	  h_resid4->Fill(10000.*residual4);
	}
    }


        TCanvas* resid=new TCanvas("resid","R3 partial track residuals and MD pulseheights",700,700);
        resid->Divide(1,3);
        resid->cd(1);
	resid_1->SetLogy();
        h_f->Draw();
	h_f->SetTitle("MD pulse height for events with no Region 3 partial track");
        resid->cd(2);
	resid_2->SetLogy();
        h_fg->Draw();
	h_fg->SetTitle("MD pulse height for events with Region 3 partial track");
        resid->cd(3);
	h_2f->SetTitle("Region 3 partial track residual vs. MD pulseheight sum");
        h_2f->Draw("colz");

	resid->SaveAs(outputPrefix+"VDC_residuals_and_pulses.pdf");

        TCanvas* c2=new TCanvas("c2","R3 partial track Residual",600,600);

	h_resid->SetXTitle("Average Residual  (#mum)");
	h_resid->SetYTitle("Events");
	h_resid->SetLabelFont(62,"x");
	h_resid->SetLabelFont(62,"y");
	h_resid->SetTitleFont(62,"x");
	h_resid->SetTitleFont(62,"y");
	h_resid->SetTitleSize(.04,"x");
	h_resid->SetTitleSize(.04,"y");
	h_resid->GetXaxis()->CenterTitle();
	h_resid->GetYaxis()->CenterTitle();

	h_resid->Draw();

	c2->SaveAs(outputPrefix+"pt_residuals.pdf");

        TCanvas* c3=new TCanvas("c3","R3 Residuals by plane",600,600);
	c3->Divide(2,2);
	c3->cd(1);
	h_resid1->Draw();
	c3->cd(2);
	h_resid2->Draw();
	c3->cd(3);
	h_resid3->Draw();
	c3->cd(4);
	h_resid4->Draw();

	c3->SaveAs(outputPrefix+"residuals_by_plane.pdf");
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
        if(i%1000==0) cout << "events processed so far: " << i << endl;
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
