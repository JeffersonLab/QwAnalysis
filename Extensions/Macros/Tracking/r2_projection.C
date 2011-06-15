
// author: Siyuan Yang
//       : College of William & Mary
//       : sxyang@email.wm.edu
// calculate the residual from combinations of hits
 
#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>


using namespace std;
const int layers=12;
const int ndirs=3;
const double wirespace=1.1684;
const double offset=-18.1102;
const double location[layers]={-338.10,-336.122,-334.144,-332.166,-330.188,-328.120,-295.400,-293.422,-291.444,-289.466,,-287.488,-285.510};
const double centery[layers]={51.013,50.711,50.955,51.013,50.711,50.955,55.013,54.711,54.955,55.013,54.711,54.955};






 void reconstruction(Int_t event_start=0,Int_t event_end=-1,Int_t run_start=8658,Int_t run_end=8659){
    
    TChain* chain=new TChain("event_tree");
    for(int run=run_start;run<run_end;run++){
    string file_name= Form ( "%s/Qweak_%d.root",gSystem->Getenv ( "QW_ROOTFILES" ),run );
    chain->Add(file_name.c_str());
    }


   
    TH1F* vertex=new TH1F("vertex in Z","vertex in Z",300,-1000,-400);
    TH1F* vertexr=new TH1F("vertex in R","vertex in R",100,0,10);
    TH1F* test=new TH1F("test","test",100,-1,1);
    TH2F* projection=new TH2F("projection","projection",80,-20,20,100,-10,40);
    TProfile2D* projection_profile=new TProfile2D("projection related with number of hits","projection related with hits",80,-20,20,100,-10,40);
    //TH1F* slopex=new TH1F("slope in x","slope in x",100,-0.2,0.2);
    
    // only calcualte from those events which has tracks
    QwEvent* fEvent=0;
    QwTrack* track=0;
    QwPartialTrack* pt=0;
    QwHit* hit=0;
   
   
    chain->SetBranchAddress ( "events",&fEvent);
    Int_t nevents=chain->GetEntries();

    if(event_end==-1)
      event_end=nevents;

    for(int i=event_start;i<event_end;i++){

      if(i%10000==0)
	cout << "events processed so far: " << i << endl;
      
      chain->GetEntry(i);

      double ntracks=0;
      ntracks=fEvent->GetNumberOfTracks();

      //if(ntracks!=0) cout << "event " << i << " has a track!" << endl;
      //fit will store the final results
      double fit[4]={0};
      
      //get rid of those events which has two partial tracks
      double npts=0;
      npts=fEvent->GetNumberOfPartialTracks();

     
      // call build function
    
      for(int j=0;j<npts;j++){
	pt=fEvent->GetPartialTrack(j);
	if(pt->GetRegion()!=2) continue;
	
		double vertexz=-(pt->fSlopeX*pt->fOffsetX + pt->fSlopeY*pt->fOffsetY)/(pt->fSlopeX*pt->fSlopeX+pt->fSlopeY*pt->fSlopeY);
		double vertex_r=sqrt((pt->fSlopeX*vertexz+pt->fOffsetX)*(pt->fSlopeX*vertexz+pt->fOffsetX)+(pt->fSlopeY*vertexz+pt->fOffsetY)*(pt->fSlopeY*vertexz+pt->fOffsetY));
	
	projection->Fill(pt->fOffsetX-568.17*pt->fSlopeX,pt->fOffsetY-568.17*pt->fSlopeY);
	// projection_profile->Fill(pt->fOffsetX-568.17*pt->fSlopeX,pt->fOffsetY-568.17*pt->fSlopeY,pt->fChi,1);
	// projection_profile->Fill(pt->fOffsetX-568.17*pt->fSlopeX,pt->fOffsetY-568.17*pt->fSlopeY,hit_in_r2,1);
      }
       } 
   
   
    TCanvas* c=new TCanvas("c","c",800,600);
    vertex->Draw();
    
    gStyle->SetPalette(1);
    projection->SetTitle("projection to collmator 2: z=-568.17");
    projection_profile->GetXaxis()->SetTitle("x axis:cm");
    projection_profile->GetYaxis()->SetTitle("y axis:cm");
    projection->Draw("colz");
    //projection_profile->SetMaximum(30);
    //projection_profile->SetMinimum(6);
    //projection_profile->Draw("colz");
    
    
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
    t1->Draw("same");
    t2->Draw("same");
    t3->Draw("same");
    t4->Draw("same");
    t5->Draw("same");
    t6->Draw("same");
    
    return;

 }

