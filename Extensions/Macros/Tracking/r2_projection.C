// Author:Siyuan Yang
// Email:sxyang@email.wm.edu
// Date: Jan 12th 2012
// 
// Description: This file is used project the r2 track back to certain planes
// Two basic cuts are set as default here: MD cut and one-track-per-event cut.
// 
// Example
// root[1] projection("ds",1,0,-1,13563,1)
// this will draw the projection of qualified R2 tracks back to the downstream target plane for package 1 over all events
// the number behind the run number 13563 specifies which octant the pkg2 of R2 covers in this run.
// Notice that the first string can be either of the following listed choices:
// "ds" downstream solid target
// "us" upstream solid target
// "coll1" downstream face of collimator 1
// "coll2" downstream face of primary collimator
// "-500" arbitrary number in z location
// Another example
// root[2] projection("-330",1,0,-1,13654,7)
// this will draw the projection of qualified R2 tracks back to z=-330 plane, where the pkg2 of R2 covers octant 7 this time.

// UPDATE Feb 12 2012
// remove the octant number from argument list. Now the octant number is deduced// from the given rum number.


#include <cstring>
#include <cstdlib>
#include <sstream>
//#include <string>

const int multitracks=18;

void projection(string target,int pkg=1,int event_start=0,int event_end=-1,int run=8658,string prefix="", string suffix=""){

  // try to get the oct number from the run number
  int oct=getOctNumber(run);

   //bool fDebug=false;
   //string folder= "/scratch/sxyang";
   string folder=gSystem->Getenv("QW_ROOTFILES");
   ostringstream ss;
   ss << folder << "/Qweak_";
   ss << prefix ;
   ss << run << suffix;
   ss << ".root";
   string file_name=ss.str();
   cout <<  file_name << endl;
   TFile* file=new TFile(file_name.c_str());
  
   double z;
   if(target=="coll1")
     z=-568.17;
   else if(target=="coll2")
     z=-370.719;
   else if(target=="ds")
     z=-633;
   else if(target=="us")
     z=-667;
   else{
     istringstream random(target);
     random >> z;
   }

   TH1F* vertex=new TH1F("vertex in Z","vertex in Z",200,-850,-450);
   TH2F* projection;
   if(z<-500 && z>-600)
     projection=new TH2F("projection","projection",240,-30,30,240,-30,30);
   else if(z<-600)
     projection=new TH2F("projection","projection",16,-2,2,16,-2,2);
   else if(z<-350)
     projection=new TH2F("projection","projection",480,-60,60,480,-60,60);
   else
     projection=new TH2F("projection","projection",720,-90,90,720,-90,90);
     
    //TH2F* projection=new TH2F("projection","projection",240,-30,30,240,-30,30);
    //TH2F* projection=new TH2F("projection","projection",240,-30,30,140,25,60);
    // TH2F* projection_target=new TH2F("projection to target plane","projection to target plane",120,-15,15,120,-15,15);
    TH2F* Directionphioff=new TH2F("DirectionPhioff vs r","DirectionPhioff vs r",1000,-0.5,0.5,200,0,20);
    TH2F* Directionthetaoff=new TH2F("DirectionThetaoff vs r","DirectionThetaoff vs r",1000,-0.05,0.05,200,0,20);
    
    TProfile2D* profile_target=new TProfile2D("1","1",120,-15,15,80,-10,10);
   
    TH1F* Xtreeline_histo=new TH1F("ar in treeline x","ar in treeline x",100,0,0.3);
    TH1F* Utreeline_histo=new TH1F("ar in treeline x","ar in treeline u",100,0,0.3);
    TH1F* Vtreeline_histo=new TH1F("ar in treeline x","ar in treeline v",100,0,0.3);
    TH1F* Chi_histo=new TH1F("chi distribution of partial track","chi distribution in r2",200,0,20);
    TH1F* x_histo=new TH1F("x coll1 distribution","x coll1 distribution",160,-70,-30);
    TH1F* y_histo=new TH1F("y coll1 distriution","y coll1 distribution",200,0,20);
    TH1F* Plane_residual[12];
    for(int i=0;i<12;++i)
      Plane_residual[i]=new TH1F(Form("residual in plane%d",i+1),Form("residual in plane%d",i+1),100,-0.5,0.5);
    TH2F* test_for_fun=new TH2F("Q2 vs position","Q2 vs position",80,30,70,100,0,0.12);


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
  
    int md_1=(oct+4)%8;
    int md_2=oct;
    
    TLeaf* mdp_1=maindet_branch->GetLeaf(Form("md%dp_f1",md_1));
    TLeaf* mdm_1=maindet_branch->GetLeaf(Form("md%dm_f1",md_1));
    TLeaf* mdp_2=maindet_branch->GetLeaf(Form("md%dp_f1",md_2));
    TLeaf* mdm_2=maindet_branch->GetLeaf(Form("md%dm_f1",md_2));
   
    //chain->SetBranchAddress ( "events",&fEvent);
    


    // cuts
    double mean_thetaoff_pkg1=1,sigma_thetaoff_pkg1=1;
    double mean_thetaoff_pkg2=1,sigma_thetaoff_pkg2=1;
    double mean_phioff_pkg1=1, sigma_phioff_pkg1=1;
    double mean_phioff_pkg2=1,sigma_phioff_pkg2=1; 
    bool opt=false;
    if(opt==true){
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
      
      mean_thetaoff_pkg1=f1->GetParameter(1);
      sigma_thetaoff_pkg1=f1->GetParameter(2);

      mean_thetaoff_pkg2=f2->GetParameter(1);
      sigma_thetaoff_pkg2=f2->GetParameter(2);

      mean_phioff_pkg1=f3->GetParameter(1);
      sigma_phioff_pkg1=f3->GetParameter(2);

      mean_phioff_pkg2=f4->GetParameter(1);
      sigma_phioff_pkg2=f4->GetParameter(2);
      
    }
    

      //for(int i=0;i<3;++i)
      //cout << "parameter[" << i << "]=" << f1->GetParameter(i) << endl;
     double width=1000;
     double pkg1_phioff_lower=mean_phioff_pkg1-width*sigma_phioff_pkg1;
     double pkg1_phioff_upper=mean_phioff_pkg1+width*sigma_phioff_pkg1;
     double pkg2_phioff_lower=mean_phioff_pkg2-width*sigma_phioff_pkg2;
     double pkg2_phioff_upper=mean_phioff_pkg2+width*sigma_phioff_pkg2;

     double pkg1_thetaoff_lower=mean_thetaoff_pkg1-width*sigma_thetaoff_pkg1;
     double pkg1_thetaoff_upper=mean_thetaoff_pkg1+width*sigma_thetaoff_pkg1;
     double pkg2_thetaoff_lower=mean_thetaoff_pkg2-width*sigma_thetaoff_pkg2;
     double pkg2_thetaoff_upper=mean_thetaoff_pkg2+width*sigma_thetaoff_pkg2;
  
    for(int i=start;i<end;++i){

      if(i%10000==0)
	cout << "events processed so far: " << i << endl;
      
      event_branch->GetEntry(i);
      maindet_branch->GetEntry(i);
       
      double mdp_value_1=mdp_1->GetValue();
      double mdm_value_1=mdm_1->GetValue();
      double mdp_value_2=mdp_2->GetValue();
      double mdm_value_2=mdm_2->GetValue();
      
      	int nhits=fEvent->GetNumberOfHits();
	int valid_hits=0;
	bool special_flag=false;
	for(int j=0;j<nhits;++j){
	  hit=fEvent->GetHit(j);
	  
	  if(hit->GetRegion() ==2 && hit->GetDriftDistance() >=0 && hit->GetHitNumber()==0 && hit->GetPackage() == pkg){
	    ++valid_hits;
	  }
	}
	// test if the md get hit
	
	if(pkg==1){
	  if(mdm_value_1 <-210 || mdm_value_1 > -150 || mdp_value_1 < -210 || mdp_value_1 > -150)
	    continue;
	}
	else if(pkg==2){
	  if(mdm_value_2 <-210 || mdm_value_2 > -150 || mdp_value_2 < -210 || mdp_value_2 > -150)
	    continue;
	}
	
	
      if(valid_hits>=multitracks || special_flag) continue;
      double ntracks=fEvent->GetNumberOfTracks();
      double npts=fEvent->GetNumberOfPartialTracks();

      double chi=0;
      // call build function
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
	oct=pt->GetOctant();
	chi=pt->fChi;
        //double vertex_z=-(pt->fSlopeX*pt->fOffsetX + pt->fSlopeY*pt->fOffsetY)/(pt->fSlopeX*pt->fSlopeX+pt->fSlopeY*pt->fSlopeY);
	//double vertex_z=track->fVertexZ;
	double vertex_z=fEvent->GetScatteringVertexZ();
	double x=pt->fOffsetX+z*pt->fSlopeX;
	double y=pt->fOffsetY+z*pt->fSlopeY;
	double x_plane=pt->fOffsetX-341.700*pt->fSlopeX;
	x_histo->Fill(x_plane);
	//x*=-1;
	//x_tar*=-1;
	//double r=sqrt(x_tar*x_tar+y_tar*y_tar);
	double r=sqrt(x*x+y*y);
	Directionphioff->Fill(track->fDirectionPhioff,r);
	Directionthetaoff->Fill(track->fDirectionThetaoff,r);

	vertex->Fill(vertex_z);

        // if (x,y) is in local coordinates:
        //double X = x;
        //double Y = y;
        //double Theta = 45./180.*3.1415926*(oct-1);
        //x = X*cos(Theta) - Y*sin(Theta);
        //y = X*sin(Theta) + Y*cos(Theta);

	projection->Fill(x,y); 

	//x_histo->Fill(x);
	y_histo->Fill(y);
	//projection_target->Fill(x_tar,y_tar);
	//profile_target->Fill(x,y,chi);
	Xtreeline_histo->Fill(pt->TResidual[1]);
	Utreeline_histo->Fill(pt->TResidual[3]);
	Vtreeline_histo->Fill(pt->TResidual[4]);
	Chi_histo->Fill(pt->fChi);
	//if(x_plane<50 && x_plane>40)
	for(int k=0;k<12;++k)
	  if(pt->fSignedResidual[k]!=-10)
	    Plane_residual[k]->Fill(pt->fSignedResidual[k]);
      }
      }
    }  // end of for loop over events
   
    //cout << "good: " << good << endl;
    cout << "oct: " << oct << endl;
    
    TCanvas* c1=new TCanvas("c","c",1000,600);
    //test_for_fun->Draw();
    /* 
    c->Divide(2,2);
    c->cd(1);
    Xtreeline_histo->Draw();
    c->cd(2);
    Utreeline_histo->Draw();
    c->cd(3);
    Vtreeline_histo->Draw();
    */
    
    TPad* spad1=new TPad("spad1","spad1",.61,.51,.99,.99);
    spad1->Draw();
    spad1->cd();
    vertex->Draw();
    c1->cd();
    TPad* spad2=new TPad("spad2","spad2",.61,0.01,.99,.49);
    spad2->Draw();
    spad2->cd();
    Chi_histo->Draw();
    //x_histo->Draw();
    //test_for_fun->Draw();
    c1->cd();
    TPad* spad3=new TPad("spad3","spad3",.01,.01,.59,.99);
    spad3->Draw();
    spad3->cd();
  
    gStyle->SetPalette(1);
    projection->GetXaxis()->SetTitle("hit global x [cm]");
    projection->GetYaxis()->SetTitle("hit global y [cm]");
    projection->Draw("colz");
    
    projection->SetTitle(Form("run %d: pkg%d projection to Z=%f",run,pkg,z));
    double PI=3.1415926;
    double px[6];
    double py[6];

    double highy,highx,middley,middlex,lowy,lowx;
    if(target=="coll1")
    {
      highy=18.46,middley=12.63,lowy=7.03;
      highx=3.65,middlex=3.65,lowx=1.91;
    } 
    else if (target=="coll2") 
    {
      highy=53.7,middley=37.1,lowy=30.57;
      highx=9.2,middlex=9.2,lowx=6.5;
    }

    double angle=oct==8? 135:(3-oct)*45;
    if(pkg==1)
      angle+=180;
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
    
    
    // TCanvas* c2=new TCanvas("c","c",800,800);
    // vertex->Draw();
    /*if(option==1){
      //c->Divide(1,3);
      c->Divide(1,2);
      c->cd(1);
    gStyle->SetPalette(1);
    projection->GetXaxis()->SetTitle("x axis:cm");
    projection->GetYaxis()->SetTitle("y axis:cm");
    projection->Draw("colz");
    projection->SetTitle(Form("run %d: pkg%d projection to primary collimator",run,pkg));
    
    // ds coll1
    double PI=3.1415926;
    oct=3;
    double angle=oct==8? -135:-(3-oct)*45;
    double Sin=sin(angle*PI/180);
    double Cos=cos(angle*PI/180);
    
    double px[6];
    double py[6];
    //double highy=18.46,middley=12.63,lowy=7.03;
    //double highx=3.65,middlex=3.65,lowx=1.91;
    double highy=53.7,middley=37.1,lowy=30.57;
    double highx=9.2,middlex=9.2,lowx=6.5;
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
    //c->cd(3);
    //Chi_histo->Draw();
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
    */
    
    return;

 }


