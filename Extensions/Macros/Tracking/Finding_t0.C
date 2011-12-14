//    Name:   Finding_t0.C
//    Author: Siyuan Yang
//    Email:  sxyang@wm.edu
//    Date:   Apr 12 2010
//    This script is used to find out the t0 value for every wire in R3(R2) and output them into the t0.txt file.
//    Before using this script, the SubtractWireTimeOffset function in QwDriftChamberVDC::ProcessEvent must be disabled.
//    Then run the QwAnlysis and get the corresponding root file. This script will use that root file(without finding t0   
//    module implemented) to find out t0 of each wire. After getting a complete t0.txt, you can enable the SubtractWireTime
//    Offset function and let every wire subtract the correct t0 value to get the real drift time.

//    the main function is called find_t0, which has the following form:
//    void find_t0(Int_t event_start=-1,Int_t event_end=-1,Double_t threshold=0.05,Int_t pl=1)
//    you can decide how many events you want to use to determine the t0 by assigning the start and end event number.
//    However, I recommend using all events in that rootfile becasue bigger statistical sample could yield more accurate
//    results.
//    the threshold is the value to determine the noise quantitiy. So if you set a very high threshold, almost all         
//    the noise could be filtered out but you also kill some of the valid hits; if you set a very low threshold, you will  
//    include some noise which makes the t0 not so accruate. For R3, we tested different threshold value and found out     
//    5%-8% was the optimal range.
//    last parameter is the plane number. Right now, this function is used to find out t0 of wires in a single plane. For  
//    R3, you can choose either plane 1 or plane 2.

//    if you have any questions about this script, just send an email to me.


#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <exception>
#include <TMath>


const Int_t kWireNumbers=279;
const Int_t kPlaneNumbers=8;


void find_t0(int event_start=-1,int event_end=-1,Double_t threshold=0.05,Int_t run_number=5150) {

    TStopwatch timer;
    timer.Start();

    string file_name= Form("%s/Qweak_%d.root",gSystem->Getenv("QW_ROOTFILES"),run_number);
    std::cout << "file name: " << file_name << std::endl;
    TFile *file = new TFile(file_name.c_str()); 

    
    int ev_start=0,ev_end=0;
    Double_t ratio=threshold;
    TH1F * h_p[kPlaneNumbers];

    QwEvent* fEvent=0;
    QwHit* hit=0;
    
    for(int i=0;i<kPlaneNumbers;++i)
	h_p[i]=new TH1F(Form("p%d",i),Form("p%d",i),1200,500,1700);

	
    TTree* event_tree=( TTree* )file->Get("event_tree");
    //   QwHitRootContainer* hitcontainer=NULL;
    //   hit_tree->SetBranchAddress("hits",&hitcontainer);
    event_tree->SetBranchStatus("events",1);
    TBranch* event_branch=event_tree->GetBranch("events");
    event_branch->SetAddress(&fEvent);   
    Int_t nevents = event_tree -> GetEntries();
    cout << "number of events: " << nevents << endl;


    if (event_start < 0 || event_end > nevents) {
        ev_start=0;
        ev_end=nevents;
    } else {
        ev_start=event_start;
        ev_end=event_end;
    }


    const Int_t dim=kWireNumbers*kPlaneNumbers;

    Double_t wire[kWireNumbers]={0},t0_max[dim]={0};
    TH1F* time_histo[dim];

    for (int pln=0;pln<kPlaneNumbers;pln++) {
        for (int w=0;w<kWireNumbers;w++) {
	  time_histo[pln*kWireNumbers+w]=new TH1F(Form("time spectrum for wire %d in plane %d",w,pln),Form("wire %d in plane %d",w,pln),300,400,1300);
        }
    }

    TH1F* dt_draw=new TH1F("","",250,-100,650);
    for (Int_t i=0;i<kWireNumbers;i++) wire[i]=i+1;    //needs to be fixed later?

    for (int ev_i=ev_start;ev_i<ev_end;++ev_i) {
      event_branch->GetEntry(ev_i);
	if(ev_i % 10000==0) cout << "events processed so far: " << ev_i << endl;
        int nhits=fEvent->GetNumberOfHits();

        for (int hit_i=0;hit_i<nhits;++hit_i) {
  	hit =(QwHit*) fEvent->GetHit(hit_i);
            
	if (hit->GetRegion()==3) {	
		if(hit->GetPackage()==1){
                int index=(hit->GetPlane()-1)*kWireNumbers+hit->GetElement()-1;
                time_histo[index]->Fill(-hit->GetTime());
	       	}
		else{
		int index=(hit->GetPlane()+3)*kWireNumbers+hit->GetElement()-1;
		time_histo[index]->Fill(-hit->GetTime());
		}
            }
        }
    }

// start to search the t0 through each wire's histograms
    ofstream myfile("t0.txt");
    if (myfile.good()){
	for(int package=1;package<3;++package){	
		myfile << "package=" << package << endl;
		Int_t plane_min=0,plane_max=0;
		if(package==1) {plane_min=1;plane_max=4;}
			else {plane_min=5;plane_max=8;}	
	  for (int pln=plane_min-1;pln<plane_max;++pln) {
            myfile << "plane= " << pln+1 << endl;
	    myfile << endl;
               for (int w=0;w<kWireNumbers;++w) {
                  int index=pln*kWireNumbers+w;
		  t0_max[index]=t0_search(time_histo[index],ratio);
		  myfile << w+1 << " " << -t0_max[index] << endl;
		  h_p[pln]->Fill(t0_max[index]);
	       }
	  }
	}
}    

    //TF1* m[kPlaneNumbers];
    TFile* fp=new TFile("t0_result.root","RECREATE");
    Double_t par[12];
    //TCanvas* c=new TCanvas("c","c",800,600);
    //c->Divide(4,2);
    for(int i=0;i<8;++i){
	//m[i]=new TF1(Form("m%d",i),"gaus",1800,2200);
	TCanvas* c=new TCanvas(Form("plane%d",i+1),Form("plane%d_t0 distribution by wire",i+1),800,600);

	TGraph* g=new TGraph(kWireNumbers,wire,&t0_max[i*kWireNumbers]);
	g->Draw("a*");
        g->GetXaxis()->SetTitle("wire");
	g->GetYaxis()->SetTitle("t0:ns");
	c->Write();
	h_p[i]->Write();
	delete g;
	delete c;
		}  

    myfile.close();
    timer.Stop();
    printf("Time used : CPU %8.2f, Real %8.2f (sec)\n", timer.CpuTime(), timer.RealTime() );

}


double t0_search(TH1F* histo,Double_t ratio) {

    double maxhit=0,max=0,cutoff=0,bin_content=0;
    for (int bin_id=1;bin_id<200;++bin_id) {
        bin_content=histo->GetBinContent(bin_id);
        if (bin_content>maxhit) {
            maxhit=bin_content;
            cutoff=ratio*maxhit+1;
        }
        if (bin_content>cutoff) max=3*bin_id+400;
    }
    return max;
}

