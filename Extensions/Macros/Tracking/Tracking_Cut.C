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
//
// update log:
//
// jpan, Fri Aug 10 01:53:25 EDT 2012
// removed the default 3 sigma cut on direction_theta_off and direction_phi_off
//
// jpan, Fri Sep  7 11:40:04 CDT 2012
// added in a function global2local_phi() to convert global azimuthal angles (-pi, pi)
// to local azimuthal angles. Now, direction_phi_off and position_phi_off are not taken
// from rootfile directly. Instead, they are calculated from "Actual Phi - Goal Phi"
// based on local azimuthal angles. This is to avoid the split of hit distributions on
// main bar when applying cuts on direction_phi_off and position_phi_off

// jpan, Mon Sep 10 12:24:23 CDT 2012
// fixed bugs in global2local_phi()
// added more histograms to plot the distributions of additional variables, including
// vertex_z, vertex_r, P0, pp, Eloss_vs_vertex_z, direction_phi_off, position_phi_off

// jpan, Sun Sep 16 13:53:03 EDT 2012
// Create an output root file to save resulting histograms and plots.
// The output file is automatically saved and named with its run number and analysis
// sequence number. Run conditions, such as cut values, are displayed and saved in a
// canvas for later reteieving. Analysis summary is saved as well.

// jpan, Mon Sep 17 17:19:10 CDT 2012
// Extract main detector pedestals using a new function getQdcPedestal() to 
// replace hard-coded pedestal values. Plot light-weighted Q2 distributions and
// compared them with Q2 distributions.

// jpan, Wed Sep 19 09:38:16 CDT 2012
// Added in 2D distribution plots of light, Q2 and light-weighted Q2,
// added in the plots for direction_theta_off and position_theta_off,
// changed the unit from [rad] to [degree] for phi_off and theta_off,

// jpan, Mon Oct  1 12:08:07 CDT 2012
// added in #pe and tdc spectra, as well as #pe vs. tdc plots; added in vertex info to
// run summary table; specify the theta_off/phi_off cuts by its mean and rms, and 
// assign different cut values to the two packages; added in default 0.5 pe cut on 
// both PMTs; added in TDC threshold condition so that TDC cuts are applied only when
// #PE yields are larger than the threshold values of both PMTs.

// jpan, Mon Oct  1 16:18:47 CDT 2012
// added in Q2/#PE/light-weighted_Q2 vs X plots to show these distributions along the bar;
// put limits on number of events to speed up the extraction of main detector pedestals;
// fixed small bugs in text of run summary table and histogram title of #pe distributions; 

// jpan, Tue Oct  2 10:38:42 CDT 2012
// Made the recontructed momentum (the momentum determined by matching tracks through
// B-field) plots available in the output and added in a cut for this parameter.

// jpan, Wed Oct  3 10:22:48 CDT 2012
// Enabled auto-selection of single PE calibration for different periods of tracking runs
// added in function to find out the parameters of matching angle residuals automatically
// instead of finding and filling in these parameters manually.

// jpan, Mon Nov  5 10:07:04 CST 2012
// Added Q2 vs. reconstructed momentum correlation plots and radial distributions of Q2,
// light, and light-weighted Q2 as requested by D. Mack.

// jpan, Mon Feb 11 10:02:27 CST 2013
// Added in the track projection to collimator, to display the hits distribution in the 
// downstream surface of collimator 2. Also added in the hit distribution in downstream 
// target window and vertex (x,y,z) distribution in the target region.
//
// jpan, Fri May 24 11:12:22 CDT 2013
// Changed the the error calculation for scattering angle and Q2 to take into account
// the case of using cross-section weight
//
// jpan, Fri Aug 30 11:44:44 CDT 2013
// Added in partial-track-level chi cuts and track-level chi cuts
// 
#include <iostream>
#include <iomanip>

// set to true for reversed package run
bool reverse_run = false;

// cut enable flags
bool enable_multiplicity_cut     = true;
bool enable_tdc_cut              = true;
bool enable_adc_cut              = true;
bool enable_scattering_angle_cut = false;
bool enable_vertex_z_cut         = false;
bool enable_vertex_r_cut         = false;
bool enable_position_theta_cut   = false;
bool enable_position_phi_cut     = true;
bool enable_direction_theta_cut  = true;
bool enable_direction_phi_cut    = true;
bool enable_position_r_off_cut   = false;
bool enable_hit_position_x_cut   = false;
bool enable_hit_position_y_cut   = false;
bool enable_hit_colli2_x_cut     = true;
bool enable_hit_colli2_y_cut     = true;
bool enable_momentum_cut         = false;
bool enable_frontPTChi_cut       = false;
bool enable_backPTChi_cut        = false;
bool enable_R2PTChi_cut          = false;
bool enable_R3PTChi_cut          = false;

// cut values
const int multiple=18;

const double degree = 3.1415927/180.0;
const double TDC_threhold = 3.0;  // TDC cut threshold value ~ 3PE

int tdc_cut_min[2] = {-210,-210}; //tdc cuts for package {1, 2}
int tdc_cut_max[2] = {-150,-150};

double num_pe_cut = 3.0;

double num_of_frontPTChi_cut = 15; // track-level chi cut
double num_of_backPTChi_cut = 15;

double num_of_R2PTChi_cut = 40;  // partial-track-level chi cut
double num_of_R3PTChi_cut = 1;

double scattering_angle_cut_min = 3.0;
double scattering_angle_cut_max = 13.0;

double vertex_z_cut_min = -675;
double vertex_z_cut_max = -625;
double vertex_r_cut_min = 0;
double vertex_r_cut_max = 1;

double num_of_sigma_position_theta_cut = 3.0;
double num_of_sigma_direction_theta_cut = 3.0;
double num_of_sigma_position_phi_cut = 3.0;
double num_of_sigma_direction_phi_cut = 3.0;

double position_theta_cut_mean1, position_theta_cut_rms1;
double position_theta_cut_mean2, position_theta_cut_rms2;
double position_phi_cut_mean1, position_phi_cut_rms1;
double position_phi_cut_mean2, position_phi_cut_rms2;
double direction_theta_cut_mean1, direction_theta_cut_rms1;
double direction_theta_cut_mean2, direction_theta_cut_rms2;
double direction_phi_cut_mean1, direction_phi_cut_rms1;
double direction_phi_cut_mean2, direction_phi_cut_rms2;

double bending_angle_position_theta_cut_min[2];
double bending_angle_position_theta_cut_max[2];
double bending_angle_position_phi_cut_min[2];
double bending_angle_position_phi_cut_max[2];
double bending_angle_direction_theta_cut_min[2];
double bending_angle_direction_theta_cut_max[2];
double bending_angle_direction_phi_cut_min[2];
double bending_angle_direction_phi_cut_max[2];

double position_r_off_cut_min = -1.0;
double position_r_off_cut_max = 1.0;

double hit_position_x_cut_min = 335-5.0; // (344-9.0) -10.0; // add in 10 cm margin
double hit_position_x_cut_max = 335+5.0; // (344+9.0) +10.0;
double hit_position_y_cut_min = -50; // -120;
double hit_position_y_cut_max = 50; // 120;

double hit_colli2_x_cut_min = 42.14-10.0;
double hit_colli2_x_cut_max = 42.14+10.0;
double hit_colli2_y_cut_min = -5.0;
double hit_colli2_y_cut_max = +5.0;

double momentum_min = 0.0;
double momentum_max = 2000.0;  // unit: MeV

// rejection flag
bool rejection;

// parameters

// main detector pedestal values
int MDm_Pedestal[]={0,230,190,250,225,272,192,270,193};
int MDp_Pedestal[]={0,195,190,240,176,197,211,245,250};

// main detector single PE calibration
//
// Calibration: October 5, 2010
//14.0	22.0	20.4	22.1	22.8	28.7	24.6	27.8
//16.7	38.5	20.4	22.4	30.1	25.3	16.1	12.3
//
// Calibration: November 2, 2010
//11.9	18.4	20.7	16.9	20.4	25.1	16.4	24.9
//18.2	37.9	19.0	22.8	28.0	25.9	29.2	12.2
//
// Calibration: March 14, 2011
//12.4	11.2	12.9	21.0	12.0	13.3	14.7	11.9
//17.0	15.8	13.8	18.5	17.9	18.6	16.4	11.4

// 2 rows for 3 calibration periods, using averages of calibration 1 & 2 for period 0
// using calibration 3 for period 1. 9 columns, col 1- 8 for 8 detectors
double MDm_SinglePE[2][9]={
    0.0, 13.0, 20.2, 20.6, 19.5, 21.6, 26.9, 20.5, 26.4,  // period 0
    0.0, 12.4, 11.2, 12.9, 21.0, 12.0, 13.3, 14.7, 11.9}; // period 1

double MDp_SinglePE[2][9]={
    0.0, 17.5, 38.2, 19.7, 22.6, 29.1, 26.6, 22.7, 12.3,  // period 0
    0.0, 17.0, 15.8, 13.8, 18.5, 17.9, 18.6, 16.4, 11.4}; // period 1


int tracking_period; // period 0: run <=11871 (before Jan 2011),
                     // period 1: run>=13653 (Nov 2011 to May 2012)

// main detector z location
//double md_zpos[9] = {0.0, 581.665, 576.705, 577.020, 577.425, 582.515, 577.955, 577.885, 577.060};
double md_zpos[9] = {0.0, 577.410, 577.415, 577.710, 578.080, 578.240, 578.615, 578.515, 577.730};//MD survey data on Oct 2012

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
  double mdm_num_pe = (mdm_adc_value-MDm_Pedestal[md_num])/MDm_SinglePE[tracking_period][md_num];
  double mdp_num_pe = (mdp_adc_value-MDp_Pedestal[md_num])/MDp_SinglePE[tracking_period][md_num];

  double pe_min = 0.5;
  if ((mdm_num_pe>pe_min) && (mdp_num_pe>pe_min) && ( (mdm_num_pe+mdp_num_pe)>num_pe_cut) )
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

bool  momentum_cut(double val)
{
  if(val> momentum_min && val< momentum_max)
    return true;
  else
  {
    if(debug)
       cout<<"rejected, momentum_cut, momentum="<<val<<endl;
    return false;
  }
}

bool  bending_angle_position_theta_cut(double val, int p)
{
  if(val> bending_angle_position_theta_cut_min[p-1] && val< bending_angle_position_theta_cut_max[p-1])
    return true;
  else
  {
    if(debug)
       cout<<"rejected, bending_angle_position_theta_cut, position_theta_off="<<val<<endl;
    return false;
  }
}

bool  bending_angle_position_phi_cut(double val, int p)
{
  if(val> bending_angle_position_phi_cut_min[p-1] && val< bending_angle_position_phi_cut_max[p-1])
    return true;
  else
  {
    if(debug)
       cout<<"rejected, bending_angle_position_phi_cut, position_phi_off="<<val<<endl;
    return false;
  }
}

bool  bending_angle_direction_theta_cut(double val, int p)
{
  if(val> bending_angle_direction_theta_cut_min[p-1] && val< bending_angle_direction_theta_cut_max[p-1])
    return true;
  else
  {
    if(debug)
       cout<<"rejected, bending_angle_direction_theta_cut, direction_theta_off="<<val<<endl;
    return false;
  }
}

