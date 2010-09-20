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
const Double_t width=15.2;
const Short_t LIBNUM=3;
const Char_t* LIBNAME[LIBNUM]=
{
    "QwHit","QwHitContainer","QwHitRootContainer"
};

void
check_libraries()
{
    for ( Short_t i=0;i<3;i++ )
    {
        if ( !TClassTable::GetDict ( Form ( "%s",LIBNAME[i] ) ) )
        {
            gSystem -> Exec ( Form ( "gcc -shared -Wl,-soname,./lib%s.so -o lib%s.so %s/Tracking/src/%s.o %s/Tracking/dictionary/%sDict.o",  LIBNAME[i], LIBNAME[i], gSystem->Getenv ( "QWANALYSIS" ), LIBNAME[i],  gSystem->Getenv ( "QWANALYSIS" ), LIBNAME[i] ) );
            gSystem -> Load ( Form ( "%s/Extensions/Macros/Tracking/lib%s.so",  gSystem->Getenv ( "QWANALYSIS" ), LIBNAME[i] ) );
        }

    }
}


void convert ( Int_t event_start=-1,Int_t event_end=-1,Int_t package=-1,Int_t plane=-1, Int_t run_number=5150 )
{
    check_libraries();
    TStopwatch timer;
    timer.Start();

    Bool_t kFit=true;

    string file_name= Form ( "%s/Qweak_%d_convert.root",gSystem->Getenv ( "QWSCRATCH" ),run_number );
    TFile *file = new TFile ( file_name.c_str() );


    Int_t ev_start=0,ev_end=0;
    Int_t start_wire=0,end_wire=280;
    Int_t t0=0,t_max=400;
    const Int_t binnumber=(t_max-t0)/resolution;

    TH1F* time_histo=new TH1F ( "time spectrum",Form("time spectrum wire%d to %d",start_wire,end_wire),binnumber,t0,t_max );
    TH1F* cluster_histo=new TH1F("cluster distribution","cluster histogram",20,0,20);

    TTree* hit_tree= ( TTree* ) file->Get ( "hit_tree" );
    QwHitRootContainer* hitcontainer=NULL;
    hit_tree->SetBranchAddress ( "hits",&hitcontainer );
    Int_t nevent=0,nhit=0;
    vector<Int_t> wirehit;
    vector<Double_t> drifttime;
    vector<Int_t> cluster_index;
    QwHit* hit=NULL;
    nevent= hit_tree -> GetEntries();
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


    for ( int ev_i=ev_start;ev_i<ev_end;ev_i++ )
    {
        hit_tree->GetEntry ( ev_i );
        if ( ev_i % 10000==0 ) cout << "events processed so far: " << ev_i << endl;
        nhit=hitcontainer->GetSize();

        wirehit.clear();
	drifttime.clear();
        cluster_index.clear();

        for (Int_t hit_i=0;hit_i<nhit;hit_i++)
        {
            hit = ( QwHit* ) hitcontainer->GetHit ( hit_i );
            if ( hit->GetRegion()==3 && hit->GetPackage()==package && hit->GetPlane()==plane)
            {
                Double_t dt=hit->GetTime();
                Int_t wire=hit->GetElement();
                if ( wire >= start_wire && wire < end_wire)
                {
		         wirehit.push_back(wire);
	                 if(dt>0 && dt <= 400){
                         drifttime.push_back(dt);time_histo->Fill(dt);}
//                     if (wirehit.size()==0)
//                     {
//                         wirehit.push_back(wire);
//                         drifttime.push_back(dt);
//                     }
//                     else if (wire!=wirehit.at(wirehit.size()-1))
//                     {
//                         wirehit.push_back(wire);
//                         drifttime.push_back(dt);
//                     }
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
    Double_t time[800]={0},distance[800]={0};
    cout << "K: " << K << endl;
    for (Int_t bin=1;bin<=binnumber;bin++)
    {
        time_temp=t0+(bin-1)*resolution;        //start from lower bound
        if (time_temp<0) continue;
        integral+=time_histo->GetBinContent(bin);
        time[bin-1]=time_temp;
        distance[bin-1]=width*integral/K;
	cout << "integral: " << integral << endl;
    }

    //read the garfield data
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


    TGraph* t=new TGraph(binnumber,time,distance);
    t->SetMarkerSize(0.2);
    t->SetMarkerColor(2);

    TGraph* t1=new TGraph(151,time1,distance1);
    t1->SetMarkerSize(0.2);
    t1->SetMarkerColor(4);


//fit setting
    if (kFit==true)
    {
        TF1* f=new TF1("m1","pol3",0,60);
        TF1* f1=new TF1("m2","pol1",61,230);
        TF1* f2=new TF1("m3","pol3",231,300);
        TF1* f3=new TF1("m4","pol1",301,400);
    }
    TMultiGraph* mg=new TMultiGraph();
    mg->Add(t);
    //mg->Add(t1);
    TCanvas* c=new TCanvas("time to distance conversion","time vs distance",800,600);
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

        Double_t m=(f2->Eval(230)-f->Eval(60))/(230-60);
        cout << "m: " << m << endl;
        cout << "b: " << f->Eval(60)-m*60 << endl;

        Double_t m1=(width-f2->Eval(300))/(400-300);
        cout << "m1: " << m1 << endl;
        cout << "b1: " << width-400*m1 << endl;
    }
    mg->GetXaxis()->SetTitle("drift time:ns");
    mg->GetYaxis()->SetTitle("drift distance:mm");
    mg->SetTitle("garfield vs real beam");


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
