/* Name  :   WireTrack_efficiency.C
 * Author:   Siyuan Yang
 * email :   jhlee@jlab.org sxyang@wm.edu
 * Date:     Tue Nov 19 16:55:06 EST 2011
 * Version:  1.0.0
 * This script is used to find the 5-wires efficiency in VDC.
 *
 * to use this script, type:
 *       bin/qwroot -l
 *       root [1]  .L Extensions/Macros/Tracking/WireTrack.C
 *       root [2]  wire_efficiency(5,0,1000,8658)
 *
 * The first argument is the number of wires in a track in which you 
 * are looking for the efficiency. The second an * d third is the start 
 * and end event numer, the fourth is the run number.
 * The function will print out the number of good tracks in each plane and the 
 * bad track which is caused by the wire inefficiency. 
 */
#include <iterator>
#include <iostream>
#include <algorithm>
#include <exception>

   


void  
wire_efficiency(Int_t wires=5,Int_t event_begin=-1,Int_t event_end=-1,Int_t run_number=1672)
{
    std::vector<int> temp_wire;
    std::vector<double> temp_time;
    TStopwatch timer;
    timer.Start();

    TCanvas* time_per_track_canvas= new TCanvas("time_track_canvas","time",10,10,1200,360);
    
    TFile* file= new TFile(Form("%s/Qweak_%d.root",getenv("QW_ROOTFILES"),run_number));
    
    if(!file)
    {
	printf("Error opening file\n");
	delete time_per_track_canvas;
	return;
	    }
    else{
        QwEvent* fEvent=0;
        QwHit* hit=0;
	TTree* event_tree=(TTree*)file->Get("event_tree");
	int nevents=event_tree->GetEntries();
	cout << "total events: " << nevents << endl;
	if(event_end==-1)
	  event_end=nevents;
	
	event_tree->SetBranchStatus("events",1);
        TBranch* event_branch=event_tree->GetBranch("events");
        event_branch->SetAddress(&fEvent);     
		
	vector<int> hitwires;
	int ngood=0,nbad=0;
	for(Int_t i=event_begin;i<event_end;++i){
	    event_branch->GetEntry(i);
	    int nhits=fEvent->GetNumberOfHits();
	    int old_index=0;
	    for(int j=0;j<nhits;++j){
	      hit=fEvent->GetHit(j);
	      if(hit->GetRegion()==3){
	       int index=(hit->GetPackage()-1)*1116+(hit->GetPlane()-1)*279+hit->GetElement();
	       if(index!=old_index){
	       hitwires.push_back(index);
	       old_index=index;
	       }
	      }
	    }
	    int half=wires/2;
	    int valid=hitwires.size();
	    int prev_wires=0,last_wire=0;
	    for(int j=0;j<valid;){
	      int m=j;
	      while(m<valid){
		if((hitwires.at(m)-hitwires.at(j))!=m-j){
		  if(m-j== half&& prev_wires==half && hitwires.at(j)==last_wire+half){
		    ++nbad;
		    prev_wires=0;
		    last_wire=0;
		  }
		  prev_wires=m-j;
		  last_wire=hitwires.at(m-1);
		  break;
		}
		++m;
	      }
	      if(m==j+wires){
		++ngood;
	       }
	      j=m;
	    }
	    
	    hitwires.clear();
	}
    }
    cout << "ngood: " << ngood << " bad: " << nbad << endl;
    timer.Stop();
}





