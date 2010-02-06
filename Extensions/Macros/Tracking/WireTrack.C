// Name  :   ReadHits.C
// Author:   Jeong Han Lee Siyuan Yang
// email :   jhlee@jlab.org sxyang@wm.edu
// Date:     Tue Nov 10 16:55:06 EST 2009
// Version:  0.0.1
//
// This script is based on Han's idea in the ReadHits.C to access the data event//  by event from the hit-based tree. In this script, three new functions has been added
//
//  Example:
//  plot_event_track(3,0,1000,1567): plots  the histogram of the event where 3 consecutive wires has been hit(say,67,68,69)//  , 0 and 1000 are the lower and upper bound of the event number in the run 1567. 
//  first argument is the number of wires you want to include in a single track,default value is 3; second and third argume//  nts is the starting and ending event number, default values are the first and last event during that run; fourth argume//  nt is the run number, default value is 1567;  
//  
//  plot_wire_track(3,0,1000,1567): plots the histogram of the wires which are included in 3 wires track.  
//  As for the explanation of arguments, please refer to the plot_event_track function.
//
//  plot_time_track(3,60,0,1000,1567): this function will create a tree named temp.root under current directory, which cont//  ains the time and distance distribution of wire 60 only from 3 wires track. That means if wire#60 is involved in a 4 wi//  re track, the time and distance will not be counted here. So the only difference between the previous two is the second//  argument, which represents the wire number.
// 

const Int_t BUFFERSIZE = 1000;
const Int_t BINNUMBER = 4096;


#include <vector>
#include <iterator>
#include <iostream>
#include <algorithm>
#include <exception>


gROOT->Reset();

const Short_t LIBNUM=3;
const Char_t* LIBNAME[LIBNUM]={
    "QwHit","QwHitContainer","QwHitRootContainer"};

void
check_libraries()
{
    for(Short_t i=0;i<3;i++){
	if(!TClassTable::GetDict(Form("%s",LIBNAME[i]))){
	    gSystem -> Exec( Form("gcc -shared -Wl,-soname,./lib%s.so -o lib%s.so %s/Tracking/src/%s.o %s/Tracking/dictionary/%sDict.o",  LIBNAME[i], LIBNAME[i], gSystem->Getenv("QWANALYSIS"), LIBNAME[i],  gSystem->Getenv("QWANALYSIS"), LIBNAME[i]) );
	  gSystem -> Load( Form("%s/Extensions/Macros/Tracking/lib%s.so",  gSystem->Getenv("QWANALYSIS"), LIBNAME[i]) );
	}
  
    }
}



Bool_t 
time_order(std::vector<Double_t>& time, Int_t& start,Int_t& wire){
    Bool_t k=false;
    Int_t center=0,center_index=0;
    if( wire % 2==1 ){
	center=(wire-1)/2;
	center_index=start+center;
	for(Int_t i=1;i<=center;i++){
	    if( (time.at(center_index+i-1)<time.at(center_index+i)) && (time.at(center_index-i+1)<time.at(center_index-i))  ){
	    k=true;
	    continue;
	}
	    else {k=false;break;}
	}
    }
    else{
	// if it's an even number, it must be greater than 2
	center=wire/2;
	center_index=start+center;
	for(Int_t i=1;i<center;i++){
	    if((time.at(center_index+i-1)<time.at(center_index+i)) && ( time.at(center_index-i) < time.at(center_index-i-1)) ){
		k=true;
		continue;
	    }
	    else{k=false;break;}
	}
    }
	return k;
    }



