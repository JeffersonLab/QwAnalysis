// QwSimTracking_Cut.C
//---------------------------------------------------------------
//
// Jie Pan, Fri May  3 15:27:55 CDT 2013, jpan@jlab.org
//
// This macro is used to look at the effects of different cuts to
// Q2 and scattering angle. Several typical runs are listed below:
//
//
// function:
// void QwSimTracking_Cut(int start=1,   // number of starting file: e.g. 1 for QwSim_1.root
//                        int end=1,     // number of ending file: e.g. 10 for QwSim_10.root
//                        int cs=0)      // cs=1: with cross-section weighting, cs=0: no weighting
//
// example: QwSimTracking_Cut(1,10,1)
//
//---------------------------------------------------------------
//
// update log:
//
// ---
// jpan, Fri May  3 15:27:55 CDT 2013
// Initial setup
//
// ---
// jpan, Thu May 16 13:56:06 CDT 2013
// Added in track rejection flag. Filling histogram only when rejection flag is false.
// Note that the filling of histograms is in event loop other than the track loop. As
// such, only the last track in an event is filled. The multiple track issue needs to
// be further investigated.
//
// ---
// jpan, Fri May 24 10:52:32 CDT 2013
// Changed the the error calculation for scattering angle and Q2 to take into account
// the case of using cross-section weight
//
// ---
// jpan, Fri Aug 30 11:44:44 CDT 2013
// Added in partial-track-level chi cuts and track-level chi cuts
// also added in number of PE cut and PE weighting
// 
#include <iostream>
#include <iomanip>

// cut enable flags
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
bool enable_momentum_cut         = false;
bool enable_PE_cut               = true;
bool enable_frontPTChi_cut       = false;
bool enable_backPTChi_cut        = false;
bool enable_R2PTChi_cut          = true;
bool enable_R3PTChi_cut          = false; 

// cut values
const double degree = 3.1415927/180.0;

double num_pe_cut = 1.0;

double num_of_frontPTChi_cut = 15; // track-level chi cut
double num_of_backPTChi_cut = 15;

double num_of_R2PTChi_cut = 10;  // partial-track-level chi cut 
double num_of_R3PTChi_cut = 15;

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

double position_theta_cut_mean, position_theta_cut_rms;
double position_phi_cut_mean, position_phi_cut_rms;
double direction_theta_cut_mean, direction_theta_cut_rms;
double direction_phi_cut_mean, direction_phi_cut_rms;

double bending_angle_position_theta_cut_min;
double bending_angle_position_theta_cut_max;
double bending_angle_position_phi_cut_min;
double bending_angle_position_phi_cut_max;
double bending_angle_direction_theta_cut_min;
double bending_angle_direction_theta_cut_max;
double bending_angle_direction_phi_cut_min;
double bending_angle_direction_phi_cut_max;

double position_r_off_cut_min = -1.0;
double position_r_off_cut_max = 1.0;

double hit_position_x_cut_min = ( 344-9.0 ) -10.0; // add in 10 cm margin
double hit_position_x_cut_max = ( 344+9.0 ) +10.0;
double hit_position_y_cut_min = -120;
double hit_position_y_cut_max = 120;

double momentum_min = 0.0;
double momentum_max = 2000.0;  // unit: MeV

// parameters

// main detector z location
//double md_zpos[9] = {0.0, 581.665, 576.705, 577.020, 577.425, 582.515, 577.955, 577.885, 577.060};
double md_zpos[9] = {0.0, 577.410, 577.415, 577.710, 578.080, 578.240, 578.615, 578.515, 577.730};//MD survey data on Oct 2012

// rejection flag
bool rejection;

// set debug flag to false to disable printing debug info
bool debug = false;

// set verbose level: 0 (no verbose), 1, 2, 3, ...
int verbose = 0;

//==================================

bool scattering_angle_cut ( double val )
{
    if ( val> scattering_angle_cut_min && val< scattering_angle_cut_max )
        return true;
    else
    {
        rejection = true;
        if ( debug )
            cout<<"rejected, scattering_angle_cut, theta_angle="<<val<<endl;
        return false;
    }
}

bool  vertex_z_cut ( double val )
{
    if ( val> vertex_z_cut_min && val< vertex_z_cut_max )
        return true;
    else
    {
        rejection = true;
        if ( debug )
            cout<<"rejected, vertex_z_cut, vertex_z="<<val<<endl;
        return false;
    }
}

bool  vertex_r_cut ( double val )
{
    if ( val> vertex_r_cut_min && val< vertex_r_cut_max )
        return true;
    else
    {
        rejection = true;
        if ( debug )
            cout<<"rejected, vertex_r_cut, vertex_r="<<val<<endl;
        return false;
    }
}

bool  momentum_cut ( double val )
{
    if ( val> momentum_min && val< momentum_max )
        return true;
    else
    {
        rejection = true;
        if ( debug )
            cout<<"rejected, momentum_cut, momentum="<<val<<endl;
        return false;
    }
}

bool  bending_angle_position_theta_cut ( double val )
{
    if ( val> bending_angle_position_theta_cut_min && val< bending_angle_position_theta_cut_max )
        return true;
    else
    {
        rejection = true;
        if ( debug )
            cout<<"rejected, bending_angle_position_theta_cut, position_theta_off="<<val<<endl;
        return false;
    }
}

bool  bending_angle_position_phi_cut ( double val )
{
    if ( val> bending_angle_position_phi_cut_min && val< bending_angle_position_phi_cut_max )
        return true;
    else
    {
        rejection = true;
        if ( debug )
            cout<<"rejected, bending_angle_position_phi_cut, position_phi_off="<<val<<endl;
        return false;
    }
}

