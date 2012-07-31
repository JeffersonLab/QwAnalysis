// Tracking_Cut.C
//---------------------------------------------------------------
// 
// Jie Pan, Thu Jul 19 22:00:12 EDT 2012, jpan@jlab.org
//
// This macro is used to look at the effects of different cuts to 
// Q2 and scattering angle. Several typical runs are listed below:
//
// run 18522 : oct1-5, tdc cut [-210,-150], mean_direction_phi=-0.0192
// run 18517 : oct2-6, md6p channel has no data
// run 8662  : oct2-6
// run 18482 : oct3-7, pass3 rootfile contains wrong octant numbers (oct1 & 5).
//                       actual octant numbers are 3 & 7.  
// run 15039 : oct3-7, tdc cut [-210,-150], mean_direction_phi=0.0087
// run 18577 : oct8-4
//
// function:
// void Tracking_Cut(int event_start=-1,
//                   int event_end=-1,
//                   int run=8658, 
//                   TString stem="Qweak_", 
//                   TString suffix="")
//
// example: Tracking_Cut(-1,10000,18522)
//
//---------------------------------------------------------------

#include <iostream>
#include <iomanip>

// set to true for reversed package run
bool reverse_run = false;

// cut enable flags
bool enable_multiplicity_cut     = true;
bool enable_tdc_cut              = true;
bool enable_adc_cut              = false;
bool enable_scattering_angle_cut = false;
bool enable_vertex_z_cut         = false;
bool enable_vertex_r_cut         = false;
bool enable_position_theta_cut   = false;
bool enable_position_phi_cut     = false;
bool enable_direction_theta_cut  = false;
bool enable_direction_phi_cut    = false;
bool enable_position_r_off_cut   = false;
bool enable_hit_position_x_cut   = false;
bool enable_hit_position_y_cut   = false;


// cut values
const int multiple=18;

int tdc_cut_min[2] = {-210,-210}; //tdc cuts for package {1, 2}
int tdc_cut_max[2] = {-150,-150};

double num_pe_cut = 0.5;

double scattering_angle_cut_min = 3.0;
double scattering_angle_cut_max = 13.0;

double vertex_z_cut_min = -675;
double vertex_z_cut_max = -625;
double vertex_r_cut_min = 0;
double vertex_r_cut_max = 1;

double bending_angle_position_theta_cut_min = -0.003; //-0.001
double bending_angle_position_theta_cut_max = 0.003; // 0.001
double bending_angle_position_phi_cut_min   = -1.0; 
double bending_angle_position_phi_cut_max   = 1.0;

double bending_angle_direction_theta_cut_min = -0.4; // -0.4;
double bending_angle_direction_theta_cut_max = 0.4; // 0.4; 
double bending_angle_direction_phi_cut_min   = -2.0; //-2.0;
double bending_angle_direction_phi_cut_max   = 2.0; //2.0;

double position_r_off_cut_min = -1.0;
double position_r_off_cut_max = 1.0;

double hit_position_x_cut_min = (344-9.0) -10.0; // add in 10 cm margin
double hit_position_x_cut_max = (344+9.0) +10.0;
double hit_position_y_cut_min = -120;
double hit_position_y_cut_max = 120;

// parameters

// main detector pedestal values
int MDm_Pedestal[]={0,230,190,250,225,272,192,270,193};
int MDp_Pedestal[]={0,195,190,240,176,197,211,245,250};

// main detector single PE calibration
double MDm_SinglePE[]={0,12.4,11.2,12.9,21.0,12.0,13.3,14.7,11.9}; //(Calibration: March 14, 2011)
double MDp_SinglePE[]={0,17.0,15.8,13.8,18.5,17.9,18.6,16.4,11.4};

// main detector z location
double md_zpos[9] = {0.0, 581.665,  576.705, 577.020, 577.425, 582.515,  577.955, 577.885, 577.060};

