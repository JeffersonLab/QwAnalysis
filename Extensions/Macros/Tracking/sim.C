#include <cstring>
#include <cstdlib>
#include <sstream>

const double us_target=-667;
const double target=-650;
const double ds_target=-633;
const double coll1=-568.17;

void sim(Int_t run=8658,std::string element="fQ2"){

  TDirectory * where=gDirectory;
  TFile sim_file(Form("%s/qweak_sim.root",gSystem->Getenv("QW_ROOTFILES")));
  TFile data_file(Form("%s/Qweak_%d_base_cut_v1.root",gSystem->Getenv("QW_ROOTFILES"),run));
  TFile data_file_1(Form("%s/Qweak_%d_base_cut_v2.root",gSystem->Getenv("QW_ROOTFILES"),run));

  TTree* sim_tree=(TTree*)sim_file.Get("h8013");
  TTree* data_tree=(TTree*)data_file.Get("event_tree");
  TTree* data_tree_1=(TTree*)data_file_1.Get("event_tree");
  where->cd();
  TH1F* sim_h=new TH1F("sim","sim",100,4,14);
  TH1F* data_h=new TH1F("data","data",100,4,14);
  TH1F* data_h_1=new TH1F("data_1","data",100,4,14);
  string sim_string;
  if(element=="fQ2")
    sim_string="q__2";
  else if(element=="fScatteringAngle")
    sim_string="theta_o";

  sim_tree->Project("sim",Form("%s",sim_string.c_str()));
  
  data_tree->Project("data",Form("events.fQwTracks.%s",element.c_str()),"events.fQwTracks.fScatteringAngle>4 && events.fQwTracks.fScatteringAngle<12");
  data_tree_1->Project("data_1",Form("events.fQwTracks.%s",element.c_str()),"events.fQwTracks.fScatteringAngle>4 && events.fQwTracks.fScatteringAngle<12");
  sim_h->SetLineColor(kRed);
  sim_h->Draw();
  sim_h->SetTitle("sim(Red) vs real data: method1(Blue) vs method2(Cyan)");
  sim_h->GetXaxis()->SetTitle(Form("x axis:%s",element.c_str()));
  data_h->SetLineColor(kBlue);
  data_h->Scale(sim_h->GetEntries()/data_h->GetEntries());
  data_h->Draw("same");
  //data_h_1->SetLineColor(6);
  //data_h_1->Scale(sim_h->GetEntries()/data_h_1->GetEntries());
  //data_h_1->Draw("same");

  return;

}

/*
 * option=1, draw the projection to collimator1 as well as the target vertex distribution
 * option=4, draw the projection to target position
 * option=5, draw the projection to the upstream target plane
 *
 */



