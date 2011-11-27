// Name  :   plotefficiency_5.C
// Author:   Peyton Rose
// email :   pwrose@email.wm.edu
// Date:    
// Version:  0.0
//
// Plots the five wire effiency of each wire, in each plane, in each package for the Region 3 drift chambers
//
//  

gROOT->Reset();

const Char_t BRANCHNAME[5] = "hits"; // Must be the same as the branch name in QwTracking.cc
                                     // or a main source which is used to create a ROOT file,
                                     // which one try to open here.

const Short_t LIBNUM          = 3;
const Char_t* LIBNAME[LIBNUM] = 
  {
      "QwHit"
    , "QwHitContainer"
    , "QwHitRootContainer"
  };


void
check_libraries()
{

  //  gROOT -> ProcessLine( Form(".L %s/Extensions/Macros/Tracking/TSuperCanvas.cc+", gSystem->Getenv("QWANALYSIS")) );

  for(Short_t i=0; i<LIBNUM; i++)
    {
      if( !TClassTable::GetDict(Form("%s", LIBNAME[i])) )
	{
	  gSystem -> Exec( Form("gcc -shared -Wl,-soname,./lib%s.so -o lib%s.so %s/Tracking/src/%s.o %s/Tracking/dictionary/%sDict.o",  
				LIBNAME[i], LIBNAME[i], gSystem->Getenv("QWANALYSIS"), LIBNAME[i],  gSystem->Getenv("QWANALYSIS"), LIBNAME[i]) );
	  gSystem -> Load( Form("%s/Extensions/Macros/Tracking/lib%s.so",  gSystem->Getenv("QWANALYSIS"), LIBNAME[i]) );
	}
    }

  return;
};