void read(Int_t evID=0, Int_t hitID = -1, Int_t run_number=1567){


  check_libraries();
  //  IQR();

  //TFile file(Form("%s/Qweak_%d.root", getenv("QW_ROOTFILES_DIR"),run_number));
  TFile file(Form("%s/rootfiles/Qweak_%d.root", getenv("QWANALYSIS"),run_number));
  TTree* tree = (TTree*) file.Get("tree");
  QwHitRootContainer* hitContainer = NULL;
  
  tree->SetBranchAddress("hits",&hitContainer);
  
  Int_t nevent  = tree->GetEntries();
  
  if(evID > nevent || evID < 0) 
    {
      evID = nevent-1;
      printf("You selected the event number which is out of range.\n");
      printf("Thus, the maximum event number %d is selected.\n", evID);
    }

  //  access one event (hitContainer) in terms of id (event number)
  tree -> GetEntry(evID);
  //  total number is started from 0
  printf("*** Event %10d\n", evID);
  printf("    --- %d hits (by QwHitRootContainer)\n", hitContainer->GetSize()); 
  
  //  access all hits in a specific event id ( GetEntry(id) )
  TClonesArray *hits_per_event = (TClonesArray*) hitContainer->GetHits();
  
  //one can also access how many hits are in there
  Int_t hitN      = 0;
  Int_t hit_begin = 0;
  Int_t hit_end   = 0;

  hitN = hits_per_event  -> GetEntries();
  printf("    --- %d hits (by TCloneArray)\n", hitN); 

  if(hitN == 0) 
    {
      printf(" *** no hit in the event number %d\n", evID);
    }
  else 
    {
      QwHit *hit = NULL;
      if(hitID > hitN || hitID < 0) 
	{
	    hit_begin=0;
	    hit_end= hitN;
	  printf("    --- You selected the hit number which is out of range.\n");
	  printf("    --- Thus, the maximum hit number %d is selected.\n", hitID);
	}
      else
	{
	  hit_begin = hitID;
	  hit_end   = hitID +1;
	}
      for(Int_t i=hit_begin; i< hit_end; i++)
	{
	  hit = (QwHit *) hits_per_event->At(i);
	  // At() : http://root.cern.ch/root/html/src/TObjArray.h.html#R3YkqE
	  // 
	  // All functions are defined in QwHit.h
	  printf("    Hit %d                           \n", i);
	  printf("    -------------------------------- \n"); 
	  printf("    --- Subbank       %6d \n",  hit->GetSubbankID()); 
	  printf("    --- Module        %6d \n",  hit->GetModule()); 
	  printf("    --- Channel       %6d \n",  hit->GetChannel()); 
	  printf("    --- Region        %6d \n",  hit->GetRegion()); 
	  printf("    --- Package       %6d \n",  hit->GetPackage()); 
	  printf("    --- Direction     %6d \n",  hit->GetDirection());
	  printf("    --- Plane         %6d \n",  hit->GetPlane());
	  printf("    --- Wire          %6d \n",  hit->GetElement());
	  printf("    -------------------------------- \n"); 
	  printf("    --- DriftDistance %14.2f \n", hit->GetDriftDistance());
	  printf("    --- RawTime       %14.2f \n", hit->GetRawTime());
	  printf("    --- Time          %14.2f \n", hit->GetTime());
	  printf("    --- HitNumber     %14d   \n", hit->GetHitNumber());
	  printf("    --- HitNumberR    %14d   \n", hit->GetHitNumberR());
	  //printf("    --- HitAmbiguity  %14d   \n", hit->GetAmbiguityID());
	  printf("    -------------------------------- \n"); 
	}
       } 
  hitContainer->Clear();
  delete hitContainer; hitContainer=NULL;
  
  file.Close();
}

   
// this function will draw the histogram of the event which has #wires tracks