void reconstruction(int option=0,int pkg=1,int event_start=0,int event_end=-1,int run=8658,string suffix=""){


   bool fDebug=false;
   string folder= "/scratch/sxyang";
   ostringstream ss;
   ss << folder << "/Qweak_";
   ss << run << suffix;
   ss << ".root";
   string file_name=ss.str();
   cout <<  file_name << endl;
   TFile* file=new TFile(file_name.c_str());

   
    TH1F* vertex=new TH1F("vertex in Z","vertex in Z",300,-1000,-400);
    TH2F* projection=new TH2F("projection","projection",200,-25,25,120,0,30);
    TH2F* projection_target=new TH2F("projection to target plane","projection to target plane",120,-15,15,80,-10,10);
    //TH2F* projection=new TH2F("projection","projection",120,-15,15,80,-10,10);
    TProfile2D* profile_target=new TProfile2D("1","1",120,-15,15,80,-10,10);
   
    TH1F* Xtreeline_histo=new TH1F("ar in treeline x","ar in treeline x",100,0,0.3);
    TH1F* Utreeline_histo=new TH1F("ar in treeline x","ar in treeline u",100,0,0.3);
    TH1F* Vtreeline_histo=new TH1F("ar in treeline x","ar in treeline v",100,0,0.3);
    TH1F* Chi_histo=new TH1F("chi distribution of partial track","chi distribution in r2",100,0,100);
    TH1F* x_histo=new TH1F("x coll1 distribution","x coll1 distribution",200,-10,10);
    TH1F* y_histo=new TH1F("y coll1 distribution","y coll1 distribution",200,0,20);
    TH1F* Plane_residual[12];
    for(int i=0;i<12;++i)
      Plane_residual[i]=new TH1F(Form("residual in plane%d",i+1),Form("residual in plane%d",i+1),100,-0.5,0.5);

    QwEvent* fEvent=0;
    QwHit* hit=0;
    QwTrack* track=0;
    QwPartialTrack* pt=0;

    TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
    Int_t nevents=event_tree->GetEntries();
    cout << "total events: " << nevents << endl;
    if(event_end==-1)
      event_end=nevents;

    event_tree->SetBranchStatus("events",1);
    TBranch* event_branch=event_tree->GetBranch("events");
    //TBranch* maindet_branch=event_tree->GetBranch("maindet");
    event_branch->SetAddress(&fEvent);
    //int md_number=1;
    //TLeaf* mdp=maindet_branch->GetLeaf(Form("md%dp_adc",md_number));
    //TLeaf* mdm=maindet_branch->GetLeaf(Form("md%dm_adc",md_number));
    
   
    
    //chain->SetBranchAddress ( "events",&fEvent);
    
    int good=0;
    for(int i=event_start;i<event_end;++i){

      if(i%10000==0)
	cout << "events processed so far: " << i << endl;
      
      // chain->GetEntry(i);
      event_branch->GetEntry(i);
      // maindet_brach->GetEntry(i);

      //double mdp_value=mdp->GetValue();
      //double mdm_value=mdm->GetValue();
      
      	int nhits=fEvent->GetNumberOfHits();
	int valid_hits=0;
	for(int j=0;j<nhits;++j){
	  hit=fEvent->GetHit(j);
	  if(hit->GetRegion() ==2 && hit->GetDriftDistance() >=0 && hit->GetHitNumber()==0 && hit->GetPackage() == pkg)
	    ++valid_hits;
	}

	if(valid_hits>=15) continue;

      double ntracks=fEvent->GetNumberOfTracks();
      double npts=fEvent->GetNumberOfPartialTracks();

      double chi=0;
      // call build function
      if(ntracks!=0){
      track=fEvent->GetTrack(0);
      for(int j=0;j<npts;++j){
        pt=fEvent->GetPartialTrack(j);
	if(pt->GetRegion()!=2 || pt->GetPackage()!=pkg || pt->GetPackage()!=track->GetPackage()) continue;
	chi=pt->fChi;
        double vertex_z=-(pt->fSlopeX*pt->fOffsetX + pt->fSlopeY*pt->fOffsetY)/(pt->fSlopeX*pt->fSlopeX+pt->fSlopeY*pt->fSlopeY);
	double x=pt->fOffsetX+coll1*pt->fSlopeX;
	double y=pt->fOffsetY+coll1*pt->fSlopeY;
	double x_tar=pt->fOffsetX+us_target*pt->fSlopeX;
	double y_tar=pt->fOffsetY+us_target*pt->fSlopeY;
	double r=sqrt(x_tar*x_tar+y_tar*y_tar);
	if( r>10 && fabs(y_tar)>3 )
	  cout << "event: " << i << " chi: " << chi << endl;

	vertex->Fill(vertex_z);
	projection->Fill(x,y);
	if(in_coll(x,y)==true)
	  ++good;
	//if(vertex_z > -600 || vertex_z < -700){
	//projection->Fill(x,y);
	//}
	//else{
	//   vertex->Fill(vertex_z);
	//}
	x_histo->Fill(x);
	y_histo->Fill(y);
	projection_target->Fill(x_tar,y_tar);
	profile_target->Fill(x,y,chi);
	Xtreeline_histo->Fill(pt->TResidual[1]);
	Utreeline_histo->Fill(pt->TResidual[3]);
	Vtreeline_histo->Fill(pt->TResidual[4]);
	//if(pt->TResidual[3]>2*pt->TResidual[1])
	//  cout << "event: " << i << endl;
	Chi_histo->Fill(pt->fChi);
	for(int k=0;k<12;++k)
	  if(pt->fSignedResidual[k]!=-10)
	    Plane_residual[k]->Fill(pt->fSignedResidual[k]);
      }
      }
    }  // end of for loop over events
   
    cout << "good: " << good << endl;
    TCanvas* c=new TCanvas("c","c",800,600);
    if(option==1){
      c->Divide(1,2);
      c->cd(1);
    gStyle->SetPalette(1);
    projection->GetXaxis()->SetTitle("x axis:cm");
    projection->GetYaxis()->SetTitle("y axis:cm");
    projection->Draw("colz");
    projection->SetTitle("run 8658: projection to coll1");
    
    // ds coll1
    
    double highy=18.46,middley=12.63,lowy=7.03;
    double highx=3.65,middlex=3.65,lowx=1.91;
    //double highy=53.7,middley=37.1,lowy=30.57;
    //double highx=9.2,middlex=9.2,lowx=6.5;
    //double highy=77.5,middley=51,lowy=39.95;
    //double highx=14.5,middlex=14.5,lowx=10.1;
    TLine* t1=new TLine(-highx,highy,highx,highy);
    TLine* t2=new TLine(-lowx,lowy,lowx,lowy);
    TLine* t3=new TLine(-highx,highy,-middlex,middley);
    TLine* t4=new TLine(highx,highy,middlex,middley);
    TLine* t5=new TLine(-middlex,middley,-lowx,lowy);
    TLine* t6=new TLine(middlex,middley,lowx,lowy);
    t1->SetLineWidth(2);
    t2->SetLineWidth(2);
    t3->SetLineWidth(2);
    t4->SetLineWidth(2);
    t5->SetLineWidth(2);
    t6->SetLineWidth(2);
    t1->Draw("same");
    t2->Draw("same");
    t3->Draw("same");
    t4->Draw("same");
    t5->Draw("same");
    t6->Draw("same");
    
    c->cd(2);
    vertex->Draw();
    vertex->GetXaxis()->SetTitle("z axis:cm");
    vertex->SetTitle("run 8658: vertex distribution");
    }
    else if(option==2){
    c->Divide(2,2);
    c->cd(1);
    Xtreeline_histo->Draw();
    c->cd(2);
    Utreeline_histo->Draw();
    c->cd(3);
    Vtreeline_histo->Draw();
    c->cd(4);
    Chi_histo->Draw();
    }
    else if(option==3){
      c->Divide(3,4);
      int a=0;
      while(a!=12){
	c->cd(a+1);
	Plane_residual[a++]->Draw();
      }
     }
    else if(option==4){
      gStyle->SetPalette(1);
    projection_target->GetXaxis()->SetTitle("x axis:cm");
    projection_target->GetYaxis()->SetTitle("y axis:cm");
    projection_target->Draw("colz");
    projection_target->SetTitle(Form("run %d: projection to target from pkg %d",run,pkg));
    }
    else if(option==5){
      gStyle->SetPalette(1);
    profile_target->GetXaxis()->SetTitle("x axis:cm");
    profile_target->GetYaxis()->SetTitle("y axis:cm");
    profile_target->Draw("colz");
    profile_target->SetTitle(Form("run %d: projection to target from pkg %d",run,pkg));
    }
    else if(option==6){
      c->Divide(2,1);
      c->cd(1);
      x_histo->Draw();
      c->cd(2);
      y_histo->Draw();
    }
    
    
    return;

 }