void 
plot_efficiency_5(UInt_t event_start=-1, 
		  UInt_t event_end=-1,Int_t run_number=200113)
{
  //  gROOT->Reset();
  if( (event_start > event_end) )
    {
      printf("You selected an invalid event range \n");
      return;
    }
  //if ( (event_begin == event_end) && (event_begin != 0))
  //  {
  //    event_begin = 0;
  //    printf("The event range is not allowed\n");
  //  }

  Bool_t debug =false;

  //check_libraries();

  TCanvas *efficiency1_c = new TCanvas("Five Wire Efficiency: Package 1: Vader and Leia","Five Wire Efficiency: Package 1: Vader and Leia",100,0,800,400);
  TCanvas *efficiency2_c = new TCanvas("Five Wire Efficiency: Package 2: Yoda and Han","Five Wire Efficiency: Package 2: Yoda and Han",100,0,800,400);
  TFile *file =  new TFile(Form("%s/Qweak_%d.root", getenv("QW_ROOTFILES"), run_number));  
  if (file->IsZombie()) 
    {
      printf("Error opening file\n"); 
      delete hits_per_plane_c; hits_per_plane_c = NULL;
      return;
    }
  else
    {
      QwEvent* fEvent=0;
      QwHit* hit=0;
      TTree* event_tree = (TTree*) file->Get("event_tree");
      TBranch* event_branch=event_tree->GetBranch("events");
      event_branch->SetAddress(&fEvent);
      //QwHitRootContainer* hitContainer = NULL;
      //tree->SetBranchAddress(BRANCHNAME,&hitContainer);
      
      Int_t    nevent      = 0;
      Int_t    nhit        = 0;
  
      Int_t    ev_i        = 0; 
      Int_t    hit_i       = 0;
      Int_t    plane_i     = 0;
      Int_t    wire_i      = 0;

      Short_t  region      = 3;
      Short_t  package     = 0;
      Short_t  wire        = 0;
      Short_t  plane       = 0;
      Short_t hitnum       = 0;
      Double_t tdc_time    = 0.0;
 
      nevent = event_tree -> GetEntries();
      printf("Run %d which has %d total events \n", run_number, nevent);
	  
    }
  int start=(event_start==-1 || event_start>nevent)? 0: event_start;
  int end=(event_end==-1 || event_end>nevent)? nevent: event_end;
  //if(event_end == 0) event_end = nevent;
      
      printf("You selected events in the range [%d,%d]\n", start, end);

//individual wire efficiency
Int_t nwire = 279; //the number of wires
Int_t ntrig1 [4] [279]; // number of times the wire should have fired for package 1; [plane] [wire]
Int_t ntrig2 [4] [279]; // number of times the wire should have fired for package 2; [plane] [wire]
Int_t nfire1 [4] [279]; // number of times the wire did fire for package 1; [plane] [wire]
Int_t nfire2 [4] [279]; // number of times the wire did fire for package 2; [plane] [wire]
Float_t eff1 [4] [279]; // 3 wire efficiency for package 1
Float_t eff2 [4] [279]; // 3 wire efficiency for package 2
Float_t err1 [4] [279]; //error for package 1
Float_t err2 [4] [279]; //error for package 2
Float_t xerr [4] [279]; // error in wire number (=0)
Double_t hit1 [4] [279]; //stores the tdc time for hitnumber 0 of each wire for package 1
Double_t hit2 [4] [279];



//set all array elements to zero
for(plane_i=0; plane_i <4; plane_i++)
{
  for(wire_i=0;wire_i<279;wire_i++)
    {
      ntrig1[plane_i][wire_i]=0;
      ntrig2[plane_i][wire_i]=0;
      nfire1[plane_i][wire_i]=0;
      nfire2[plane_i][wire_i]=0;
	  eff1[plane_i][wire_i]=0;
	  eff2[plane_i][wire_i]=0;
	  hit1[plane_i][wire_i]=0;
	  hit2[plane_i][wire_i]=0;
	  err1[plane_i][wire_i]=0;
	  err2[plane_i][wire_i]=0;
	  xerr[plane_i][wire_i]=0;
    }//for(wire_i)
 }//for(plane_i)


for(ev_i=start; ev_i<end; ev_i++) //iterate through all desired events in the root file
{
	wire_i=0;
	plane_i = 0;
	
	for(plane_i=0; plane_i<4; plane_i++) //reset hit tracker
	{
		for(wire_i=0; wire_i<nwire;wire_i++)
		{
			hit1[plane_i][wire_i]=0;
			hit2[plane_i][wire_i]=0;
		}//for(wire_i)
	}//for(plane_i)

	//tree -> GetEntry(ev_i);
	event_tree->GetEntry(ev_i);
	//nhit = hitContainer->GetSize(); //find the total number of hits in this event
	nhit=fEvent->GetNumberOfHits();
	hit_i=0;

	for(hit_i=0; hit_i<nhit; hit_i++) // iterate through all hits in each event
	{
		//get information about this particluar hit
	  //hit = (QwHit*) hitContainer->GetHit(hit_i);
	    hit=fEvent->GetHit(hit_i);
	    region    = (Short_t) hit->GetRegion(); 
	    package   = (Short_t) hit->GetPackage();
	    wire     = (Short_t) hit->GetElement();
	    plane     = (Short_t) hit->GetPlane();
		hitnum = (Short_t) hit->GetHitNumber();
		tdc_time = (Double_t) hit->GetTime();
		
	if(region == 3 && hitnum == 0) // only valid for region 3, and for the first hit (per event) on each wire
	{	
		if(package == 1)
		{
			hit1[plane-1][wire-1] = tdc_time;
		}//if(package)
		else if(package == 2)
		{
			hit2[plane-1][wire-1] = tdc_time;
		}//else if(package)
	}//if(region)	
	}//for(hit_i)
	
	
	//count the number of triggers and fires
	
	plane_i=0;
	wire_i=0;
	
	
	for(plane_i=0; plane_i<4; plane_i++) // iterate through wire numbers
	{
		for(wire_i=2; wire_i<277; wire_i++) // count the number of triggers and the number of successful fires
		{
			if(hit1[plane_i][wire_i-1]!=0 && hit1[plane_i][wire_i-2]>hit1[plane_i][wire_i-1] && hit1[plane_i][wire_i+1]!=0 && hit1[plane_i][wire_i+2]>hit1[plane_i][wire_i+1])
				ntrig1[plane_i][wire_i]++;
			if(hit1[plane_i][wire_i-1]!=0 && hit1[plane_i][wire_i-2]>hit1[plane_i][wire_i-1] && hit1[plane_i][wire_i+1]!=0 && hit1[plane_i][wire_i+2]>hit1[plane_i][wire_i+1] && hit1[plane_i][wire_i]!=0)
				nfire1[plane_i][wire_i]++;
			if(hit2[plane_i][wire_i-1]!=0 && hit2[plane_i][wire_i-2]>hit2[plane_i][wire_i-1] && hit2[plane_i][wire_i+1]!=0 && hit2[plane_i][wire_i+2]>hit2[plane_i][wire_i+1])
				ntrig2[plane_i][wire_i]++;
			if(hit2[plane_i][wire_i-1]!=0 && hit2[plane_i][wire_i-2]>hit2[plane_i][wire_i-1] && hit2[plane_i][wire_i+1]!=0 && hit2[plane_i][wire_i+2]>hit2[plane_i][wire_i+1] && hit2[plane_i][wire_i]!=0)
				nfire2[plane_i][wire_i]++;
		}//for(wire)	
	}//for(plane)

	//print progress to screen
	if(ev_i%1000 == 0)
	  {
	    cout << ev_i << " completed. \n" ;
	  }
	
}//for(ev_i)


//calculate the efficiency
	
	plane_i = 0;
	wire_i = 0;

for(plane_i=0; plane_i<4; plane_i++)
{
for(wire_i=0; wire_i<279; wire_i++)
{
	if(ntrig1[plane_i][wire_i] != 0)
	{
		eff1[plane_i][wire_i]= 100*float(nfire1[plane_i][wire_i]) / float(ntrig1[plane_i][wire_i]);
	} //if(ntrig1)
	
	else if (ntrig1[plane_i][wire_i] == 0)
	{
		eff1[plane_i][wire_i] = -10;
	}//else if(ntrig1)
	
	if(ntrig2[plane_i][wire_i] != 0)
	{
	eff2[plane_i][wire_i]= 100*float(nfire2[plane_i][wire_i]) / float(ntrig2[plane_i][wire_i]);
	}//if(ntrig2)
	
	else if (ntrig2[plane_i][wire_i] == 0)
	{
		eff2[plane_i][wire_i] = -10;
	}//else if(ntrig2)
}//for(wire)
}//for(plane)

//calculate the error

	plane_i = 0;
	wire_i = 0;
	
for(plane_i=0; plane_i<4; plane_i++)
{
for(wire_i=0; wire_i<279; wire_i++)
{
	if(ntrig1[plane_i][wire_i] != 0)
	{
		err1[plane_i][wire_i]= float((1/float(ntrig1[plane_i][wire_i]))*sqrt(float(ntrig1[plane_i][wire_i])*eff1[plane_i][wire_i]*float(100-eff1[plane_i][wire_i])));
	} //if(ntrig1)
	else if(ntrig1[plane_i][wire_i] == 0)
	{
		err1[plane_i][wire_i]=0;
	} //else if(ntrig1)
	
	if(ntrig2[plane_i][wire_i] != 0)
	{
		err2[plane_i][wire_i]= float((1/float(ntrig2[plane_i][wire_i]))*sqrt(float(ntrig2[plane_i][wire_i])*eff2[plane_i][wire_i]*float(100-eff2[plane_i][wire_i])));
	} // if(ntrig2)
	else if(ntrig2[plane_i][wire_i] == 0)
	{
		err2[plane_i][wire_i]=0;
	} //else if(ntrig2)
} // for (wire_i)
} // for(plane_i)


//calculate the total efficiency for each plane

Int_t ptrig1 [4]; //number of plane triggers for package 1 [plane]
Int_t ptrig2 [4];
Int_t pfire1 [4]; // number of plane fires for package 1 [plane]
Int_t pfire2 [4];
Float_t peff1 [4]; // plane efficiency for package 1 [plane]
Float_t peff2 [4];
Float_t perr1 [4]; // plane error for package 1 [plane]
Float_t perr2 [4];

//initialize arrays

plane_i = 0;

for(plane_i=0; plane_i<4; plane_i++)
{
	ptrig1[plane_i] = 0;
	ptrig2[plane_i] = 0;
	pfire1[plane_i] = 0;
	pfire2[plane_i] = 0;
	peff1[plane_i] = 0;
	peff2[plane_i] = 0;
	perr1[plane_i] = 0;
	perr2[plane_i] = 0;
}// for(plane_i)

//calculate ptrig and pfire

plane_i = 0;
wire_i = 0;

for(plane_i=0;plane_i<4;plane_i++)
{
for(wire_i=0;wire_i<279;wire_i++)
{
	ptrig1[plane_i] = ptrig1[plane_i] + ntrig1[plane_i][wire_i];
	ptrig2[plane_i] = ptrig2[plane_i] + ntrig2[plane_i][wire_i];
	pfire1[plane_i] = pfire1[plane_i] + nfire1[plane_i][wire_i];
	pfire2[plane_i] = pfire2[plane_i] + nfire2[plane_i][wire_i];
}//for(wire_i)
}//for(plane_i)

//calculate the efficiency

plane_i = 0;
for(plane_i=0;plane_i<4;plane_i++)
{
	if(ptrig1[plane_i] != 0)
	{
	peff1[plane_i] = 100* float(pfire1[plane_i]) / float(ptrig1[plane_i]);
	}//if(ptrig1)
	
	if(ptrig2[plane_i] != 0)
	{
	peff2[plane_i] = 100* float(pfire2[plane_i]) / float(ptrig2[plane_i]);
	}//if(ptrig2)
}//for(plane_i)

//calculate the error for each plane

plane_i = 0;

 for(plane_i=0;plane_i<4; plane_i++)
{
	perr1[plane_i] = float((1/float(ptrig1[plane_i]))*sqrt(float(ptrig1[plane_i])*peff1[plane_i]*float(100-peff1[plane_i])));
	perr2[plane_i] = float((1/float(ptrig2[plane_i]))*sqrt(float(ptrig2[plane_i])*peff2[plane_i]*float(100-peff2[plane_i])));
}//for(plane_i)


//calculate the overall efficiency

Int_t ttrig = 0;
Int_t tfire = 0;
Float_t teff = 0;
Float_t terr = 0;

plane_i = 0;

for(plane_i=0;plane_i<4;plane_i++)
{
	ttrig = ttrig + ptrig1[plane_i] + ptrig2[plane_i];
	tfire = tfire + pfire1[plane_i] + pfire2[plane_i];
}//for(plane_i)

teff = 100*float(tfire)/float(ttrig);

terr = float((1/float(ttrig))*sqrt(float(ttrig)*teff*float(100-teff)));


cout << "The 5-wire total plane efficiencies are: \n";
cout << "Vader V-Plane: " << peff1[0] << " +/- " << perr1[0] << " %. \n";
cout << "Vader U-Plane: " << peff1[1] << " +/- " << perr1[1] << " %. \n";
cout << "Leia V-Plane: " << peff1[2] << " +/- " << perr1[2] << " %. \n";
cout << "Leia U-Plane: " << peff1[3] << " +/- " << perr1[3] << " %. \n";
cout << "Yoda V-Plane: " << peff2[0] << " +/- " << perr2[0] << " %. \n";
cout << "Yoda U-Plane: " << peff2[1] << " +/- " << perr2[1] << " %. \n";
cout << "Han V-Plane: " << peff2[2] << " +/- " << perr2[2] << " %. \n";
cout << "Han U-Plane: " << peff2[3] << " +/- " << perr2[3] << " %. \n";

cout << "The overall 5-wire efficiency is: " << teff << " +/- " << terr << " %. \n";

wire_i = 1;

//create the x-axis for the graphs
Float_t wirenum[279];
for(wire_i=1; wire_i<280; wire_i++)
{
	wirenum[wire_i-1]= wire_i;
}//for(wire_i)


	//setup to draw the efficiency for package 1
	efficiency1_c -> Divide(2,2);
	
	//Package 1, Plane 1
	Short_t padnumber1 = 1;
	efficiency1_c -> cd(padnumber1);
	TGraph *gr11 = new TGraphErrors(279,wirenum, eff1[0],xerr[0],err1[0]);
	gr11 -> Draw("A*");
	gr11 -> SetTitle("Vader V-Plane Efficiency");
	gr11 -> GetXaxis() -> SetTitle("Wire Number");
	gr11 -> GetYaxis() -> SetTitle("Five Wire Efficiency");
	
	//Package 1, Plane 2
	Short_t padnumber1 = 2;
	efficiency1_c -> cd(padnumber1);
	TGraph *gr12 = new TGraphErrors(279,wirenum, eff1[1],xerr[1],err1[1]);
	gr12 -> Draw("A*");
	gr12 -> SetTitle("Vader U-Plane Efficiency");
	gr12 -> GetXaxis() -> SetTitle("Wire Number");
	gr12 -> GetYaxis() -> SetTitle("Five Wire Efficiency");
	
	//Package 1, Plane 3
	Short_t padnumber1 = 3;
	efficiency1_c -> cd(padnumber1);
	TGraph *gr13 = new TGraphErrors(279,wirenum, eff1[2],xerr[2],err1[2]);
	gr13 -> Draw("A*");
	gr13 -> SetTitle("Leia V-Plane Efficiency");
	gr13 -> GetXaxis() -> SetTitle("Wire Number");
	gr13 -> GetYaxis() -> SetTitle("Five Wire Efficiency");
	
	//Package 1, Plane 4
	Short_t padnumber1 = 4;
	efficiency1_c -> cd(padnumber1);
	TGraph *gr14 = new TGraphErrors(279,wirenum, eff1[3],xerr[3],err1[3]);
	gr14 -> Draw("A*");
	gr14 -> SetTitle("Leia U-Plane Efficiency");
	gr14 -> GetXaxis() -> SetTitle("Wire Number");
	gr14 -> GetYaxis() -> SetTitle("Five Wire Efficiency");
	
	//setup to draw the efficiency for package 2
	efficiency2_c -> Divide(2,2);
	
	//Package 2, Plane 1
	Short_t padnumber2 = 1;
	efficiency2_c -> cd(padnumber2);
	TGraph *gr21 = new TGraphErrors(279,wirenum, eff2[0],xerr[0],err2[0]);
	gr21 -> Draw("A*");
	gr21 -> SetTitle("Yoda V-Plane Efficiency");
	gr21 -> GetXaxis() -> SetTitle("Wire Number");
	gr21 -> GetYaxis() -> SetTitle("Five Wire Efficiency");
	
	//Package 2, Plane 2
	Short_t padnumber2 = 2;
	efficiency2_c -> cd(padnumber2);
	TGraph *gr22 = new TGraphErrors(279,wirenum, eff2[1],xerr[1],err2[1]);
	gr22 -> Draw("A*");
	gr22 -> SetTitle("Yoda U-Plane Efficiency");
	gr22 -> GetXaxis() -> SetTitle("Wire Number");
	gr22 -> GetYaxis() -> SetTitle("Five Wire Efficiency");
	
	//Package 2, Plane 3
	Short_t padnumber2 = 3;
	efficiency2_c -> cd(padnumber2);
	TGraph *gr23 = new TGraphErrors(279,wirenum, eff2[2],xerr[2],err2[2]);
	gr23 -> Draw("A*");
	gr23 -> SetTitle("Han V-Plane Efficiency");
	gr23 -> GetXaxis() -> SetTitle("Wire Number");
	gr23 -> GetYaxis() -> SetTitle("Five Wire Efficiency");
	
	//Package 2, Plane 4
	Short_t padnumber2 = 4;
	efficiency2_c -> cd(padnumber2);
	TGraph *gr24 = new TGraphErrors(279,wirenum, eff2[3],xerr[3],err2[3]);
	gr24 -> Draw("A*");
	gr24 -> SetTitle("Han U-Plane Efficiency");
	gr24 -> GetXaxis() -> SetTitle("Wire Number");
	gr24 -> GetYaxis() -> SetTitle("Five Wire Efficiency");
}//plot_efficiency