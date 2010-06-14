//    Name:   Finding_t0.C
//    Author: Siyuan Yang
//    Email:  sxyang@wm.edu
//    Date:   Apr 12 2010
//    This script is used to find out the t0 value for every wire in R3(R2) and output them into the t0.txt file.
//    Before using this script, the SubtractWireTimeOffset function in QwDriftChamberVDC::ProcessEvent must be disabled.
//    Then run the QwAnlysis and get the corresponding root file. This script will use that root file(without finding t0   //    module implemented) to find out t0 of each wire. After getting a complete t0.txt, you can enable the SubtractWireTime//    Offset function and let every wire subtract the correct t0 value to get the real drift time. 

//    the main function is called find_t0, which has the following form:
//    void find_t0(Int_t event_start=-1,Int_t event_end=-1,Double_t threshold=0.05,Int_t pl=1)
//    you can decide how many events you want to use to determine the t0 by assigning the start and end event number. 
//    However, I recommend using all events in that rootfile becasue bigger statistical sample could yield more accurate 
//    results.
//    the threshold is the value to determine the noise quantitiy. So if you set a very high threshold, almost all         //    the noise could be filtered out but you also kill some of the valid hits; if you set a very low threshold, you will  //    include some noise which makes the t0 not so accruate. For R3, we tested different threshold value and found out     //    5%-8% was the optimal range.
//    last parameter is the plane number. Right now, this function is used to find out t0 of wires in a single plane. For  //    R3, you can choose either plane 1 or plane 2. 

//    if you have any questions about this script, just send an email to me.








#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <exception>
#include <TMath>

gROOT->Reset();
const Int_t WireNumbers=279;


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



void find_t0(Int_t event_start=-1,Int_t event_end=-1,Double_t threshold=0.05,Int_t pl=1) {



    check_libraries();
    TStopwatch timer;
    timer.Start();


    TFile *file = new TFile(Form("%s/rootfiles/Qweak_1821.root",gSystem->Getenv("QWANALYSIS")));
    Int_t plane=pl,ev_start=0,ev_end=0;
    Double_t ratio=threshold;

    ofstream myfile("t0.txt");
    if (myfile.good())
	myfile << "package=" << 1 << endl;
        myfile << "plane=" << plane << endl;

    TTree* hit_tree=(TTree*)file->Get("tree");
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

    Float_t wire[WireNumbers]={0},t0_max[WireNumbers]={0};
    TH1F* time_histo[WireNumbers];
    for (int i=0;i<WireNumbers;i++) {
        time_histo[i]=new TH1F(Form("time spectrum for wire %d",i),Form("wire %d",i),2000,0,2000);
    }
    
    for (Int_t i=0;i<WireNumbers;i++) wire[i]=i+1;

    int wire_id=0;
    for (int ev_i=ev_start;ev_i<ev_end;ev_i++) {
        tree->GetEntry(ev_i);
        nhit=hitcontainer->GetSize();

        for (int hit_i=0;hit_i<nhit;hit_i++) {
            hit = (QwHit*) hitcontainer->GetHit(hit_i);
            wire_id=hit->GetElement();
            if (hit->GetPlane()==1&&hit->GetHitNumber()==0)
                time_histo[wire_id-1]->Fill(-(hit->GetTime()));
        }
    }

	for(int j=0;j<WireNumbers;j++){          //find t0 for every single wire
	double maxhit=0,max=0;
	for(int bin_id=5;bin_id<2000;bin_id++){
		if(time_histo[j]->GetBinContent(bin_id)>maxhit) maxhit=time_histo[j]->GetBinContent(bin_id);}
	int cutoff=ratio*maxhit+1;
	for(int bin_id=1000;bin_id<2000;bin_id++){
		if(time_histo[j]->GetBinContent(bin_id)>cutoff) max=bin_id;
		}
	
	t0_max[j]=max;
	myfile << j+1 << " " << t0_max[j] << endl;
}

    myfile.close();

    timer.Stop();

    TGraph* g=new TGraph(WireNumbers,wire,t0_max);
    g->Draw("a*");

    printf("Time used : CPU %8.2f, Real %8.2f (sec)\n", timer.CpuTime(), timer.RealTime() );

}
