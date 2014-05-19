
// author: Siyuan Yang
// this script is devoted to project the region3's partial track to the //plane of maindetector, trigger scintillator, scanner and z=0. Use the //project_root function only. 
// To use that function, firstly you must put the //rootfile which contains the data under $QWSCRATCH directory and then type:
//project_root(command,package,runnumber,drawoption)
//
//
//here are some examples:
///project_root("MD",1,6327): draws the plot for main detector projection map without weights for package 1 from run 6327
///project_root("TS_p+m",2,6327) draws the plot for trigger scintllator projection with weights adc_p+adc_m for package 2 from run 6327
// actually, there are five weight mode you can choose: p,m,p+m,p-m and if you do not specify anything,just like the first example, that is not weights
// another thing to keep in mind is that you must use underscore "_" to separate the type("MD","TS","QTOR") with other options
///project_root("MD_p-m_profile",1,6327) draws the plot for main detector projection with the weights p-m as the profile value

// also add some functions to perform the following task:
// 1) see if 100% of MD big pulses have clean VDC tracks
// 2) study the angle dependence of MD pulse height, for light-weight yield



#include<iostream>
#include<fstream>
#include <string>

const double pe_convert[]={14.0141,16.6726,21.9799,38.5315,20.4254,20.3896,22.1042,22.3945,22.7986,30.0517,28.7274,25.3396,24.6273,16.0718,27.8305,12.3251};

TCanvas *c;

void md_profile(int octant, int runNumber) {
  string detector = "MD_";
  int package=0;
  TLine *leftEdgeTop;
  TLine *rightEdgeTop;
  TLine *leftEdgeBottom;
  TLine *rightEdgeBottom;

  if (octant==1) {
    package=1;
    leftEdgeTop = new TLine(-342.754,-115,-342.724,115);
    leftEdgeTop->SetLineWidth(2);
    leftEdgeTop->SetLineColor(kRed);

    rightEdgeTop = new TLine(-324.754,-115,-324.724,115);
    rightEdgeTop->SetLineWidth(2);
    rightEdgeTop->SetLineColor(kRed);

    leftEdgeBottom = new TLine(-342.754,0,-342.724,1700);
    leftEdgeBottom->SetLineWidth(2);
    leftEdgeBottom->SetLineColor(kRed);

    rightEdgeBottom = new TLine(-324.754,0,-324.724,1700);
    rightEdgeBottom->SetLineWidth(2);
    rightEdgeBottom->SetLineColor(kRed);
  }
  else if (octant==5) {
    package=2;
    //need to correct these against DS aluminum
    leftEdgeTop = new TLine(335.074,-115,335.074,115);
    leftEdgeTop->SetLineWidth(2);
    leftEdgeTop->SetLineColor(kRed);

    rightEdgeTop = new TLine(353.074,-115,353.074,115);
    rightEdgeTop->SetLineWidth(2);
    rightEdgeTop->SetLineColor(kRed);

    leftEdgeBottom = new TLine(352.754,0,352.724,1500);
    leftEdgeBottom->SetLineWidth(2);
    leftEdgeBottom->SetLineColor(kRed);

    rightEdgeBottom = new TLine(334.754,0,334.724,1500);
    rightEdgeBottom->SetLineWidth(2);
    rightEdgeBottom->SetLineColor(kRed);
   }
  else cout <<"This only works for octant 1/5 for now." <<endl;

  project_root(detector,package,octant,runNumber);

  c->cd(1);
  leftEdgeTop->Draw("same");
  rightEdgeTop->Draw("same");

  c->cd(2);
  leftEdgeBottom->Draw("same");
  rightEdgeBottom->Draw("same");

  c->Print(Form("rootfile_plots/run%i/md_profile_octant%i.png",runNumber,octant));

} //end function MD_profile

