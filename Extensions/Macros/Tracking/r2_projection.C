#include <cstring>
#include <cstdlib>
#include <sstream>

const double us_target=-667;
const double target=-650;
const double ds_target=-633;
const double coll1=-568.17;
const double coll2=-370.719;

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



void projection(int option=0,int pkg=1,int event_start=0,int event_end=-1,int run=8658,string suffix=""){


   bool fDebug=false;
   //string folder= "/scratch/sxyang";
   string folder="/work/hallc/qweak/sxyang/rootfiles";
   ostringstream ss;
   ss << folder << "/Qweak_";
   ss << run << suffix;
   ss << ".root";
   string file_name=ss.str();
   cout <<  file_name << endl;
   TFile* file=new TFile(file_name.c_str());

   
    TH1F* vertex=new TH1F("vertex in Z","vertex in Z",300,-1000,-400);
    // TH2F* projection=new TH2F("projection","projection",160,-20,20,100,5,30);
    TH2F* projection=new TH2F("projection","projection",320,-40,40,320,-40,40);
    TH2F* projection_target=new TH2F("projection to target plane","projection to target plane",120,-15,15,120,-15,15);
    TH2F* Directionphioff=new TH2F("DirectionPhioff vs r","DirectionPhioff vs r",1000,-0.5,0.5,200,0,20);
    TH2F* Directionthetaoff=new TH2F("DirectionThetaoff vs r","DirectionThetaoff vs r",1000,-0.05,0.05,200,0,20);
    
    //TH2F* projection=new TH2F("projection","projection",120,-15,15,80,-10,10);
    TProfile2D* profile_target=new TProfile2D("1","1",120,-15,15,80,-10,10);
   
    TH1F* Xtreeline_histo=new TH1F("ar in treeline x","ar in treeline x",100,0,0.3);
    TH1F* Utreeline_histo=new TH1F("ar in treeline x","ar in treeline u",100,0,0.3);
    TH1F* Vtreeline_histo=new TH1F("ar in treeline x","ar in treeline v",100,0,0.3);
    TH1F* Chi_histo=new TH1F("chi distribution of partial track","chi distribution in r2",1000,0,100);
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
    
    int start=(event_start==-1)? 0:event_start;
    int end=(event_end==-1)? nevents:event_end;
    event_tree->SetBranchStatus("events",1);
    TBranch* event_branch=event_tree->GetBranch("events");
    TBranch* maindet_branch=event_tree->GetBranch("maindet");
    event_branch->SetAddress(&fEvent);
  
    int md_1=5;
    int md_2=1;
    
    TLeaf* tsp_1=maindet_branch->GetLeaf(Form("md%dp_f1",md_1));
    TLeaf* tsm_1=maindet_branch->GetLeaf(Form("md%dm_f1",md_1));
    TLeaf* tsp_2=maindet_branch->GetLeaf(Form("md%dp_f1",md_2));
    TLeaf* tsm_2=maindet_branch->GetLeaf(Form("md%dm_f1",md_2));
   
    //chain->SetBranchAddress ( "events",&fEvent);
    


    // cuts
      double mean_phioff_pkg1=-0.00603;
      double mean_phioff_pkg2=-0.01637;
      double mean_thetaoff_pkg1=-0.00128;
      double mean_thetaoff_pkg2=-0.01144;

      double rms_phioff_pkg1=0.04053;
      double rms_phioff_pkg2=0.04011;
      double rms_thetaoff_pkg1=0.00857;
      double rms_thetaoff_pkg2=0.00858;

      double width=25;
      double pkg1_phioff_lower=mean_phioff_pkg1-width*rms_phioff_pkg1;
      double pkg1_phioff_upper=mean_phioff_pkg1+width*rms_phioff_pkg1;
      double pkg2_phioff_lower=mean_phioff_pkg2-width*rms_phioff_pkg2;
      double pkg2_phioff_upper=mean_phioff_pkg2+width*rms_phioff_pkg2;

      double pkg1_thetaoff_lower=mean_thetaoff_pkg1-width*rms_thetaoff_pkg1;
      double pkg1_thetaoff_upper=mean_thetaoff_pkg1+width*rms_thetaoff_pkg1;
      double pkg2_thetaoff_lower=mean_thetaoff_pkg2-width*rms_thetaoff_pkg2;
      double pkg2_thetaoff_upper=mean_thetaoff_pkg2+width*rms_thetaoff_pkg2;

    int good=0;
    int oct=0;
    for(int i=start;i<end;++i){

      if(i%10000==0)
	cout << "events processed so far: " << i << endl;
      
      event_branch->GetEntry(i);
      maindet_branch->GetEntry(i);
       
      double tsp_value_1=tsp_1->GetValue();
      double tsm_value_1=tsm_1->GetValue();
      double tsp_value_2=tsp_2->GetValue();
      double tsm_value_2=tsm_2->GetValue();
      
      	int nhits=fEvent->GetNumberOfHits();
	int valid_hits=0;
	for(int j=0;j<nhits;++j){
	  hit=fEvent->GetHit(j);
	  if(hit->GetRegion() ==2 && hit->GetDriftDistance() >=0 && hit->GetHitNumber()==0 && hit->GetPackage() == pkg)
	    ++valid_hits;
	}

	// test if the md get hit
	
	if(pkg==1){
	  if(tsm_value_1 <-1800 || tsm_value_1 > -1200 || tsp_value_1 < -1800 || tsp_value_1 > -1200)
	    continue;
	}
	else if(pkg==2){
	  if(tsm_value_2 <-1800 || tsm_value_2 > -1200 || tsp_value_2 < -1800 || tsp_value_2 > -1200)
	    continue;
	}
	
	
	//if(valid_hits>=18) continue;
      double ntracks=fEvent->GetNumberOfTracks();
      double npts=fEvent->GetNumberOfPartialTracks();

      double chi=0;
      // call build function
      //if(ntracks!=0){
      for(int nts=0;nts<ntracks;++nts){
      track=fEvent->GetTrack(nts);
      if(track->GetPackage()!=pkg){
	continue;
      }
      else if(pkg==1){
      if(track->fDirectionPhioff<pkg1_phioff_lower || track->fDirectionPhioff>pkg1_phioff_upper || track->fDirectionThetaoff < pkg1_thetaoff_lower || track->fDirectionThetaoff > pkg1_thetaoff_upper) continue;
      }
      else{
      if(track->fDirectionPhioff<pkg2_phioff_lower || track->fDirectionPhioff> pkg2_phioff_upper || track->fDirectionThetaoff < pkg2_thetaoff_lower || track->fDirectionThetaoff >  pkg2_thetaoff_upper ) continue;
      }
      for(int j=0;j<npts;++j){
        pt=fEvent->GetPartialTrack(j);
	if(pt->GetRegion()!=2 || pt->GetPackage()!=pkg || pt->GetPackage()!=track->GetPackage()) continue;
	if(oct==0)
	  oct=pt->GetOctantNumber();
	chi=pt->fChi;
        double vertex_z=-(pt->fSlopeX*pt->fOffsetX + pt->fSlopeY*pt->fOffsetY)/(pt->fSlopeX*pt->fSlopeX+pt->fSlopeY*pt->fSlopeY);
	double x=pt->fOffsetX+coll1*pt->fSlopeX;
	double y=pt->fOffsetY+coll1*pt->fSlopeY;
	double x_tar=pt->fOffsetX+ds_target*pt->fSlopeX;
	double y_tar=pt->fOffsetY+ds_target*pt->fSlopeY;
	x*=-1;
	x_tar*=-1;
	double r=sqrt(x_tar*x_tar+y_tar*y_tar);
	Directionphioff->Fill(track->fDirectionPhioff,r);
	Directionthetaoff->Fill(track->fDirectionThetaoff,r);


	vertex->Fill(vertex_z);
	projection->Fill(x,y);

	x_histo->Fill(x);
	y_histo->Fill(y);
	projection_target->Fill(x_tar,y_tar);
	profile_target->Fill(x,y,chi);
	Xtreeline_histo->Fill(pt->TResidual[1]);
	Utreeline_histo->Fill(pt->TResidual[3]);
	Vtreeline_histo->Fill(pt->TResidual[4]);
	Chi_histo->Fill(pt->fChi);
	for(int k=0;k<12;++k)
	  if(pt->fSignedResidual[k]!=-10)
	    Plane_residual[k]->Fill(pt->fSignedResidual[k]);
      }
      }
    }  // end of for loop over events
   
    //cout << "good: " << good << endl;
    cout << "oct: " << oct << endl;
    TCanvas* c=new TCanvas("c","c",800,800);
    if(option==1){
      c->Divide(1,3);
      c->cd(1);
    gStyle->SetPalette(1);
    projection->GetXaxis()->SetTitle("x axis:cm");
    projection->GetYaxis()->SetTitle("y axis:cm");
    projection->Draw("colz");
    projection->SetTitle(Form("run %d: pkg%d projection to primary collimator",run,pkg));
    
    // ds coll1
    double PI=3.1415926;
    double angle=oct==8? -135:-(3-oct)*45;
    double Sin=sin(angle*PI/180);
    double Cos=cos(angle*PI/180);
    
    double px[6];
    double py[6];
    double highy=18.46,middley=12.63,lowy=7.03;
    double highx=3.65,middlex=3.65,lowx=1.91;
    //double highy=53.7,middley=37.1,lowy=30.57;
    //double highx=9.2,middlex=9.2,lowx=6.5;
    //double highy=77.5,middley=51,lowy=39.95;
    //double highx=14.5,middlex=14.5,lowx=10.1;
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
    
    //TLine* t1=new TLine(-highx,highy,highx,highy);
    //TLine* t2=new TLine(-lowx,lowy,lowx,lowy);
    //TLine* t3=new TLine(-highx,highy,-middlex,middley);
    //TLine* t4=new TLine(highx,highy,middlex,middley);
    //TLine* t5=new TLine(-middlex,middley,-lowx,lowy);
    //TLine* t6=new TLine(middlex,middley,lowx,lowy);
    TLine* t1=new TLine(px[0],py[0],px[1],py[1]);
    TLine* t2=new TLine(px[2],py[2],px[3],py[3]);
    TLine* t3=new TLine(px[0],py[0],px[4],py[4]);
    TLine* t4=new TLine(px[1],py[1],px[5],py[5]);
    TLine* t5=new TLine(px[4],py[4],px[2],py[2]);
    TLine* t6=new TLine(px[5],py[5],px[3],py[3]);
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
    vertex->SetTitle(Form("run %d: pkg%d vertex distribution",run,pkg));
    c->cd(3);
    Chi_histo->Draw();
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
    c->Divide(1,2);
    c->cd(1);
    gStyle->SetPalette(1);
    projection_target->GetXaxis()->SetTitle("x axis:cm");
    projection_target->GetYaxis()->SetTitle("y axis:cm");
    projection_target->Draw("colz");
    projection_target->SetTitle(Form("run %d: projection to target from pkg %d",run,pkg));
    c->cd(2);
    vertex->Draw();
    vertex->GetXaxis()->SetTitle("z axis:cm");
    vertex->SetTitle(Form("run %d: pkg%d vertex distribution",run,pkg));
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
    else if(option==7){
    gStyle->SetPalette(1);
    c->Divide(1,2);
    c->cd(1);
    Directionphioff->GetXaxis()->SetTitle("fDirectionPhioff: rad");
    Directionphioff->GetYaxis()->SetTitle("r: cm");
    Directionphioff->Draw("colz");
    c->cd(2);
    Directionthetaoff->GetXaxis()->SetTitle("fDirectionThetaoff: rad");
    Directionthetaoff->GetYaxis()->SetTitle("r: cm");
    Directionthetaoff->Draw("colz");
    
    }
    
    
    return;

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
