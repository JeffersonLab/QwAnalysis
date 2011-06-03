
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

const int planes=6;
using namespace std;


void clean_r2(Int_t run_start=8598,Int_t run_end=8600,Double_t width=5.84){
    Int_t start=0,end=0;
    const int td=130;
    const int t0=25;
    TChain* chain=new TChain("event_tree");
    for(int run=run_start;run<run_end;run++){
    string file_name= Form ( "%s/Qweak_%d_convert.root",gSystem->Getenv ( "QW_ROOTFILES" ),run );
    chain->Add(file_name.c_str());
    }

    TH1F* final=new TH1F("clean time","clean_time",500,-100,400);
    QwEvent* fEvent=0;
    QwHit* hit=0;

   
    chain->SetBranchAddress ( "events",&fEvent );
    Int_t nevents=chain->GetEntries();

    cout << "total events:  " << nevents << endl;
    vector<double> time;
    Int_t count=0;
    for(int i=0;i<50;i++){
        if(i%10000==0)
        cout << "events process so far: " << i << endl;  
        chain->GetEntry(i);

        int nhits=fEvent->GetNumberOfHits();
	// only require the first six planes get one hit per plane

        int a[planes]={0};
        bool clean_event=true;
	bool single_pkg=true;
	if(nhits==0) continue;
        int pkg=fEvent->GetHit(0)->GetPackage();
	//cout << "nhits: " << nhits << endl;
        for(int j=0;j<nhits;j++){
                hit=fEvent->GetHit(j);
		if(hit->GetPackage()!=pkg && single_pkg==true){
		  single_pkg=false;
		}
		if(hit->GetRegion()==2 && hit->GetPlane()<=planes && hit->GetHitNumber()==0){
                a[hit->GetPlane()-1]++;
		time.push_back(hit->GetTime());
		}
                 }
        
	
        for(int j=0;j<planes;j++){
	  if(pkg==2 && j==9) continue;
	  //cout << "a[" << j << "]=" << a[j] << endl;
	  if(a[j]>1 || a[j]==0){
	    clean_event=false;
	    break;
	    }
	}



	if(clean_event && single_pkg==true){
	  cout << "event: " << i << endl;
	  count++;
	  //cout << "qualified event: " << i << endl;
	  for(int j=0;j<time.size();j++){
	    // if(hitnumber.at(j)==0)
	    double dtime=time.at(j);
	       if(dtime >=t0 && dtime < (td+t0))
	      final->Fill(dtime);
	  }
	}
     	  time.clear();
        }
    TCanvas* c=new TCanvas("c","c",800,800);
    c->Divide(2,2);
    c->cd(1);
    final->Draw();
    c->cd(2);
    // start to convert time to distance

    final->GetXaxis()->SetLimits(-(100+t0),(400-t0));
    final->Draw();
    
     c->cd(3);
     int K=final->Integral();
     cout << "K:"<< K << endl;
     int integral=0;
     // upper limit of the drift time,which corresponds to the width
     
     double drifttime[td]={0};
     double distance[td]={0};
     
     for (Int_t bin=1;bin<=(td+100+t0);bin++)
    {
     
      double time_temp=(bin-1)-(100+t0);        //start from lower bound
	 if (time_temp<0) continue;
	 integral+=(final->GetBinContent(bin));
	 drifttime[bin-(100+t0+1)]=time_temp;
	 distance[bin-(100+t0+1)]=width*integral/K;
	 cout << drifttime[bin-(100+t0+1)] << " " << distance[bin-(100+t0+1)] << endl;
    }
     cout << "count: " << count << endl;
    TGraph* tg=new TGraph(td,drifttime,distance);
    TF1* fit=new TF1("m6","pol6",0,td-1);
    tg->SetFillColor(2);
    tg->Fit("m6","R");
    tg->Draw("ap");
    return;
}
