//    Name:   Cluster.C
//    Author: Siyuan Yang
//    Email:  sxyang@email.wm.edu
//    Date:   Aug 12 2010
// This script is used to find the cluster size distribution either for one plane or for a single wire, which will be decided by the first parameter of the main function: find_cluster.If it is assigned -1, the program will output cluster size distribution with any wire involved while if it is assigned a reasonable wire number(e.g,15), the output will show the cluster size distribution with this wire involed. Notice that the last argument in the function arguments list is the threshold number, it means a track is regarded as a valid one only if every plane has at least #threshold number of hits. For instance, if threshold=1, then we require every plane must have at least one hit before we continue to find out the cluster information of this track. To add this option is to exclude noise brought by the unknown sources. 


#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <exception>
#include <TMath>

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



void find_cluster ( Int_t wire==-1,Int_t package=-1,Int_t run_number=830,Int_t cut=1 )
{

    check_libraries();
    TStopwatch timer;
    timer.Start();

    string file_name= Form ( "%s/Qweak_%d_noTrack.root",gSystem->Getenv ( "QWSCRATCH" ),run_number );
    TFile *file = new TFile ( file_name.c_str() );

    TTree* hit_tree= ( TTree* ) file->Get ( "hit_tree" );
    QwHitRootContainer* hitcontainer=NULL;
    hit_tree->SetBranchAddress ( "hits",&hitcontainer );
    Int_t nevent=0,nhit=0,threshold=cut;
    vector<Int_t> wirehit;
    vector<Int_t> stack;
    QwHit* hit=NULL;
    nevent= hit_tree -> GetEntries();
    cout << "number of events: " << nevent << endl;

    TH1D* cluster[5];
    TH2D* cluster2[5];
    TCanvas* c=new TCanvas("c","c",800,800);
    c->Divide(2,2);
    for (Int_t plane=1;plane<5;plane++)
    {
        c->cd(plane);
        cluster[plane]=new TH1D(Form("wire%d clusters in plane%d package%d",wire,package,plane),Form("wire%d clusters in plane%d package%d",wire,package,plane),14,0,14);
        cluster2[plane]=new TH2D(Form("clusters in plane%d package%d",wire,package,plane),Form("clusters in plane%d package%d",plane,package),279,1,280,14,0,14);
	cluster2[plane]->SetMinimum(0);
	cluster2[plane]->SetMaximum(2500);
	cluster2[plane]->SetContour(20);
        for ( int ev_i=0;ev_i<nevent;ev_i++ )
        {
            hit_tree->GetEntry ( ev_i );
            if ( ev_i % 10000==0 )
                cout << "events processed so far: " << ev_i << endl;
            nhit=hitcontainer->GetSize();

            Int_t count=0,old_index=0;

            wirehit.clear();
            for (Int_t hit_i=0;hit_i<nhit;hit_i++)
            {
                hit = ( QwHit* ) hitcontainer->GetHit ( hit_i );
                if (hit->GetRegion()==3)
                {
                    Int_t plane_index=4*(hit->GetPackage()-1)+hit->GetPlane();
// fill in wirehit from specified plane
                    if ( hit->GetPackage() ==package && hit->GetPlane() ==plane )
                    {
                        if (wirehit.size()==0) wirehit.push_back(hit->GetElement());
                        else if (hit->GetElement()!=wirehit.at(wirehit.size()-1))
                            wirehit.push_back(hit->GetElement());
                        else {};
                    }
// fill the plane number to stack with contidion of at least #threshold hits for every plane
		     if(stack.size()==0){ 
			   stack.push_back(plane_index);
			   count++;
			}
			else if(plane_index!=old_index){
			   count=0;
			   stack.push_back(plane_index);
			   count++;
			}
			else if(plane_index==old_index && count<threshold){
			   stack.push_back(plane_index);
			   count++;
			}
			else {};
			   old_index=plane_index;
                }
            }
            Int_t track_pkg=valid_track(stack,threshold);

            stack.clear();
            if (track_pkg!=5 && track_pkg!=package) continue;
            else {};

//             if (wirehit.size()==0) continue;
            if (wire>0)
            {
                Int_t cluster_size=cluster_match(wirehit,wire);
                if (cluster_size!=0) cluster[plane]->Fill(cluster_size);
            }
            else if (wire==-1)
            {
                for (Int_t wire_index=0;wire_index<wirehit.size();wire_index++)
                {
		    Int_t wire_number=wirehit.at(wire_index);
                    Int_t cluster_size=cluster_match(wirehit,wire_number);
                    cluster2[plane]->Fill(wire_number,cluster_size);
  		    cluster[plane]->Fill(cluster_size);
                }
            }
        }

        cluster[plane]->SetFillColor(2);

        if (wire>0)
            cluster[plane]->Draw();
        else
        {
            gStyle->SetPalette(1);
            cluster2[plane]->Draw("colz");
        }
    }
    timer.Stop();
    printf ( "Time used : CPU %8.2f, Real %8.2f (sec)\n", timer.CpuTime(), timer.RealTime() );
}


Int_t cluster_match(vector<Int_t>& temp_wire, const Int_t& wire)
{
    Int_t wire_count=0,old_index=0;
    for (Int_t count=0;count<temp_wire.size();count++)
    {
        if (temp_wire.at(count)==(temp_wire.at(old_index)+count-old_index))
        {
            wire_count++;
            continue;
        }

        Int_t old_wire=temp_wire.at(old_index);
        if (wire >= old_wire && wire < (old_wire+wire_count) )
            return wire_count;
        old_index+=wire_count;
        wire_count=0;
        count--;
    }

    Int_t old_wire=temp_wire.at(old_index);
    if (wire >=old_wire && wire < (old_wire+wire_count) )
        return wire_count;
    wire_count=0;
    return wire_count;
}

// require every plane gets #th hits
Int_t valid_track(vector<Int_t>& v,Int_t th=1)
{

    Int_t nhit=v.size(),sum1=0,sum2=0;
    if (nhit<4*th) return 0;
    for (Int_t i=0;i<nhit;i++)
    {
        if (i<4*th) sum1+=v.at(i);
        if (i>=nhit-4*th) sum2+=v.at(i);
    }
    if (sum1==10*th && sum2 ==26*th) return 5;
    if (sum1==10*th) return 1;
    else if (sum2==26*th) return 2;
    else return 0;
}