void test_linear(){

  //double Dx=2.3112;
  double space=1.1684;
  double Dx=2.1174;
  //double z[4]={-336.522,-330.588,-293.822,-287.888};
  double z[3]={-334.544,-328.61,-285.91};
  double y[3]={36.7242,37.2947,42.2192};
  double w[3]={16,17,18};
  /*
  for(int i=0;i<3;++i){
      y[i]=(w[i]-0.5)*space+y[i];
      if(i!=0)
	y[i]+=Dx;
  }
  */
  TGraph* t=new TGraph(3,z,y);
  TF1 *f=new TF1("m1","pol1");
  t->Fit("m1");
  t->SetMarkerSize(1);
  t->SetMarkerStyle(21);
  t->Draw("AP");
  double res=0;
  for(int i=0;i<3;++i)
    res+=fabs(f->Eval(z[i])-y[i]);
  cout << "res: " << res/3 << endl;
  return;

}


void draw_element(int pkg=1,int run=8658){
   string file_name=Form("%s/Qweak_%d_check_v2.root",gSystem->Getenv ( "QW_ROOTFILES" ),run);
   TFile* file=new TFile(file_name.c_str());

   
    TH1F* vertex=new TH1F("vertex in Z","vertex in Z",300,-1000,-400);
    TH2F* projection=new TH2F("projection","projection",200,-25,25,120,0,30);
    TH2F* projection_target=new TH2F("projection to target plane","projection to target plane",120,-15,15,80,-10,10);
    TProfile2D* profile_target=new TProfile2D("1","1",120,-15,15,80,-10,10);
   
    TH1F* Chi_histo=new TH1F("chi distribution of partial track","chi distribution in r2",100,0,100);
    TH1F* Plane_residual[12];
    for(int i=0;i<12;++i)
      Plane_residual[i]=new TH1F(Form("residual in plane%d",i+1),Form("residual in plane%d",i+1),100,-0.5,0.5);

    QwEvent* fEvent=0;
    QwHit* hit=0;
    QwTrack* track=0;
    QwPartialTrack* pt=0;

    TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );

    TCanvas* c=new TCanvas("c","c",800,600);
    c->Divide(3,4);
    for(int i=0;i<12;++i){
      cout << "i: " << i << endl;
      c->cd(i+1);
      event_tree->Draw("events.fQwHits.fElement",Form("events.fQwHits.fRegion==2 && events.fQwHits.fPackage==%d && events.fQwHits.fPlane==%d",pkg,i+1));
    }
}


bool in_coll(double x,double y){
  if(y<7.03 || y>18.46 )
    return false;
  if(y<12.63){
    if(fabs(x) < (1.91+(y-7.03)/3.218))
      return true;
    else
      return false;
  }
  
  if(fabs(x)<3.65)
    return true;
  else
    return false;
}
