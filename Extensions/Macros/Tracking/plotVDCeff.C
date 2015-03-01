// Name: plotVDCeff.C
// original Name:   plotefficiency_5.C
// Author:   Peyton Rose
// email :   pwrose@email.wm.edu
// Date:    
// Version:  1.0
//
// Plots the five-wire effiency of each wire, in each plane, in each package for the Region 3 drift chambers
//
//  Usage: 
//     in qwroot:
//         .L plotVDCeff.C
//          plot_efficiency(1,10000,15125) 
//              calculates and plots efficiencies for events 1 to 10000 for run 15125
//
//     The five-wire efficiency is calculated by looking at the adjacent two wires on each side of a given
//   VDC wire. For a given wire, a "trigger" is an of event in which all 4 adjacent wires had first-hits
//   in the appropriate time window (-10ns to 400 ns) and in which the time-ordering of the hits is appropriate
//   for a straight track. A "fire" is defined as the number in which the tested wire has a first hit in the 
//   -10ns to 400ns. The efficiency for that wire is then calculated as the fraction fire/trigger. For wires
//   with no triggers, the efficiency is stored as -10%.
//
//  
// Modifed November 2011 by D.S. Armstrong:
//    - updated name of tree to event_tree
//    - changed file input naming scheme
//    - checked that wires are hit by using -10000 as time value if not hit at all (was 0, which
//      is no longer appropriate)
//    - added output of dead or very low efficiency wires to cout
//
// Modifed June 2012 by D.S. Armstrong
//      - again changed file input naming scheme
//      - changed to use new variable TimeNS (time in ns; fTime is no longer in ns)


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

  TCanvas *efficiency1_c = new TCanvas("Five Wire Efficiency: Package 1: Vader and Leia",Form("Five Wire Efficiency: Package 1: Vader and Leia: Run %d",run_number),100,0,800,400);
  TCanvas *efficiency2_c = new TCanvas("Five Wire Efficiency: Package 2: Yoda and Han",Form("Five Wire Efficiency: Package 2: Yoda and Han: Run %d",run_number),100,0,800,400);
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
	  hit1[plane_i][wire_i]=-10000;
	  hit2[plane_i][wire_i]=-10000;
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
			hit1[plane_i][wire_i]=-10000;
			hit2[plane_i][wire_i]=-10000;
		}//for(wire_i)
	}//for(plane_i)

	//tree -> GetEntry(ev_i);
	event_tree->GetEntry(ev_i);
	//nhit = hitContainer->GetSize(); //find the total number of hits in this event
	nhit=fEvent->GetNumberOfHits();
	hit_i=0;

	for(hit_i=0; hit_i<nhit; hit_i++) // iterate through all hits in each event
	{
		//get information about this particular hit
	  //hit = (QwHit*) hitContainer->GetHit(hit_i);
	    hit=fEvent->GetHit(hit_i);
	    region    = (Short_t) hit->GetRegion(); 
	    package   = (Short_t) hit->GetPackage();
	    wire     = (Short_t) hit->GetElement();
	    plane     = (Short_t) hit->GetPlane();
		hitnum = (Short_t) hit->GetHitNumber();
		tdc_time = (Double_t) hit->GetTimeNs();
		
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
			if(hit1[plane_i][wire_i-1]!=-10000 && hit1[plane_i][wire_i-2]>hit1[plane_i][wire_i-1] && 
hit1[plane_i][wire_i+1]!=-10000 && hit1[plane_i][wire_i+2]>hit1[plane_i][wire_i+1]
			   && hit1[plane_i][wire_i-1] < 400 && hit1[plane_i][wire_i-1] > -10
			   && hit1[plane_i][wire_i-2] < 400 && hit1[plane_i][wire_i-2] > -10
                           && hit1[plane_i][wire_i+1] < 400 && hit1[plane_i][wire_i+1] > -10
			   && hit1[plane_i][wire_i+2] < 400 && hit1[plane_i][wire_i+2] > -10
			   ){
				ntrig1[plane_i][wire_i]++;
				if (hit1[plane_i][wire_i] < 400 && hit1[plane_i][wire_i] > -10 ){
				  nfire1[plane_i][wire_i]++;}
			}
			if(hit2[plane_i][wire_i-1]!=-10000 && hit2[plane_i][wire_i-2]>hit2[plane_i][wire_i-1] && 
hit2[plane_i][wire_i+1]!=-10000 && hit2[plane_i][wire_i+2]>hit2[plane_i][wire_i+1]
			   && hit2[plane_i][wire_i-1] < 400 && hit2[plane_i][wire_i-1] > -10
			   && hit2[plane_i][wire_i-2] < 400 && hit2[plane_i][wire_i-2] > -10
                           && hit2[plane_i][wire_i+1] < 400 && hit2[plane_i][wire_i+1] > -10
			   && hit2[plane_i][wire_i+2] < 400 && hit2[plane_i][wire_i+2] > -10
			   ){
				ntrig2[plane_i][wire_i]++;
				if (hit2[plane_i][wire_i] < 400 && hit2[plane_i][wire_i] > -10 ){
				  nfire2[plane_i][wire_i]++;}
			}
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
	if (eff1[plane_i][wire_i]>-1&&eff1[plane_i][wire_i]<10){
	  cout << "Dead Wire # " << wire_i << " of package 1  and plane " << plane_i << " for tries = " << ntrig1[plane_i][wire_i] << "  efficiency = " << eff1[plane_i][wire_i] << " \n";}
		//		cout << "Efficiency of wire " << wire_i << " of package 1  and plane " << plane_i << " for tries = " << ntrig1[plane_i][wire_i] << " is "  << eff1[plane_i][wire_i] << " \n";
	} //if(ntrig1)
	
	else if (ntrig1[plane_i][wire_i] == 0)
	{
		eff1[plane_i][wire_i] = -10;
	}//else if(ntrig1)

	if(ntrig2[plane_i][wire_i] != 0)
	{
	eff2[plane_i][wire_i]= 100*float(nfire2[plane_i][wire_i]) / float(ntrig2[plane_i][wire_i]);
	if (eff2[plane_i][wire_i]>-1&&eff2[plane_i][wire_i]<10){
	  cout << "Dead Wire # " << wire_i << " of package 2  and plane " << plane_i  << " for tries = " << ntrig2[plane_i][wire_i] << "   efficiency = " << eff2[plane_i][wire_i] << " \n";}
	}//if(ntrig2)
	
	else if (ntrig2[plane_i][wire_i] == 0)
	{
		eff2[plane_i][wire_i] = -10;
	}//else if(ntrig2)
}//for(wire)
}///for(plane)

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
cout << "Vader V-Plane: " << peff1[0] << " +/- " << perr1[0] << " %. tries: " << ptrig1[0] << " \n";
cout << "Vader U-Plane: " << peff1[1] << " +/- " << perr1[1] << " %. tries: " << ptrig1[1] << "\n";
cout << "Leia V-Plane: " << peff1[2] << " +/- " << perr1[2] << " %. tries: " << ptrig1[2] << "\n";
cout << "Leia U-Plane: " << peff1[3] << " +/- " << perr1[3] << " %. tries: " << ptrig1[3] << "\n";
cout << "Yoda V-Plane: " << peff2[0] << " +/- " << perr2[0] << " %. tries: " << ptrig2[0] << "\n";
cout << "Yoda U-Plane: " << peff2[1] << " +/- " << perr2[1] << " %. tries: " << ptrig2[1] << "\n";
cout << "Han V-Plane: " << peff2[2] << " +/- " << perr2[2] << " %. tries: " << ptrig2[2] << "\n";
cout << "Han U-Plane: " << peff2[3] << " +/- " << perr2[3] << " %. tries: " << ptrig2[3] << "\n";

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
	gr11 -> SetTitle(Form("Vader V-Plane Efficiency   Run %d",run_number));
	gr11 -> GetXaxis() -> SetTitle("Wire Number");
	gr11 -> GetYaxis() -> SetTitle("Five Wire Efficiency");
	
	//Package 1, Plane 2
	Short_t padnumber1 = 2;
	efficiency1_c -> cd(padnumber1);
	TGraph *gr12 = new TGraphErrors(279,wirenum, eff1[1],xerr[1],err1[1]);
	gr12 -> Draw("A*");
	gr12 -> SetTitle(Form("Vader U-Plane Efficiency   Run %d",run_number));
	gr12 -> GetXaxis() -> SetTitle("Wire Number");
	gr12 -> GetYaxis() -> SetTitle("Five Wire Efficiency");
	
	//Package 1, Plane 3
	Short_t padnumber1 = 3;
	efficiency1_c -> cd(padnumber1);
	TGraph *gr13 = new TGraphErrors(279,wirenum, eff1[2],xerr[2],err1[2]);
	gr13 -> Draw("A*");
	gr13 -> SetTitle(Form("Leia V-Plane Efficiency   Run %d",run_number));
	gr13 -> GetXaxis() -> SetTitle("Wire Number");
	gr13 -> GetYaxis() -> SetTitle("Five Wire Efficiency");
	
	//Package 1, Plane 4
	Short_t padnumber1 = 4;
	efficiency1_c -> cd(padnumber1);
	TGraph *gr14 = new TGraphErrors(279,wirenum, eff1[3],xerr[3],err1[3]);
	gr14 -> Draw("A*");
	gr14 -> SetTitle(Form("Leia U-Plane Efficiency   Run %d",run_number));
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
	gr21 -> SetTitle(Form("Yoda V-Plane Efficiency   Run %d",run_number));
	gr21 -> SetTitle("Leia U-Plane Efficiency");
	gr21 -> GetXaxis() -> SetTitle("Wire Number");
	gr21 -> GetYaxis() -> SetTitle("Five Wire Efficiency");
	
	//Package 2, Plane 2
	Short_t padnumber2 = 2;
	efficiency2_c -> cd(padnumber2);
	TGraph *gr22 = new TGraphErrors(279,wirenum, eff2[1],xerr[1],err2[1]);
	gr22 -> Draw("A*");
	gr22 -> SetTitle(Form("Yoda U-Plane Efficiency   Run %d",run_number));
	gr22 -> GetXaxis() -> SetTitle("Wire Number");
	gr22 -> GetYaxis() -> SetTitle("Five Wire Efficiency");
	
	//Package 2, Plane 3
	Short_t padnumber2 = 3;
	efficiency2_c -> cd(padnumber2);
	TGraph *gr23 = new TGraphErrors(279,wirenum, eff2[2],xerr[2],err2[2]);
	gr23 -> Draw("A*");
	gr23 -> SetTitle(Form("Han V-Plane Efficiency   Run %d",run_number));
	gr23 -> GetXaxis() -> SetTitle("Wire Number");
	gr23 -> GetYaxis() -> SetTitle("Five Wire Efficiency");
	
	//Package 2, Plane 4
	Short_t padnumber2 = 4;
	efficiency2_c -> cd(padnumber2);
	TGraph *gr24 = new TGraphErrors(279,wirenum, eff2[3],xerr[3],err2[3]);
	gr24 -> Draw("A*");
	gr24 -> SetTitle(Form("Han U-Plane Efficiency   Run %d",run_number));
	gr24 -> GetXaxis() -> SetTitle("Wire Number");
	gr24 -> GetYaxis() -> SetTitle("Five Wire Efficiency");
}  
//plot_efficiency_5
