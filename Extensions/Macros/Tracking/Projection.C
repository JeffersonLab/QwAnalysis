




// author: Siyuan Yang
// this script is devoted to project the region3's partial track to the //plane of maindetector, trigger scintillator, scanner and z=0. Use the //project_root function only. To use that function, firstly you must put the //rootfile which contains the data under $QWSCRATCH directory and then type:
//project_root(command,package,runnumber,drawoption)
//here are some examples:
///project_root("MD",1,6327): draws the plot for main detector projection map without weights for package 1 from run 6327
///project_root("TS_p+m",2,6327) draws the plot for trigger scintllator projection with weights adc_p+adc_m for package 2 from run 6327
// actually, there are five weight mode you can choose: p,m,p+m,p-m and if you do not specify anything,just like the first example, that is not weights
// another thing to keep in mind is that you must use underscore "_" to separate the type("MD","TS","QTOR") with other options
///project_root("MD_p-m_profile",1,6327) draws the plot for main detector projection with the weights p-m as the profile value   





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

void project_root(string command="", int package=1,int md_number=1,int run_number=6327){
	string file_name= Form ( "%s/Qweak_%d.root",gSystem->Getenv ( "QWSCRATCH" ),run_number );	
        TFile *file = new TFile ( file_name.c_str() );

        TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
 	QwEvent* fEvent=0;
	QwPartialTrack* pt=0; 
	QwTrackingTreeLine* tl=0;

	// section to process the command
	
	size_t type_found;
	string type;
	type_found=command.find_first_of("_");
	if(type_found==string::npos){
	 cerr << "bad things happens!" << endl;
         return;
	}
	else{
	 type.assign(command,0,type_found);
	}

	bool IsProfile=false;  
	if(command.find("profile")==string::npos)
		IsProfile=false;
	else IsProfile=true;

	int mode=0;   //p,m,m+p,p-m;starting from 1
	if(command.find("+")!=string::npos)
		mode=3;
	else if(command.find("-")!=string::npos)
		mode=4;
	else if(command.find("p")!=string::npos)
		mode=1;
	else if(command.find("m")!=string::npos)
		mode=2;
	string w_title;
	if(mode==0)
		w_title="not weighted";
	else
		w_title="weighted";
	TH2F* h_2d;
	TProfile2D* hp_2d;
	if(IsProfile==false)
		h_2d=new TH2F(Form("h_2d %s not profile",w_title.c_str()),"h_2d ",120,280,400,200,-100,100);
	else 
		hp_2d=new TProfile2D(Form("hp_2d %s profile",w_title.c_str()),"h_2d ",120,280,400,200,-100,100);
		
	
	TBranch* branch_event=event_tree->GetBranch("events");
	TBranch* branch=event_tree->GetBranch("maindet");
	branch_event->SetAddress(&fEvent);
	TLeaf* mdp=branch->GetLeaf(Form("md%dp_adc",md_number));
	TLeaf* mdm=branch->GetLeaf(Form("md%dm_adc",md_number));
        

	Int_t nevents=event_tree->GetEntries();
	

	double dz=0;
	if(type=="QTOR")
	   dz=0;
	else if(type=="TS")
	   dz=539.74;
	else if(type=="MD")
	   dz=577.515;
	else if(type=="SC")
	   dz=591.515;
	else {
	  cerr << "no such plane!" << endl;
	  return;
	  }



	for(int i=0;i<nevents;i++){
			
		branch_event->GetEntry(i);
		branch->GetEntry(i);
		
		double xoffset,yoffset,xslope,yslope,x,y;
		for(int p=0;p< fEvent->GetNumberOfPartialTracks();p++){
			pt=fEvent->GetPartialTrack(p);
			if(pt->GetRegion()==3 && pt->GetPackage()==package){
			   xoffset=pt->fOffsetX;
			   yoffset=pt->fOffsetY;
			   xslope=pt->fSlopeX;
			   yslope=pt->fSlopeY;
			   x=xoffset+xslope*dz;
			   y=yoffset+yslope*dz;
			   int m=0,p=0,weight=1; 		
			   p=mdp->GetValue();
			   m=mdm->GetValue();
			   switch(mode){
				case 1:
				weight=p;
				break;
				case 2:
				weight=m;
				break;	
				case 3:
				weight=m+p;
				break;
				case 4:
				weight=p-m;
				break;
				default:
				weight=1;
			}
						
			   
			   if(p!=0 && m!=0){
				if(IsProfile==false)
				h_2d->Fill(y,x,weight);
				else
				hp_2d->Fill(y,x,weight);	
			   }
			}
		}
	}


	gStyle->SetPalette(1);
	if(IsProfile==false){
	string title="track projection on main detector: " + w_title + ": not profile";
	h_2d->GetXaxis()->SetTitle("position x:cm");
	h_2d->GetYaxis()->SetTitle("position y:cm");
	h_2d->SetTitle(title.c_str());
	h_2d->Draw("colz");
	}
	else{
	string title="track projection on main detector: " + w_title + "light per event";
	hp_2d->GetXaxis()->SetTitle("position x:cm");
	hp_2d->GetYaxis()->SetTitle("position y:cm");
	hp_2d->SetTitle(title.c_str());
	hp_2d->Draw("colz");
		}
	return;
};
