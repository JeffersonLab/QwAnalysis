
// author: Siyuan Yang
//       : College of William & Mary
//       : sxyang@email.wm.edu
//this script contains many tools(functions) to diagnose the quality of treelines in R3. 
///efficiency will return how many qualified events does specific coda run have; we define the qualified events as every plane must have minimum number of hits required to perform the pattern recognition search.
/// slope function will show the slope diagnosis information, as the name of every graph suggests. 
/// the last one will print the average residual of plane0 and single plane.
 
#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>


using namespace std;

const double wirespace=0.496965;


void efficiency(int ev_start=-1,int ev_end=-1,Int_t run_number=6327){
 
    Int_t start=0,end=0;
    string file_name= Form ( "%s/Qweak_%d.root",gSystem->Getenv ( "QWSCRATCH" ),run_number );
    TFile *file = new TFile ( file_name.c_str() );
    ofstream ifile;
    ifile.open("failed_events_list.txt");
           
    TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
    QwEvent* fEvent=0;
    QwHit* hit=0;
    QwPartialTrack* pt=0;

    event_tree->SetBranchAddress ( "events",&fEvent );
    Int_t nevent=event_tree->GetEntries();



    if(ev_start==-1 && ev_end==-1) {start=0;end=nevent;}
        else {
        start=ev_start;end=ev_end;}
    

    int qualified_events=0;
    int total_pt=0;
    for(int i=start;i< end;i++){
        event_tree->GetEntry(i);
        int nhits=fEvent->GetNumberOfHits();
        int npts=fEvent->GetNumberOfPartialTracks();
        int a[8]={0};            //represents eight planes, in case both packages get hits
        for(int j=0;j<npts;j++){
                pt=fEvent->GetPartialTrack(j);
                if(pt->GetRegion()==3)
                        total_pt++;}         
                      
        for(int j=0;j<nhits;j++){
                hit=fEvent->GetHit(j);
                if(hit->GetRegion()==3){
                   a[(hit->GetPackage()-1)*4+hit->GetPlane()-1]++;
                }
        }
        
                if(a[0]>=4&&a[1]>=4&&a[2]>=4&&a[3]>=4){
                        qualified_events++;
                }
                else if(a[4]>=4&&a[5]>=4&&a[6]>=4&&a[7]>=4){
                        qualified_events++;
                }
        }

        cout << "efficiency: " << (double)total_pt/qualified_events << endl;
        
    return;
}