bool  bending_angle_direction_theta_cut ( double val )
{
    if ( val> bending_angle_direction_theta_cut_min && val< bending_angle_direction_theta_cut_max )
        return true;
    else
    {
        rejection = true;
        if ( debug )
            cout<<"rejected, bending_angle_direction_theta_cut, direction_theta_off="<<val<<endl;
        return false;
    }
}

bool  bending_angle_direction_phi_cut ( double val )
{
    if ( val > bending_angle_direction_phi_cut_min && val < bending_angle_direction_phi_cut_max )
        return true;
    else
    {
        rejection = true;
        if ( debug )
            cout<<"rejected, bending_angle_direction_phi_cut, direction_phi_off="<<val<<endl;
        return false;
    }
}

bool  position_resolution_cut ( double val )
{
    if ( val> position_r_off_cut_min && val< position_r_off_cut_max )
        return true;
    else
    {
        rejection = true;
        if ( debug )
            cout<<"rejected, position_resolution_cut, position_r_off="<<val<<endl;
        return false;
    }
}

bool  hit_position_x_cut ( double val )
{
    if ( fabs ( val ) > hit_position_x_cut_min && fabs ( val ) < hit_position_x_cut_max )
        return true;
    else
    {
        rejection = true;
        if ( debug )
            cout<<"rejected, hit_position_x_cut, x="<<val<<endl;
        return false;
    }
}

bool  hit_position_y_cut ( double val )
{
    if ( fabs ( val ) > hit_position_y_cut_min && fabs ( val ) < hit_position_y_cut_max )
        return true;
    else
    {
        rejection = true;
        if ( debug )
            cout<<"rejected, hit_position_y_cut, y="<<val<<endl;
        return false;
    }
}

