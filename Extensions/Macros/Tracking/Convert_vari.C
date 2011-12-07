//    Name:   Cluster.C
//    Author: Siyuan Yang
//    Email:  sxyang@email.wm.edu
//    Date:   Sep 12 2010
// This script is used to find the t-x relationship in R3. It is supposed that you know //the time histogram and the final drift distance is expected to be flat. The width //before the main program is the maximum geometric drift distance, which may be varied //according to the nominal angle of track you assumed here.  
// Notice that no single analytical function can describe the t-x relationship so we have to cut into four sections. 






#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <exception>
#include <TMath>

const Double_t resolution=0.5;
const Double_t width=15.5;
// set time window
const Double_t tmin=0;
const Double_t tmax=310;
const Int_t binnumber=(tmax-tmin)/resolution;

// kCompare: compare with garfield data
// kFit: fit the data
const Bool_t kCompare=kFALSE;
const Bool_t kFit=kFALSE;
const Int_t cut[3]={60,230,300};



void convert ( Int_t event_start=-1,Int_t event_end=-1,Int_t package=-1,Int_t plane=-1, Int_t run_number=6327 )
{
    TStopwatch timer;
    timer.Start();

//     compare with garfield data
    string file_name= Form ( "%s/Qweak_%d_convert.root",gSystem->Getenv ( "QW_ROOTFILES" ),run_number );
    TFile *file = new TFile ( file_name.c_str() );


    Int_t ev_start=0,ev_end=0;
    Int_t start_wire=0,end_wire=280;
    bool flag=false;
    if(plane==-1 && package ==-1) flag=true;
//     const Int_t binnumber=(t_max-t0)/resolution;

    TH1F* time_histo=new TH1F ( "time spectrum",Form("time spectrum wire%d to %d",start_wire,end_wire),binnumber,tmin,tmax );
    TH1F* cluster_histo=new TH1F("cluster distribution","cluster histogram",20,0,20);

    TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
    QwEvent* fEvent=NULL;
    QwHitContainer* hitcontainer=NULL;
    event_tree->SetBranchAddress ( "events",&fEvent );
    Int_t nevent=0,nhit=0;
    vector<Int_t> wirehit;
    vector<Double_t> drifttime;
    vector<Int_t> cluster_index;
    QwHit* hit=0;
    nevent= event_tree -> GetEntries();
    cout << "number of events: " << nevent << endl;


    if ( event_start < 0 || event_end > nevent )
    {
        ev_start=0;
        ev_end=nevent;
    }
    else
    {
        ev_start=event_start;
        ev_end=event_end;
    }


    for ( int ev_i=ev_start;ev_i<ev_end;++ev_i )
    {
        event_tree->GetEntry ( ev_i );
        if ( ev_i % 10000==0 ) cout << "events processed so far: " << ev_i << endl;
        nhit=fEvent->GetNumberOfHits();

        wirehit.clear();
	drifttime.clear();
        cluster_index.clear();

		
//         for (Int_t hit_i=0;hit_i<nhit;hit_i++)
//         {
//             hit = ( QwHit* ) fEvent->GetHit ( hit_i );
// 	    if( hit->GetRegion()==3){
// 		if(flag!=true && hit->GetPackage()!=package && hit->GetPlane()!=plane){
// 			continue;
// 		}
// 		else{
// 		Double_t dt=hit->GetTime();
//                 Int_t wire=hit->GetElement();
//                 if ( wire >= start_wire && wire < end_wire)
//                   {
// 		         wirehit.push_back(wire);
// 	                 if(dt> tmin && dt <= tmax){
//                          drifttime.push_back(dt);time_histo->Fill(dt);}
//                   }
// 		}
// 	    }
// 	} 
  	
	 for (Int_t hit_i=0;hit_i<nhit;++hit_i)
       {
           hit = ( QwHit* ) fEvent->GetHit ( hit_i );
	         if ( hit->GetRegion()==3)
            {
                Double_t dt=hit->GetTime();
                Int_t wire=hit->GetElement();
                if ( wire >= start_wire && wire < end_wire)
                {
		         wirehit.push_back(wire);
	                 if(dt> tmin && dt <= tmax){
                         drifttime.push_back(dt);
			 time_histo->Fill(dt);
			 }
                }
            }
	}

        //find_cluster(wirehit,cluster_index,1,50);
//         for (Int_t i=0;i<cluster_index.size();i+=2)
//         {
// 	    cluster_histo->Fill(cluster_index.at(i+1)-cluster_index.at(i));
//             for (Int_t j=cluster_index.at(i);j<cluster_index.at(i+1);j++)
//             {
//                 Double_t temp_t=drifttime.at(j);
//                 if (temp_t>0 && temp_t<400) time_histo->Fill(temp_t);
//             }
//         }
     }


    Int_t K=time_histo->GetEntries();
    if (K==0)
    {
        cerr << "too few events!" << endl;
        return;
    }
    Int_t integral=0;
    Double_t time_temp=0;
    // the array size has been hard coded
    //Double_t time[binnumber]={0},distance[binnumber]={0};
    Double_t time[620]=={0};
    Double_t distance[620]={0};
    cout << "K: " << K << endl;
    for (Int_t bin=1;bin<=binnumber;++bin)
    {
        time_temp=tmin+(bin-1)*resolution;        //start from lower bound
        if (time_temp<0) continue;
        integral+=time_histo->GetBinContent(bin);
        time[bin-1]=time_temp;
        distance[bin-1]=width*integral/K;
	cout << time_temp << " " << distance[bin-1] << endl;	
// 	if(time_temp==60.5) cout << "distance: " << distance[bin-1] << endl;
// 	cout << "integral: " << integral << endl;
    }
    //read the garfield data
    if (kCompare){ 
    Double_t time1[151]={0},distance1[151]={0};
    Int_t linecount=0,wordcount=0;
    ifstream ifile;
    string s=Form("%s/Extensions/Macros/Tracking/xt_data.txt",gSystem->Getenv ( "QWSCRATCH" ));
    cout << s << endl;
    ifile.open(s.c_str());
    while (!ifile.eof())
    {
        char str[100];
        wordcount=0;
        ifile.getline(str,50);
        char* pch;
        pch=strtok(str," !\t");
        while (pch!=0)
        {
            wordcount++;
            if (wordcount==1) distance1[linecount]=atof(pch);
            else if (wordcount==2) time1[linecount++]=atof(pch);
            pch=strtok(0," !\t");
        }
    }

    TGraph* t1=new TGraph(151,time1,distance1);
    t1->SetMarkerSize(0.2);
    t1->SetMarkerColor(4);
}

    
    TGraph* t=new TGraph(binnumber,time,distance);
    t->SetMarkerSize(0.2);
    t->SetMarkerColor(2);

    


//fit setting
    if (kFit==true)
    {
        TF1* f=new TF1("m1","pol3",0,cut[0]);
        TF1* f1=new TF1("m2","pol1",cut[0],cut[1]);
        TF1* f2=new TF1("m3","pol3",cut[1]+1,cut[2]);
        TF1* f3=new TF1("m4","pol1",cut[2]+1,tmax);
    }
    
    
    TMultiGraph* mg=new TMultiGraph();
    mg->Add(t);
    if(kCompare)
    mg->Add(t1);
    TCanvas* c=new TCanvas("time to distance conversion","time vs distance",800,600);
    mg->Draw("a*");
    mg->GetXaxis()->SetTitle("drift time:ns");
    mg->GetYaxis()->SetTitle("drift distance:mm");
    mg->SetTitle("drift-time vs drift-distance");
    /*
    c->Divide(2,2);
    c->cd(1);
    time_histo->Draw();
    time_histo->GetXaxis()->SetTitle("ns");
    c->cd(2);
    cluster_histo->SetFillColor(kRed);
    cluster_histo->Draw();
    c->cd(3);
    mg->Draw("a*");
    if (kFit==true)
    {
        mg->Fit(m1,"R");

        mg->Fit(m2,"R+");
        mg->Fit(m3,"R+");
        mg->Fit(m4,"R+");

        Double_t m=(f2->Eval(cut[1])-f->Eval(cut[0]))/(cut[1]-cut[0]);
        cout << "m: " << m << endl;
        cout << "b: " << f->Eval(cut[0])-cut[0]*m << endl;

        Double_t m1=(width-f2->Eval(cut[2]))/(tmax-cut[2]);
        cout << "m1: " << m1 << endl;
        cout << "b1: " << width-tmax*m1 << endl;
    }
    mg->GetXaxis()->SetTitle("drift time:ns");
    mg->GetYaxis()->SetTitle("drift distance:mm");
    mg->SetTitle("garfield vs real beam");

    */
    timer.Stop();
    printf ( "Time used : CPU %8.2f, Real %8.2f (sec)\n", timer.CpuTime(), timer.RealTime() );

}

void find_cluster(std::vector<Int_t>& wire,std::vector<Int_t>& index,Int_t lower,Int_t upper)
{
    Int_t old_index=0,count=0;
    for (Int_t i=0;i<wire.size();i++)
    {
        if (wire.at(i)==wire.at(old_index)+i-old_index)
        {
            count++;
            continue;
        }

        if (count>=lower && count<upper)
        {
            index.push_back(old_index);
            index.push_back(old_index+count);
        }
        old_index+=count;
        count=0;
        i--;
    }

    if (count>=lower && count < upper)
    {
        index.push_back(wire.size()-count);
        index.push_back(wire.size());
    }
}