void GetData(int package=1,int run_number=18554)
{
    string file_name= Form ( "%s/Qweak_%d.root",gSystem->Getenv ( "QW_ROOTFILES" ),run_number );
    TFile *file = new TFile ( file_name.c_str() );
    ofstream ifile;
    ifile.open("output.dat");

    TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
    QwEvent* fEvent=NULL;
    QwPartialTrack* pt=NULL;
    QwTreeLine* tl=NULL;
    event_tree->SetBranchAddress("events",&fEvent);
    Int_t nevents=event_tree->GetEntries();
    bool valid=false;
    Int_t count=0;
    Int_t reverse=1;
    Int_t pkg=package;
    if (package==1) reverse=-1;
    cout << "number of events: " << nevents << endl;
    for (int i=0;i<nevents;i++)
    {
        valid=false;
        count=0;
        event_tree->GetEntry(i);

        for (int p=0;p<fEvent->GetNumberOfPartialTracks();p++)
        {
            pt=fEvent->GetPartialTrack(p);
            if (pt->GetRegion()==3 && pt->GetPackage()==pkg)
            {
                valid=true;
                break;
            }
        }
        if (valid)
        {
            int ntl=fEvent->GetNumberOfTreeLines();
            ifile << "!" << endl;
            for (int t=0;t<ntl;t++)
            {
                tl=fEvent->GetTreeLine(t);
                if (tl->GetRegion()==3&&tl->GetPlane()==0&&count<3)
                {
                    count++;
                    if (count==1)
                        ifile << "u " << (-tl->GetOffset())/tl->GetSlope() << " " << reverse*tl->GetSlope() << endl;
                    else
                        ifile << "v " << (-tl->GetOffset())/tl->GetSlope() << " " << reverse*tl->GetSlope() << endl;
                }
            }
        }
    }
    return;
};



void project()
{
    gStyle->SetPalette(1);
    gStyle->SetMarkerStyle(2);
    string file_name="/home/jefferson/src/TrackFinder/debug/src/xy_MD_1.dat";
//      string file_name= Form ( "%s/xy_md.dat",gSystem->Getenv ( "QWSCRATCH" ) );
    ifstream ifile(file_name.c_str());
//      ifile.open("src/TrackFinder/debug/src/xy_md.dat");
    TH2F* h=new TH2F("h","h",1200,-600,600,200,-100,100);
//      TH2F* h=new TH2F("h","h",120,-60,60,60,-360,-300);
    TH1F* hp=new TH1F("profile","profile",100,250,350);
//      hp=new TProfile("profile","profile",50,-350,-300);
    if (ifile.is_open())
    {

        while (!ifile.eof())
        {
            char str[500];
            char* pch;
            ifile.getline(str,300);
            pch=strtok(str,"! ");
            int count=0;
            double x=0,y=0;
            while (pch!=0)
            {
                if (count==0) x=atof(pch);
                else y=atof(pch);
                count++;
                pch=strtok(0,"! ");
            }
            if (count>2) cerr << "error happens!" << endl;
            h->Fill(x,y);
            hp->Fill(x);

        }
    }
    h->GetXaxis()->SetTitle("position x:cm");
    h->GetYaxis()->SetTitle("position y:cm");
    h->SetTitle("track projection on main detector");
    hp->GetXaxis()->SetTitle("position x:cm");
    hp->SetTitle("track projection on scanner");
//     hp->Draw("p");
     h->Draw("colz");
    return;
}