// set debug flag to false to disable printing debug info
bool debug = false;

// set verbose level: 0 (no verbose), 1, 2, 3, ...
int verbose = 0;

//--------------------------------------------------------------------

bool tdc_cut(double val1, double val2, int pkg)
{
  if(val1 < tdc_cut_min[pkg-1] || val1 > tdc_cut_max[pkg-1])
     {
        if(debug)
           cout<<"rejected, pkg"<<pkg<<"_tdc_cut, mdm_tdc=" <<val1<<endl;
        return false;
     }
  if(val2 < tdc_cut_min[pkg-1] || val2> tdc_cut_max[pkg-1]) 
     {
        if(debug)
           cout<<"rejected, pkg"<<pkg<<"_tdc_cut, mdp_tdc=" <<val2<<endl;
        return false;
     }
  return true;
}

// light_yield_cut: based on ADC values, return true if one side of main detector PMTs
// has #PE value larger than threshold. 
bool light_yield_cut(double mdm_adc_value, double mdp_adc_value, int md_num)
{
  double mdm_num_pe = (mdm_adc_value-MDm_Pedestal[md_num])/MDm_SinglePE[md_num];
  double mdp_num_pe = (mdp_adc_value-MDp_Pedestal[md_num])/MDp_SinglePE[md_num];
  if(mdm_num_pe >num_pe_cut)
     return true;
  else if (mdp_num_pe >num_pe_cut)
     return true;
  else
    {
      if(debug)
         cout<<"rejected, light_yield_cut, mdm #PE=" <<mdm_num_pe<<", mdp #PE="<<mdp_num_pe<<endl;
      return false;
    }

}

bool scattering_angle_cut(double val)
{
  if(val> scattering_angle_cut_min && val< scattering_angle_cut_max)
     return true;
  else
  {
     if(debug)
        cout<<"rejected, scattering_angle_cut, theta_angle="<<val<<endl;
     return false;
  }
}

bool  vertex_z_cut(double val)
{
  if(val> vertex_z_cut_min && val< vertex_z_cut_max)
    return true;
  else
  {
    if(debug)
       cout<<"rejected, vertex_z_cut, vertex_z="<<val<<endl;
    return false;
  }
}

bool  vertex_r_cut(double val)
{
  if(val> vertex_r_cut_min && val< vertex_r_cut_max)
    return true;
  else
  {
    if(debug)
       cout<<"rejected, vertex_r_cut, vertex_r="<<val<<endl;
    return false;
  }
}

bool  bending_angle_position_theta_cut(double val)
{
  if(val> bending_angle_position_theta_cut_min && val< bending_angle_position_theta_cut_max)
    return true;
  else
  {
    if(debug)
       cout<<"rejected, bending_angle_position_theta_cut, position_theta_off="<<val<<endl;
    return false;
  }
}

bool  bending_angle_position_phi_cut(double val)
{
  if(val> bending_angle_position_phi_cut_min && val< bending_angle_position_phi_cut_max)
    return true;
  else
  {
    if(debug)
       cout<<"rejected, bending_angle_position_phi_cut, position_phi_off="<<val<<endl;
    return false;
  }
}

bool  bending_angle_direction_theta_cut(double val)
{
  if(val> bending_angle_direction_theta_cut_min && val< bending_angle_direction_theta_cut_max)
    return true;
  else
  {
    if(debug)
       cout<<"rejected, bending_angle_direction_theta_cut, direction_theta_off="<<val<<endl;
    return false;
  }
}

bool  bending_angle_direction_phi_cut(double val)
{
  if(val> bending_angle_direction_phi_cut_min && val< bending_angle_direction_phi_cut_max)
    return true;
  else
  {
    if(debug)
       cout<<"rejected, bending_angle_direction_phi_cut, direction_phi_off="<<val<<endl;
    return false;
  }
}

