/*********************************************************** 
Programmer: Valerie Gray
Purpose: To output all the graphs of log (chi) and  log(Chi) values vs time in in region 2 for each plane of wires, x,u,v

Each canvas is set up so that going across it is all planes (black), X (Red),U(Blue),V(Green) and Drift Distance is on the top and Drift Time is on the Bottom.

The first canvas is the log of Chi values for the whole package or the each wire direction.
The second canvas is the log of Chi values vs time (CodaEventNumber) for the whole package or the each wrie direction.

Plane numbers 
x - fDirection==1
u - fDirection==3
v - fDirection==4

Entry Conditions: the run number, bool for first 100k
Date: 03-22-2012
Modified:03-30-2012
Assisted By: Juan Carlos Cornejo and Wouter Deconinck
*********************************************************/

#include "auto_shared_includes.h"
#include "TH2D.h"

#include "TSystem.h"

#include <fstream>
#include <iostream>
#include <iomanip>

//define a prefix for all the output files - global don't need to pass
TString Prefix;

void Chi(int runnum, bool is100k)
{

// changed the outputPrefix so that it is compatble with both Root and writing to a file by setting the enviromnet properly
//deifne the prefix as the directory that the files will be outputed to
Prefix = Form(TString(gSystem->Getenv("QWSCRATCH"))+"/tracking/www/run_%d/Chi_%d_",runnum,runnum);

// groups root files for a run together
TChain* event_tree = new TChain ("event_tree");

//add the root files to the chain the event_tree branches
event_tree->Add(Form("$QW_ROOTFILES/Qweak_%d.root",runnum));


//Create the canvas
TCanvas c1("c1", "log(Chi) - Package 1 across top, Package 2 across bottom", 900,1000);

//divide the canvas
c1.Divide(4,2);

//defines the 4 chi histograms for package 1
TH1D* c = new TH1D ("c","log(Chi) - Package 1",30,0.01,5.0);
TH1D* cX = new TH1D ("cX","X plane log(Chi) - Package 1",30,0.01,5.0);
TH1D* cU = new TH1D ("cU","U plane log(Chi) - Package 1",30,0.01,5.0);
TH1D* cV = new TH1D ("cV","V plane log(Chi) - Package 1",30,0.01,5.0);

//set the x-axis (the Chi axis) to the log scale _ sets for the whole canvas

//set line colors of all the histograms
cX->SetLineColor(2);
cU->SetLineColor(4);
cV->SetLineColor(8);

//defines the 4 chi histograms for package 2
TH1D* c2 = new TH1D ("c2","log(Chi) - Package 2",30,0.01,5.0);
TH1D* cX2 = new TH1D ("cX2","X plane log(Chi) - Package 2",30,0.01,5.0);
TH1D* cU2 = new TH1D ("cU2","U plane log(Chi) - Package 2",30,0.01,5.0);
TH1D* cV2 = new TH1D ("cV2","V plane log(Chi) - Package 2",30,0.01,5.0);

//set line colors of all the histograms
cX2->SetLineColor(2);
cU2->SetLineColor(4);
cV2->SetLineColor(8);

//start with graphing the Chi for package 1  - they go across the top
//select pad 1
c1.cd(1);

//draw the Chi in the x plane
event_tree->Draw("log(events.fQwTreeLines.fChi)>>c","events.fQwTreeLines.fRegion==2&&events.fQwTreeLines.fPackage==1");

//select pad 2
c1.cd(2);

//draw the Chi in the x plane
event_tree->Draw("log(events.fQwTreeLines.fChi)>>cX","events.fQwTreeLines.fRegion==2&&events.fQwTreeLines.fPackage==1&&events.fQwTreeLines.fDirection==1");

//select pad 3
c1.cd(3);

//draw the chi in the u plane
event_tree->Draw("log(events.fQwTreeLines.fChi)>>cU","events.fQwTreeLines.fRegion==2&&events.fQwTreeLines.fPackage==1&&events.fQwTreeLines.fDirection==3");

//select pad 4
c1.cd(4);

//draw the chi in the v plane
event_tree->Draw("log(events.fQwTreeLines.fChi)>>cV","events.fQwTreeLines.fRegion==2&&events.fQwTreeLines.fPackage==1&&events.fQwTreeLines.fDirection==4");


//Now let's graph the chi for Package 2 - they go across the bottom
//select pad 5
c1.cd(5);

//draw the Chi in the x plane
event_tree->Draw("log(events.fQwTreeLines.fChi)>>c2","events.fQwTreeLines.fRegion==2&&events.fQwTreeLines.fPackage==2");

//select pad 6
c1.cd(6);

//draw the Chi in the x plane
event_tree->Draw("log(events.fQwTreeLines.fChi)>>cX2","events.fQwTreeLines.fRegion==2&&events.fQwTreeLines.fPackage==2&&events.fQwTreeLines.fDirection==1");

//select pad 7
c1.cd(7);

//draw the chi in the u plane
event_tree->Draw("log(events.fQwTreeLines.fChi)>>cU2","events.fQwTreeLines.fRegion==2&&events.fQwTreeLines.fPackage==2&&events.fQwTreeLines.fDirection==3");

//select pad 8
c1.cd(8);

//draw the chi in the v plane
event_tree->Draw("log(events.fQwTreeLines.fChi)>>cV2","events.fQwTreeLines.fRegion==2&&events.fQwTreeLines.fPackage==2&&events.fQwTreeLines.fDirection==4");

//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
c1.SaveAs(Prefix+"_chi.png");

//write the stats about chi to a file

//print to file 
//this file and evrything related to it is fout
std::ofstream fout;
    
//open file
// open file with outputPrefix+q2.txt which will store the output of the vlaues to a file in a easy way that should be able to be read back into a program if needed
fout.open(Prefix+"Chi.txt");
if (!fout.is_open()) cout << "File not opened" << endl;
    
//outputPrefix will inculed run number which we need.
    
//Name what each coulmn is.
//Note direction is 0 - all planes, 1 - the X planes, 3 - U planes, 4 - V planes
//Error is the RMS.sqrt(N)
fout << "What \t Run \t pkg \t DIR \t Value \t Error" <<endl; 
fout << "Chi \t " << runnum << "\t 1 \t 0 \t" << setprecision(5) << c->GetMean() << "\t" << setprecision(4) << c->GetRMS()/sqrt(c->GetEntries()) << endl;

fout << "Chi \t " << runnum << "\t 1 \t 1 \t" << setprecision(5) << cX->GetMean() << "\t" << setprecision(4) << cX->GetRMS()/sqrt(cX->GetEntries()) << endl;

fout << "Chi \t " << runnum << "\t 1 \t 3 \t" << setprecision(5) << cU->GetMean() << "\t" << setprecision(4) << cU->GetRMS()/sqrt(cU->GetEntries()) << endl;

fout << "Chi \t " << runnum << "\t 1 \t 4 \t" << setprecision(5) << cV->GetMean() << "\t" << setprecision(4) << cV->GetRMS()/sqrt(cV->GetEntries()) << endl;

fout << "Chi \t " << runnum << "\t 2 \t 0 \t" << setprecision(5) << c2->GetMean() << "\t" << setprecision(4) << c2->GetRMS()/sqrt(c2->GetEntries()) << endl;

fout << "Chi \t " << runnum << "\t 2 \t 1 \t" << setprecision(5) << cX2->GetMean() << "\t" << setprecision(4) << cX2->GetRMS()/sqrt(cX2->GetEntries()) << endl;

fout << "Chi \t " << runnum << "\t 2 \t 3 \t" << setprecision(5) << cU2->GetMean() << "\t" << setprecision(4) << cU2->GetRMS()/sqrt(cU2->GetEntries()) << endl;

fout << "Chi \t " << runnum << "\t 2 \t 4 \t" << setprecision(5) << cV2->GetMean() << "\t" << setprecision(4) << cV2->GetRMS()/sqrt(cV2->GetEntries()) << endl;

//close the file
fout.close();


//Create the canvas this canvas is for the chi vs time plots
TCanvas c3("c3", "log(Chi) vs time - Package 1 across top, Package 2 across bottom", 900,1000);

//divide the canvas
c3.Divide(4,2);


//start with graphing the Chi for package 1  - they go across the top
//select pad 1
c3.cd(1);

//draw the Chi vs time in the x plane
event_tree->Draw("log(events.fQwTreeLines.fChi):CodaEventNumber>>h(,,,60,0.01,10.0)","events.fQwTreeLines.fRegion==2&&events.fQwTreeLines.fPackage==1");

//get a pointer to the histogram I defined above form root 
TH2D* h = (TH2D*)gDirectory->Get("h");

//set the titile to said histogram
h->SetTitle("log(Chi) vs time - Package 1");

//redraw h
h->Draw();


//select pad 2
c3.cd(2);

//draw the Chi vs time in the x plane
event_tree->Draw("log(events.fQwTreeLines.fChi):CodaEventNumber>>hX(,,,60,0.01,10.0)","events.fQwTreeLines.fRegion==2&&events.fQwTreeLines.fPackage==1&&events.fQwTreeLines.fDirection==1");

//get a pointer to the histogram I defined above form root
TH2D* hX = (TH2D*)gDirectory->Get("hX");

//set the titile to said histogram
hX->SetTitle("X Plane - log(Chi) vs time - Package 1");

//Set the line color of this histogram
hX->SetLineColor(2);

//redraw hX
hX->Draw();


//select pad 3
c3.cd(3);

//draw the chi vs time in the u plane
event_tree->Draw("log(events.fQwTreeLines.fChi):CodaEventNumber>>hU(,,,60,0.01,10.0)","events.fQwTreeLines.fRegion==2&&events.fQwTreeLines.fPackage==1&&events.fQwTreeLines.fDirection==3");

//get a pointer to the histogram I defined above form root
TH2D* hU = (TH2D*)gDirectory->Get("hU");

//set the titile to said histogram
hU->SetTitle("U Plane - log(Chi) vs time - Package 1");

//Set the line color of this histogram
hU->SetLineColor(4);

//redraw hU
hU->Draw();


//select pad 4
c3.cd(4);

//draw the chi vs time in the v plane
event_tree->Draw("log(events.fQwTreeLines.fChi):CodaEventNumber>>hV(,,,60,0.01,10.0)","events.fQwTreeLines.fRegion==2&&events.fQwTreeLines.fPackage==1&&events.fQwTreeLines.fDirection==4");

//get a pointer to the histogram I defined above form root
TH2D* hV = (TH2D*)gDirectory->Get("hV");

//set the titile to said histogram
hV->SetTitle("V Plane - log(Chi) vs time - Package 1");

//Set the line color of this histogram
hV->SetLineColor(8);

//redraw hV
hV->Draw();


//Now let's graph the chi for Package 2 - they go across the bottom
//select pad 5
c3.cd(5);

//draw the Chi vs time in the x plane
event_tree->Draw("log(events.fQwTreeLines.fChi):CodaEventNumber>>h2(,,,60,0.01,10.0)","events.fQwTreeLines.fRegion==2&&events.fQwTreeLines.fPackage==2");

//get a pointer to the histogram I defined above form root 
TH2D* h2 = (TH2D*)gDirectory->Get("h2");

//set the titile to said histogram
h2->SetTitle("log(Chi) vs time - Package 2");

//redraw h2
h2->Draw();


//select pad 6
c3.cd(6);

//draw the Chi vs time in the x plane
event_tree->Draw("log(events.fQwTreeLines.fChi):CodaEventNumber>>hX2(,,,60,0.01,10.0)","events.fQwTreeLines.fRegion==2&&events.fQwTreeLines.fPackage==2&&events.fQwTreeLines.fDirection==1");

//get a pointer to the histogram I defined above form root
TH2D* hX2 = (TH2D*)gDirectory->Get("hX2");

//set the titile to said histogram
hX2->SetTitle("X Plane - log(Chi) vs time - Package 2");

//Set the line color of this histogram
hX2->SetLineColor(2);

//redraw hX2
hX2->Draw();

//select pad 7
c3.cd(7);

//draw the chi vs time in the u plane
event_tree->Draw("log(events.fQwTreeLines.fChi):CodaEventNumber>>hU2(,,,60,0.01,10.0)","events.fQwTreeLines.fRegion==2&&events.fQwTreeLines.fPackage==2&&events.fQwTreeLines.fDirection==3");

//get a pointer to the histogram I defined above form root
TH2D* hU2 = (TH2D*)gDirectory->Get("hU2");

//set the titile to said histogram
hU2->SetTitle("U Plane - log(Chi) vs time - Package 2");

//Set the line color of this histogram
hU2->SetLineColor(4);

//redraw hU
hU2->Draw();

//select pad 8
c3.cd(8);

//draw the chi vs time in the v plane
event_tree->Draw("log(events.fQwTreeLines.fChi):CodaEventNumber>>hV2(,,,60,0.01,10.0)","events.fQwTreeLines.fRegion==2&&events.fQwTreeLines.fPackage==2&&events.fQwTreeLines.fDirection==4");

//get a pointer to the histogram I defined above form root
TH2D* hV2 = (TH2D*)gDirectory->Get("hV2");

//set the titile to said histogram
hV2->SetTitle("V Plane - log(Chi) vs time - Package 2");

//Set the line color of this histogram
hV2->SetLineColor(8);

//redraw hV
hV2->Draw();


//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
c3.SaveAs(Prefix+"_chi_vs_time.png");

return;

}