void  
plot_event_track(Int_t wires=3,Int_t event_begin=-1,Int_t event_end=-1,Int_t run_number=1567)
{
    check_libraries();
    std::vector<Int_t> temp_wire;
    std::vector<Double_t> temp_time;
    if( event_begin > event_end)
    {
	printf("You selected the wrong range of the event\n");
	return;
    }
    Bool debug = false;
    check_libraries();
    TStopwatch timer;
    timer.Start();

    TCanvas* time_per_track_canvas= new TCanvas("time_track_canvas","time",10,10,1200,360);
    
    TFile* file= new TFile(Form("%s/rootfiles/Qweak_%d.root",getenv("QWANALYSIS"),run_number));
    if(file->IsZombie())
    {
	printf("Error opening file\n");
	delete time_per_track_canvas;
	return;
	    }
    else{
	TTree* tree=(TTree*)file->Get("tree");
	QwHitRootContainer* hitcontainer=NULL;
	tree->SetBranchAddress("hits",&hitcontainer);
	Double_t tdc_time=0.0;
	Int_t nevent=0,nhit=0;
	Int_t plane=0,wire=0,old_wire=0,start_wire=0;
	QwHit* hit=NULL;
	

	nevent= tree -> GetEntries();
	printf("Run %d has total event %d\n", run_number,nevent);

	if(event_begin==-1 || event_end == -1){
	    event_begin=0;
	    event_end=nevent-1;
	}
	
      
	TH1F* histo=NULL;
	bool region_status=false;
	histo= new TH1F("Region_3", 
  			 Form("event Histogram for %d wires track of Region 3", 
  					  wires), BINNUMBER, 0, 0);
	histo -> SetDefaultBufferSize(1000);
	for(Int_t ev_i=event_begin;ev_i<=event_end;ev_i++){
	    tree->GetEntry(ev_i);
	    nhit=hitcontainer->GetSize();
	    if(nhit<wires) continue;   //if in this event, the hit number is smaller than given number, go to next one
 
	    hit = (QwHit*) hitcontainer->GetHit(0);
	      plane=hit->GetPlane();
	      if(plane==2) wire=(hit->GetElement())+1000;
	      else wire=hit->GetElement();
	      tdc_time=hit->GetTime();
	      temp_wire.push_back(wire);
	      temp_time.push_back(tdc_time);
	      
	    
	for(Int_t hit_i=1; hit_i<nhit; hit_i++)
	    {
	      hit = (QwHit*) hitcontainer->GetHit(hit_i);
	      old_wire=wire;
	      plane=hit->GetPlane();
	      if(plane==2) wire=(hit->GetElement())+1000;
	      else wire=hit->GetElement();
	      if(wire==old_wire) continue;
	      tdc_time=hit->GetTime();
	      temp_wire.push_back(wire);
	      temp_time.push_back(tdc_time);
	    }
	
	
	
	Int_t count=0,found=0,sum=0;
	std::vector<Int_t> index_found;
	
	std::vector<Int_t>::iterator w_beg=temp_wire.begin();
	std::vector<Int_t>::iterator w_end=temp_wire.end();
	std::vector<Int_t>::iterator w=w_beg;
	std::vector<Int_t>::iterator w_old=w;
	Int_t index=0,index_old=0,center=0;
	start_wire= *w;

	
        while ( w!=w_end )
        {

	    sum+= ( *w++ );
	    index++;
	    count++;
	    if ( sum== ( count*start_wire+count* ( count-1 ) /2 ) ) continue;
	    if ( ( --count ) == wires )
	    {
		
		//if ( ( temp_time.at(index_old+1) < temp_time.at(index_old) ) && ( temp_time.at(index_old+1) < temp_time.at(index_old+2) ) )
		if(time_order(temp_time,index_old,wires)==true)
		{   
		    found++;
		    index_found.push_back (index_old);
		}
	    }
	    --w;
	    index_old=--index;
	    count=0;
	    sum=0;
	    start_wire= *w ;

	}
	
	if ( count == wires )
	{
	    
	    //if ( ( temp_time.at(index_old+1) < temp_time.at(index_old) ) && ( temp_time.at(index_old+1) < temp_time.at(index_old+2) ) ){
	    if(time_order(temp_time,index_old,wires)==true){
		found++;
		index_found.push_back (index_old);
	    }
	}
	if(index_found.size()!=0) histo-> Fill (ev_i);
	temp_time.clear();
	temp_wire.clear();
	index_found.clear();
   	}
    }

    histo -> SetLineColor(kRed);
    histo -> GetXaxis()-> SetTitle("event number");
    histo -> GetYaxis()-> SetTitle("Number of measurements");
    histo -> GetXaxis()-> CenterTitle();
    histo -> GetYaxis()-> CenterTitle();
    histo -> Draw();
    timer.Stop();
}




