
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
	string file_name= Form ( "%s/Qweak_%d.root",gSystem->Getenv ( "QWSCRATCH" ),run_number );	
        TFile *file = new TFile ( file_name.c_str() );
	TH1F* hp=new TH1F("profile","profile",200,-100,100);
	TH1F* hp_w=new TH1F("profile_weighted","profile_weighted",200,-100,100);
	h_r=new TProfile("weight profile","weight profile vs y",200,-100,100);
	TH2F* h=new TH2F("h","h",120,280,400,200,-100,100);
	TH2F* h_w=new TH2F("h_w","h_w",120,280,400,200,-100,100);


        TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
 	QwEvent* fEvent=NULL;
	QwPartialTrack* pt=NULL; 
	QwTrackingTreeLine* tl=NULL;

	TBranch* branch_event=event_tree->GetBranch("events");
	TBranch* branch=event_tree->GetBranch("maindet");
	branch_event->SetAddress(&fEvent);
	TLeaf* md5p=branch->GetLeaf("md5p_adc");
	TLeaf* md5m=branch->GetLeaf("md5m_adc");

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
		
		branch_event->GetEntry(i);
		branch->GetEntry(i);
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
			   hp_w->Fill(x,(md5p->GetValue()+md5m->GetValue()));
			   hp->Fill(x,1);
			   h_r->Fill(x,(md5p->GetValue()+md5m->GetValue()),1);
			   h_w->Fill(y,x,(md5p->GetValue()+md5m->GetValue()));
			   h->Fill(y,x);
			}
		}
	}

	
	gStyle->SetPalette(1);
	hp->GetXaxis()->SetTitle("position y:cm");
	hp->SetTitle("track projection on main detector(nonweighted)");
	hp->SetLineColor(kRed);
	hp_w->GetXaxis()->SetTitle("position y:cm");
	hp_w->SetTitle("track projection on main detector(stacked)");
	hp_w->SetLineColor(kBlue);
// 	hp->SetMarkerStyle(2);
//  	hp->Draw();
	h->GetXaxis()->SetTitle("position x:cm");
	h->GetYaxis()->SetTitle("position y:cm");
	h->SetTitle("track projection on main detector(not weighted)");
	h_w->GetXaxis()->SetTitle("position x:cm");
	h_w->GetYaxis()->SetTitle("position y:cm");
	h_w->SetTitle("track projection on main detector(weighted)");
	h_r->GetXaxis()->SetTitle("position y:cm");
	h_r->GetYaxis()->SetTitle("weight (md5m+md5p)");
	h_r->SetTitle(" weight factor depending on y");
	h_w->Draw("colz");
// 	h_r->Draw();
// 	hp2->Draw();
	
// 	TCanvas* c=new TCanvas("c","c",400,400);
// 	hp_w->Draw();
// 	hp->Scale(3700);
// 	hp->Draw("same");
// 	leg=new TLegend(0.1,0.7,0.48,0.9);
// 	leg->SetHeader("title");
// 	leg->AddEntry(hp_w,"weighted");	
// 	leg->AddEntry(hp,"not weighted");
// 	leg->Draw();
	
	return;
};