bool  position_resolution_cut(double val)
{
  if(val> position_r_off_cut_min && val< position_r_off_cut_max)
    return true;
  else
  {
    if(debug)
       cout<<"rejected, position_resolution_cut, position_r_off="<<val<<endl;
    return false;
  }
}

bool  hit_position_x_cut(double val)
{
  if(fabs(val)> hit_position_x_cut_min && fabs(val)< hit_position_x_cut_max)
    return true;
  else
  {
    if(debug)
       cout<<"rejected, hit_position_x_cut, x="<<val<<endl;
    return false;
  }
}

bool  hit_position_y_cut(double val)
{
  if(fabs(val)> hit_position_y_cut_min && fabs(val)< hit_position_y_cut_max)
    return true;
  else
  {
    if(debug)
       cout<<"rejected, hit_position_y_cut, y="<<val<<endl;
    return false;
  }
}

//---------------------------------------------------------------------

void Tracking_Cut(int event_start=-1,int event_end=-1,int run=8658, TString stem="Qweak_", TString suffix="")
{
   gROOT->SetStyle("Plain");
   TString file_name = "$QW_ROOTFILES/"+stem+Form("%d",run)+suffix+".root";
   cout <<  file_name << endl;
   
   TFile* file=new TFile(file_name);

   QwEvent* fEvent=0;
   QwTrack* track=0;
   QwPartialTrack* pt=0;
   TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );

   TCanvas* c=new TCanvas("c","Q2 and scattering angle distributions",800,800);

   //Get the oct number
   int oct=getOctNumber(event_tree);
   
   Int_t nevents=event_tree->GetEntries();
   cout << "total events: " << nevents << endl;
   int start=(event_start==-1)? 0:event_start;
   int end=(event_end==-1)? nevents:event_end;

    event_tree->SetBranchStatus("events",1);
    TBranch* event_branch=event_tree->GetBranch("events");
    TBranch* maindet_branch=event_tree->GetBranch("maindet");
    TBranch* trig_branch=event_tree->GetBranch("trigscint");
    event_branch->SetAddress(&fEvent);
    int md_1=(oct+4)%8;
    int md_2=oct;

    TLeaf* mdp_adc_1=maindet_branch->GetLeaf(Form("md%dp_adc",md_1));
    TLeaf* mdm_adc_1=maindet_branch->GetLeaf(Form("md%dm_adc",md_1));
    TLeaf* mdp_adc_2=maindet_branch->GetLeaf(Form("md%dp_adc",md_2));
    TLeaf* mdm_adc_2=maindet_branch->GetLeaf(Form("md%dm_adc",md_2));
    
    TLeaf* mdp_tdc_1=maindet_branch->GetLeaf(Form("md%dp_f1",md_1));
    TLeaf* mdm_tdc_1=maindet_branch->GetLeaf(Form("md%dm_f1",md_1));
    TLeaf* mdp_tdc_2=maindet_branch->GetLeaf(Form("md%dp_f1",md_2));
    TLeaf* mdm_tdc_2=maindet_branch->GetLeaf(Form("md%dm_f1",md_2));

    double mean_thetaoff_pkg1=1,sigma_thetaoff_pkg1=1;
    double mean_thetaoff_pkg2=1,sigma_thetaoff_pkg2=1;
    double mean_phioff_pkg1=1, sigma_phioff_pkg1=1;
    double mean_phioff_pkg2=1,sigma_phioff_pkg2=1;

    bool opt=false;

    if(opt){
          
      TH1F* pkg1_theta=new TH1F("a","a",500,-1,1);
      TH1F* pkg2_theta=new TH1F("b","b",500,-1,1);

      TH1F* pkg1_phi=new TH1F("c","c",500,-1,1);
      TH1F* pkg2_phi=new TH1F("d","d",500,-1,1);

      TF1* f1=new TF1("f1","gaus",-1,1);
      f1->SetParameters(1,-0.5,1);

      TF1* f2=new TF1("f2","gaus",-1,1);
      f2->SetParameters(1,-0.5,1);

      TF1* f3=new TF1("f3","gaus",-1,1);
      f1->SetParameters(1,-0.5,1);

      TF1* f4=new TF1("f4","gaus",-1,1);
      f1->SetParameters(1,-0.5,1);

      
      event_tree->Project("a","events.fQwTracks.fDirectionThetaoff","events.fQwTracks.fPackage==1");
      pkg1_theta->Fit("f1","QN0");

      event_tree->Project("b","events.fQwTracks.fDirectionThetaoff","events.fQwTracks.fPackage==2");
      pkg2_theta->Fit("f2","QN0");

      event_tree->Project("c","events.fQwTracks.fDirectionPhioff","events.fQwTracks.fPackage==1");
      pkg1_phi->Fit("f3","QN0");

      event_tree->Project("d","events.fQwTracks.fDirectionPhioff","events.fQwTracks.fPackage==2");
      pkg2_phi->Fit("f4","QN0");
      
      double mean_thetaoff_pkg1=f1->GetParameter(1);
      double sigma_thetaoff_pkg1=f1->GetParameter(2);

      double mean_thetaoff_pkg2=f2->GetParameter(1);
      double sigma_thetaoff_pkg2=f2->GetParameter(2);

      double mean_phioff_pkg1=f3->GetParameter(1);
      double sigma_phioff_pkg1=f3->GetParameter(2);

      double mean_phioff_pkg2=f4->GetParameter(1);
      double sigma_phioff_pkg2=f4->GetParameter(2);
      
     }

     double width=3;
     double pkg1_phioff_lower=mean_phioff_pkg1-width*sigma_phioff_pkg1;
     double pkg1_phioff_upper=mean_phioff_pkg1+width*sigma_phioff_pkg1;
     double pkg2_phioff_lower=mean_phioff_pkg2-width*sigma_phioff_pkg2;
     double pkg2_phioff_upper=mean_phioff_pkg2+width*sigma_phioff_pkg2;

     double pkg1_thetaoff_lower=mean_thetaoff_pkg1-width*sigma_thetaoff_pkg1;
     double pkg1_thetaoff_upper=mean_thetaoff_pkg1+width*sigma_thetaoff_pkg1;
     double pkg2_thetaoff_lower=mean_thetaoff_pkg2-width*sigma_thetaoff_pkg2;
     double pkg2_thetaoff_upper=mean_thetaoff_pkg2+width*sigma_thetaoff_pkg2;

    // histograms
    TH2F* hit_dist1 = new TH2F("hit_dist1",Form("Run %d, Hits Distribution in Oct %d",run,md_1),480,-120,120,100,310,360);
    TH2F* hit_dist2 = new TH2F("hit_dist2",Form("Run %d, Hits Distribution in Oct %d",run,md_2),480,-120,120,100,310,360);
    TH1F* angle = new TH1F("theta",Form("Run %d, Scattering Angle Distribution, Oct %d + %d",run,md_1,md_2),400,3,14);
    TH1F* q2 = new TH1F("q2",Form("Run %d, Q2 Distribution, Oct %d + %d",run,md_1,md_2),400,0,0.08);
    TH1F* angle_1 = new TH1F("theta_1",Form("Run %d, Scattering Angle in Package 1, Oct %d",run,md_1),400,3,14);
    TH1F* angle_2 = new TH1F("theta_2",Form("Run %d, Scattering Angle in Package 2, Oct %d",run,md_2),400,3,14);
    TH1F* q2_1 = new TH1F("q2_1",Form("Run %d, Q2 Distribution in Package 1, Oct %d",run,md_1),400,0,0.08);
    TH1F* q2_2 = new TH1F("q2_2",Form("Run %d, Q2 Distribution in Package 2, Oct %d",run,md_2),400,0,0.08);
    
    for(int i=start;i<end;++i)  // start to loop over all events
    {

      if(i%100000==0)
	cout << "events processed so far: " << i << endl;
      
      event_branch->GetEntry(i);
      maindet_branch->GetEntry(i);
      trig_branch->GetEntry(i);
     
      double Q2_val = fEvent->fKinElasticWithLoss.fQ2;
      if(Q2_val<=0)
         continue;
  
      double mdp_adc_value_1=mdp_adc_1->GetValue();
      double mdm_adc_value_1=mdm_adc_1->GetValue();
      double mdp_adc_value_2=mdp_adc_2->GetValue();
      double mdm_adc_value_2=mdm_adc_2->GetValue();

      double mdp_tdc_value_1=mdp_tdc_1->GetValue();
      double mdm_tdc_value_1=mdm_tdc_1->GetValue();
      double mdp_tdc_value_2=mdp_tdc_2->GetValue();
      double mdm_tdc_value_2=mdm_tdc_2->GetValue();
      
      double ntracks=fEvent->GetNumberOfTracks();
      // those filters are related with the quality of matching of r2 and r3 tracks

      int nhits=fEvent->GetNumberOfHits();
      int valid_hits_1=0,valid_hits_2=0;
      for(int j=0;j<nhits;++j)
        {
	  hit=fEvent->GetHit(j);
	  if(hit->GetRegion() ==2 && hit->GetDriftDistance() >=0 && hit->GetHitNumber()==0)
          {
	    if(hit->GetPackage()==1)
	        ++valid_hits_1;
	    else
	        ++valid_hits_2;
	  }
	}

      //----------------------
      // scattering angle cut
      //----------------------
      double angle_val=fEvent->GetScatteringAngle();
      if(enable_scattering_angle_cut)
        {
           if(! scattering_angle_cut(angle_val))
               continue;
        }

      //----------------------
      // scattering vertex cut
      //----------------------

      double vertex_z = fEvent->fScatteringVertexZ;
      if(enable_vertex_z_cut)
        {
           if(! vertex_z_cut(vertex_z))
               continue;
        }

      double vertex_r = fEvent->fScatteringVertexR;
      if(enable_vertex_r_cut)
        {
           if(! vertex_r_cut(vertex_r))
               continue;
        }


      //----------------------
      // hit position cut
      //----------------------

      double dz = 577.5;
      double xoffset,yoffset,xslope,yslope;
      double x=0, y=0;
      bool hit_position_cut = true;
      for (int num_p=0; num_p< fEvent->GetNumberOfPartialTracks();num_p++)
        {
          pt=fEvent->GetPartialTrack(num_p);
          if (pt->GetRegion()==3) 
            {
              int pkg = pt->GetPackage();
              if(pkg==1)
                   dz = md_zpos[(oct+4)%8];
              else if(pkg==2)
                   dz = md_zpos[oct];

               xoffset = pt->fOffsetX;
               yoffset = pt->fOffsetY;
               xslope  = pt->fSlopeX;
               yslope  = pt->fSlopeY;
               x       = xoffset+xslope*dz;
               y       = yoffset+yslope*dz;

              if(debug) 
                 cout<<"event#"<<i<<", pkg"<<pkg<<", pt#"<<num_p<<": hit at ("<<x<<","<<y<<")"<<endl;

              global2local(&x,&y,oct,pkg);

              if(enable_hit_position_x_cut)
              {
                 if(! hit_position_x_cut(x))
                 {
                   hit_position_cut = false;
                   continue;
                 }
              }

              if(enable_hit_position_y_cut)
              {
                 if(! hit_position_y_cut(y))
                 {
                   hit_position_cut = false;
                   continue;
                 }
              }

            }
         }

      if(! hit_position_cut)
         continue;


      for(int j=0;j<ntracks;++j)
      {
	track=fEvent->GetTrack(j);
        int package = track->GetPackage();

      //----------------------
      // bending angle cut
      //----------------------
        
      double direction_theta_off = track->fDirectionThetaoff;
      double direction_phi_off = track->fDirectionPhioff;
 
       // at least, applying a 3 sigma boundary cut 
       if(package==1)
       {
          if( direction_phi_off<pkg1_phioff_lower || direction_phi_off>pkg1_phioff_upper ||
              direction_theta_off<pkg1_thetaoff_lower || direction_theta_off>pkg1_thetaoff_upper )
               continue;
       }
       else if (package==2)
       {
          if( direction_phi_off<pkg2_phioff_lower || direction_phi_off>pkg2_phioff_upper ||
              direction_theta_off<pkg2_thetaoff_lower || direction_theta_off>pkg2_thetaoff_upper )
               continue;
       }

       // further mathching angle cuts

        if(enable_direction_theta_cut)
        {
           if(! bending_angle_direction_theta_cut(direction_theta_off))
                continue;
        }

        if(enable_direction_phi_cut)
        {
           if(! bending_angle_direction_phi_cut(direction_phi_off))
                continue;
        }

        if(enable_position_theta_cut)
        {
           if(! bending_angle_position_theta_cut(track->fPositionThetaoff))
                continue;
        }

        if(enable_position_phi_cut)
        {
           if(! bending_angle_position_phi_cut(track->fPositionPhioff))
                continue;
        }


      //---------------------------------
      // matching position resolution cut
      //---------------------------------

        if(enable_position_r_off_cut)
        {
           if(! position_resolution_cut(track->fPositionRoff))
                continue;
        }


      //----------------------
      // TDC cut
      //----------------------

        if(package==1)
        {
           if(enable_tdc_cut)
           {
              if(! tdc_cut(mdm_tdc_value_1, mdp_tdc_value_1,package))
                 continue;
           }
        }

        if(package==2)
        {
           if(enable_tdc_cut)
           {
              if(! tdc_cut(mdm_tdc_value_2, mdp_tdc_value_2,package))
                 continue;
           }
        } 

      //----------------------
      // ADC (light yield) cut
      //----------------------

        if(package==1)
        {
           if(enable_adc_cut)
           {
              if(! light_yield_cut(mdm_adc_value_1,mdp_adc_value_1,md_1))
                 continue;
           } 
        } 

        if(package==2)
        {
           if(enable_adc_cut)
           {
              if(! light_yield_cut(mdm_adc_value_2,mdp_adc_value_2,md_2))
                 continue;
           }
        }

      //----------------------
      // multiplicity cut
      //----------------------

        if(package==1)
        {
           if(enable_multiplicity_cut)
           {
              if(valid_hits_1 > multiple )
                 continue;
           } 
        } 

        if(package==2)
        {
           if(enable_multiplicity_cut)
           {
              if(valid_hits_2 > multiple )
                 continue;
           }
        }

      //----------------------
      // fill histograms
      //----------------------

	if(package==1 ) 
          {
                angle_1->Fill(angle_val);
                q2_1->Fill(Q2_val);
                angle->Fill(angle_val);
                q2->Fill(Q2_val);
                hit_dist1->Fill(y,x);
	   }
         else if(package==2 ) 
           {
                angle_2->Fill(angle_val);
                q2_2->Fill(Q2_val);
                angle->Fill(angle_val);
                q2->Fill(Q2_val);
                hit_dist2->Fill(y,x);
	   }

      } // end of loop over tracks
    }  // end of loop over all events
   
    cout << "\n**********************************"<<endl;
    cout <<"Run#: "<<run<<endl;
    cout << "number of tracks:" << endl;
    cout << "pkg1: " << angle_1->GetEntries() << endl;
    cout << "pkg2: " << angle_2->GetEntries() << endl;

    cout << "\nscattering angle: " << endl;
    if(angle->GetEntries()) {
      cout << "all : " << setprecision(5) << angle->GetMean() << " +/- " << setprecision(3) << angle->GetRMS()/sqrt(angle->GetEntries()) << " deg"<<endl; 
    }
    if(angle_1->GetEntries()!=0) {
      cout << "pkg1: " << setprecision(5) << angle_1->GetMean() << " +/- " << setprecision(3) << angle_1->GetRMS()/sqrt(angle_1->GetEntries()) << " deg"<< endl;
    }
    if(angle_2->GetEntries()!=0) {
      cout << "pkg2: " <<  setprecision(5) << angle_2->GetMean() << " +/- " <<  setprecision(3) << angle_2->GetRMS()/sqrt(angle_2->GetEntries()) << " deg"<<endl;
    }

    cout << "\nQ2: " << endl;
    if(q2->GetEntries()) {
      cout << "all : " <<  setprecision(5) << q2->GetMean() << " +/- " <<  setprecision(3) << q2->GetRMS()/sqrt(q2->GetEntries()) << " (GeV/c)^2"<<endl;
    }
    if(q2_1->GetEntries()!=0) {
      cout << "pkg1: " <<  setprecision(5) << q2_1->GetMean() << " +/- " <<  setprecision(3) << q2_1->GetRMS()/sqrt(q2_1->GetEntries()) << " (GeV/c)^2"<<endl;
    }
    if(q2_2->GetEntries()!=0) {
      cout << "pkg2: " <<  setprecision(5) << q2_2->GetMean() << " +/- " <<  setprecision(3) << q2_2->GetRMS()/sqrt(q2_2->GetEntries()) << " (GeV/c)^2"<<endl<<endl;
    }
 
    // Draw histograms
    gStyle->SetPalette(1);
    c->Clear();   
    c->Divide(2,4);
    
    c->cd(1);
    hit_dist1->Draw("colz");
    hit_dist1->GetXaxis()->SetTitle("Y [cm]");
    hit_dist1->GetYaxis()->SetTitle("X [cm]");

    c->cd(2);
    hit_dist2->Draw("colz");
    hit_dist2->GetXaxis()->SetTitle("Y [cm]");
    hit_dist2->GetYaxis()->SetTitle("X [cm]");

    c->cd(3);
    angle_1->Draw();
    angle_1->GetXaxis()->SetTitle("Scattering Angle [degree]");

    c->cd(4);
    q2_1->Draw();
    q2_1->GetXaxis()->SetTitle("Q2 [(GeV/c)^2]");
    
     c->cd(5);
    angle_2->Draw();
    angle_2->GetXaxis()->SetTitle("Scattering Angle [degree]");

    c->cd(6);
    q2_2->Draw();
    q2_2->GetXaxis()->SetTitle("Q2 [(GeV/c)^2]");
    
    c->cd(7);
    angle->Draw();
    angle->GetXaxis()->SetTitle("Scattering Angle [degree]");

    c->cd(8);
    q2->Draw();
    q2->GetXaxis()->SetTitle("Q2 [(GeV/c)^2]");

    return;

 }

//----------------------------------------------------------------

int getOctNumber(TTree* event_tree){

  event_tree->Draw("events.fQwPartialTracks.fOctant>>R2_Oct",
                   "events.fQwPartialTracks.fRegion==2 && events.fQwPartialTracks.fPackage==2");

  int r2_oct = int (R2_Oct->GetMean()); // location of pkg2
  return (r2_oct);
}

void global2local(double* x, double* y, int oct, int pkg)
{
   if(reverse_run)
       oct = (oct+4)%8;
   if(pkg==1)
       oct = (oct+4)%8;
   if(debug)
       cout<<"-->rotated oct"<<oct<<" - ("<<*x<<","<<*y<<") to ";

   double theta = -45.0*3.14159/180.0*(oct-1);
   double xx = *x*cos(theta) - *y*sin(theta);
   double yy = *x*sin(theta) + *y*cos(theta);
   *x = xx;
   *y = yy;

   if(debug)
      cout<<"("<<*x<<","<<*y<<")"<<endl;
}