//-----------------------------------------------
//*********************************************
//-----------------------------------------------


// this function will draw the histogram of the time from a certain wire during the #wires track
void  
plot_time_track(Int_t wires=3,Int_t wire_num=1,Int_t event_begin=-1,Int_t event_end=-1,Int_t run_number=1567)
{
    check_libraries();
    std::vector<Int_t> temp_wire;
    std::vector<Double_t> temp_time;
    std::vector<Double_t> temp_distance;
    Double_t drift_time=0,drift_distance=0;


    if( event_begin > event_end)
    {
	printf("You selected the wrong range of the event\n");
	return;
    }
    Bool debug = false;
    check_libraries();
    TStopwatch timer;
    timer.Start();

    //TCanvas* time_per_track_canvas= new TCanvas("time_track_canvas","time",10,10,1100,700);
    
    TFile* file= new TFile(Form("%s/rootfiles/Qweak_%d.root",getenv("QWANALYSIS"),run_number));
    TFile* f=new TFile("temp.root","RECREATE");
    
    if(file->IsZombie())
    {
	printf("Error opening file\n");
	delete time_per_track_canvas;
	return;
	    }
    else{
	TTree* tree=(TTree*)file->Get("tree");
	
	TTree* tree2=new TTree("tree","data of time and distance");
	tree2->Branch("time",&drift_time,"drift_time/D");
	tree2->Branch("distance",&drift_distance,"drift_distance/D");
	
	
	QwHitRootContainer* hitcontainer=NULL;
	tree->SetBranchAddress("hits",&hitcontainer);
	Double_t tdc_time=0,tdc_distance=0;
	Int_t nevent=0,nhit=0;
	Int_t plane=0,wire=0,old_wire=0,start_wire=0;
	QwHit* hit=NULL;
	

	nevent= tree -> GetEntries();
	printf("Run %d has total event %d\n", run_number,nevent);

	if(event_begin==-1 || event_end == -1){
	    event_begin=0;
	    event_end=nevent-1;
	}
	
      
	//TH1F* histo=NULL;
//	bool region_status=false;
	//histo= new TH1F("Region_3", 
//  			 Form("Time Histogram of %d Wire for %d wires track of Region 3", 
	//		      wire_num,wires), BINNUMBER, 0, 0);
    //histo -> SetDefaultBufferSize(1000);
	for(Int_t ev_i=event_begin;ev_i<=event_end;ev_i++){
	    
	    //tree2->Fill();
	    //test++;


	    tree->GetEntry(ev_i);
	    nhit=hitcontainer->GetSize();
	    if(nhit<wires) continue;   //if in this event, the hit number is smaller than 5, go to next one
 
	    hit = (QwHit*) hitcontainer->GetHit(0);
	      plane=hit->GetPlane();
	      if(plane==2) wire=(hit->GetElement())+1000;
	      else wire=hit->GetElement();
	      tdc_time=hit->GetTime();
	      tdc_distance=hit->GetDriftDistance();
	      temp_wire.push_back(wire);
	      temp_time.push_back(tdc_time);
	      temp_distance.push_back(tdc_distance);
	      
	    
	for(Int_t hit_i=1; hit_i<nhit; hit_i++)
	    {
	      hit = (QwHit*) hitcontainer->GetHit(hit_i);
	      old_wire=wire;
	      plane=hit->GetPlane();
	      if(plane==2) wire=(hit->GetElement())+1000;
	      else wire=hit->GetElement();
	      if(wire==old_wire) continue;
	      tdc_time=hit->GetTime();
	      tdc_distance=hit->GetDriftDistance();
	      temp_wire.push_back(wire);
	      temp_time.push_back(tdc_time);
	      temp_distance.push_back(tdc_distance);
	    }
	
	
	
	Int_t count=0,found=0,sum=0;
	//std::vector<Int_t> index_found;
	
	std::vector<Int_t>::iterator w_beg=temp_wire.begin();
	std::vector<Int_t>::iterator w_end=temp_wire.end();
	std::vector<Int_t>::iterator w=w_beg;
	std::vector<Int_t>::iterator w_old=w;
	Int_t index=0,index_old=0,center=0;
	start_wire= *w;
	

	while ( w!=w_end )
        {

	    sum+= ( *w++ );
	    index++;
	    count++;
	    if ( sum == ( count*start_wire+count* ( count-1 ) /2 ) ) continue;
	    if ( ( --count ) == wires )
	    {
	       	if(time_order(temp_time,index_old,wires)==true)
		{ 
		if( wire_num >= start_wire && wire_num <= start_wire+wires-1 )
		     {
			 drift_time=temp_time.at(wire_num-start_wire+index_old);
			 drift_distance=temp_distance.at(wire_num-start_wire+index_old);
			 tree2->Fill();
			 //histo->Fill(temp_time.at(wire_num-start_wire+index_old));
		     }
		}
	    }
	    --w;
	    index_old=--index;
	    count=0;
	    sum=0;
	    start_wire= *w ;

	}
	
	if ( count == wires )
	{
	    if(time_order(temp_time,index_old,wires)==true){
		if(wire_num > = start_wire && wire_num <= start_wire+wires-1 )
		{
		    drift_time=temp_time.at(wire_num-start_wire+index_old);
		    drift_distance=temp_distance.at(wire_num-start_wire+index_old);
		    tree2->Fill();
		    //histo->Fill(temp_time.at(wire_num-start_wire+index_old));
		}
	    }
	}
	temp_time.clear();
	temp_wire.clear();
	temp_distance.clear();
   	}
    } 

//histo -> SetLineColor(kRed);
//  histo -> GetXaxis()-> SetTitle("time(ns)");
//  histo -> GetYaxis()-> SetTitle("Number of measurements");
//  histo -> GetXaxis()-> CenterTitle();
//  histo -> GetYaxis()-> CenterTitle();
    //histo -> Draw();
    f->Write();
    timer.Stop();

    printf("Time used : CPU %8.2f, Real %8.2f (sec)\n", timer.CpuTime(), timer.RealTime());
	}