void project_root(string command="", int package=1,int md_number=1,int run_number=6327,
		  TString file_suffix="Qweak_")
{
//this z_pos was the original used. We believe it is 5 cm off now. 2012-05-09 JAM
  Double_t md_zpos[9] = {0.0, 581.665,  576.705, 577.020, 577.425, 582.515,  577.955, 577.885, 577.060};
//  Double_t md_zpos[9] = {0.0, 576.665,  571.705, 572.020, 572.425, 577.515,  572.955, 572.885, 572.060};

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

  TBranch* branch_event=event_tree->GetBranch("events");
  TBranch* branch     = event_tree->GetBranch("maindet");
  // TBranch* branch_hits = event_tree->GetBranch("fQwHits");

  branch_event->SetAddress(&fEvent);
  // brahch_hits->SetAddress(&hits);

  TLeaf* mdp=branch->GetLeaf(Form("md%dp_adc",md_number));
  TLeaf* mdm=branch->GetLeaf(Form("md%dm_adc",md_number));


  Int_t nevents=event_tree->GetEntries();


  Double_t dz = 0.0;
  
  if      (type=="QTOR") dz = 0.0;
  else if (type=="TS")   dz = 539.74;
  else if (type=="MD")   dz = md_zpos[md_number];
  else if (type=="SC")   dz = 591.515;
  else {
    cerr << "no such plane!" << endl;
    return;
  }



  for (int i=0;i<nevents;i++)
    {

      if(i % 1000 == 0) cout << "events process so far: " << i << endl;
      branch_event->GetEntry(i);
      branch->GetEntry(i);
 
      Double_t xoffset,yoffset,xslope,yslope,x,y;
      //      weight=0;
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

	      Double_t m = 0;
	      Double_t p = 0;
	      Double_t weight = 1.0;
	      p = mdp->GetValue();
	      m = mdm->GetValue();
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

	      //		printf("x %10.2f y %10.2f weight %10.2f mdp %10.2f mdm %10.2f \n", x, y, weight, p, m);
	      if (p!=0.0 && m!=0.0)
                {
		  if (IsProfile==false)
		    h_2d->Fill(x,y);
		  else
		    hp_2d->Fill(x,y,weight);
                }
            }
        }
    }

  c = new TCanvas("c","c",10, 10, 800,800);
  c->Divide(1,2);
  gStyle->SetPalette(1);
  gStyle->SetMarkerStyle(2);
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
      string title="track projection on " + type + "  " +  w_title + ": not profile";
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
  return;
};


// default: md,p+m mode
void CleanTrack(int package=1, int md_number=5,int run=6327)
{
    gStyle->SetPalette(1);
    string file_name= Form ( "%s/Qweak_%d.root",gSystem->Getenv ( "QWSCRATCH" ),run );
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
    TH1F* h_f=new TH1F(Form("h_f %s profile",w_title.c_str()),"h_f ",1000,0,10000);
    TH2F* h_2f=new TH2F(Form("h_2f %s profile",w_title.c_str()),"h_2f ",1000,0,10000,300,0,0.3);
    
    int nevents=event_tree->GetEntries();

    for (int i=0;i<nevents;i++)
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
        if(valid==false)
        h_f->Fill(mdp->GetValue()+mdm->GetValue());
        h_2f->Fill((mdp->GetValue()+mdm->GetValue()),residual);
    }
        TCanvas* c=new TCanvas("c","c",700,700);
        c->Divide(1,2);
        c->cd(1);
        h_f->Draw();
        c->cd(2);
        h_2f->Draw("colz");
  }

void Angle(int package=1, int md_number=5,int run=6327)
{
    gStyle->SetPalette(1);
    string file_name= Form ( "%s/Qweak_%d.root",gSystem->Getenv ( "QWSCRATCH" ),run );
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
    TProfile* h_x=new TProfile(Form("h_x %s profile",w_title.c_str()),"h_2f ",200,-1,1);
    TProfile* h_y=new TProfile(Form("h_y %s profile",w_title.c_str()),"h_2f ",200,-1,1);
    TProfile2D* h_2f=new TProfile2D(Form("h_2f %s profile",w_title.c_str()),"h_2f ",200,-1,1,200,-1,1);
    
    int nevents=event_tree->GetEntries();

    for (int i=0;i<nevents;i++)
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
    h_x->Draw();
    c->cd(2);
    h_y->Draw();
    c->cd(3);
    h_2f->Draw("colz");
  }
