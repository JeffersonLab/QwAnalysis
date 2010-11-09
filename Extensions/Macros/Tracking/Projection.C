
#include<iostream>
#include<fstream>
#include <string>

void GetData(int package=1,int run_number=6327){
	string file_name= Form ( "%s/Qweak_%d.root",gSystem->Getenv ( "QWSCRATCH" ),run_number );	
        TFile *file = new TFile ( file_name.c_str() );
	ofstream ifile;
	ifile.open("output.dat");

        TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
 	QwEvent* fEvent=NULL;
	QwPartialTrack* pt=NULL; 
	QwTrackingTreeLine* tl=NULL;
	event_tree->SetBranchAddress("events",&fEvent);
	Int_t nevents=event_tree->GetEntries();
	bool valid=false;
	Int_t count=0;
	Int_t reverse=1;
	Int_t pkg=package;
	if(package==1) reverse=-1;
	cout << "number of events: " << nevents << endl;
	for(int i=0;i<nevents;i++){
		valid=false;
		count=0;
		event_tree->GetEntry(i);

		for(int p=0;p<fEvent->GetNumberOfPartialTracks();p++){
			pt=fEvent->GetPartialTrack(p);
			if(pt->GetRegion()==3 && pt->GetPackage()==pkg){
				valid=true;
				break;
			}
		}
		if(valid){
			int ntl=fEvent->GetNumberOfTreeLines();
			ifile << "!" << endl;
			for(int t=0;t<ntl;t++){
				tl=fEvent->GetTreeLine(t);
				if(tl->GetRegion()==3&&tl->GetPlane()==0&&count<3){
				count++;
				if(count==1)
				ifile << "u " << (-tl->GetOffset())/tl->GetSlope() << " " << reverse*tl->GetSlope() << endl;
				else
				ifile << "v " << (-tl->GetOffset())/tl->GetSlope() << " " << reverse*tl->GetSlope() << endl;
				}
			}
		}
	}
	return;
};



void project(){
	gStyle->SetPalette(1);
	gStyle->SetMarkerStyle(2);
	string file_name="/home/jefferson/src/TrackFinder/debug/src/xy_TS_1.dat";
// 	string file_name= Form ( "%s/xy_md.dat",gSystem->Getenv ( "QWSCRATCH" ) );	
	ifstream ifile(file_name.c_str());
// 	ifile.open("src/TrackFinder/debug/src/xy_md.dat");
	TH2F* h=new TH2F("h","h",1200,-600,600,200,-100,100);
// 	TH2F* h=new TH2F("h","h",120,-60,60,60,-360,-300);
	TH1F* hp=new TH1F("profile","profile",100,250,350);
// 	hp=new TProfile("profile","profile",50,-350,-300);
	if(ifile.is_open()){
		
		while(!ifile.eof()){
			char str[500];	
			char* pch;
			ifile.getline(str,300);
			pch=strtok(str,"! ");
			int count=0;
			double x=0,y=0;
			while(pch!=0){
				if(count==0) x=atof(pch);
				else y=atof(pch);
				count++;
				pch=strtok(0,"! ");
			}
			if(count>2) cerr << "error happens!" << endl;
			h->Fill(x,y);	
			hp->Fill(x);
						
                }	
	}
	h->GetXaxis()->SetTitle("position x:cm");
	h->GetYaxis()->SetTitle("position y:cm");
	h->SetTitle("track projection on main detector");
	hp->GetXaxis()->SetTitle("position x:cm");
	hp->SetTitle("track projection on scanner");
 	hp->Draw("p");	
// 	h->Draw("colz");
	return;
}

void project_root(string s="", int package=1,int run_number=6327){
	string file_name= Form ( "%s/Qweak_%d_50000.root",gSystem->Getenv ( "QWSCRATCH" ),run_number );	
        TFile *file = new TFile ( file_name.c_str() );
	ofstream ifile;
	ifile.open("output.dat");
	TH1F* hp=new TH1F("profile","profile",100,250,350);
	TH2F* h=new TH2F("h","h",1200,-600,600,200,-100,100);


        TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
 	QwEvent* fEvent=NULL;
	QwPartialTrack* pt=NULL; 
	QwTrackingTreeLine* tl=NULL;
	event_tree->SetBranchAddress("events",&fEvent);
	Int_t nevents=event_tree->GetEntries();
	

	double dz=0;
	if(s=="QTOR")
		dz=0;
	else if(s=="TS")
		dz=539.74;
	else if(s=="MD")
		dz=577.515;
	else if(s=="SC")
		dz=591.515;

	cout << "number of events: " << nevents << endl;
	for(int i=0;i<nevents;i++){
		event_tree->GetEntry(i);
		double xoffset,yoffset,xslope,yslope,x,y;
		for(int p=0;p<fEvent->GetNumberOfPartialTracks();p++){
			pt=fEvent->GetPartialTrack(p);
			if(pt->GetRegion()==3 && pt->GetPackage()==package){
			   xoffset=pt->fOffsetX;
			   yoffset=pt->fOffsetY;
			   xslope=pt->fSlopeX;
			   yslope=pt->fSlopeY;
			   x=xoffset+xslope*dz;
			   y=yoffset+yslope*dz;
			   hp->Fill(y);
			   h->Fill(y,x);
			}
		}
	}
	gStyle->SetPalette(1);
	hp->GetXaxis()->SetTitle("position x:cm");
	hp->SetTitle("track projection on trigger");
//  	hp->Draw();
	h->GetXaxis()->SetTitle("position x:cm");
	h->GetYaxis()->SetTitle("position y:cm");
	h->SetTitle("track projection on main detector");
	h->Draw("colz");
	
	return;
};