//==================================================================
// this function is used to plot the distribution of wires given the contiguous wires in the track

void  
plot_wire_track(Int_t wires=3,Int_t event_begin=-1,Int_t event_end=-1,Int_t run_number=1567)
{
    check_libraries();
    std::vector<Int_t> temp_wire;
    std::vector<Double_t> temp_time;
    if( event_begin > event_end)
    {
	printf("You selected the wrong range of the event\n");
	return;
    }
    Bool debug = false;
    check_libraries();
    TStopwatch timer;
    timer.Start();

    TCanvas* time_per_track_canvas= new TCanvas("time_track_canvas","time",10,10,1200,700);
    
    TFile* file= new TFile(Form("%s/rootfiles/Qweak_%d.root",getenv("QWANALYSIS"),run_number));
    if(file->IsZombie())
    {
	printf("Error opening file\n");
	delete time_per_track_canvas;
	return;
	    }
    else{
	TTree* tree=(TTree*)file->Get("tree");
	QwHitRootContainer* hitcontainer=NULL;
	tree->SetBranchAddress("hits",&hitcontainer);
	Double_t tdc_time=0.0;
	Int_t nevent=0,nhit=0;
	Int_t plane=0,wire=0,old_wire=0,start_wire=0;
	QwHit* hit=NULL;
	

	nevent= tree -> GetEntries();
	printf("Run %d has total event %d\n", run_number,nevent);

	if(event_begin==-1 || event_end == -1){
	    event_begin=0;
	    event_end=nevent-1;
	}
	
      
	TH1F* histo=NULL;
	bool region_status=false;
	histo= new TH1F("Region_3", 
  			 Form("wire Histogram for %d wires track in Region 3", 
			      wires), BINNUMBER, 0, 0);
	histo -> SetDefaultBufferSize(1000);
	for(Int_t ev_i=event_begin;ev_i<=event_end;ev_i++){
	    tree->GetEntry(ev_i);
	    nhit=hitcontainer->GetSize();
	    if(nhit<wires) continue;   //if in this event, the hit number is smaller than wires, go to next one
 
	    hit = (QwHit*) hitcontainer->GetHit(0);
	      plane=hit->GetPlane();
	      if(plane==2) wire=(hit->GetElement())+1000;
	      else wire=hit->GetElement();
	      tdc_time=hit->GetTime();
	      temp_wire.push_back(wire);
	      temp_time.push_back(tdc_time);
	      
	    
	for(Int_t hit_i=1; hit_i<nhit; hit_i++)
	    {
	      hit = (QwHit*) hitcontainer->GetHit(hit_i);
	      old_wire=wire;
	      plane=hit->GetPlane();
	      if(plane==2) wire=(hit->GetElement())+1000;
	      else wire=hit->GetElement();
	      if(wire==old_wire) continue;
	      tdc_time=hit->GetTime();
	      temp_wire.push_back(wire);
	      temp_time.push_back(tdc_time);
	    }
	
	
	
	Int_t count=0,found=0,sum=0;
	//std::vector<Int_t> index_found;
	
	std::vector<Int_t>::iterator w_beg=temp_wire.begin();
	std::vector<Int_t>::iterator w_end=temp_wire.end();
	std::vector<Int_t>::iterator w=w_beg;
	std::vector<Int_t>::iterator w_old=w;
	Int_t index=0,index_old=0,center=0;
	start_wire= *w;

	
        while ( w!=w_end )
        {

	    sum+= ( *w++ );
	    index++;
	    count++;
	    if ( sum== ( count*start_wire+count* ( count-1 ) /2 ) ) continue;
	    if ( ( --count ) == wires )
	    {
		if(time_order(temp_time,index_old,wires)==true)
		{
		    for(Int_t i=0;i<wires;i++)
		     {
			 //found++;
			 //index_found.push_back (index_old);
			 //histo->Fill(temp_wire.at(i+index_old));
			 histo->Fill(start_wire+i);
		     }
		}
	    }
	    --w;
	    index_old=--index;
	    count=0;
	    sum=0;
	    start_wire= *w ;

	}
	
	if ( count == wires )
	{
	    if(time_order(temp_time,index_old,wires)==true){
		for(Int_t i=0;i<wires;i++)
		{
		    //found++;
		    //index_found.push_back (index_old);
		    //histo->Fill(temp_wire.at(i+index_old));
		    histo->Fill(start_wire+i);
		}
	    }
	}
	temp_time.clear();
	temp_wire.clear();
	//index_found.clear();
   	}
    }

    histo -> SetLineColor(kRed);
    histo -> GetXaxis()-> SetTitle("wire number");
    histo -> GetYaxis()-> SetTitle("Number of measurements");
    histo -> GetXaxis()-> CenterTitle();
    histo -> GetYaxis()-> CenterTitle();
    histo -> Draw();
    timer.Stop();

    printf("Time used : CPU %8.2f, Real %8.2f (sec)\n", timer.CpuTime(), timer.RealTime());
}














