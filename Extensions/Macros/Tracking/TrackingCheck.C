
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


void efficiency(int ev_start=-1,int ev_end=-1,Int_t run_number=6327,const string subsystem="r3"){
 
    Int_t start=0,end=0;
    string file_name= Form ( "%s/Qweak_%d.root",gSystem->Getenv ( "QWSCRATCH" ),run_number );
    TFile *file = new TFile ( file_name.c_str() );
    ofstream events_log;
    events_log.open(Form("%s/failed_events_list.txt",gSystem->Getenv ( "QWSCRATCH" )));
           
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
        if(i%1000==0)
        cout << "events process so far: " << i << endl;  
        event_tree->GetEntry(i);
        int nhits=fEvent->GetNumberOfHits();
        int npts=fEvent->GetNumberOfPartialTracks();
        int a[8]={0};            //represents eight planes, in case both packages get hits
        bool has_pt=false;
        for(int j=0;j<npts;j++){
                pt=fEvent->GetPartialTrack(j);
                if(pt->GetRegion()==3){
                        total_pt++;
                        has_pt=true;
                }
        }         
                      
        for(int j=0;j<nhits;j++){
                hit=fEvent->GetHit(j);
                if(hit->GetRegion()==3){
                   a[(hit->GetPackage()-1)*4+hit->GetPlane()-1]++;
                }
        }
        
                if(a[0]>=4&&a[1]>=4&&a[2]>=4&&a[3]>=4){
                        qualified_events++;
                        if(has_pt==false)
                             events_log << "event: " << i+1 << endl;
                }
                else if(a[4]>=4&&a[5]>=4&&a[6]>=4&&a[7]>=4){
                        qualified_events++;
                        if(has_pt==false)
                             events_log << "event: " << i+1 << endl;       
                }
                
        //NOTE:adding one more funtion in to print out the failed events
        }

        cout << "efficiency: " << (double)total_pt/qualified_events << endl;
        events_log.close();
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
    
    event_tree->Project("dt","fQwHits.fTimeNs","fQwHits.fRegion==3");
    
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


void bridge(int run=5148){
    string file_name= Form ( "%s/Qweak_%d_test.root",gSystem->Getenv ( "QWSCRATCH" ),run );
    TFile *file = new TFile ( file_name.c_str() );
           
    TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
    QwEvent* fEvent=0;
    QwPartialTrack* pt=0;
    TH2F* histo=new TH2F("r2 vs r3","r2 vs r3",100,-50,50,100,-50,50);
    event_tree->SetBranchAddress("events",&fEvent);
    int nevents=event_tree->GetEntries();
    for(int i=0;i<nevents;i++){
        if(nevents/1000==0) cout << "events process so far: " << i << std::endl;
        event_tree->GetEntry(i);
        int npts=fEvent->GetNumberOfPartialTracks();
        double r2=0,r3=0;
        for(int j=0;j<npts;j++){
                pt=fEvent->GetPartialTrack(j);
                if(pt->GetRegion()==2)
                        r2=pt->fSlopeY/pt->fSlopeX;
                else if(pt->GetRegion()==3)
                        r3=pt->fSlopeY/pt->fSlopeX;
        }
        if(r2 !=0 && r3!=0)
                  histo->Fill(r2,r3);
        }
        histo->Draw();
}


void vertex(double dz=-500,int run=5148){
    string file_name= Form ( "%s/Qweak_%d.root",gSystem->Getenv ( "QWSCRATCH" ),run );
    TFile *file = new TFile ( file_name.c_str() );
           
    TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
    QwEvent* fEvent=0;
    QwPartialTrack* pt=0;
    TH2F* histo=new TH2F(Form("project to z=%f",dz),Form("project to z=%f",dz),1000,-50,50,1000,-50,50);
    TH1F* hx=new TH1F("vertex x","vertex x",300,-700,-400);
    TH1F* hy=new TH1F("vertex y","vertex y",300,-700,-400);
    TCanvas* c=new TCanvas("c","c",600,600);
    c->Divide(1,2);
    TBranch* branch_event=event_tree->GetBranch("events");
    TBranch* branch_trig=event_tree->GetBranch("trigscint");
    TBranch* branch_main=event_tree->GetBranch("maindet");
    branch_event->SetAddress(&fEvent);
    TLeaf* trig=branch_trig->GetLeaf("ts1mt_f1");
    TLeaf* main=branch_main->GetLeaf("md5m_f1");
//     event_tree->SetBranchAddress("events",&fEvent);
    int nevents=event_tree->GetEntries();
    int counts=0;
    int trig_counts=0;
    int total_counts=0;
    int only_r3=0,only_r2=0,r23=0;
    for(int i=0;i<nevents;i++){
        if(i%1000==0) cout << "events process so far: " << i << std::endl;
//         event_tree->GetEntry(i);
        branch_event->GetEntry(i);
        branch_trig->GetEntry(i);
        branch_main->GetEntry(i);
        int value=trig->GetValue();
        int npts=fEvent->GetNumberOfPartialTracks();
        bool trig_fired=false,main_fired=false;
        if(trig->GetValue()!=0){
                trig_fired=true;
                trig_counts++;
                }
        if(main->GetValue()!=0)
                main_fired=true;
        if(main->GetValue()!=0 && trig->GetValue()!=0)
             total_counts++;   
        bool r2=false,r3=false;
        for(int j=0;j<npts;j++){
                pt=fEvent->GetPartialTrack(j);
                double zx=0,zy=0,x=0,y=0;
                if(pt->GetRegion()==2 && pt->GetPackage()==1 ){
                    r2=true;
                    zx=-pt->fOffsetX/pt->fSlopeX;
                    zy=-pt->fOffsetY/pt->fSlopeY;
                    x=pt->fSlopeX*dz+pt->fOffsetX;
                    y=pt->fSlopeY*dz+pt->fOffsetY;
                }
                else if(pt->GetRegion()==3 && pt->GetPackage()==1)
                    r3=true;
                }
                
                
                if(r2==true && trig_fired==true && main_fired ==true)
                        counts++;    
           /*if(zx!=0 && zy!=0){
                hx->Fill(zx);
                hy->Fill(zy);
                histo->Fill(x,y);
               } */   
        }
                
//         gStyle->SetPalette(1);
//         c->cd(1);
// //         histo->Draw("colz");
//         hx->Draw();
//         c->cd(2);
//         hy->Draw();
        std::cout << "both trig and main fired: " << total_counts << std::endl;
        std::cout << "only trig fired: " << trig_counts << std::endl;
        std::cout << "within trig_counts: " << counts  << " events have both r2 and r3 tracks." << std::endl;
}