void slope(int ev_start=-1,int ev_end=-1,int run_number=6327){
    

    Int_t start=0,end=0;
    string file_name= Form ( "%s/Qweak_%d.root",gSystem->Getenv ( "QWSCRATCH" ),run_number );
    TFile *file = new TFile ( file_name.c_str() );
 
    TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
    QwEvent* fEvent=0;
    QwTrackingTreeLine* tl=0;
    QwPartialTrack* pt=0;

    event_tree->SetBranchAddress ( "events",&fEvent );
    Int_t nevent=event_tree->GetEntries();
    // define the histograms needed later
    TH1F* uslope=new TH1F("slope in u direction","uslope",400,-2,2);
    TH1F* vslope=new TH1F("slope in v direction","vslope",400,-2,2);
    TH1F* uslope_diff=new TH1F("slope_diff in u direction","uslope_diff",200,-1,1 );
    TH1F* vslope_diff=new TH1F("slope_diff in v direction","vslope_diff",200,-1,1 );

    if(ev_start==-1 && ev_end==-1) {start=0;end=nevent;}
        else {
        start=ev_start;end=ev_end;}

    for(int i=start;i< end;i++){
        event_tree->GetEntry(i);
        if(i%1000==0) cout << "events process so far: " << i << endl;
        int ntls=fEvent->GetNumberOfTreeLines();
        int npts=fEvent->GetNumberOfPartialTracks();

        //u,v direction
        double slope[2];
        double fslope[2];
        double bslope[2];
        int reverse=1;        
        bool r3=false;    // if there's any r3 track
        for(int j=0;j<npts;j++){
                pt=fEvent->GetPartialTrack(j);
                if(pt->GetRegion()==3 && r3==false){
                 r3=true;
                 if(pt->GetPackage()==1) reverse=-1;
                        else reverse=1;
                 }
        }
        
        if(r3==false) continue;

        for(int j=0;j<ntls;j++){
                
                treeline=fEvent->GetTreeLine(j);
                
                if(treeline->GetRegion()==3 && treeline->GetPlane()==0)
                slope[treeline->GetDirection()-3]=treeline->GetSlope();
                if(treeline->GetRegion()==3 && treeline->GetPlane()!=0){
                        if(treeline->GetPlane()<3)
                          fslope[treeline->GetDirection()-3]=reverse*treeline->GetSlope();
                        else
                          bslope[treeline->GetDirection()-3]=reverse*treeline->GetSlope();
                }
        }
        uslope->Fill(slope[0]);
        vslope->Fill(slope[1]);
//         if(((fslope[0]+bslope[0])/(2*wirespace)-slope[0])>1) cout << "event: " << i << endl;
        
        uslope_diff->Fill((fslope[0]+bslope[0])/(2*wirespace)-slope[0]);
        vslope_diff->Fill((fslope[1]+bslope[1])/(2*wirespace)-slope[1]);
        }
        
        TCanvas* c=new TCanvas("slope","slope",600,600);
        c->Divide(2,2);
        c->cd(1);
        uslope->SetLineColor(2);
        uslope->SetTitle("slope in u direction");
        uslope->Draw();
        c->cd(2);
        vslope->SetLineColor(3);
        vslope->SetTitle("slope in v direction");
        vslope->Draw();
        c->cd(3);
        uslope_diff->SetLineColor(4);
        uslope_diff->SetTitle("slope difference in u direction");
        uslope_diff->Draw();
        c->cd(4);
        vslope_diff->SetLineColor(6);
        vslope_diff->SetTitle("slope difference in v direction");
        vslope_diff->Draw();
    return;
}



void residual(int run_number=6327){
    string file_name= Form ( "%s/Qweak_%d.root",gSystem->Getenv ( "QWSCRATCH" ),run_number );
    TFile *file = new TFile ( file_name.c_str() );
 
    TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
    QwEvent* fEvent=0;
    QwTrackingTreeLine* tl=0;
    QwPartialTrack* pt=0;
    TH1F* dd=new TH1F("dd","dd",500,-5,5);
    TH1F* dt=new TH1F("dt","dt",700,-100,600);
    TH1F* ar_0=new TH1F("ar_0","ar_0",100,0,0.5);
    TH1F* ar_1=new TH1F("ar_1","ar_1",100,0,0.5);
    
    event_tree->Project("dt","fQwHits.fTime","fQwHits.fRegion==3");
    
    event_tree->Project("dd","fQwHits.fDistance","fQwHits.fRegion==3");
    
    event_tree->Project("ar_0","fQwTreeLines.fAverageResidual","fQwTreeLines.fPlane==0&&fQwTreeLines.fRegion==3");
    
    event_tree->Project("ar_1","fQwTreeLines.fAverageResidual","fQwTreeLines.fPlane!=0&&fQwTreeLines.fRegion==3");
    ar_0->SetLineColor(kRed);
    ar_1->SetLineColor(kBlue);
    ar_0->SetTitle("average residual in plane 0");
    ar_1->SetTitle("average residual in single plane");
        
    TCanvas* c=new TCanvas("ar","ar",700,600);
    c->Divide(2,2);
    c->cd(1);
    dt->Draw();
    c->cd(2);
    dd->Draw();
    c->cd(3);
    ar_0->Draw();
    c->cd(4);
    ar_1->Draw();
    return;
}
