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

gROOT->Reset();
const Int_t kWireNumbers=279;
const Int_t kPlaneNumbers=4;

const Short_t LIBNUM=3;
const Char_t* LIBNAME[LIBNUM]={
    "QwHit","QwHitContainer","QwHitRootContainer"
};

void
check_libraries() {
    for (Short_t i=0;i<3;i++) {
        if (!TClassTable::GetDict(Form("%s",LIBNAME[i]))) {
            gSystem -> Exec( Form("gcc -shared -Wl,-soname,./lib%s.so -o lib%s.so %s/Tracking/src/%s.o %s/Tracking/dictionary/%sDict.o",  LIBNAME[i], LIBNAME[i], gSystem->Getenv("QWANALYSIS"), LIBNAME[i],  gSystem->Getenv("QWANALYSIS"), LIBNAME[i]) );
            gSystem -> Load( Form("%s/Extensions/Macros/Tracking/lib%s.so",  gSystem->Getenv("QWANALYSIS"), LIBNAME[i]) );
        }

    }
}



void find_t0(Int_t event_start=-1,Int_t event_end=-1,Double_t threshold=0.05,Int_t run_number=1672) {



    check_libraries();
    TStopwatch timer;
    timer.Start();

-    TFile *file = new TFile(Form("%s/Qweak_%d.root",gSystem->Getenv("QW_ROOTFILES"),run_number));
    Int_t ev_start=0,ev_end=0;
    Double_t ratio=threshold;
    TH1F * h_p[4];
    for(int i=0;i<4;i++)
	h_p[i]=new TH1F(Form("p%d",i),Form("p%d",i),1500,1000,2500);
//    TH1F* h_p1=new TH1F("p1","p1",1500,1000,2500);
//    TH1F* h_p2=new TH1F("p2","p2",1500,1000,2500);
//    TH1F* h_p3=new TH1F("p3","p3",1500,1000,2500);
//    TH1F* h_p4=new TH1F("p4","p4",1500,1000,2500);


	
    TTree* hit_tree=(TTree*)file->Get("event_tree");
    QwHitRootContainer* hitcontainer=NULL;
    hit_tree->SetBranchAddress("hits",&hitcontainer);
    Int_t nevent=0,nhit=0;
    QwHit* hit=NULL;
    nevent= hit_tree -> GetEntries();
    cout << "number of events: " << nevent << endl;


    if (event_start < 0 || event_end > nevent) {
        ev_start=0;
        ev_end=nevent;
    } else {
        ev_start=event_start;
        ev_end=event_end;
    }


    const Int_t dim=kWireNumbers*kPlaneNumbers;

    Double_t wire[kWireNumbers]={0},t0_max[dim]={0};
    TH1F* time_histo[dim];

    for (int pln=0;pln<kPlaneNumbers;pln++) {
        for (int w=0;w<kWireNumbers;w++) {
            time_histo[pln*kWireNumbers+w]=new TH1F(Form("time spectrum for wire %d in plane %d",w,pln),Form("wire %d in plane %d",w,pln),2500,0,2500);
        }
    }


    for (Int_t i=0;i<kWireNumbers;i++) wire[i]=i+1;    //needs to be fixed later?

    for (int ev_i=ev_start;ev_i<ev_end;ev_i++) {
        tree->GetEntry(ev_i);
	if(ev_i % 1000==0) cout << "events processed so far: " << ev_i << endl;
        nhit=hitcontainer->GetSize();

        for (int hit_i=0;hit_i<nhit;hit_i++) {
            hit = (QwHit*) hitcontainer->GetHit(hit_i);
            
            if (hit->GetHitNumber()==0) {
		if(hit->GetPlane()<5){
                int index=(hit->GetPlane()-1)*kWireNumbers+hit->GetElement()-1;
                time_histo[index]->Fill(-(hit->GetTime()));
		}
		else{
		int index=(hit->GetPlane()-5)*kWireNumbers+hit->GetElement()-1;
		time_histo[index]->Fill(-(hit->GetTime()));
		}
            }
        }
    }



// start to search the t0 through each wire's histograms
    ofstream myfile("t0.txt");
    if (myfile.good())
        myfile << "package=" << 1 << endl;

    for (int pln=0;pln<kPlaneNumbers;pln++) {
        myfile << "plane= " << pln+1 << endl;
	myfile << endl;
        for (int w=0;w<kWireNumbers;w++) {
            int index=pln*kWireNumbers+w;

            t0_max[index]=t0_search(time_histo[index],threshold);
            myfile << w+1 << " " << -t0_max[index] << endl;
	    
//	    if(pln==0) h_p1->Fill(t0_max[index]);
//		else h_p2->Fill(t0_max[index]);
	    h_p[pln]->Fill(t0_max[index]);
        }
	std::cout << "happy here? " << std::endl;
    }


    
/*	
    m1=new TF1("m1","gaus",1800,2200);
    m2=new TF1("m2","gaus",1800,2200);

    TCanvas* c=new TCanvas("c","c",800,600);
    c->Divide(4,2);
    c->cd(1);
    TGraph* g=new TGraph(kWireNumbers,wire,t0_max);
    g->Draw("a*");
    c->cd(2);
    h_p1->Fit(m1,"R");
    h_p1->Draw();
    c->cd(3);
    TGraph* g1=new TGraph(kWireNumbers,wire,&t0_max[kWireNumbers]);
    g1->Draw("a*");
    c->cd(4);
    h_p2->Fit(m2,"R");
    h_p2->Draw();
*/  

    TF1* m[4];
    Double_t par[12];
    TCanvas* c=new TCanvas("c","c",800,600);
    c->Divide(4,2);
    for(int i=0;i<4;i++){
	m[i]=new TF1(Form("m%d",i),"gaus",1800,2200);
	c->cd(2*i+1);
	TGraph* g=new TGraph(kWireNumbers,wire,&t0_max[i*kWireNumbers]);
	g->Draw("a*");
	c->cd(2*i+2);
	h_p[i]->Fit(m[i],"R");
	h_p[i]->Draw();
	m[i]->GetParameters(&par[3*i]);
		}  
    
//    Double_t par[6];
//    m1->GetParameters(&par[0]);
//    m2->GetParameters(&par[3]);



    myfile << "range average: " << endl;
    myfile <<  par[1]-5*par[2] << " " << par[1]+5*par[2] << " " << par[1] << endl;
    myfile <<  par[4]-5*par[5] << " " << par[4]+5*par[5] << " " << par[4] << endl;
    myfile <<  par[7]-5*par[8] << " " << par[7]+5*par[8] << " " << par[7] << endl;
    myfile <<  par[10]-5*par[11] << " " << par[10]+5*par[11] << " " << par[10] << endl;

    myfile.close();

    timer.Stop();
    printf("Time used : CPU %8.2f, Real %8.2f (sec)\n", timer.CpuTime(), timer.RealTime() );

}


double t0_search(TH1F* histo,Double_t ratio) {

    double maxhit=0,max=0,cutoff=0,bin_content=0;
    for (int bin_id=5;bin_id<2500;bin_id++) {
        bin_content=histo->GetBinContent(bin_id);
        if (bin_content>maxhit) {
            maxhit=bin_content;
            cutoff=ratio*maxhit+1;
        }
        if (bin_content>cutoff) max=bin_id;
    }
    return max;
}