bool  bending_angle_direction_phi_cut(double val, int p)
{
  if(val > bending_angle_direction_phi_cut_min[p-1] && val < bending_angle_direction_phi_cut_max[p-1])
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

bool hit_colli2_x_cut(double val)
{
  if(fabs(val)> hit_colli2_x_cut_min && fabs(val)< hit_colli2_x_cut_max)
    return true;
  else
  {
    if(debug)
       cout<<"rejected, hit_colli2_x_cut, x="<<val<<endl;
    return false;
  }
}

bool hit_colli2_y_cut(double val)
{
  if(fabs(val)> hit_colli2_y_cut_min && fabs(val)< hit_colli2_y_cut_max)
    return true;
  else
  {
    if(debug)
       cout<<"rejected, hit_colli2_y_cut, y="<<val<<endl;
    return false;
  }
}

bool frontPTChi_cut ( double val)
{
   if (val< num_of_frontPTChi_cut)
        return true;
    else
    {
        rejection = true;
        if ( debug )
            cout<<"rejected, track-level frontPTChi_cut, frontPTChi="<<val<<endl;
        return false;
    }
}

bool backPTChi_cut ( double val)
{
   if (val< num_of_backPTChi_cut)
        return true;
    else
    {
        rejection = true;
        if ( debug )
            cout<<"rejected, track-level backPTChi_cut, backPTChi="<<val<<endl;
        return false;
    }
}

bool PTChi_cut ( double val, double num_of_PTChi_cut )
{
   if (val< num_of_PTChi_cut)
        return true;
    else
    {
        rejection = true;
        if ( debug )
            cout<<"rejected, partial-track-level PTChi_cut, PTChi="<<val<<endl;
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

   //define canvas for drawing histograms
   TCanvas* c0=new TCanvas("c0","Q2 and scattering angle distributions",800,800);
   
   Int_t nevents=event_tree->GetEntries();
   cout << "total events: " << nevents << endl;
   int start=(event_start==-1)? 0:event_start;
   int end=(event_end==-1)? nevents:event_end;

   //Find the run period according run number
   if(run<=11871)
      tracking_period = 0;
   else
      tracking_period = 1; 

   cout<<"\nSingle PE calibration for tracking period "<<tracking_period<<":"<<endl<<endl;
   cout<<"MD#\t1\t2\t3\t4\t5\t6\t7\t8"<<endl<<"minus";
   for(int kk=1;kk<=8;kk++)
   {
      cout<<"\t"<<MDm_SinglePE[tracking_period][kk];
   }
   cout<<endl<<"plus";
   for(int kk=1;kk<=8;kk++)
   {
      cout<<"\t"<<MDp_SinglePE[tracking_period][kk];
   }
   cout<<endl<<endl;

   //Get the oct number and main detector pedestals
   int oct=getOctNumber(event_tree);
   getQdcPedestal(event_tree,start,end);

   //Get parameters of matching angle residuals
   getMatchingAngles(event_tree,start,end);

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


    // define histograms
    TH2F* hit_dist1 = new TH2F("hit_dist1",Form("Run %d, Hits Distribution in Oct %d",run,md_1),480,-120,120,100,310,360);
    TH2F* hit_dist2 = new TH2F("hit_dist2",Form("Run %d, Hits Distribution in Oct %d",run,md_2),480,-120,120,100,310,360);
    TH1F* angle = new TH1F("theta",Form("Run %d, Scattering Angle Distribution, Oct %d + %d",run,md_1,md_2),400,3,14);
    TH1F* q2 = new TH1F("q2",Form("Run %d, Q2 Distribution, Oct %d + %d",run,md_1,md_2),400,0,0.08);
    TH1F* angle_1 = new TH1F("theta_1",Form("Run %d, Scattering Angle in Package 1, Oct %d",run,md_1),400,3,14);
    TH1F* angle_2 = new TH1F("theta_2",Form("Run %d, Scattering Angle in Package 2, Oct %d",run,md_2),400,3,14);
    TH1F* q2_1 = new TH1F("q2_1",Form("Run %d, Q2 Distribution in Package 1, Oct %d",run,md_1),400,0,0.08);
    TH1F* q2_2 = new TH1F("q2_2",Form("Run %d, Q2 Distribution in Package 2, Oct %d",run,md_2),400,0,0.08);

    //define extra histograms
    TH2F*  r2_proj[3];
    r2_proj[0] = new TH2F("r2_proj",Form("Run %d, Hits Distribution Collimator in Oct %d + %d",run,md_1,md_2),480,-60,60,400,-60,60);
    r2_proj[1] = new TH2F("r2_proj_1",Form("Run %d, Pkg1 Hits Distribution Collimator in Oct %d",run,md_1),480,-60,60,400,-60,60);
    r2_proj[2] = new TH2F("r2_proj_2",Form("Run %d, Pkg2 Hits Distribution Collimator in Oct %d",run,md_2),480,-60,60,400,-60,60);

    TH3F*  vertex_dist[3];
    vertex_dist[0] = new TH3F("vertex_dist",Form("Run %d, Vertex Distribution (pkg1+pkg2), Oct %d + %d",run,md_1,md_2),200,-750,-550,100,-10,10,100,-10,10);
    vertex_dist[1] = new TH3F("vertex_dist_1",Form("Run %d, Vertex Distribution (pkg1), Oct %d",run,md_1),200,-750,-550,100,-10,10,100,-10,10);
    vertex_dist[2] = new TH3F("vertex_dist_2",Form("Run %d, Vertex Distribution (pkg2), Oct %d",run,md_2),200,-750,-550,100,-10,10,100,-10,10);

    TH2F*  vertex_2D[3];
    vertex_2D[0] = new TH2F("vertex_2D_xy",Form("Run %d, Vertex Distribution (pkg1+pkg2), Oct %d + %d",run,md_1,md_2),240,-3,3,240,-3,3);
    vertex_2D[1] = new TH2F("vertex_2D_xy_1",Form("Run %d, Vertex Distribution (pkg1), Oct %d",run,md_1),240,-3,3,240,-3,3);
    vertex_2D[2] = new TH2F("vertex_2D_xy_2",Form("Run %d, Vertex Distribution (pkg2), Oct %d",run,md_2),240,-3,3,240,-3,3);

    TH2F*  vertex_2D_xz[3];
    vertex_2D_xz[0] = new TH2F("vertex_2D_xz",Form("Run %d, Vertex Distribution (pkg1+pkg2), Oct %d + %d",run,md_1,md_2),400,-750,-550,240,-3,3);
    vertex_2D_xz[1] = new TH2F("vertex_2D_xz_1",Form("Run %d, Vertex Distribution (pkg1), Oct %d",run,md_1),400,-750,-550,240,-3,3);
    vertex_2D_xz[2] = new TH2F("vertex_2D_xz_2",Form("Run %d, Vertex Distribution (pkg2), Oct %d",run,md_2),400,-750,-550,240,-3,3);

    TH2F*  vertex_2D_yz[3];
    vertex_2D_yz[0] = new TH2F("vertex_2D_yz",Form("Run %d, Vertex Distribution (pkg1+pkg2), Oct %d + %d",run,md_1,md_2),400,-750,-550,240,-3,3);
    vertex_2D_yz[1] = new TH2F("vertex_2D_yz_1",Form("Run %d, Vertex Distribution (pkg1), Oct %d",run,md_1),400,-750,-550,240,-3,3);
    vertex_2D_yz[2] = new TH2F("vertex_2D_yz_2",Form("Run %d, Vertex Distribution (pkg2), Oct %d",run,md_2),400,-750,-550,240,-3,3);

    TH2F*  hit_tgt_window[3];
    hit_tgt_window[0] = new TH2F("hit_tgt_window",Form("Run %d, Hit Distribution (pkg1+pkg2) in Downstream Target Window, Oct %d + %d",run,md_1,md_2),400,-10,10,400,-10,10);
    hit_tgt_window[1] = new TH2F("hit_tgt_window_1",Form("Run %d, Hit Distribution (pkg1) in Downstream Target Window, Oct %d",run,md_1),400,-10,10,400,-10,10);
    hit_tgt_window[2] = new TH2F("hit_tgt_window_2",Form("Run %d, Hit Distribution (pkg2) in Downstream Target Window, Oct %d",run,md_2),400,-10,10,400,-10,10);

    TH1F* light_weighted_q2[3];
    light_weighted_q2[0] = new TH1F("light_weighted_q2_all",Form("Run %d, Light-weighted Q2 Distribution, Oct %d + %d",run,md_1,md_2),400,0,0.08);
    light_weighted_q2[1] = new TH1F("light_weighted_q2_p1",Form("Run %d, Light-weighted Q2 Distribution in Package 1, Oct %d",run,md_1),400,0,0.08);
    light_weighted_q2[2] = new TH1F("light_weighted_q2_p2",Form("Run %d, Light-weighted Q2 Distribution in Package 2, Oct %d",run,md_2),400,0,0.08);

    TH1F* histo_vertex_r[3];
    histo_vertex_r[0] = new TH1F("histo_vertex_r_all",Form("Run %d, Vertex R, Oct %d + %d",run,md_1,md_2),8000,0,70);
    histo_vertex_r[1] = new TH1F("histo_vertex_r_p1",Form("Run %d, Vertex R in Package 1, Oct %d",run,md_1),8000,0,70);
    histo_vertex_r[2] = new TH1F("histo_vertex_r_p2",Form("Run %d, Vertex R in Package 2, Oct %d",run,md_2),8000,0,70);

    TH1F* histo_vertex_z[3];        
    histo_vertex_z[0] = new TH1F("histo_vertex_z_all",Form("Run %d, Vertex Z, Oct %d + %d",run,md_1,md_2),8000,-1500,-1);
    histo_vertex_z[1] = new TH1F("histo_vertex_z_p1",Form("Run %d, Vertex Z in Package 1, Oct %d",run,md_1),8000,-1500,-1);
    histo_vertex_z[2] = new TH1F("histo_vertex_z_p2",Form("Run %d, Vertex Z in Package 2, Oct %d",run,md_2),8000,-1500,-1);

    TH1F* histo_momentum[3];
    histo_momentum[0] = new TH1F("histo_momentum_all",Form("Run %d, Reconstructed Momentum, Oct %d + %d",run,md_1,md_2),8000,0,2000);
    histo_momentum[1] = new TH1F("histo_momentum_p1",Form("Run %d, Reconstructed Momentum in Package 1, Oct %d",run,md_1),8000,0,2000);
    histo_momentum[2] = new TH1F("histo_momentum_p2",Form("Run %d, Reconstructed Momentum in Package 2, Oct %d",run,md_2),8000,0,2000);

    TH2F* q2_vs_momentum[3];
    q2_vs_momentum[0] = new TH2F("q2_vs_momentum_all",Form("Run %d, Q2 vs Reconstructed Momentum, Oct %d + %d",run,md_1,md_2),8000,0,2000,400,0,0.08);
    q2_vs_momentum[1] = new TH2F("q2_vs_momentum_p1",Form("Run %d, Q2 vs Reconstructed Momentum in Package 1, Oct %d",run,md_1),8000,0,2000,400,0,0.08);
    q2_vs_momentum[2] = new TH2F("q2_vs_momentum_p2",Form("Run %d, Q2 vs Reconstructed Momentum in Package 2, Oct %d",run,md_2),8000,0,2000,400,0,0.08);

    TH1F* histo_p0[3];
    histo_p0[0] = new TH1F("histo_P0_all",Form("Run %d, P0, Oct %d + %d",run,md_1,md_2),400,1.0,1.2);
    histo_p0[1] = new TH1F("histo_P0_p1",Form("Run %d, P0, Package 1, Oct %d",run,md_1),400,1.0,1.2);
    histo_p0[2] = new TH1F("histo_P0_p2",Form("Run %d, P0, Package 2, Oct %d",run,md_2),400,1.0,1.2);

    TH1F* histo_pp[3];
    histo_pp[0] = new TH1F("histo_Pp[0]_all",Form("Run %d, Pp, Oct %d + %d",run,md_1,md_2),400,1.0,1.2);
    histo_pp[1] = new TH1F("histo_Pp[1]_p1",Form("Run %d, Pp, Package 1, Oct %d",run,md_1),400,1.0,1.2);
    histo_pp[2] = new TH1F("histo_Pp[2]_p2",Form("Run %d, Pp, Package 2, Oct %d",run,md_2),400,1.0,1.2);

    TH2F* eloss_vs_verz[3];
    eloss_vs_verz[0] = new TH2F("eloss_vs_verx_all",Form("Run %d, Eloss vs. vertex Z, Oct %d + %d",run,md_1,md_2),400,-680,-620,400,0,24);
    eloss_vs_verz[1] = new TH2F("eloss_vs_verx_p1",Form("Run %d, Eloss vs. vertex Z in Oct %d",run,md_1),400,-680,-620,400,0,24);
    eloss_vs_verz[2] = new TH2F("eloss_vs_verx_p2",Form("Run %d, Eloss vs. vertex Z in Oct %d",run,md_2),400,-680,-620,400,0,24);

    TH1F* histo_direction_phi_off[3];
    histo_direction_phi_off[0] = new TH1F("histo_direction_phi_off_all",Form("Run %d, Direction_Phi_Off, Oct %d + %d",run,md_1,md_2),400,-2/degree,2/degree);
    histo_direction_phi_off[1] = new TH1F("histo_direction_phi_off_p1",Form("Run %d, Direction_Phi_Off, Package 1, Oct %d",run,md_1),400,-2/degree,2/degree);
    histo_direction_phi_off[2] = new TH1F("histo_direction_phi_off_p2",Form("Run %d, Direction_Phi_Off, Package 2, Oct %d",run,md_2),400,-2/degree,2/degree);

    TH1F* histo_position_phi_off[3];
    histo_position_phi_off[0] = new TH1F("histo_position_phi_off_all",Form("Run %d, Position_Phi_Off, Oct %d + %d",run,md_1,md_2),400,-1/degree,1/degree);
    histo_position_phi_off[1] = new TH1F("histo_position_phi_off_p1",Form("Run %d, Position_Phi_Off, Package 1, Oct %d",run,md_1),400,-1/degree,1/degree);
    histo_position_phi_off[2] = new TH1F("histo_position_phi_off_p2",Form("Run %d, Position_Phi_Off, Package 2, Oct %d",run,md_2),400,-1/degree,1/degree);

    TH1F* histo_direction_theta_off[3];
    histo_direction_theta_off[0] = new TH1F("histo_direction_theta_off_all",Form("Run %d, Direction_Theta_Off, Oct %d + %d",run,md_1,md_2),400,-0.4/degree,0.4/degree);
    histo_direction_theta_off[1] = new TH1F("histo_direction_theta_off_p1",Form("Run %d, Direction_Theta_Off, Package 1, Oct %d",run,md_1),400,-0.4/degree,0.4/degree);
    histo_direction_theta_off[2] = new TH1F("histo_direction_theta_off_p2",Form("Run %d, Direction_Theta_Off, Package 2, Oct %d",run,md_2),400,-0.4/degree,0.4/degree);

    TH1F* histo_position_theta_off[3];
    histo_position_theta_off[0] = new TH1F("histo_position_theta_off_all",Form("Run %d, Position_Theta_Off, Oct %d + %d",run,md_1,md_2),400,-0.003/degree,0.003/degree);
    histo_position_theta_off[1] = new TH1F("histo_position_theta_off_p1",Form("Run %d, Position_Theta_Off, Package 1, Oct %d",run,md_1),400,-0.003/degree,0.003/degree);
    histo_position_theta_off[2] = new TH1F("histo_position_theta_off_p2",Form("Run %d, Position_Theta_Off, Package 2, Oct %d",run,md_2),400,-0.003/degree,0.003/degree);

    TH1F* histo_md1_pe[3];
    histo_md1_pe[0] = new TH1F("histo_md1_pe_tot",Form("Run %d, Total #PE, Oct %d",run,md_1),500,0,500);
    histo_md1_pe[1] = new TH1F("histo_md1m_pe",Form("Run %d, MD%d-  #PE, Oct %d",run,md_1,md_1),500,0,250);
    histo_md1_pe[2] = new TH1F("histo_md1p_pe",Form("Run %d, MD%d+  #PE, Oct %d",run,md_1,md_1),500,0,250);
    TH1F* histo_md2_pe[3];
    histo_md2_pe[0] = new TH1F("histo_md2_pe_tot",Form("Run %d, Total #PE, Oct %d",run,md_2),500,0,500);
    histo_md2_pe[1] = new TH1F("histo_md2m_pe",Form("Run %d, MD%d-  #PE, Oct %d",run,md_2,md_2),500,0,250);
    histo_md2_pe[2] = new TH1F("histo_md2p_pe",Form("Run %d, MD%d+  #PE, Oct %d",run,md_2,md_2),500,0,250);

    TH1F* histo_tdc1m = new TH1F("histo_tdc1m",Form("Run %d, TDC -, Oct %d",run,md_1),500,-350,0);
    TH1F* histo_tdc1p = new TH1F("histo_tdc1p",Form("Run %d, TDC +, Oct %d",run,md_1),500,-350,0);
    TH1F* histo_tdc2m = new TH1F("histo_tdc2m",Form("Run %d, TDC -, Oct %d",run,md_2),500,-350,0);
    TH1F* histo_tdc2p = new TH1F("histo_tdc2p",Form("Run %d, TDC +, Oct %d",run,md_2),500,-350,0);

    TH2F* pe_vs_tdc_1m = new TH2F("pe_vs_tdc_1m",Form("Run %d, #PE vs. TDC -, Oct %d",run,md_1),500,-350,0,500,0,250);
    TH2F* pe_vs_tdc_1p = new TH2F("pe_vs_tdc_1p",Form("Run %d, #PE vs. TDC +, Oct %d",run,md_1),500,-350,0,500,0,250);
    TH2F* pe_vs_tdc_2m = new TH2F("pe_vs_tdc_2m",Form("Run %d, #PE vs. TDC -, Oct %d",run,md_2),500,-350,0,500,0,250);
    TH2F* pe_vs_tdc_2p = new TH2F("pe_vs_tdc_2p",Form("Run %d, #PE vs. TDC +, Oct %d",run,md_2),500,-350,0,500,0,250);

    TProfile2D* q2_dist1 = new TProfile2D("q2_dist1",Form("Run %d, Q2 Distribution in Oct %d",run,md_1),480,-120,120,100,310,360);
    TProfile2D* q2_dist2 = new TProfile2D("q2_dist2",Form("Run %d, Q2 Distribution in Oct %d",run,md_2),480,-120,120,100,310,360);
    TProfile2D* light_dist1 = new TProfile2D("light_dist1",Form("Run %d, Light Distribution in Oct %d",run,md_1),480,-120,120,100,310,360);
    TProfile2D* light_dist2 = new TProfile2D("light_dist2",Form("Run %d, light Distribution in Oct %d",run,md_2),480,-120,120,100,310,360);
    TProfile2D* weighted_q2_dist1 = new TProfile2D("weighted_q2_dist1",Form("Run %d, Light-weighted Q2 Distribution in Oct %d",run,md_1),480,-120,120,100,310,360);
    TProfile2D* weighted_q2_dist2 = new TProfile2D("weighted_q2_dist2",Form("Run %d, Light-weighted Q2 Distribution in Oct %d",run,md_2),480,-120,120,100,310,360);

    TProfile* q2_vs_x1 = new TProfile("q2_vs_x1",Form("Run %d, Q2 vs. X in Oct %d",run,md_1),240,-120,120);
    TProfile* q2_vs_x2 = new TProfile("q2_vs_x2",Form("Run %d, Q2 vs. X in Oct %d",run,md_2),240,-120,120);
    TProfile* light_vs_x1 = new TProfile("light_vs_x1",Form("Run %d, Light vs. X in Oct %d",run,md_1),240,-120,120);
    TProfile* light_vs_x2 = new TProfile("light_vs_x2",Form("Run %d, light vs. X in Oct %d",run,md_2),240,-120,120);
    TProfile* weighted_q2_vs_x1 = new TProfile("weighted_q2_vs_x1",Form("Run %d, Light-weighted Q2 vs. X in Oct %d",run,md_1),240,-120,120);
    TProfile* weighted_q2_vs_x2 = new TProfile("weighted_q2_vs_x2",Form("Run %d, Light-weighted Q2 vs. X in Oct %d",run,md_2),240,-120,120);

    TProfile* q2_vs_y1 = new TProfile("q2_vs_y1",Form("Run %d, Q2 vs. Y in Oct %d",run,md_1),120,335-20,335+20);
    TProfile* q2_vs_y2 = new TProfile("q2_vs_y2",Form("Run %d, Q2 vs. Y in Oct %d",run,md_2),120,335-20,335+20);
    TProfile* light_vs_y1 = new TProfile("light_vs_y1",Form("Run %d, Light vs. Y in Oct %d",run,md_1),120,335-20,335+20);
    TProfile* light_vs_y2 = new TProfile("light_vs_y2",Form("Run %d, light vs. Y in Oct %d",run,md_2),120,335-20,335+20);
    TProfile* weighted_q2_vs_y1 = new TProfile("weighted_q2_vs_y1",Form("Run %d, Light-weighted Q2 vs. Y in Oct %d",run,md_1),120,335-20,335+20);
    TProfile* weighted_q2_vs_y2 = new TProfile("weighted_q2_vs_y2",Form("Run %d, Light-weighted Q2 vs. Y in Oct %d",run,md_2),120,335-20,335+20);

    TH1F* front_pt_chi[3];
    front_pt_chi[0] = new TH1F ( "front_pt_chi_0",Form ( "Run %d, Oct %d + %d, Front Partial Track Chi-distribution",run,md_1,md_2 ),4000,0,1000 );
    front_pt_chi[1] = new TH1F ( "front_pt_chi_1",Form ( "Run %d, Oct %d, Front Partial Track Chi-distribution",run,md_1 ),4000,0,1000 );
    front_pt_chi[2] = new TH1F ( "front_pt_chi_2",Form ( "Run %d, Oct %d, Front Partial Track Chi-distribution",run,md_2 ),4000,0,1000 );
    
    TH1F* back_pt_chi[3];
    back_pt_chi[0] = new TH1F ( "back_pt_chi_0",Form ( "Run %d, Oct %d + %d, Back Partial Track Chi-distribution",run,md_1,md_2 ),400,0,100 );
    back_pt_chi[1] = new TH1F ( "back_pt_chi_1",Form ( "Run %d, Oct %d, Back Partial Track Chi-distribution",run,md_1 ),400,0,100 );
    back_pt_chi[2] = new TH1F ( "back_pt_chi_2",Form ( "Run %d, Oct %d, Back Partial Track Chi-distribution",run,md_2 ),400,0,100 );

   // Fetch events from tree
    for(int i=start;i<end;i++)  // start to loop over all events
    {

      if(i%100000==0)
	cout << "events processed so far: " << i << endl;

      event_branch->GetEntry(i);
      maindet_branch->GetEntry(i);
      trig_branch->GetEntry(i);

      rejection = false;

      // Performing partial-track-level chi cut
      // reject an event if one of its partial track chi values
      // is larger than the pre-defined cut value
      int Num_Of_PT_Tracks = fEvent->GetNumberOfPartialTracks();
      for (int j=0; j<Num_Of_PT_Tracks; j++)
      {
              pt = fEvent->GetPartialTrack(j);
              int Region = pt->GetRegion();
              double chi_val = pt->fChi;

              if( Region==2 && enable_R2PTChi_cut )
              {
                   if ( ! PTChi_cut ( chi_val, num_of_R2PTChi_cut ) )
                       continue;
              }

              if( Region==3 && enable_R3PTChi_cut )
              {
                   if ( ! PTChi_cut ( chi_val, num_of_R3PTChi_cut ) )
                       continue;
              }
      }

      if (rejection == true)
         continue;
   
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
      double r2_x=0, r2_y=0;
      double tgt_win_x=0, tgt_win_y=0;
      double vertex_x=0,vertex_y=0;

      for(int j=0;j<ntracks; j++) // TODO: what shall we do with multiple tracks? 
      {

          track=fEvent->GetTrack(j);
          int R2_pkg, R3_pkg; 
          pt = track->fBack;
          if (pt->GetRegion()==3)
            {
              R3_pkg = pt->GetPackage();
              if(R3_pkg==1)
                   dz = md_zpos[(oct+4)%8];
              else if(R3_pkg==2)
                   dz = md_zpos[oct];

              xoffset = pt->fOffsetX;
              yoffset = pt->fOffsetY;
              xslope  = pt->fSlopeX;
              yslope  = pt->fSlopeY;
              x       = xoffset+xslope*dz;
              y       = yoffset+yslope*dz;

 //             if(debug)
 //                 cout<<"event#"<<i<<", pkg"<<pkg<<", hit at ("<<x<<","<<y<<")"<<endl;

              global2local(&x,&y,oct,R3_pkg);

              if(enable_hit_position_x_cut)
              {
                 if(! hit_position_x_cut(x))
                   continue;
              }

              if(enable_hit_position_y_cut)
              {
                 if(! hit_position_y_cut(y))
                   continue;
              }
            }

      pt = track->fFront;
      if(pt->GetRegion()==2)
      {
          R2_pkg = pt->GetPackage();
          double collimator2_z=-370.719;
          double tgt_downstream_window_z = -635.0;
          double r2_xoffset = pt->fOffsetX;
          double r2_yoffset = pt->fOffsetY;
          double r2_xslope  = pt->fSlopeX;
          double r2_yslope  = pt->fSlopeY;
          r2_x = r2_xoffset+r2_xslope*collimator2_z;
          r2_y = r2_yoffset+r2_yslope*collimator2_z;

          if(R2_pkg == 1)
             oct = md_1;
          else if (R2_pkg == 2)
             oct = md_2;
 
          global2local(&r2_x,&r2_y,oct,R2_pkg);

          if(enable_hit_colli2_x_cut)
          {
              if(! hit_colli2_x_cut(r2_x))
                continue;
          }

          if(enable_hit_colli2_y_cut)
          {
              if(! hit_colli2_y_cut(r2_y))
                continue;
          }

          vertex_x = r2_xoffset+r2_xslope*vertex_z;
          vertex_y = r2_yoffset+r2_yslope*vertex_z;

          tgt_win_x = r2_xoffset+r2_xslope*tgt_downstream_window_z;
          tgt_win_y = r2_yoffset+r2_yslope*tgt_downstream_window_z;
      }

      int package = track->GetPackage();
      if(!reverse_run)
      {
        if(R2_pkg!=package || R3_pkg!=package)
          {
             cout<<"Warning: unmatched packages, R2, R3, track pkg="
                 <<R2_pkg<<", "<<R3_pkg<<", "<<package<<endl;
             continue;
          }
      }

      //----------------------
      // bending angle cut
      //----------------------
        
      double direction_theta_off = track->fDirectionThetaoff;
      double position_theta_off = track->fPositionThetaoff;
 
      double phi[4];
      phi[0] = global2local_phi(track->fEndDirectionActual.Phi(), oct, package);
      phi[1] = global2local_phi(track->fEndDirectionGoal.Phi(), oct,package);
      phi[2] = global2local_phi(track->fEndPositionActual.Phi(), oct,package);
      phi[3] = global2local_phi(track->fEndPositionGoal.Phi(), oct,package);
      
      double direction_phi_off = phi[0]-phi[1];
      double position_phi_off = phi[2]-phi[3];

      if(debug)
      {
        cout<<"direction_phi_off = "<<direction_phi_off<<endl;
        cout<<"position_phi_off = "<<position_phi_off<<endl;
      }

       // mathching angle cuts

        if(enable_direction_theta_cut)
        {
           if(! bending_angle_direction_theta_cut(direction_theta_off,package))
                continue;
        }

        if(enable_direction_phi_cut)
        {
           if(! bending_angle_direction_phi_cut(direction_phi_off,package))
                continue;
        }

        if(enable_position_theta_cut)
        {
           if(! bending_angle_position_theta_cut(position_theta_off,package))
                continue;
        }

        if(enable_position_phi_cut)
        {
           if(! bending_angle_position_phi_cut(position_phi_off,package))
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

      //----------------------------
      // reconstructed momentum cut
      //----------------------------

      double momentum = track->fMomentum;
      if(enable_momentum_cut)
        {
           if(! momentum_cut(momentum))
               continue;
        }

      //----------------------
      // TDC cut
      //----------------------

        if(package==1)
        {
           if(enable_tdc_cut)
           {
              double mdm_num_pe = (mdm_adc_value_1-MDm_Pedestal[md_1])/MDm_SinglePE[tracking_period][md_1];
              double mdp_num_pe = (mdp_adc_value_1-MDp_Pedestal[md_1])/MDp_SinglePE[tracking_period][md_1];
              if(mdm_num_pe>TDC_threhold && mdp_num_pe>TDC_threhold)
              {
                 if(! tdc_cut(mdm_tdc_value_1, mdp_tdc_value_1,package))
                    continue;
              }
           }
        }

        if(package==2)
        {
           if(enable_tdc_cut)
           {
              double mdm_num_pe = (mdm_adc_value_2-MDm_Pedestal[md_2])/MDm_SinglePE[tracking_period][md_2];
              double mdp_num_pe = (mdp_adc_value_2-MDp_Pedestal[md_2])/MDp_SinglePE[tracking_period][md_2];
              if(mdm_num_pe>TDC_threhold && mdp_num_pe>TDC_threhold)
              {
                 if(! tdc_cut(mdm_tdc_value_2, mdp_tdc_value_2,package))
                    continue;
              }
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

      // obtain additional parameters
      double Eloss  = fEvent->fHydrogenEnergyLoss;
      double P0_val = fEvent->fKinElasticWithLoss.fP0;
      double Pp_val = fEvent->fKinElasticWithLoss.fPp;

      // Performing track-level chi cut

      // front partial track Chi cuts
      double frontPTChi_val = track->fFront->fChi;
      if ( enable_frontPTChi_cut )
      {
           if ( ! frontPTChi_cut ( frontPTChi_val) )
               continue;
      }

      // back partial track Chi cut
      double backPTChi_val = track->fBack->fChi;
      if ( enable_backPTChi_cut )
      {
           if ( ! backPTChi_cut ( backPTChi_val) )
               continue;
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

                // fill additional histograms
                double mdm_num_pe = (mdm_adc_value_1-MDm_Pedestal[md_1])/MDm_SinglePE[tracking_period][md_1];
                double mdp_num_pe = (mdp_adc_value_1-MDp_Pedestal[md_1])/MDp_SinglePE[tracking_period][md_1];
                double total_num_pe = mdm_num_pe+mdp_num_pe;
                light_weighted_q2[0]->Fill(Q2_val,total_num_pe);
                light_weighted_q2[1]->Fill(Q2_val,total_num_pe);

                histo_vertex_r[0]->Fill(vertex_r);
                histo_vertex_r[1]->Fill(vertex_r);
                histo_vertex_z[0]->Fill(vertex_z);
                histo_vertex_z[1]->Fill(vertex_z);

                histo_momentum[0]->Fill(momentum);
                histo_momentum[1]->Fill(momentum);

                q2_vs_momentum[0]->Fill(momentum,Q2_val);
                q2_vs_momentum[1]->Fill(momentum,Q2_val);

                histo_p0[0]->Fill(P0_val);
                histo_p0[1]->Fill(P0_val);
                histo_pp[0]->Fill(Pp_val);
                histo_pp[1]->Fill(Pp_val);

                eloss_vs_verz[0]->Fill(vertex_z,Eloss);
                eloss_vs_verz[1]->Fill(vertex_z,Eloss);

                histo_direction_phi_off[0]->Fill(direction_phi_off/degree);
                histo_direction_phi_off[1]->Fill(direction_phi_off/degree);
                histo_position_phi_off[0]->Fill(position_phi_off/degree);
                histo_position_phi_off[1]->Fill(position_phi_off/degree);

                histo_direction_theta_off[0]->Fill(direction_theta_off/degree);
                histo_direction_theta_off[1]->Fill(direction_theta_off/degree);
                histo_position_theta_off[0]->Fill(position_theta_off/degree);
                histo_position_theta_off[1]->Fill(position_theta_off/degree);

                histo_md1_pe[0]->Fill(total_num_pe);
                histo_md1_pe[1]->Fill(mdm_num_pe);
                histo_md1_pe[2]->Fill(mdp_num_pe);

                r2_proj[0]->Fill(r2_x,r2_y);
                r2_proj[1]->Fill(r2_x,r2_y);

                vertex_dist[0]->Fill(vertex_z,vertex_x,vertex_y);
                vertex_dist[1]->Fill(vertex_z,vertex_x,vertex_y);

                vertex_2D[0]->Fill(vertex_x,vertex_y);
                vertex_2D[1]->Fill(vertex_x,vertex_y);

                vertex_2D_xz[0]->Fill(vertex_z,vertex_x);
                vertex_2D_xz[1]->Fill(vertex_z,vertex_x);
                vertex_2D_yz[0]->Fill(vertex_z,vertex_y);
                vertex_2D_yz[1]->Fill(vertex_z,vertex_y);

                hit_tgt_window[0]->Fill(tgt_win_x,tgt_win_y);
                hit_tgt_window[1]->Fill(tgt_win_x,tgt_win_y);

                histo_tdc1m->Fill(mdm_tdc_value_1);
                histo_tdc1p->Fill(mdp_tdc_value_1);
                pe_vs_tdc_1m->Fill(mdm_tdc_value_1,mdm_num_pe);
                pe_vs_tdc_1p->Fill(mdp_tdc_value_1,mdp_num_pe);

                q2_dist1->Fill(y,x,Q2_val);
                light_dist1->Fill(y,x,total_num_pe);
                weighted_q2_dist1->Fill(y,x,Q2_val*total_num_pe);

                q2_vs_x1->Fill(y,Q2_val);
                light_vs_x1->Fill(y,total_num_pe);
                weighted_q2_vs_x1->Fill(y,Q2_val,total_num_pe);

                q2_vs_y1->Fill(x,Q2_val);
                light_vs_y1->Fill(x,total_num_pe);
                weighted_q2_vs_y1->Fill(x,Q2_val,total_num_pe);

                front_pt_chi[0]->Fill (frontPTChi_val );
                back_pt_chi[0]->Fill (backPTChi_val );
                front_pt_chi[1]->Fill (frontPTChi_val );
                back_pt_chi[1]->Fill (backPTChi_val );
           }
         else if(package==2 ) 
           {
                angle_2->Fill(angle_val);
                q2_2->Fill(Q2_val);
                angle->Fill(angle_val);
                q2->Fill(Q2_val);
                hit_dist2->Fill(y,x);

                // fill additional histograms
                double mdm_num_pe = (mdm_adc_value_2-MDm_Pedestal[md_2])/MDm_SinglePE[tracking_period][md_2];
                double mdp_num_pe = (mdp_adc_value_2-MDp_Pedestal[md_2])/MDp_SinglePE[tracking_period][md_2];
                double total_num_pe = mdm_num_pe+mdp_num_pe;
                light_weighted_q2[0]->Fill(Q2_val,total_num_pe);
                light_weighted_q2[2]->Fill(Q2_val,total_num_pe);

                histo_vertex_r[0]->Fill(vertex_r);
                histo_vertex_r[2]->Fill(vertex_r);
                histo_vertex_z[0]->Fill(vertex_z);
                histo_vertex_z[2]->Fill(vertex_z);

                histo_momentum[0]->Fill(momentum);
                histo_momentum[2]->Fill(momentum);

                q2_vs_momentum[0]->Fill(momentum,Q2_val);
                q2_vs_momentum[2]->Fill(momentum,Q2_val);

                histo_p0[0]->Fill(P0_val);
                histo_p0[2]->Fill(P0_val);
                histo_pp[0]->Fill(Pp_val);
                histo_pp[2]->Fill(Pp_val);

                eloss_vs_verz[0]->Fill(vertex_z,Eloss);
                eloss_vs_verz[2]->Fill(vertex_z,Eloss);

                histo_direction_phi_off[0]->Fill(direction_phi_off/degree);
                histo_direction_phi_off[2]->Fill(direction_phi_off/degree);
                histo_position_phi_off[0]->Fill(position_phi_off/degree);
                histo_position_phi_off[2]->Fill(position_phi_off/degree);

                histo_direction_theta_off[0]->Fill(direction_theta_off/degree);
                histo_direction_theta_off[2]->Fill(direction_theta_off/degree);
                histo_position_theta_off[0]->Fill(position_theta_off/degree);
                histo_position_theta_off[2]->Fill(position_theta_off/degree);

                histo_md2_pe[0]->Fill(total_num_pe);
                histo_md2_pe[1]->Fill(mdm_num_pe);
                histo_md2_pe[2]->Fill(mdp_num_pe);

                r2_proj[0]->Fill(r2_x,r2_y);
                r2_proj[2]->Fill(r2_x,r2_y);

                vertex_dist[0]->Fill(vertex_z,vertex_x,vertex_y);
                vertex_dist[2]->Fill(vertex_z,vertex_x,vertex_y);

                vertex_2D[0]->Fill(vertex_x,vertex_y);
                vertex_2D[2]->Fill(vertex_x,vertex_y);

                vertex_2D_xz[0]->Fill(vertex_z,vertex_x);
                vertex_2D_xz[2]->Fill(vertex_z,vertex_x);
                vertex_2D_yz[0]->Fill(vertex_z,vertex_y);
                vertex_2D_yz[2]->Fill(vertex_z,vertex_y);

                hit_tgt_window[0]->Fill(tgt_win_x,tgt_win_y);
                hit_tgt_window[2]->Fill(tgt_win_x,tgt_win_y);

                histo_tdc2m->Fill(mdm_tdc_value_2);
                histo_tdc2p->Fill(mdp_tdc_value_2);
                pe_vs_tdc_2m->Fill(mdm_tdc_value_2,mdm_num_pe);
                pe_vs_tdc_2p->Fill(mdp_tdc_value_2,mdp_num_pe);

                q2_dist2->Fill(y,x,Q2_val);
                light_dist2->Fill(y,x,total_num_pe);
                weighted_q2_dist2->Fill(y,x,Q2_val*total_num_pe);

                q2_vs_x2->Fill(y,Q2_val);
                light_vs_x2->Fill(y,total_num_pe);
                weighted_q2_vs_x2->Fill(y,Q2_val,total_num_pe);

                q2_vs_y2->Fill(x,Q2_val);
                light_vs_y2->Fill(x,total_num_pe);
                weighted_q2_vs_y2->Fill(x,Q2_val,total_num_pe);

                front_pt_chi[0]->Fill (frontPTChi_val );
                back_pt_chi[0]->Fill (backPTChi_val );
                front_pt_chi[2]->Fill (frontPTChi_val );
                back_pt_chi[2]->Fill (backPTChi_val );
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
      cout << "all : " << setprecision(5) << angle->GetMean() << " +/- " << setprecision(3) << angle->GetMeanError() << " deg"<<endl; 
    }
    if(angle_1->GetEntries()!=0) {
      cout << "pkg1: " << setprecision(5) << angle_1->GetMean() << " +/- " << setprecision(3) << angle_1->GetMeanError() << " deg"<< endl;
    }
    if(angle_2->GetEntries()!=0) {
      cout << "pkg2: " <<  setprecision(5) << angle_2->GetMean() << " +/- " <<  setprecision(3) << angle_2->GetMeanError() << " deg"<<endl;
    }

    cout << "\nQ2: " << endl;
    if(q2->GetEntries()) {
      cout << "all : " <<  setprecision(5) << q2->GetMean() << " +/- " <<  setprecision(3) << q2->GetMeanError() << " (GeV/c)^2"<<endl;
    }
    if(q2_1->GetEntries()!=0) {
      cout << "pkg1: " <<  setprecision(5) << q2_1->GetMean() << " +/- " <<  setprecision(3) << q2_1->GetMeanError() << " (GeV/c)^2"<<endl;
    }
    if(q2_2->GetEntries()!=0) {
      cout << "pkg2: " <<  setprecision(5) << q2_2->GetMean() << " +/- " <<  setprecision(3) << q2_2->GetMeanError() << " (GeV/c)^2"<<endl<<endl;
    }
 
    cout << "\nLight-weighted Q2: " << endl;
    if(light_weighted_q2[0]->GetEntries()!=0) {
      cout << "all : " <<  setprecision(5) << light_weighted_q2[0]->GetMean() << " +/- " <<  setprecision(3) << light_weighted_q2[0]->GetMeanError() << " (GeV/c)^2"<<endl;
    }
    if(light_weighted_q2[1]->GetEntries()!=0) {
      cout << "pkg1: " <<  setprecision(5) << light_weighted_q2[1]->GetMean() << " +/- " <<  setprecision(3) << light_weighted_q2[1]->GetMeanError() << " (GeV/c)^2"<<endl;
    }
    if(light_weighted_q2[2]->GetEntries()!=0) {
      cout << "pkg2: " <<  setprecision(5) << light_weighted_q2[2]->GetMean() << " +/- " <<  setprecision(3) << light_weighted_q2[2]->GetMeanError() << " (GeV/c)^2"<<endl<<endl;
    }

    // Draw histograms
    gStyle->SetPalette(1);
    c0->Clear();   

    // canvas 0
    c0->Divide(2,3);

    c0->cd(1);
    angle_1->Draw();
    angle_1->GetXaxis()->SetTitle("Scattering Angle [degree]");

    c0->cd(2);
    double SumOfWeights = q2_1->GetSumOfWeights();
    if(SumOfWeights!=0)
       q2_1->Scale(1.0/SumOfWeights);
    SumOfWeights = light_weighted_q2[1]->GetSumOfWeights();
    if(SumOfWeights!=0)
       light_weighted_q2[1]->Scale(1.0/SumOfWeights);
    q2_1->Draw();
    q2_1->GetXaxis()->SetTitle("Q2 [(GeV/c)^2]");
    light_weighted_q2[1]->SetLineColor(kRed);
    light_weighted_q2[1]->Draw("sames");
    
    c0->cd(3);
    angle_2->Draw();
    angle_2->GetXaxis()->SetTitle("Scattering Angle [degree]");

    c0->cd(4);
    SumOfWeights = q2_2->GetSumOfWeights();
    if(SumOfWeights!=0)
       q2_2->Scale(1.0/SumOfWeights);
    SumOfWeights = light_weighted_q2[2]->GetSumOfWeights();
    if(SumOfWeights!=0)
       light_weighted_q2[2]->Scale(1.0/SumOfWeights);
    q2_2->Draw();
    q2_2->GetXaxis()->SetTitle("Q2 [(GeV/c)^2]");
    light_weighted_q2[2]->SetLineColor(kRed);
    light_weighted_q2[2]->Draw("sames");
    
    c0->cd(5);
    angle->Draw();
    angle->GetXaxis()->SetTitle("Scattering Angle [degree]");

    c0->cd(6);
    SumOfWeights = q2->GetSumOfWeights();
    if(SumOfWeights!=0)
       q2->Scale(1.0/SumOfWeights);
    SumOfWeights = light_weighted_q2[0]->GetSumOfWeights();
    if(SumOfWeights!=0)
       light_weighted_q2[0]->Scale(1.0/SumOfWeights);
    q2->Draw();
    q2->GetXaxis()->SetTitle("Q2 [(GeV/c)^2]");
    light_weighted_q2[0]->SetLineColor(kRed);
    light_weighted_q2[0]->Draw("sames");

    // draw additional plots

    // canvas 1
    TCanvas* c1=new TCanvas("c1","scattering vertex distributions",800,600);
    c1->Divide(2,2);
   
    c1->cd(1);
    c1_1->SetLogy();
    histo_vertex_z[1]->Draw();
    histo_vertex_z[1]->GetXaxis()->SetTitle("Vertex Z [cm]");

    c1->cd(3);
    c1_3->SetLogy();
    histo_vertex_z[2]->Draw();
    histo_vertex_z[2]->GetXaxis()->SetTitle("Vertex Z [cm]");

    //c1->cd(5);
    //c1_5->SetLogy();
    //histo_vertex_z[0]->Draw();
    //histo_vertex_z[0]->GetXaxis()->SetTitle("Vertex Z [cm]");

    c1->cd(2);
    c1_2->SetLogy();
    histo_vertex_r[1]->Draw();
    histo_vertex_r[1]->GetXaxis()->SetTitle("Vertex R [cm]");

    c1->cd(4);
    c1_4->SetLogy();
    histo_vertex_r[2]->Draw();
    histo_vertex_r[2]->GetXaxis()->SetTitle("Vertex R [cm]");

    //c1->cd(6);
    //c1_6->SetLogy();
    //histo_vertex_r[0]->Draw();
    //histo_vertex_r[0]->GetXaxis()->SetTitle("Vertex R [cm]");

    // canvas 2
    TCanvas* c2=new TCanvas("c2","momentum distributions",800,800);
    c2->Divide(2,3);

    c2->cd(1);
    //c2_1->SetLogy();
    histo_p0[1]->Draw();
    histo_p0[1]->GetXaxis()->SetTitle("P0 [GeV]");

    c2->cd(3);
    //c2_3->SetLogy();
    histo_p0[2]->Draw();
    histo_p0[2]->GetXaxis()->SetTitle("P0 [GeV]");

    //c2->cd(5);
    //c2_5->SetLogy();
    //histo_p0[0]->Draw();
    //histo_p0[0]->GetXaxis()->SetTitle("P0 [GeV]");

    c2->cd(2);
    //c2_2->SetLogy();
    histo_pp[1]->Draw();
    histo_pp[1]->GetXaxis()->SetTitle("Pp [GeV]");

    c2->cd(4);
    //c2_4->SetLogy();
    histo_pp[2]->Draw();
    histo_pp[2]->GetXaxis()->SetTitle("Pp [GeV]");

    //c2->cd(6);
    //c2_6->SetLogy();
    //histo_pp[0]->Draw();
    //histo_pp[0]->GetXaxis()->SetTitle("Pp [GeV]");

    c2->cd(5);
    eloss_vs_verz[1]->Draw();
    eloss_vs_verz[1]->GetXaxis()->SetTitle("Vertex Z [cm]");
    eloss_vs_verz[1]->GetYaxis()->SetTitle("Eloss [MeV]");

    c2->cd(6);
    eloss_vs_verz[2]->Draw();
    eloss_vs_verz[2]->GetXaxis()->SetTitle("Vertex Z [cm]");
    eloss_vs_verz[2]->GetYaxis()->SetTitle("Eloss [MeV]");

    // canvas 3
    TCanvas* c3=new TCanvas("c3","azimuthal angle residuals",800,800);
    c3->Divide(2,3);

    c3->cd(1);
    c3_1->SetLogy();
    histo_direction_phi_off[1]->Draw();
    histo_direction_phi_off[1]->GetXaxis()->SetTitle("direction_phi_off [deg]");

    c3->cd(3);
    c3_3->SetLogy();
    histo_direction_phi_off[2]->Draw();
    histo_direction_phi_off[2]->GetXaxis()->SetTitle("direction_phi_off [deg]");

    c3->cd(5);
    c3_5->SetLogy();
    histo_direction_phi_off[0]->Draw();
    histo_direction_phi_off[0]->GetXaxis()->SetTitle("direction_phi_off [deg]");

    c3->cd(2);
    c3_2->SetLogy();
    histo_position_phi_off[1]->Draw();
    histo_position_phi_off[1]->GetXaxis()->SetTitle("position_phi_off [deg]");

    c3->cd(4);
    c3_4->SetLogy();
    histo_position_phi_off[2]->Draw();
    histo_position_phi_off[2]->GetXaxis()->SetTitle("position_phi_off [deg]");

    c3->cd(6);
    c3_6->SetLogy();
    histo_position_phi_off[0]->Draw();
    histo_position_phi_off[0]->GetXaxis()->SetTitle("position_phi_off [deg]");

    // canvas 4
    TCanvas* c4=new TCanvas("c4","radial angle residuals",800,800);
    c4->Divide(2,3);

    c4->cd(1);
    c4_1->SetLogy();
    histo_direction_theta_off[1]->Draw();
    histo_direction_theta_off[1]->GetXaxis()->SetTitle("direction_theta_off [deg]");

    c4->cd(3);
    c4_3->SetLogy();
    histo_direction_theta_off[2]->Draw();
    histo_direction_theta_off[2]->GetXaxis()->SetTitle("direction_theta_off [deg]");

    c4->cd(5);
    c4_5->SetLogy();
    histo_direction_theta_off[0]->Draw();
    histo_direction_theta_off[0]->GetXaxis()->SetTitle("direction_theta_off [deg]");

    c4->cd(2);
    c4_2->SetLogy();
    histo_position_theta_off[1]->Draw();
    histo_position_theta_off[1]->GetXaxis()->SetTitle("position_theta_off [deg]");

    c4->cd(4);
    c4_4->SetLogy();
    histo_position_theta_off[2]->Draw();
    histo_position_theta_off[2]->GetXaxis()->SetTitle("position_theta_off [deg]");

    c4->cd(6);
    c4_6->SetLogy();
    histo_position_theta_off[0]->Draw();
    histo_position_theta_off[0]->GetXaxis()->SetTitle("position_theta_off [deg]");

    // canvas 5
    TCanvas* c5=new TCanvas("c5","MD light yield",800,800);
    c5->Divide(2,3);

    c5->cd(1);
    histo_md1_pe[0]->Draw();
    histo_md1_pe[0]->GetXaxis()->SetTitle("#PE");
    histo_md1_pe[0]->GetYaxis()->SetTitle("counts");

    c5->cd(2);
    histo_md2_pe[0]->Draw();
    histo_md2_pe[0]->GetXaxis()->SetTitle("#PE");
    histo_md2_pe[0]->GetYaxis()->SetTitle("counts");

    c5->cd(3);
    histo_md1_pe[1]->Draw();
    histo_md1_pe[1]->GetXaxis()->SetTitle("#PE");
    histo_md1_pe[1]->GetYaxis()->SetTitle("counts");

    c5->cd(4);
    histo_md2_pe[1]->Draw();
    histo_md2_pe[1]->GetXaxis()->SetTitle("#PE");
    histo_md2_pe[1]->GetYaxis()->SetTitle("counts");

    c5->cd(5);
    histo_md1_pe[2]->Draw();
    histo_md1_pe[2]->GetXaxis()->SetTitle("#PE");
    histo_md1_pe[2]->GetYaxis()->SetTitle("counts");

    c5->cd(6);
    histo_md2_pe[2]->Draw();
    histo_md2_pe[2]->GetXaxis()->SetTitle("#PE");
    histo_md2_pe[2]->GetYaxis()->SetTitle("counts");

    // canvas 6
    TCanvas* c6=new TCanvas("c6","2D distributions of hits, #PE and Q2",800,800);
    c6->Divide(2,4);

    c6->cd(1);
    hit_dist1->Draw("colz");
    hit_dist1->GetXaxis()->SetTitle("Y [cm]");
    hit_dist1->GetYaxis()->SetTitle("X [cm]");

    c6->cd(2);
    hit_dist2->Draw("colz");
    hit_dist2->GetXaxis()->SetTitle("Y [cm]");
    hit_dist2->GetYaxis()->SetTitle("X [cm]");

    c6->cd(3);
    light_dist1->Draw("colz");
    light_dist1->GetXaxis()->SetTitle("Y [cm]");
    light_dist1->GetYaxis()->SetTitle("X [cm]");

    c6->cd(4);
    light_dist2->Draw("colz");
    light_dist2->GetXaxis()->SetTitle("Y [cm]");
    light_dist2->GetYaxis()->SetTitle("X [cm]");

    c6->cd(5);
    q2_dist1->Draw("colz");
    q2_dist1->GetXaxis()->SetTitle("Y [cm]");
    q2_dist1->GetYaxis()->SetTitle("X [cm]");
    
    c6->cd(6);
    q2_dist2->Draw("colz");
    q2_dist2->GetXaxis()->SetTitle("Y [cm]");
    q2_dist2->GetYaxis()->SetTitle("X [cm]");

    c6->cd(7);
    weighted_q2_dist1->Draw("colz");
    weighted_q2_dist1->GetXaxis()->SetTitle("Y [cm]");
    weighted_q2_dist1->GetYaxis()->SetTitle("X [cm]");
    
    c6->cd(8);
    weighted_q2_dist2->Draw("colz");
    weighted_q2_dist2->GetXaxis()->SetTitle("Y [cm]");
    weighted_q2_dist2->GetYaxis()->SetTitle("X [cm]");

    // canvas 7
    TCanvas* c7=new TCanvas("c7","TDC spectra",800,800);
    c7->Divide(2,4);

    c7->cd(1);
    histo_tdc1m->Draw();
    histo_tdc1m->GetXaxis()->SetTitle("tdc");
    histo_tdc1m->GetYaxis()->SetTitle("counts");

    c7->cd(2);
    histo_tdc2m->Draw();
    histo_tdc2m->GetXaxis()->SetTitle("tdc");
    histo_tdc2m->GetYaxis()->SetTitle("counts");

    c7->cd(3);
    histo_tdc1p->Draw();
    histo_tdc1p->GetXaxis()->SetTitle("tdc");
    histo_tdc1p->GetYaxis()->SetTitle("counts");

    c7->cd(4);
    histo_tdc2p->Draw();
    histo_tdc2p->GetXaxis()->SetTitle("tdc");
    histo_tdc2p->GetYaxis()->SetTitle("counts");

    c7->cd(5);
    pe_vs_tdc_1m->Draw();
    pe_vs_tdc_1m->GetXaxis()->SetTitle("tdc");
    pe_vs_tdc_1m->GetYaxis()->SetTitle("#PE");

    c7->cd(6);
    pe_vs_tdc_2m->Draw();
    pe_vs_tdc_2m->GetXaxis()->SetTitle("tdc");
    pe_vs_tdc_2m->GetYaxis()->SetTitle("#PE");

    c7->cd(7);
    pe_vs_tdc_1p->Draw();
    pe_vs_tdc_1p->GetXaxis()->SetTitle("tdc");
    pe_vs_tdc_1p->GetYaxis()->SetTitle("#PE");

    c7->cd(8);
    pe_vs_tdc_2p->Draw();
    pe_vs_tdc_2p->GetXaxis()->SetTitle("tdc");
    pe_vs_tdc_2p->GetYaxis()->SetTitle("#PE");

    // canvas 8
    TCanvas* c8=new TCanvas("c8","1D distributions of #PE and Q2",800,600);
    c8->Divide(2,3);

    c8->cd(1);
    light_vs_x1->Draw();
    light_vs_x1->GetYaxis()->SetTitle("#PE");
    light_vs_x1->GetXaxis()->SetTitle("X [cm]");

    c8->cd(2);
    light_vs_x2->Draw();
    light_vs_x2->GetYaxis()->SetTitle("#PE");
    light_vs_x2->GetXaxis()->SetTitle("X [cm]");

    c8->cd(3);
    q2_vs_x1->Draw();
    q2_vs_x1->GetYaxis()->SetTitle("Q2 [GeV/c^2]");
    q2_vs_x1->GetXaxis()->SetTitle("X [cm]");

    c8->cd(4);
    q2_vs_x2->Draw();
    q2_vs_x2->GetYaxis()->SetTitle("Q2 [GeV/c^2]");
    q2_vs_x2->GetXaxis()->SetTitle("X [cm]");

    c8->cd(5);
    weighted_q2_vs_x1->Draw("colz");
    weighted_q2_vs_x1->GetYaxis()->SetTitle("Q2 [GeV/c^2]");
    weighted_q2_vs_x1->GetXaxis()->SetTitle("X [cm]");

    c8->cd(6);
    weighted_q2_vs_x2->Draw();
    weighted_q2_vs_x2->GetYaxis()->SetTitle("Q2 [GeV/c^2]");
    weighted_q2_vs_x2->GetXaxis()->SetTitle("X [cm]");

    // canvas 9
    TCanvas* c9=new TCanvas("c9","1D distributions of #PE and Q2",800,600);
    c9->Divide(2,3);

    c9->cd(1);
    light_vs_y1->Draw();
    light_vs_y1->GetYaxis()->SetTitle("#PE");
    light_vs_y1->GetXaxis()->SetTitle("Y [cm]");

    c9->cd(2);
    light_vs_y2->Draw();
    light_vs_y2->GetYaxis()->SetTitle("#PE");
    light_vs_y2->GetXaxis()->SetTitle("Y [cm]");

    c9->cd(3);
    q2_vs_y1->Draw();
    q2_vs_y1->GetYaxis()->SetTitle("Q^{2} [GeV/c^{2}]");
    q2_vs_y1->GetXaxis()->SetTitle("Y [cm]");

    c9->cd(4);
    q2_vs_y2->Draw();
    q2_vs_y2->GetYaxis()->SetTitle("Q^{2} [GeV/c^{2}]");
    q2_vs_y2->GetXaxis()->SetTitle("Y [cm]");

    c9->cd(5);
    weighted_q2_vs_y1->Draw("colz");
    weighted_q2_vs_y1->GetYaxis()->SetTitle("Q^{2} [GeV/c^{2}]");
    weighted_q2_vs_y1->GetXaxis()->SetTitle("Y [cm]");

    c9->cd(6);
    weighted_q2_vs_y2->Draw();
    weighted_q2_vs_y2->GetYaxis()->SetTitle("Q^{2} [GeV/c^{2}]");
    weighted_q2_vs_y2->GetXaxis()->SetTitle("Y [cm]");

    // canvas 10
    TCanvas* c10=new TCanvas("c10","Reconstructed Momentum Distributions",1000,800);
    c10->Divide(2,3);

    c10->cd(1);
    histo_momentum[0]->Draw();
    histo_momentum[0]->GetXaxis()->SetTitle("Reconstructed Momentum [MeV]");

    c10->cd(3);
    histo_momentum[1]->Draw();
    histo_momentum[1]->GetXaxis()->SetTitle("Reconstructed Momentum [MeV]");

    c10->cd(5);
    histo_momentum[2]->Draw();
    histo_momentum[2]->GetXaxis()->SetTitle("Reconstructed Momentum [MeV]");

    c10->cd(2);
    q2_vs_momentum[0]->Draw();
    q2_vs_momentum[0]->GetXaxis()->SetTitle("Reconstructed Momentum [MeV]");
    q2_vs_momentum[0]->GetYaxis()->SetTitle("Q^{2} [GeV/c^{2}] [MeV]");

    c10->cd(4);
    q2_vs_momentum[1]->Draw();
    q2_vs_momentum[1]->GetXaxis()->SetTitle("Reconstructed Momentum [MeV]");
    q2_vs_momentum[1]->GetYaxis()->SetTitle("Q^{2} [GeV/c^{2}] [MeV]");

    c10->cd(6);
    q2_vs_momentum[2]->Draw();
    q2_vs_momentum[2]->GetXaxis()->SetTitle("Reconstructed Momentum [MeV]");
    q2_vs_momentum[2]->GetYaxis()->SetTitle("Q^{2} [GeV/c^{2}] [MeV]");

    // canvas 11
    TCanvas* c11=new TCanvas("c11","Track Projection: Hit Distributions in Collimator and Target",1200,800);
    c11->Divide(3,2);

    c11->cd(1);
    r2_proj[0]->Draw("colz");
    r2_proj[0]->GetXaxis()->SetTitle("x [cm]");
    r2_proj[0]->GetYaxis()->SetTitle("y [cm]");
    Draw_Collimator2_Frame(md_1);
    Draw_Collimator2_Frame(md_2);

    c11->cd(2);
    r2_proj[1]->Draw("colz");
    r2_proj[1]->GetXaxis()->SetTitle("x [cm]");
    r2_proj[1]->GetYaxis()->SetTitle("y [cm]");
    Draw_Collimator2_Frame(md_1);

    c11->cd(3);
    r2_proj[2]->Draw("colz");
    r2_proj[2]->GetXaxis()->SetTitle("x [cm]");
    r2_proj[2]->GetYaxis()->SetTitle("y [cm]");
    Draw_Collimator2_Frame(md_2);

    c11->cd(4);
    hit_tgt_window[0]->Draw("colz");
    hit_tgt_window[0]->GetXaxis()->SetTitle("x [cm]");
    hit_tgt_window[0]->GetYaxis()->SetTitle("y [cm]");
    Draw_Raster();

    c11->cd(5);
    hit_tgt_window[1]->Draw("colz");
    hit_tgt_window[1]->GetXaxis()->SetTitle("x [cm]");
    hit_tgt_window[1]->GetYaxis()->SetTitle("y [cm]");
    Draw_Raster();

    c11->cd(6);
    hit_tgt_window[2]->Draw("colz");
    hit_tgt_window[2]->GetXaxis()->SetTitle("x [cm]");
    hit_tgt_window[2]->GetYaxis()->SetTitle("y [cm]");
    Draw_Raster();

    // canvas 12
    TCanvas* c12=new TCanvas("c12","Vertex X-Y & 3D Distributions in Target",1200,800);
    c12->Divide(3,2);

    c12->cd(1);
    vertex_2D[0]->Draw("colz");
    vertex_2D[0]->GetXaxis()->SetTitle("vertex x [cm]");
    vertex_2D[0]->GetYaxis()->SetTitle("vertex y [cm]");
    Draw_Raster();

    c12->cd(2);
    vertex_2D[1]->Draw("colz");
    vertex_2D[1]->GetXaxis()->SetTitle("vertex x [cm]");
    vertex_2D[1]->GetYaxis()->SetTitle("vertex y [cm]");
    Draw_Raster();

    c12->cd(3);
    vertex_2D[2]->Draw("colz");
    vertex_2D[2]->GetXaxis()->SetTitle("vertex x [cm]");
    vertex_2D[2]->GetYaxis()->SetTitle("vertex y [cm]");
    Draw_Raster();

    c12->cd(4);
    vertex_dist[0]->Draw();
    vertex_dist[0]->GetXaxis()->SetTitle("vertex z [cm]");
    vertex_dist[0]->GetYaxis()->SetTitle("vertex x [cm]");
    vertex_dist[0]->GetZaxis()->SetTitle("vertex y [cm]");

    c12->cd(5);
    vertex_dist[1]->Draw();
    vertex_dist[1]->GetXaxis()->SetTitle("vertex z [cm]");
    vertex_dist[1]->GetYaxis()->SetTitle("vertex x [cm]");
    vertex_dist[1]->GetZaxis()->SetTitle("vertex y [cm]");

    c12->cd(6);
    vertex_dist[2]->Draw();
    vertex_dist[2]->GetXaxis()->SetTitle("vertex z [cm]");
    vertex_dist[2]->GetYaxis()->SetTitle("vertex x [cm]");
    vertex_dist[2]->GetZaxis()->SetTitle("vertex y [cm]");

    // canvas 13
    TCanvas* c13=new TCanvas("c13","Vertex X-Z & Y-Z Distributions in Target",1200,800);
    c13->Divide(3,2);

    c13->cd(1);
    vertex_2D_xz[0]->Draw("colz");
    vertex_2D_xz[0]->GetXaxis()->SetTitle("vertex z [cm]");
    vertex_2D_xz[0]->GetYaxis()->SetTitle("vertex x [cm]");

    c13->cd(2);
    vertex_2D_xz[1]->Draw("colz");
    vertex_2D_xz[1]->GetXaxis()->SetTitle("vertex z [cm]");
    vertex_2D_xz[1]->GetYaxis()->SetTitle("vertex x [cm]");

    c13->cd(3);
    vertex_2D_xz[2]->Draw("colz");
    vertex_2D_xz[2]->GetXaxis()->SetTitle("vertex z [cm]");
    vertex_2D_xz[2]->GetYaxis()->SetTitle("vertex x [cm]");

    c13->cd(4);
    vertex_2D_yz[0]->Draw("colz");
    vertex_2D_yz[0]->GetXaxis()->SetTitle("vertex z [cm]");
    vertex_2D_yz[0]->GetYaxis()->SetTitle("vertex y [cm]");

    c13->cd(5);
    vertex_2D_yz[1]->Draw("colz");
    vertex_2D_yz[1]->GetXaxis()->SetTitle("vertex z [cm]");
    vertex_2D_yz[1]->GetYaxis()->SetTitle("vertex y [cm]");

    c13->cd(6);
    vertex_2D_yz[2]->Draw("colz");
    vertex_2D_yz[2]->GetXaxis()->SetTitle("vertex z [cm]");
    vertex_2D_yz[2]->GetYaxis()->SetTitle("vertex y [cm]");

    // canvas 14
    TCanvas* c14=new TCanvas("c14","Chi-Distributions",1200,800);
    c14->Divide(3,2);

    c14->cd(1);
    front_pt_chi[0]->Draw ();
    front_pt_chi[0]->GetXaxis()->SetTitle("chi");

    c14->cd(2);
    front_pt_chi[1]->Draw ();
    front_pt_chi[1]->GetXaxis()->SetTitle("chi");
    
    c14->cd(3);
    front_pt_chi[2]->Draw ();
    front_pt_chi[2]->GetXaxis()->SetTitle("chi");

    c14->cd(4);
    back_pt_chi[0]->Draw ();
    back_pt_chi[0]->GetXaxis()->SetTitle("chi");

    c14->cd(5);
    back_pt_chi[1]->Draw ();
    back_pt_chi[1]->GetXaxis()->SetTitle("chi");

    c14->cd(6);
    back_pt_chi[2]->Draw ();
    back_pt_chi[2]->GetXaxis()->SetTitle("chi");
    
    // canvas for run conditions
    TCanvas* run_condition=new TCanvas("run_condition","Tracking Cut Run Conditions",800,800);
    TPaveText *condition_txt = new TPaveText(0.05,0.05,0.95,0.95);
    condition_txt->SetTextSize(0.025);
    condition_txt->AddText("Run Conditions:");
    //condition_txt->AddLine(.0,.5,1.,.5);
    condition_txt->SetTextAlign(12);
    condition_txt->AddText(Form("Run# %d, total events :%d, processed event# %d to %d",run, nevents,start,end));
 
   if(enable_tdc_cut){
      condition_txt->AddText(Form("package 1: tdc_cut_min[0]=%d, tdc_cut_max[0]=%d",tdc_cut_min[0], tdc_cut_max[0]));
      condition_txt->AddText(Form("package 2: tdc_cut_min[1]=%d, tdc_cut_max[1]=%d",tdc_cut_min[1],tdc_cut_max[1]));
    }
    if(enable_adc_cut){
      condition_txt->AddText(Form("num_pe_cut=%f",num_pe_cut));
    }
    if(enable_scattering_angle_cut){
      condition_txt->AddText(Form("scattering_angle_cut_min=%f, scattering_angle_cut_max=%f",scattering_angle_cut_min, scattering_angle_cut_max));
    }
    if(enable_vertex_z_cut){
      condition_txt->AddText(Form("vertex_z_cut_min=%f, vertex_z_cut_max=%f", vertex_z_cut_min,vertex_z_cut_max));
    }
    if(enable_vertex_r_cut){
      condition_txt->AddText(Form("vertex_r_cut_min=%f, vertex_r_cut_max=%f", vertex_r_cut_min,vertex_r_cut_max));
    }
    if(enable_position_theta_cut)
    {
      for(int pp=0; pp<2; pp++)
          condition_txt->AddText(Form("position_theta_cut_min%d=%f, position_theta_cut_max%d=%f",
                            pp,bending_angle_position_theta_cut_min[pp]/degree,
                            pp,bending_angle_position_theta_cut_max[pp]/degree));
    }
    if(enable_position_phi_cut)
    {
      for(int pp=0; pp<2; pp++)
          condition_txt->AddText(Form("position_phi_cut_min%d=%f, position_phi_cut_max%d=%f",
                            pp,bending_angle_position_phi_cut_min[pp]/degree,
                            pp,bending_angle_position_phi_cut_max[pp]/degree));
    }
    if(enable_direction_theta_cut)
    {
      for(int pp=0; pp<2; pp++)
          condition_txt->AddText(Form("direction_theta_cut_min%d=%f, direction_theta_cut_max%d=%f",
                            pp,bending_angle_direction_theta_cut_min[pp]/degree,
                            pp,bending_angle_direction_theta_cut_max[pp]/degree));
    }
    if(enable_direction_phi_cut)
    {
      for(int pp=0; pp<2; pp++)
          condition_txt->AddText(Form("direction_phi_cut_min%d=%f, direction_phi_cut_max%d=%f",
                            pp,bending_angle_direction_phi_cut_min[pp]/degree,
                            pp,bending_angle_direction_phi_cut_max[pp]/degree));
    }
    if(enable_position_r_off_cut){
      condition_txt->AddText(Form("position_r_off_cut_min=%f, position_r_off_cut_max=%f",position_r_off_cut_min,position_r_off_cut_max));
    }
    if(enable_hit_position_x_cut){
      condition_txt->AddText(Form("hit_position_x_cut_min=%f, hit_position_x_cut_max=%f", hit_position_x_cut_min, hit_position_x_cut_max));
    }
    if(enable_hit_position_y_cut){
      condition_txt->AddText(Form("hit_position_y_cut_min=%f, hit_position_y_cut_max=%f", hit_position_y_cut_min, hit_position_y_cut_max));
    }

    if ( enable_frontPTChi_cut )
    {
        condition_txt->AddText ( Form ( "num_of_frontPTChi_cut=%.2f", num_of_frontPTChi_cut ) );
    }
    if ( enable_backPTChi_cut )
    {
        condition_txt->AddText ( Form ( "num_of_backPTChi_cut=%.2f", num_of_backPTChi_cut ) );
    }
    if ( enable_R2PTChi_cut )
    {
        condition_txt->AddText ( Form ( "num_of_R2PTChi_cut=%.2f", num_of_R2PTChi_cut ) );
    }
    if ( enable_R3PTChi_cut )
    {
        condition_txt->AddText ( Form ( "num_of_R3PTChi_cut=%.2f", num_of_R3PTChi_cut ) );
    }
    condition_txt->Draw();

    TCanvas* run_summary=new TCanvas("run_summary","Tracking Cut Run Summary",800,800);
    TPaveText *summary_txt = new TPaveText(0.05,0.05,0.95,0.95);
    summary_txt->SetTextSize(0.015);
    summary_txt->AddText("Run Summary:");
    summary_txt->SetTextAlign(12);
    summary_txt->AddText(Form("Run# %d, total events :%d, processed event# %d to %d",run, nevents,start,end));

    int num_tracks = angle->GetEntries();
    int num_tracks1 = angle_1->GetEntries();
    int num_tracks2 = angle_2->GetEntries();
    if(num_tracks!=0)
    {
      summary_txt->AddText(Form("number of tracks: %d (all), %d (pkg1), %d (pkg2)", 
                           num_tracks, num_tracks1, num_tracks2));
      summary_txt->AddText(Form("scattering angle: %f +/- %f deg (all)", 
                           angle->GetMean(), angle->GetMeanError()));
      if(num_tracks1!=0)
      {
        summary_txt->AddText(Form("scattering angle: %f +/- %f deg (pkg1)", 
                             angle_1->GetMean(), angle_1->GetMeanError()));
      }
      if(num_tracks2!=0)
      {
        summary_txt->AddText(Form("scattering angle: %f +/- %f deg (pkg2)", 
                             angle_2->GetMean(), angle_2->GetMeanError()));
      }
    }

    if(num_tracks!=0)
    {
      summary_txt->AddText(Form("vertex z: %f +/- %f(rms) cm,  vertex r: %f +/- %f(rms) cm  (all)", 
                           histo_vertex_z[0]->GetMean(), histo_vertex_z[0]->GetRMS(), 
                           histo_vertex_r[0]->GetMean(), histo_vertex_r[0]->GetRMS() ));
      if(num_tracks1!=0)
      {
         summary_txt->AddText(Form("vertex z: %f +/- %f(rms) cm,  vertex r: %f +/- %f(rms) cm  (pkg1)", 
                              histo_vertex_z[1]->GetMean(), histo_vertex_z[1]->GetRMS(), 
                              histo_vertex_r[1]->GetMean(), histo_vertex_r[1]->GetRMS() ));
      }
      if(num_tracks2!=0)
      {
         summary_txt->AddText(Form("vertex z: %f +/- %f(rms) cm,  vertex r: %f +/- %f(rms) cm  (pkg2)",
                              histo_vertex_z[2]->GetMean(), histo_vertex_z[2]->GetRMS(),
                              histo_vertex_r[2]->GetMean(), histo_vertex_r[2]->GetRMS() ));
      }
    }

    if(q2->GetEntries()) 
    {
      summary_txt->AddText(Form("Q2: %f +/- %f (GeV/c)^2 (all)",
                           q2->GetMean(),q2->GetMeanError()));
      if(q2_1->GetEntries()!=0) 
      {
         summary_txt->AddText(Form("Q2: %f +/- %f (GeV/c)^2 (pkg1)",
                              q2_1->GetMean(),q2_1->GetMeanError()));
      }
      if(q2_2->GetEntries()!=0) 
      {
         summary_txt->AddText(Form("Q2: %f +/- %f (GeV/c)^2 (pkg2)",
                              q2_2->GetMean(),q2_2->GetMeanError()));
      }
    }

    if(light_weighted_q2[0]->GetEntries()) 
    {
      summary_txt->AddText(Form("Light-weighted Q2: %f +/- %f (GeV/c)^2 (all)",
                           light_weighted_q2[0]->GetMean(),
                           light_weighted_q2[0]->GetMeanError()));
      if(light_weighted_q2[1]->GetEntries()!=0) 
      {
         summary_txt->AddText(Form("Light-weighted Q2: %f +/- %f (GeV/c)^2 (pkg1)",
                              light_weighted_q2[1]->GetMean(),
                              light_weighted_q2[1]->GetMeanError()));
      }
      if(light_weighted_q2[2]->GetEntries()!=0) 
      {
         summary_txt->AddText(Form("Light-weighted Q2: %f +/- %f (GeV/c)^2 (pkg2)",
                              light_weighted_q2[2]->GetMean(),
                              light_weighted_q2[2]->GetMeanError()));
      }
    }

    summary_txt->Draw();
    
    //////////////////////////////////////
    // Save histograms to a file
    //////////////////////////////////////

    int sequence_num;
    TFile *hist_file; 

    for(sequence_num = 0; sequence_num <200; sequence_num++)
    {
      hist_file = new TFile(Form("TCRun_%d_%d.root",run,sequence_num),"READ");
      if(hist_file->IsZombie())
      {
        hist_file = new TFile(Form("TCRun_%d_%d.root",run,sequence_num),"CREATE");
        cout<<"Created output data file TCRun_"<<run<<"_"<<sequence_num<<".root"<<endl;
        break;
      }
      else
        hist_file->Close();
    }

    // save canvas
    run_condition->Write();
    run_summary->Write();
    c0->Write();
    c1->Write();
    c2->Write();
    c3->Write();
    c4->Write();
    c5->Write();
    c6->Write();
    c7->Write();
    c8->Write();
    c9->Write();
    c10->Write();
    c11->Write();
    c12->Write();
    c13->Write();
    c14->Write();

    //save histograms
    hit_dist1->Write();
    hit_dist2->Write();
    angle->Write();
    angle_1->Write();
    angle_2->Write();
    q2->Write();
    q2_1->Write();
    q2_2->Write();

    for(int i=0;i<=2;i++)
    {
        light_weighted_q2[i]->Write();
        histo_vertex_z[i]->Write();
        histo_vertex_r[i]->Write();
        histo_p0[i]->Write();
        histo_pp[i]->Write();
        eloss_vs_verz[i]->Write();
        histo_direction_phi_off[i]->Write();
        histo_position_phi_off[i]->Write();
        histo_direction_theta_off[i]->Write();
        histo_position_theta_off[i]->Write();
        histo_md1_pe[i]->Write();
        histo_md2_pe[i]->Write();
        histo_momentum[i]->Write();
        q2_vs_momentum[i]->Write();
        r2_proj[i]->Write();
        vertex_dist[i]->Write();
        vertex_2D[i]->Write();
        vertex_2D_xz[i]->Write();
        vertex_2D_yz[i]->Write();
        hit_tgt_window[i]->Write();
        front_pt_chi[i]->Write();
        back_pt_chi[i]->Write();
    }

    histo_tdc1m->Write();
    histo_tdc1p->Write();
    histo_tdc2m->Write();
    histo_tdc2p->Write();
    pe_vs_tdc_1m->Write();
    pe_vs_tdc_1p->Write();
    pe_vs_tdc_2m->Write();
    pe_vs_tdc_2p->Write();

    light_dist1->Write();
    light_dist2->Write();
    q2_dist1->Write();
    q2_dist2->Write();
    weighted_q2_dist1->Write();
    weighted_q2_dist2->Write();

    light_vs_x1->Write();
    light_vs_x2->Write();
    q2_vs_x1->Write();
    q2_vs_x2->Write();
    weighted_q2_vs_x1->Write();
    weighted_q2_vs_x2->Write();

    light_vs_y1->Write();
    light_vs_y2->Write();
    q2_vs_y1->Write();
    q2_vs_y2->Write();
    weighted_q2_vs_y1->Write();
    weighted_q2_vs_y2->Write();

    hist_file->Close();

    return;

 }

//----------------------------------------------------------------

int getOctNumber(TTree* event_tree)
{
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

double global2local_phi(double phi, int octant, int pkg)
{
   double pi = 3.1415927;
   if(reverse_run)
       octant = (octant+4)%8;
   if(pkg==1)
       octant = (octant+4)%8;
   if(debug)
       cout<<"-->rotated oct"<<octant<<" phi angle from "<<phi*180.0/pi<<" deg to ";

   double offset = (octant-1)*pi/4.0;
   if(phi<-(2.0*pi)/16.0)
      phi = phi+2.0*pi;
   phi = phi - offset;

   if(debug)
      cout<<phi*180.0/pi<<" deg"<<endl;

   return phi;
}

void getQdcPedestal(TTree* event_tree, int evt_start, int evt_end)
{
   for(int ii=1; ii<=8; ii++)
   {
      event_tree->Draw(Form("md%dm_adc>>m_adc",ii),
                       Form("CodaEventNumber>=%d && CodaEventNumber<=%d",evt_start,evt_end));
      event_tree->Draw(Form("md%dp_adc>>p_adc",ii),
                       Form("CodaEventNumber>=%d && CodaEventNumber<=%d",evt_start,evt_end));

      TF1 g_m("g_m","gaus", 1, 350);
      TF1 g_p("g_p","gaus", 1, 350);
      //g_m.SetParameters(1e6,200,10);
      //g_p.SetParameters(1e6,200,10);

      m_adc->Fit("g_m","R");
      p_adc->Fit("g_p","R");

      MDm_Pedestal[ii] = g_m.GetParameter(1);
      MDp_Pedestal[ii] = g_p.GetParameter(1);
    }

    cout<<"\nMain detector pedestals (minus, plus):"<<endl;
    for(int jj=1;jj<=8;jj++)
         cout<<"MD"<<jj<<"    "<<MDm_Pedestal[jj]<<"\t"<<MDp_Pedestal[jj]<<endl;
    cout<<endl;   
}

void getMatchingAngles(TTree* event_tree, int evt_start, int evt_end)
{
   TH1F* dir_theta = new TH1F("dir_theta","dir_theta",500, -5*degree, 5*degree);
   TH1F* dir_phi   = new TH1F("dir_phi","dir_phi",500, -15*degree, 15*degree);
   TH1F* pos_theta = new TH1F("pos_theta","dir_theta",500, -2*degree, 2*degree);
   TH1F* pos_phi   = new TH1F("pos_phi","pos_phi",500, -15*degree, 15*degree);

   for(int ipkg=1;ipkg<=2;ipkg++) 
   {
      event_tree->Draw("fQwTracks.fDirectionThetaoff>>dir_theta",
                    Form("CodaEventNumber>=%d && CodaEventNumber<=%d && events.fQwPartialTracks.fPackage==%d",
                    evt_start,evt_end,ipkg));

      event_tree->Draw("fQwTracks.fDirectionPhioff>>dir_phi",
                    Form("CodaEventNumber>=%d && CodaEventNumber<=%d && fQwTracks.fDirectionPhioff<3.14159/2.0 && fQwTracks.fDirectionPhioff>-3.14159/2.0 && events.fQwPartialTracks.fPackage==%d", evt_start,evt_end,ipkg));

      event_tree->Draw("fQwTracks.fPositionThetaoff>>pos_theta",
                    Form("CodaEventNumber>=%d && CodaEventNumber<=%d && events.fQwPartialTracks.fPackage==%d", evt_start,evt_end,ipkg));

      event_tree->Draw("fQwTracks.fPositionPhioff>>pos_phi",
                    Form("CodaEventNumber>=%d && CodaEventNumber<=%d && fQwTracks.fPositionPhioff<3.14159/2.0 && fQwTracks.fPositionPhioff>-3.14159/2.0 && events.fQwPartialTracks.fPackage==%d", evt_start,evt_end,ipkg));


      if(ipkg == 1)
      {
         position_theta_cut_mean1 = pos_theta->GetMean();
         position_theta_cut_rms1  = pos_theta->GetRMS();

         position_phi_cut_mean1 = pos_phi->GetMean();
         position_phi_cut_rms1  = pos_phi->GetRMS();

         direction_theta_cut_mean1 = dir_theta->GetMean();
         direction_theta_cut_rms1  = dir_theta->GetRMS();

         direction_phi_cut_mean1 = dir_phi->GetMean();
         direction_phi_cut_rms1  = dir_phi->GetRMS();

         cout<<"\nParameters of matching angle residuals in package 1:\n"<<endl;
         cout<<"position_theta_cut_mean1 = "<<position_theta_cut_mean1/degree<<" deg"<<endl;
         cout<<"position_theta_cut_rms1  = "<<position_theta_cut_rms1/degree <<" deg"<<endl;
         cout<<"position_phi_cut_mean1   = "<<position_phi_cut_mean1/degree<<" deg"<<endl;
         cout<<"position_phi_cut_rms1    = "<<position_phi_cut_rms1/degree <<" deg"<<endl;
         cout<<"direction_theta_cut_mean1= "<<direction_theta_cut_mean1/degree<<" deg"<<endl;
         cout<<"direction_theta_cut_rms1 = "<<direction_theta_cut_rms1/degree <<" deg"<<endl;
         cout<<"direction_phi_cut_mean1  = "<<direction_phi_cut_mean1/degree<<" deg"<<endl;
         cout<<"direction_phi_cut_rms1   = "<<direction_phi_cut_rms1/degree <<" deg"<<endl;
      }
      else
      {
         position_theta_cut_mean2 = pos_theta->GetMean();
         position_theta_cut_rms2  = pos_theta->GetRMS();

         position_phi_cut_mean2 = pos_phi->GetMean();
         position_phi_cut_rms2  = pos_phi->GetRMS();

         direction_theta_cut_mean2 = dir_theta->GetMean();
         direction_theta_cut_rms2  = dir_theta->GetRMS();

         direction_phi_cut_mean2 = dir_phi->GetMean();
         direction_phi_cut_rms2  = dir_phi->GetRMS();

         cout<<"\nParameters of matching angle residuals in package 2:\n"<<endl;
         cout<<"position_theta_cut_mean2 = "<<position_theta_cut_mean2/degree<<" deg"<<endl;
         cout<<"position_theta_cut_rms2  = "<<position_theta_cut_rms2/degree <<" deg"<<endl;
         cout<<"position_phi_cut_mean2   = "<<position_phi_cut_mean2/degree<<" deg"<<endl;
         cout<<"position_phi_cut_rms2    = "<<position_phi_cut_rms2/degree <<" deg"<<endl;
         cout<<"direction_theta_cut_mean2= "<<direction_theta_cut_mean2/degree<<" deg"<<endl;
         cout<<"direction_theta_cut_rms2 = "<<direction_theta_cut_rms2/degree <<" deg"<<endl;
         cout<<"direction_phi_cut_mean2  = "<<direction_phi_cut_mean2/degree<<" deg"<<endl;
         cout<<"direction_phi_cut_rms2   = "<<direction_phi_cut_rms2/degree <<" deg"<<endl;
      }
   }

   cout<<endl;

   bending_angle_position_theta_cut_min[0] = 
      position_theta_cut_mean1-num_of_sigma_position_theta_cut*position_theta_cut_rms1;
   bending_angle_position_theta_cut_max[0] =
       position_theta_cut_mean1+num_of_sigma_position_theta_cut*position_theta_cut_rms1;
   bending_angle_position_phi_cut_min[0] =
       position_phi_cut_mean1-num_of_sigma_position_phi_cut*position_phi_cut_rms1;
   bending_angle_position_phi_cut_max[0] =
       position_phi_cut_mean1+num_of_sigma_position_phi_cut*position_phi_cut_rms1;

   bending_angle_direction_theta_cut_min[0] =
       direction_theta_cut_mean1-num_of_sigma_direction_theta_cut*direction_theta_cut_rms1;
   bending_angle_direction_theta_cut_max[0] =
       direction_theta_cut_mean1+num_of_sigma_direction_theta_cut*direction_theta_cut_rms1;
   bending_angle_direction_phi_cut_min[0] =
       direction_phi_cut_mean1-num_of_sigma_direction_phi_cut*direction_phi_cut_rms1;
   bending_angle_direction_phi_cut_max[0] =
       direction_phi_cut_mean1+num_of_sigma_direction_phi_cut*direction_phi_cut_rms1;

   bending_angle_position_theta_cut_min[1] =
       position_theta_cut_mean2-num_of_sigma_position_theta_cut*position_theta_cut_rms2;
   bending_angle_position_theta_cut_max[1] =
       position_theta_cut_mean2+num_of_sigma_position_theta_cut*position_theta_cut_rms2;
   bending_angle_position_phi_cut_min[1] =
       position_phi_cut_mean2-num_of_sigma_position_phi_cut*position_phi_cut_rms2;
   bending_angle_position_phi_cut_max[1] =
       position_phi_cut_mean2+num_of_sigma_position_phi_cut*position_phi_cut_rms2;

   bending_angle_direction_theta_cut_min[1] =
       direction_theta_cut_mean2-num_of_sigma_direction_theta_cut*direction_theta_cut_rms2;
   bending_angle_direction_theta_cut_max[1] =
       direction_theta_cut_mean2+num_of_sigma_direction_theta_cut*direction_theta_cut_rms2;
   bending_angle_direction_phi_cut_min[1] =
       direction_phi_cut_mean2-num_of_sigma_direction_phi_cut*direction_phi_cut_rms2;
   bending_angle_direction_phi_cut_max[1] =
       direction_phi_cut_mean2+num_of_sigma_direction_phi_cut*direction_phi_cut_rms2;
   
}

void Draw_Collimator2_Frame(int Octant=1)
{
    double PI=3.1415926;
    double px[6];
    double py[6];

    double highy,highx,middley,middlex,lowy,lowx;
    highy=53.7,middley=37.1,lowy=30.57;
    highx=9.2,middlex=9.2,lowx=6.5;

    double angle;
    if(Octant==8)
      angle=135.0;
    else
      angle = (3-Octant)*45;

    double Sin=sin(angle*PI/180);
    double Cos=cos(angle*PI/180);

    px[0]=-Cos*highx+Sin*highy;
    py[0]=Sin*highx+Cos*highy;
    px[1]=Cos*highx+Sin*highy;
    py[1]=-Sin*highx+Cos*highy;
    px[2]=-Cos*lowx+Sin*lowy;
    py[2]=Sin*lowx+Cos*lowy;
    px[3]=Cos*lowx+Sin*lowy;
    py[3]=-Sin*lowx+Cos*lowy;
    px[4]=-Cos*middlex+Sin*middley;
    py[4]=Sin*middlex+Cos*middley;
    px[5]=Cos*middlex+Sin*middley;
    py[5]=-Sin*middlex+Cos*middley;


    TLine* t1=new TLine(px[0],py[0],px[1],py[1]);
    TLine* t2=new TLine(px[2],py[2],px[3],py[3]);
    TLine* t3=new TLine(px[0],py[0],px[4],py[4]);
    TLine* t4=new TLine(px[1],py[1],px[5],py[5]);
    TLine* t5=new TLine(px[4],py[4],px[2],py[2]);
    TLine* t6=new TLine(px[5],py[5],px[3],py[3]);
    t1->SetLineWidth(1);
    t2->SetLineWidth(1);
    t3->SetLineWidth(1);
    t4->SetLineWidth(1);
    t5->SetLineWidth(1);
    t6->SetLineWidth(1);
    t1->Draw("same");
    t2->Draw("same");
    t3->Draw("same");
    t4->Draw("same");
    t5->Draw("same");
    t6->Draw("same");
}

void Draw_Raster()
{
    double px[4];
    double py[4];

    px[0]=-0.2;
    py[0]=-0.2;
    px[1]= 0.2;
    py[1]=-0.2;
    px[2]= 0.2;
    py[2]= 0.2;
    px[3]=-0.2;
    py[3]= 0.2;

    TLine* t1=new TLine(px[0],py[0],px[1],py[1]);
    TLine* t2=new TLine(px[1],py[1],px[2],py[2]);
    TLine* t3=new TLine(px[2],py[2],px[3],py[3]);
    TLine* t4=new TLine(px[3],py[3],px[0],py[0]);
    t1->SetLineWidth(1);
    t2->SetLineWidth(1);
    t3->SetLineWidth(1);
    t4->SetLineWidth(1);
    t1->Draw("same");
    t2->Draw("same");
    t3->Draw("same");
    t4->Draw("same");
}