void buildtrack(double slope1,double offset1,double slope2,double offset2,double slope3,double offset3,double* fit)
{

  // cout << "slope1: " << slope1 << " " << offset1 << " " << slope2 << " " << offset2 << endl;
  double zx=-338.10,zu=-336.122,zv=-334.144;
  double centerx=51.013,centeru=50.711,centerv=50.955;
  double wirespace=1.1684,first=-18.1102;
  double slopex=slope1,offsetx=offset1;
  double slopeu=slope2,offsetu=offset2;
  double slopev=slope3,offsetv=offset3;

  double cosu=0.6,sinu=-0.8;
  double cosv=0.6,sinv=0.8;

  double x=offsetx+slopex*zx;
  double u=offsetu+slopeu*zx-(centeru-centerx)*cosu;
  double v=offsetv+slopev*zx-(centerv-centerx)*cosv;

  double ux=u*cosu,uy=u*sinu;
  double vx=v*cosv,vy=v*sinv;

  x-=0.5*wirespace;
  x+=first;

  ux-=0.5*wirespace;
  ux+=first;

  vx-=0.5*wirespace;
  vx+=first;
  //three planes
  // first x
  double para[3][4];
  para[0][0]=1;
  para[0][1]=0;
  para[0][2]=-slopex;
  para[0][3]=-(x+para[0][2]*zx);

  // first u
  para[1][0]=-0.75;
  para[1][1]=1;
  para[1][2]=fabs(slopeu*para[1][0]);
  para[1][3]=-(para[1][0]*ux+para[1][1]*uy+para[1][2]*zx);

 // first v
  para[2][0]=0.75;
  para[2][1]=1;
  para[2][2]=-fabs(slopev*para[2][0]);
  para[2][3]=-(para[2][0]*vx+para[2][1]*vy+para[2][2]*zx);
 


  // set two points
  double z1=-63,z2=-439;
  // question is: how do we want to combine, three optionsvoid 
  double x1=0,x2=0,y1=0,y2=0;
  double i=1,j=2; // means u,v
  
  y1=((para[i][0]*para[j][2]-para[i][2]*para[j][0])*z1+(para[i][0]*para[j][3]-para[i][3]*para[j][0]))/(para[i][1]*para[j][0]-para[i][0]*para[j][1]);
  x1=-(para[i][1]*y1+para[i][2]*z1+para[i][3])/para[i][0];

  y2=((para[i][0]*para[j][2]-para[i][2]*para[j][0])*z2+(para[i][0]*para[j][3]-para[i][3]*para[j][0]))/(para[i][1]*para[j][0]-para[i][0]*para[j][1]);
  x2=-(para[i][1]*y2+para[i][2]*z2+para[i][3])/para[i][0];

  //fit[3]=(x1-x2)/(z1-z2);
  //fit[2]=x1-fit[1]*z1;
 

  fit[1]=(y1-y2)/(z1-z2);
  fit[0]=y1-fit[1]*z1;

  fit[3]=slopex;
  fit[2]=offsetx+centerx+first-0.5*wirespace;
  //fit[2]+=centerx;
  
  // cout << "x,y(z=0)=(" << fit[0] << "," << fit[2] << ") d(x,y)/dz=(" << fit[1] << "," << fit[3] << ")" << endl;  
  return;
}

/*

 void signed_check(Int_t event_start=0,Int_t event_end=-1,Int_t run_start=8658,Int_t run_end=8659){
    
    TChain* chain=new TChain("event_tree");
    for(int run=run_start;run<run_end;run++){
    string file_name= Form ( "%s/Qweak_%d_base.root",gSystem->Getenv ( "QW_ROOTFILES" ),run );
    chain->Add(file_name.c_str());
    }


   
    double signed[24]={0};
 

    
    QwEvent* fEvent=0;
    QwTrack* track=0;
    QwPartialTrack* pt=0;

  
    chain->SetBranchAddress ( "events",&fEvent);
    Int_t nevents=chain->GetEntries();

    
    if(event_end==-1)
      event_end=nevents;

    for(int i=event_start;i<event_end;i++){

      if(i%10000==0)
	cout << "events processed so far: " << i << endl;
      
      chain->GetEntry(i);

      double ntracks=0;
      ntracks=fEvent->GetNumberOfTracks();

      if(ntracks!=0){

	double npts=fEvent->GetNumberOfPartialTracks();
      }

*/