int getOctNumber(int run){

  if((run>=15025 && run <= 15036) || (run>=13671 && run<=13673) || (run>=8662 && run<=8666) ){
    return 2;
  }
  else if((run>=15037 && run<=15084) || (run>=15111 && run<=15116) || (run>=13676 && run<=13680) || (run>=8668 && run<=8675)){
    return 7;
  }
  else if ((run>=15085 && run<=15087) || (run>=13707 && run<=13708)){
    return 6;
  }
  else if ((run>=15089 && run<= 15110) || (run>=13674 && run<=13675) || (run>=8676 && run<=8679)){
    return 8;
  }
  else{
    return 1;
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

void Draw_time(int pkg==1,int event_start=0,int event_end=-1,int run=8658,string suffix=""){
  // try to get the oct number from the run number

   //bool fDebug=false;
   //string folder= "/scratch/sxyang";
   string folder=gSystem->Getenv("QW_ROOTFILES");
   ostringstream ss;
   ss << folder << "/Qweak_";
   ss << run << suffix;
   ss << ".root";
   string file_name=ss.str();
   cout <<  file_name << endl;
   TFile* file=new TFile(file_name.c_str());
 
     
    //TH2F* projection=new TH2F("projection","projection",240,-30,30,240,-30,30);
    //TH2F* projection=new TH2F("projection","projection",240,-30,30,140,25,60);


    QwEvent* fEvent=0;
    QwHit* hit=0;

    TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
    Int_t nevents=event_tree->GetEntries();
    cout << "total events: " << nevents << endl;
    
    int start=(event_start==-1)? 0:event_start;
    int end=(event_end==-1)? nevents:event_end;
    event_tree->SetBranchStatus("events",1);
    TBranch* event_branch=event_tree->GetBranch("events");
    TBranch* maindet_branch=event_tree->GetBranch("maindet");
    event_branch->SetAddress(&fEvent);
  
    TH1F* r2_time=new TH1F("r2 time","r2 time",150,-50,250);
    TH1F* r3_time=new TH1F("r3 time","r3 time",200,-50,350);
    TH1F* r2_distance=new TH1F("distance","distance",50,0,1);
  
  
  
    for(int i=start;i<end;++i){

      if(i%10000==0)
	cout << "events processed so far: " << i << endl;
      
      event_branch->GetEntry(i);
  
      int nhits=fEvent->GetNumberOfHits();
      for(int j=0;j<nhits;++j){
	hit=fEvent->GetHit(j);
	if(hit->GetRegion()==2){
	  r2_time->Fill(hit->GetTimeNs());
	  if(hit->GetDriftDistance()!=-5)
	    r2_distance->Fill(hit->GetDriftDistance());
	}
	else{
	  r3_time->Fill(hit->GetTimeNs());
	}
      }
    }

    TCanvas* c=new TCanvas("c","c",800,600);
    c->Divide(1,2);
    c->cd(1);
    r2_time->SetMinimum(0);
    r2_time->Draw();
    c->cd(2);
    r2_distance->Draw();
}