bool PE_cut ( double val)
{
   if (val> num_pe_cut)
        return true;
    else
    {
        rejection = true;
        if ( debug )
            cout<<"rejected, PE_cut, PE="<<val<<endl;
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

void QwSimTracking_Cut ( int start=1,int end=1, int cs=0 )
{
    gROOT->SetStyle ( "Plain" );
    gStyle->SetPalette ( 1 );
    //gStyle->SetOptStat(0);

    // define histograms
    TH2F* hit_dist = new TH2F ( "hit_dist",Form ( "QwSimRun %d-%d, Hits Distribution",start,end ),480,-120,120,100,310,360 );
    TH1F* angle = new TH1F ( "theta",Form ( "QwSimRun %d-%d, Scattering Angle Distribution",start,end ),400,3,14 );
    TH1F* q2 = new TH1F ( "q2",Form ( "QwSimRun %d-%d, Q2 Distribution",start,end ),400,0,0.08 );


    //define extra histograms
    TH2F*  r2_proj;
    r2_proj = new TH2F ( "r2_proj",Form ( "QwSimRun %d-%d, Hits Distribution Collimator",start,end ),480,-60,60,400,-60,60 );

    TH3F*  vertex_dist;
    vertex_dist = new TH3F ( "vertex_dist",Form ( "QwSimRun %d-%d, Vertex Distribution",start,end ),200,-750,-550,100,-10,10,100,-10,10 );

    TH2F*  vertex_2D;
    vertex_2D = new TH2F ( "vertex_2D_xy",Form ( "QwSimRun %d-%d, Vertex Distribution",start,end ),240,-3,3,240,-3,3 );

    TH2F*  vertex_2D_xz;
    vertex_2D_xz = new TH2F ( "vertex_2D_xz",Form ( "QwSimRun %d-%d, Vertex Distribution",start,end ),400,-750,-550,240,-3,3 );

    TH2F*  vertex_2D_yz;
    vertex_2D_yz = new TH2F ( "vertex_2D_yz",Form ( "QwSimRun %d-%d, Vertex Distribution",start,end ),400,-750,-550,240,-3,3 );

    TH2F*  hit_tgt_window;
    hit_tgt_window = new TH2F ( "hit_tgt_window",Form ( "QwSimRun %d-%d, Hit Distribution  in Downstream Target Window",start,end ),400,-10,10,400,-10,10 );


    TH1F* histo_vertex_r;
    histo_vertex_r = new TH1F ( "histo_vertex_r_all",Form ( "QwSimRun %d-%d, Vertex R",start,end ),8000,0,70 );

    TH1F* histo_vertex_z;
    histo_vertex_z = new TH1F ( "histo_vertex_z_all",Form ( "QwSimRun %d-%d, Vertex Z",start,end ),8000,-1500,-1 );

    TH1F* histo_momentum;
    histo_momentum = new TH1F ( "histo_momentum_all",Form ( "QwSimRun %d-%d, Reconstructed Momentum",start,end ),8000,0,2000 );

    TH2F* q2_vs_momentum;
    q2_vs_momentum = new TH2F ( "q2_vs_momentum_all",Form ( "QwSimRun %d-%d, Q2 vs Reconstructed Momentum",start,end ),8000,0,2000,400,0,0.08 );

    TH1F* histo_p0;
    histo_p0 = new TH1F ( "histo_P0_all",Form ( "QwSimRun %d-%d, P0",start,end ),400,1.0,1.2 );

    TH1F* histo_pp;
    histo_pp = new TH1F ( "histo_Pp[0]_all",Form ( "QwSimRun %d-%d, Pp",start,end ),400,1.0,1.2 );

    TH2F* eloss_vs_verz;
    eloss_vs_verz = new TH2F ( "eloss_vs_verx_all",Form ( "QwSimRun %d-%d, Eloss vs. vertex Z",start,end ),400,-680,-620,400,0,24 );

    TH1F* histo_direction_phi_off;
    histo_direction_phi_off = new TH1F ( "histo_direction_phi_off_all",Form ( "QwSimRun %d-%d, Direction_Phi_Off",start,end ),400,-2/degree,2/degree );

    TH1F* histo_position_phi_off;
    histo_position_phi_off = new TH1F ( "histo_position_phi_off_all",Form ( "QwSimRun %d-%d, Position_Phi_Off",start,end ),400,-1/degree,1/degree );

    TH1F* histo_direction_theta_off;
    histo_direction_theta_off = new TH1F ( "histo_direction_theta_off_all",Form ( "QwSimRun %d-%d, Direction_Theta_Off",start,end ),400,-0.4/degree,0.4/degree );

    TH1F* histo_md_pe;
    histo_md_pe = new TH1F ( "histo_md_pe",Form ( "QwSimRun %d-%d, MD_PE",start,end ),500,0,500); 

    TH1F* light_weighted_q2 = new TH1F ( "light_weighted_q2",Form("QwSimRun %d-%d, Light-Weighted Q2 distribution",start,end ),400,0,0.08);

    TH1F* histo_position_theta_off;
    histo_position_theta_off = new TH1F ( "histo_position_theta_off_all",Form ( "QwSimRun %d-%d, Position_Theta_Off",start,end ),400,-0.003/degree,0.003/degree );

    TProfile2D* q2_dist = new TProfile2D ( "q2_dist",Form ( "QwSimRun %d-%d, Q2 Distribution",start,end ),480,-120,120,100,310,360 );
    TProfile* q2_vs_x = new TProfile ( "q2_vs_x",Form ( "QwSimRun %d-%d, Q2 vs. X",start,end ),240,-120,120 );
    TProfile* q2_vs_y = new TProfile ( "q2_vs_y",Form ( "QwSimRun %d-%d, Q2 vs. Y",start,end ),120,335-20,335+20 );
    
    TH1F* front_pt_chi;
    front_pt_chi = new TH1F ( "front_pt_chi",Form ( "QwSimRun %d-%d, Front Partial Track Chi-distribution",start,end ),400,0,40 );
    
    TH1F* back_pt_chi;
    back_pt_chi = new TH1F ( "back_pt_chi",Form ( "QwSimRun %d-%d, Back Partial Track Chi-distribution",start,end ),400,0,40 );

    TString Path = "$QW_ROOTFILES/"; // qwsimtracking-analyzed events

    QwEvent* fEvent     = 0;
    QwEvent* fOriginals = 0;
    QwTrack* track      = 0;
    QwPartialTrack* pt  = 0;

    //define canvas for drawing histograms
    TCanvas* c0=new TCanvas ( "c0","Q2 and scattering angle distributions",800,300 );

    for ( Int_t ifile = start; ifile <= end; ifile++ )
    {
        TFile* rootfile = new TFile ( Path+Form ( "QwSim_%i.root", ifile ),"r" );
        cout<<"\nOpened file: "<<Path+Form ( "QwSim_%i.root", ifile ) <<endl;

        TTree* event_tree = ( TTree* ) gROOT->FindObject ( "event_tree" );

        TBranch* event_branch=event_tree->GetBranch ( "events" );
        event_branch->SetAddress ( &fEvent );

        TBranch* original_branch=event_tree->GetBranch ( "originals" );
        original_branch->SetAddress ( &fOriginals );

        //Get the oct number
        int oct=getOctNumber ( event_tree );
        int md_1= ( oct+4 ) %8;
        int md_2=oct;

        //Get parameters of matching angle residuals
        getMatchingAngles ( event_tree );

        // Fetch events from tree
        int num_of_events = event_tree->GetEntries();
        for ( int i=0; i<num_of_events; i++ )   // start to loop over all events
        {
            if ( i%5000==0 && i>0)
                cout << "events processed so far: " << i << endl;

            event_tree->GetEntry ( i );
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
            if(Q2_val<=0.01 || Q2_val>=0.08)
                continue;

            double ntracks= fEvent->GetNumberOfTracks();
            // those filters are related with the quality of matching of r2 and r3 tracks

            int nhits=fEvent->GetNumberOfHits();

            double xsect = ( cs==1 ) ? ( fOriginals->fCrossSection ) : 1.0;

            //----------------------
            // scattering angle cut
            //----------------------
            double angle_val=fEvent->GetScatteringAngle();
            if ( enable_scattering_angle_cut )
            {
                if ( ! scattering_angle_cut ( angle_val ) )
                    continue;
            }

            //----------------------
            // scattering vertex cut
            //----------------------

            double vertex_z = fEvent->fScatteringVertexZ;
            if ( enable_vertex_z_cut )
            {
                if ( ! vertex_z_cut ( vertex_z ) )
                    continue;
            }

            double vertex_r = fEvent->fScatteringVertexR;
            if ( enable_vertex_r_cut )
            {
                if ( ! vertex_r_cut ( vertex_r ) )
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

            for ( int j=0; j<ntracks; j++ )
            {
                track=fEvent->GetTrack ( j );
                pt = track->fBack;
                if ( pt->GetRegion() ==3 )
                {
                    dz = md_zpos[oct];
                    xoffset = pt->fOffsetX;
                    yoffset = pt->fOffsetY;
                    xslope  = pt->fSlopeX;
                    yslope  = pt->fSlopeY;
                    x       = xoffset+xslope*dz;
                    y       = yoffset+yslope*dz;

                    if ( debug )
                    {
                        cout<<"event#"<<i<<", hit at ( "<<x<<","<<y<<" ) "<<endl;
                    }

                    global2local ( &x,&y,oct );

                    if ( enable_hit_position_x_cut )
                    {
                        if ( ! hit_position_x_cut ( x ) )
                            continue;
                    }

                    if ( enable_hit_position_y_cut )
                    {
                        if ( ! hit_position_y_cut ( y ) )
                            continue;
                    }
                }

                pt = track->fFront;
                if ( pt->GetRegion() ==2 )
                {
                    double collimator2_z=-370.719;
                    double tgt_downstream_window_z = -635.0;
                    double r2_xoffset = pt->fOffsetX;
                    double r2_yoffset = pt->fOffsetY;
                    double r2_xslope  = pt->fSlopeX;
                    double r2_yslope  = pt->fSlopeY;
                    r2_x = r2_xoffset+r2_xslope*collimator2_z;
                    r2_y = r2_yoffset+r2_yslope*collimator2_z;
                    vertex_x = r2_xoffset+r2_xslope*vertex_z;
                    vertex_y = r2_yoffset+r2_yslope*vertex_z;

                    tgt_win_x = r2_xoffset+r2_xslope*tgt_downstream_window_z;
                    tgt_win_y = r2_yoffset+r2_yslope*tgt_downstream_window_z;
                }

                //----------------------
                // bending angle cut
                //----------------------

                double direction_theta_off = track->fDirectionThetaoff;
                double position_theta_off = track->fPositionThetaoff;

                double phi[4];
                phi[0] = global2local_phi ( track->fEndDirectionActual.Phi(), oct );
                phi[1] = global2local_phi ( track->fEndDirectionGoal.Phi(), oct );
                phi[2] = global2local_phi ( track->fEndPositionActual.Phi(), oct );
                phi[3] = global2local_phi ( track->fEndPositionGoal.Phi(), oct );
                double direction_phi_off = phi[0]-phi[1];
                double position_phi_off = phi[2]-phi[3];

                if ( debug )
                {
                    cout<<"direction_theta_off = "<<direction_theta_off/degree<<" deg"<<endl;
                    cout<<"position_theta_off = "<<position_theta_off/degree<<" deg"<<endl;
                    cout<<"direction_phi_off = "<<direction_phi_off/degree<<" deg"<<endl;
                    cout<<"position_phi_off = "<<position_phi_off/degree<<" deg"<<endl;
                }

                // mathching angle cuts
                if ( enable_direction_theta_cut )
                {
                    if ( ! bending_angle_direction_theta_cut ( direction_theta_off ) )
                        continue;
                }

                if ( enable_direction_phi_cut )
                {
                    if ( ! bending_angle_direction_phi_cut ( direction_phi_off ) )
                        continue;
                }

                if ( enable_position_theta_cut )
                {
                    if ( ! bending_angle_position_theta_cut ( position_theta_off ) )
                        continue;
                }

                if ( enable_position_phi_cut )
                {
                    if ( ! bending_angle_position_phi_cut ( position_phi_off ) )
                        continue;
                }


                //---------------------------------
                // matching position resolution cut
                //---------------------------------

                if ( enable_position_r_off_cut )
                {
                    if ( ! position_resolution_cut ( track->fPositionRoff ) )
                        continue;
                }

                //----------------------------
                // reconstructed momentum cut
                //----------------------------

                double momentum = track->fMomentum;
                if ( enable_momentum_cut )
                {
                    if ( ! momentum_cut ( momentum ) )
                        continue;
                }

                               // obtain additional parameters
                double Eloss  = fEvent->fHydrogenEnergyLoss;
                double P0_val = fEvent->fKinElasticWithLoss.fP0;
                double Pp_val = fEvent->fKinElasticWithLoss.fPp;

	  
           } // end of loop over tracks
                
           //  main detector PE cut     
           if ( enable_PE_cut )
           {
              vector<float> PEs;
              float PE_val = 0;
              int PE_vec_size = fEvent->fMD_TotalNbOfPEs.size();
              PEs.resize(PE_vec_size);
              for (int n=0; n<PE_vec_size; n++)
              {
                 PEs[n] = fEvent->fMD_TotalNbOfPEs.at(n);
                 if( n>0 )
                    cout<<"Warning: multiple detector hits, MD_PE["<<n<<"]: "<<PEs[n]<<endl;
                 if(PE_val<PEs[n])
                    PE_val = PEs[n];  // taking the maximum number of PEs
              }

              if ( ! PE_cut ( PE_val ) )
                    continue;
           }

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

           // note: filling histograms out of track-loop eliminate multiple tracks
           // in one event, e.g. only the last track is counted

            if (rejection == false)
            {
                angle->Fill ( angle_val,xsect );
                q2->Fill ( Q2_val,xsect );
                hit_dist->Fill ( y,x,xsect );

                // fill additional histograms
 
                histo_md_pe->Fill ( PE_val,xsect );

                light_weighted_q2->Fill ( Q2_val,PE_val*xsect);

                histo_vertex_r->Fill ( vertex_r,xsect );
                histo_vertex_z->Fill ( vertex_z,xsect );

                histo_momentum->Fill ( momentum,xsect );
                q2_vs_momentum->Fill ( momentum,Q2_val,xsect );

                histo_p0->Fill ( P0_val,xsect );
                histo_pp->Fill ( Pp_val,xsect );

                eloss_vs_verz->Fill ( vertex_z,Eloss,xsect );

                histo_direction_phi_off->Fill ( direction_phi_off/degree,xsect );
                histo_position_phi_off->Fill ( position_phi_off/degree,xsect );

                histo_direction_theta_off->Fill ( direction_theta_off/degree,xsect );
                histo_position_theta_off->Fill ( position_theta_off/degree,xsect );

                r2_proj->Fill ( r2_x,r2_y,xsect );

                vertex_dist->Fill ( vertex_z,vertex_x,vertex_y,xsect );

                vertex_2D->Fill ( vertex_x,vertex_y,xsect );

                vertex_2D_xz->Fill ( vertex_z,vertex_x,xsect );
                vertex_2D_yz->Fill ( vertex_z,vertex_y,xsect );

                hit_tgt_window->Fill ( tgt_win_x,tgt_win_y,xsect );

                q2_dist->Fill ( y,x,Q2_val,xsect );
		
		front_pt_chi->Fill (frontPTChi_val,xsect );
		back_pt_chi->Fill (backPTChi_val,xsect );
             }

        }  // end of loop over all events
    }// end of loop over files: for (Int_t ifile = start; ifile <= end; ifile++)

    int num_of_valid_tracks = angle->GetEntries();

    cout << "\n**********************************"<<endl;
    cout <<"QwSim Run#: "<<start<<" - "<<end<<endl;
    cout << "number of tracks: " << num_of_valid_tracks << endl;

    if ( num_of_valid_tracks )
    {
        cout << "scattering angle: " << setprecision ( 5 ) << angle->GetMean() << " +/- "
             << setprecision ( 3 ) << angle->GetMeanError() << " deg"<<endl;
    }

    if ( q2->GetEntries() )
    {
        cout << "Q2: "<< setprecision ( 5 ) << q2->GetMean() << " +/- "
             <<  setprecision ( 3 ) << q2->GetMeanError() << " ( GeV/c ) ^2"<<endl<<endl;
    }

    // Draw histograms
    gStyle->SetPalette ( 1 );
    c0->Clear();

    // canvas 0
    c0->Divide ( 2,1 );

    c0->cd ( 1 );
    angle->Draw();
    angle->GetXaxis()->SetTitle ( "Scattering Angle [degree]" );

    c0->cd ( 2 );
    q2->Draw();
    q2->GetXaxis()->SetTitle ( "Q2 [ ( GeV/c ) ^2]" );

    // draw additional plots

    // canvas 1
    TCanvas* c1=new TCanvas ( "c1","scattering vertex distributions",800,300 );
    c1->Divide ( 2,1 );

    c1->cd ( 1 );
    c1_1->SetLogy();
    histo_vertex_z->Draw();
    histo_vertex_z->GetXaxis()->SetTitle ( "Vertex Z [cm]" );

    c1->cd ( 2 );
    c1_2->SetLogy();
    histo_vertex_r->Draw();
    histo_vertex_r->GetXaxis()->SetTitle ( "Vertex R [cm]" );

    // canvas 2
    TCanvas* c2 = new TCanvas ( "c2","momentum distributions",1200,300 );
    c2->Divide ( 3,1 );

    c2->cd ( 1 );
    //c2_1->SetLogy();
    histo_p0->Draw();
    histo_p0->GetXaxis()->SetTitle ( "P0 [GeV]" );

    c2->cd ( 2 );
    //c2_2->SetLogy();
    histo_pp->Draw();
    histo_pp->GetXaxis()->SetTitle ( "Pp [GeV]" );

    c2->cd ( 3 );
    eloss_vs_verz->Draw();
    eloss_vs_verz->GetXaxis()->SetTitle ( "Vertex Z [cm]" );
    eloss_vs_verz->GetYaxis()->SetTitle ( "Eloss [MeV]" );

    // canvas 3
    TCanvas* c3 = new TCanvas ( "c3","azimuthal angle residuals",800,300 );
    c3->Divide ( 2,1 );

    c3->cd ( 1 );
    c3_1->SetLogy();
    histo_direction_phi_off->Draw();
    histo_direction_phi_off->GetXaxis()->SetTitle ( "direction_phi_off [deg]" );

    c3->cd ( 2 );
    c3_2->SetLogy();
    histo_position_phi_off->Draw();
    histo_position_phi_off->GetXaxis()->SetTitle ( "position_phi_off [deg]" );

    // canvas 4
    TCanvas* c4 = new TCanvas ( "c4","radial angle residuals",800,300 );
    c4->Divide ( 2,1 );

    c4->cd ( 1 );
    c4_1->SetLogy();
    histo_direction_theta_off->Draw();
    histo_direction_theta_off->GetXaxis()->SetTitle ( "direction_theta_off [deg]" );

    c4->cd ( 2 );
    c4_2->SetLogy();
    histo_position_theta_off->Draw();
    histo_position_theta_off->GetXaxis()->SetTitle ( "position_theta_off [deg]" );

    // canvas 5
    TCanvas* c5 = new TCanvas ( "c5","Reconstructed Momentum Distributions",800,300 );
    c5->Divide ( 2,1 );

    c5->cd ( 1 );
    histo_momentum->Draw();
    histo_momentum->GetXaxis()->SetTitle ( "Reconstructed Momentum [MeV]" );

    c5->cd ( 2 );
    q2_vs_momentum->Draw();
    q2_vs_momentum->GetXaxis()->SetTitle ( "Reconstructed Momentum [MeV]" );
    q2_vs_momentum->GetYaxis()->SetTitle ( "Q^ {2} [GeV/c^ {2}] [MeV]" );

    // canvas 6
    TCanvas* c6 = new TCanvas ( "c6","Track Projection: Hit Distributions in Collimator and Target",800,300 );
    c6->Divide ( 2,1 );

    c6->cd ( 1 );
    r2_proj->Draw ( "colz" );
    r2_proj->GetXaxis()->SetTitle ( "x [cm]" );
    r2_proj->GetYaxis()->SetTitle ( "y [cm]" );
    Draw_Collimator2_Frame ( md_1 );
    Draw_Collimator2_Frame ( md_2 );

    c6->cd ( 2 );
    hit_tgt_window->Draw ( "colz" );
    hit_tgt_window->GetXaxis()->SetTitle ( "x [cm]" );
    hit_tgt_window->GetYaxis()->SetTitle ( "y [cm]" );
    Draw_Raster();

    // canvas 7
    TCanvas* c7=new TCanvas ( "c7","Vertex X-Y & 3D Distributions in Target",800,300 );
    c7->Divide ( 2,1 );

    c7->cd ( 1 );
    vertex_2D->Draw ( "colz" );
    vertex_2D->GetXaxis()->SetTitle ( "vertex x [cm]" );
    vertex_2D->GetYaxis()->SetTitle ( "vertex y [cm]" );
    Draw_Raster();

    c7->cd ( 2 );
    vertex_dist->Draw();
    vertex_dist->GetXaxis()->SetTitle ( "vertex z [cm]" );
    vertex_dist->GetYaxis()->SetTitle ( "vertex x [cm]" );
    vertex_dist->GetZaxis()->SetTitle ( "vertex y [cm]" );

    // canvas 8
    TCanvas* c8=new TCanvas ( "c8","Vertex X-Z & Y-Z Distributions in Target",800,300 );
    c8->Divide ( 2,1 );

    c8->cd ( 1 );
    vertex_2D_xz->Draw ( "colz" );
    vertex_2D_xz->GetXaxis()->SetTitle ( "vertex z [cm]" );
    vertex_2D_xz->GetYaxis()->SetTitle ( "vertex x [cm]" );

    c8->cd ( 2 );
    vertex_2D_yz->Draw ( "colz" );
    vertex_2D_yz->GetXaxis()->SetTitle ( "vertex z [cm]" );
    vertex_2D_yz->GetYaxis()->SetTitle ( "vertex y [cm]" );

    // canvas 9
    TCanvas* c9=new TCanvas ( "c9","Light-weighted Q2 Distribution",800,300 );

    c9->Divide ( 2,1 );

    c9->cd ( 1 );
    histo_md_pe->Draw();
    histo_md_pe->GetXaxis()->SetTitle("Total number of PE");

    c9->cd ( 2 );
    light_weighted_q2->Draw();
    light_weighted_q2->GetXaxis()->SetTitle("Light-weighted Q2 [(GeV/c)^2]");
    
    // canvas 10
    TCanvas* c10=new TCanvas ( "c10","Partial Track Chi-Distribution",800,300 );
    
    c10->Divide ( 2,1);

    c10->cd ( 1 );
    front_pt_chi->Draw();
    front_pt_chi->GetXaxis()->SetTitle("Front Chi");
    
    c10->cd ( 2 );
    back_pt_chi->Draw();
    back_pt_chi->GetXaxis()->SetTitle("Back Chi");
    
    // canvas for run conditions
    TCanvas* run_condition=new TCanvas ( "run_condition","Tracking Cut Run Conditions",600,400 );
    TPaveText *condition_txt = new TPaveText ( 0.05,0.05,0.95,0.95 );
    condition_txt->SetTextSize ( 0.035 );
    condition_txt->AddText ( "Run Conditions:" );
    //condition_txt->AddLine(.0,.5,1.,.5);
    condition_txt->SetTextAlign ( 12 );
    condition_txt->AddText ( Form ( "QwSimRun# %d-%d, total events :%d",start,end,num_of_valid_tracks ) );

    if ( enable_scattering_angle_cut )
    {
        condition_txt->AddText ( Form ( "scattering_angle_cut_min=%f, scattering_angle_cut_max=%f",
                                        scattering_angle_cut_min,
                                        scattering_angle_cut_max ) );
    }
    if ( enable_vertex_z_cut )
    {
        condition_txt->AddText ( Form ( "vertex_z_cut_min=%f, vertex_z_cut_max=%f", vertex_z_cut_min,vertex_z_cut_max ) );
    }
    if ( enable_vertex_r_cut )
    {
        condition_txt->AddText ( Form ( "vertex_r_cut_min=%f, vertex_r_cut_max=%f", vertex_r_cut_min,vertex_r_cut_max ) );
    }
    if ( enable_position_theta_cut )
    {
        condition_txt->AddText ( Form ( "position_theta_cut_min=%f, position_theta_cut_max=%f",
                                        bending_angle_position_theta_cut_min/degree,
                                        bending_angle_position_theta_cut_max/degree ) );
    }
    if ( enable_position_phi_cut )
    {
        condition_txt->AddText ( Form ( "position_phi_cut_min=%f, position_phi_cut_max=%f",
                                        bending_angle_position_phi_cut_min/degree,
                                        bending_angle_position_phi_cut_max/degree ) );
    }
    if ( enable_direction_theta_cut )
    {
        condition_txt->AddText ( Form ( "direction_theta_cut_min=%f, direction_theta_cut_max=%f",
                                        bending_angle_direction_theta_cut_min/degree,
                                        bending_angle_direction_theta_cut_max/degree ) );
    }
    if ( enable_direction_phi_cut )
    {
        condition_txt->AddText ( Form ( "direction_phi_cut_min=%f, direction_phi_cut_max=%f",
                                        bending_angle_direction_phi_cut_min/degree,
                                        bending_angle_direction_phi_cut_max/degree ) );
    }
    if ( enable_position_r_off_cut )
    {
        condition_txt->AddText ( Form ( "position_r_off_cut_min=%f, position_r_off_cut_max=%f",position_r_off_cut_min,position_r_off_cut_max ) );
    }
    if ( enable_hit_position_x_cut )
    {
        condition_txt->AddText ( Form ( "hit_position_x_cut_min=%f, hit_position_x_cut_max=%f", hit_position_x_cut_min, hit_position_x_cut_max ) );
    }
    if ( enable_hit_position_y_cut )
    {
        condition_txt->AddText ( Form ( "hit_position_y_cut_min=%f, hit_position_y_cut_max=%f", hit_position_y_cut_min, hit_position_y_cut_max ) );
    }
    if ( enable_PE_cut )
    {
        condition_txt->AddText ( Form ( "num_pe_cut=%.2f", num_pe_cut ) );
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

    TCanvas* run_summary=new TCanvas ( "run_summary","Tracking Cut Run Summary",600,400 );
    TPaveText *summary_txt = new TPaveText ( 0.05,0.05,0.95,0.95 );
    summary_txt->SetTextSize ( 0.035 );
    summary_txt->AddText ( "Run Summary:" );
    summary_txt->SetTextAlign ( 12 );
    summary_txt->AddText ( Form ( "QwSimRun# %d - %d, total events: %d",start,end,num_of_valid_tracks ) );

    if ( num_of_valid_tracks!=0 )
    {
        summary_txt->AddText ( Form ( "number of tracks: %d", num_of_valid_tracks ) );
        summary_txt->AddText ( Form ( "scattering angle: %f +/- %f deg",
                                      angle->GetMean(), angle->GetMeanError() ) );

        summary_txt->AddText ( Form ( "vertex z: %f +/- %f ( rms ) cm,  vertex r: %f +/- %f ( rms ) cm",
                                      histo_vertex_z->GetMean(), histo_vertex_z->GetRMS(),
                                      histo_vertex_r->GetMean(), histo_vertex_r->GetRMS() ) );
    }

    if ( q2->GetEntries() )
    {
        summary_txt->AddText ( Form ( "Q2:  %f +/- %f ( GeV/c ) ^2",
                                      q2->GetMean(),q2->GetMeanError() ) );
    }

    if ( light_weighted_q2->GetEntries() )
    {
        summary_txt->AddText ( Form ( "Light-weighted Q2: %f +/- %f ( GeV/c ) ^2",
                   light_weighted_q2->GetMean(),light_weighted_q2->GetMeanError() ) );
    }

    summary_txt->Draw();


    //////////////////////////////////////
    // Save histograms to a file
    //////////////////////////////////////

    int sequence_num;
    TFile *hist_file;

    for ( sequence_num = 0; sequence_num <200; sequence_num++ )
    {
        hist_file = new TFile ( Form ( "QwSimTCRun_%d-%d_%d.root",start,end,sequence_num ),"READ" );
        if ( hist_file->IsZombie() )
        {
            hist_file = new TFile ( Form ( "QwSimTCRun_%d-%d_%d.root",start,end,sequence_num ),"CREATE" );
            cout<<"Created output data file QwSimTCRun_"<<start<<"-"<<end<<"_"<<sequence_num<<".root"<<endl<<endl;
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

    //save histograms
    hit_dist->Write();

    angle->Write();
    q2->Write();

    histo_md_pe->Write();
    light_weighted_q2->Write();

    front_pt_chi->Write();
    back_pt_chi->Write();

    histo_vertex_z->Write();
    histo_vertex_r->Write();
    histo_p0->Write();
    histo_pp->Write();
    eloss_vs_verz->Write();
    histo_direction_phi_off->Write();
    histo_position_phi_off->Write();
    histo_direction_theta_off->Write();
    histo_position_theta_off->Write();
    histo_momentum->Write();
    q2_vs_momentum->Write();
    r2_proj->Write();
    vertex_dist->Write();
    vertex_2D->Write();
    vertex_2D_xz->Write();
    vertex_2D_yz->Write();
    hit_tgt_window->Write();

    hist_file->Close();

    return;

}

//========================

int getOctNumber ( TTree* event_tree )
{
    event_tree->Draw ( "events.fQwPartialTracks.fOctant>>R2_Oct" );

    int r2_oct = int ( R2_Oct->GetMean() );
    return ( r2_oct );
}

void global2local ( double* x, double* y, int oct )
{
    if ( debug )
    {
        cout<<"-->rotated oct"<<oct<<" - ( "<< *x <<","<< *y <<" ) to ";
    }

    double theta = -45.0*3.14159/180.0* ( oct-1 );
    double xx = *x*cos ( theta ) - *y*sin ( theta );
    double yy = *x*sin ( theta ) + *y*cos ( theta );
    *x = xx;
    *y = yy;

    if ( debug )
        cout<<" ( "<<*x<<","<<*y<<" ) "<<endl;
}

double global2local_phi ( double phi, int octant )
{
    double pi = 3.1415927;

    if ( debug )
        cout<<"-->rotated oct"<<octant<<" phi angle from "<<phi*180.0/pi<<" deg to ";

    double offset = ( octant-1 ) *pi/4.0;
    if ( phi<- ( 2.0*pi ) /16.0 )
        phi = phi+2.0*pi;
    phi = phi - offset;

    if ( debug )
        cout<<phi*180.0/pi<<" deg"<<endl;

    return phi;
}


void getMatchingAngles ( TTree* event_tree )
{
    TH1F* dir_theta = new TH1F ( "dir_theta","dir_theta",500, -5*degree, 5*degree );
    TH1F* dir_phi   = new TH1F ( "dir_phi","dir_phi",500, -15*degree, 15*degree );
    TH1F* pos_theta = new TH1F ( "pos_theta","dir_theta",500, -2*degree, 2*degree );
    TH1F* pos_phi   = new TH1F ( "pos_phi","pos_phi",500, -15*degree, 15*degree );

    event_tree->Draw ( "events.fQwTracks.fDirectionThetaoff>>dir_theta", "originals.fCrossSection" );

    event_tree->Draw ( "events.fQwTracks.fDirectionPhioff>>dir_phi",
                       "originals.fCrossSection*(events.fQwTracks.fDirectionPhioff<3.14159/2.0 && events.fQwTracks.fDirectionPhioff>-3.14159/2.0)" );

    event_tree->Draw ( "events.fQwTracks.fPositionThetaoff>>pos_theta","originals.fCrossSection" );
    event_tree->Draw ( "events.fQwTracks.fPositionPhioff>>pos_phi",
                       "originals.fCrossSection*(events.fQwTracks.fPositionPhioff<3.14159/2.0 && events.fQwTracks.fPositionPhioff>-3.14159/2.0)" );

    position_theta_cut_mean = pos_theta->GetMean();
    position_theta_cut_rms  = pos_theta->GetRMS();

    position_phi_cut_mean = pos_phi->GetMean();
    position_phi_cut_rms  = pos_phi->GetRMS();

    direction_theta_cut_mean = dir_theta->GetMean();
    direction_theta_cut_rms  = dir_theta->GetRMS();

    direction_phi_cut_mean = dir_phi->GetMean();
    direction_phi_cut_rms  = dir_phi->GetRMS();

    cout<<endl<<"Parameters of matching angle residuals:"<<endl<<endl;
    cout<<"position_theta_cut_mean = "<<position_theta_cut_mean/degree<<" deg"<<endl;
    cout<<"position_theta_cut_rms  = "<<position_theta_cut_rms/degree <<" deg"<<endl;
    cout<<"position_phi_cut_mean   = "<<position_phi_cut_mean/degree<<" deg"<<endl;
    cout<<"position_phi_cut_rms    = "<<position_phi_cut_rms/degree <<" deg"<<endl;
    cout<<"direction_theta_cut_mean= "<<direction_theta_cut_mean/degree<<" deg"<<endl;
    cout<<"direction_theta_cut_rms = "<<direction_theta_cut_rms/degree <<" deg"<<endl;
    cout<<"direction_phi_cut_mean  = "<<direction_phi_cut_mean/degree<<" deg"<<endl;
    cout<<"direction_phi_cut_rms   = "<<direction_phi_cut_rms/degree <<" deg"<<endl;

    bending_angle_position_theta_cut_min = position_theta_cut_mean-num_of_sigma_position_theta_cut*position_theta_cut_rms;
    bending_angle_position_theta_cut_max = position_theta_cut_mean+num_of_sigma_position_theta_cut*position_theta_cut_rms;
    bending_angle_position_phi_cut_min = position_phi_cut_mean-num_of_sigma_position_phi_cut*position_phi_cut_rms;
    bending_angle_position_phi_cut_max = position_phi_cut_mean+num_of_sigma_position_phi_cut*position_phi_cut_rms;

    bending_angle_direction_theta_cut_min = direction_theta_cut_mean-num_of_sigma_direction_theta_cut*direction_theta_cut_rms;
    bending_angle_direction_theta_cut_max = direction_theta_cut_mean+num_of_sigma_direction_theta_cut*direction_theta_cut_rms;
    bending_angle_direction_phi_cut_min = direction_phi_cut_mean-num_of_sigma_direction_phi_cut*direction_phi_cut_rms;
    bending_angle_direction_phi_cut_max = direction_phi_cut_mean+num_of_sigma_direction_phi_cut*direction_phi_cut_rms;
}

void Draw_Collimator2_Frame ( int Octant=1 )
{
    double PI=3.1415926;
    double px[6];
    double py[6];

    double highy,highx,middley,middlex,lowy,lowx;
    highy=53.7,middley=37.1,lowy=30.57;
    highx=9.2,middlex=9.2,lowx=6.5;

    double angle;
    if ( Octant==8 )
        angle=135.0;
    else
        angle = ( 3-Octant ) *45;

    double Sin=sin ( angle*PI/180 );
    double Cos=cos ( angle*PI/180 );

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


    TLine* t1=new TLine ( px[0],py[0],px[1],py[1] );
    TLine* t2=new TLine ( px[2],py[2],px[3],py[3] );
    TLine* t3=new TLine ( px[0],py[0],px[4],py[4] );
    TLine* t4=new TLine ( px[1],py[1],px[5],py[5] );
    TLine* t5=new TLine ( px[4],py[4],px[2],py[2] );
    TLine* t6=new TLine ( px[5],py[5],px[3],py[3] );
    t1->SetLineWidth ( 1 );
    t2->SetLineWidth ( 1 );
    t3->SetLineWidth ( 1 );
    t4->SetLineWidth ( 1 );
    t5->SetLineWidth ( 1 );
    t6->SetLineWidth ( 1 );
    t1->Draw ( "same" );
    t2->Draw ( "same" );
    t3->Draw ( "same" );
    t4->Draw ( "same" );
    t5->Draw ( "same" );
    t6->Draw ( "same" );
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

    TLine* t1=new TLine ( px[0],py[0],px[1],py[1] );
    TLine* t2=new TLine ( px[1],py[1],px[2],py[2] );
    TLine* t3=new TLine ( px[2],py[2],px[3],py[3] );
    TLine* t4=new TLine ( px[3],py[3],px[0],py[0] );
    t1->SetLineWidth ( 1 );
    t2->SetLineWidth ( 1 );
    t3->SetLineWidth ( 1 );
    t4->SetLineWidth ( 1 );
    t1->Draw ( "same" );
    t2->Draw ( "same" );
    t3->Draw ( "same" );
    t4->Draw ( "same" );
}
