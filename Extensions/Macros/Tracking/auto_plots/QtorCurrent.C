/*********************************************************** 
Programmer: Valerie Gray
Purpose: To graph the QTOR currnt as a function of time.

Also print out the Qtor currnet to a txt file

Entry Conditions: the run number, bool for first 100k
Date: 06-28-2012
Modified: 05-05-2014
Assisted By: Wouter Deconinck
*********************************************************/
#include "TSystem.h"
#include "auto_shared_includes.h"
#include "TH2D.h"

#include <fstream>
#include <iostream>

//define a prefix for all the output files - global don't need to pass
TString Prefix;

void QtorCurrent(int runnum, bool is100k)
{

	// groups root files for a run together
	TChain* chain = new TChain ("Slow_Tree");

	//add the root files to the chain the Slow_Tree branches
	chain->Add(Form("$QW_ROOTFILES/Qweak_%d.root",runnum));

	//deifne the prefix as the directory that the files will be outputed to
	//Prefix = Form(TString(gSystem->Getenv("QWSCRATCH"))+"/tracking/www/run_%d/QTOR_%d_",runnum, runnum);

        Prefix = Form(
             TString(gSystem->Getenv("WEBDIR")) + "/run_%d/%d_QTOR_",
             runnum, runnum);

	//Create a canvas and histogram
	TCanvas c1( "c1", "QTOR current vs. Time (as Event Number)", 1000, 700);
	c1.Divide(1,4);


	//Histogram of the QTOR current (DCCT set point)
	c1.cd(1);
	TH2D* h1 = new TH2D ("h1","QTOR current vs Time (as Event Number)",30,1,0, 30,1,0);
	h1->GetYaxis()->SetTitle("QTOR current, DCCT setpoint (A)");
	h1->GetXaxis()->SetTitle("Time (as Entry Number)");
	h1->SetMarkerStyle(20);
	h1->SetMarkerSize(2);
	//Here the "L" draws a line through the point on the graph <-want
	//the "C" conects the dots smoothly
	//one needs to place "" in the second place of the draw command (the cuts) part so that
	//root recognizes that that the L is not a cut but a draw option and then root :)
	chain->Draw("qw_qt_mps_i_set:Entry$>>h1","" ,"L");
	gStyle->SetStatH(0.3);                
	// 	gStyle->SetOptStat(10101);                
	h1->Draw("L"); //for some reason ROOT refused to actually draw this line


	//Histogram of the QTOR current (DCCT set point)
	c1.cd(2);
	TH2D* h2 = new TH2D ("h2","QTOR current vs Time (as Event Number)",30,1,0, 30,1,0);
	h2->GetYaxis()->SetTitle("QTOR current, DCCT readback (A)");
	h2->GetXaxis()->SetTitle("Time (as Entry Number)");
	h2->SetMarkerStyle(3);

	//Here the "L" draws a line through the point on the graph <-want
	//the "C" conects the dots smoothly
	//one needs to place "" in the second place of the draw command (the cuts) part so that
	//root recognizes that that the L is not a cut but a draw option and then root :)
	chain->Draw("qw_qt_mps_i_dcct:Entry$>>h2","" ,"L");
	h2->Draw("L"); //for some reason ROOT refused to actually draw this line


	//Histogram of the QTOR current (DCCT set point)
	c1.cd(3);
	TH2D* h3 = new TH2D ("h3","Hall probe vs Time (as Event Number)",30,1,0, 30,1,0);
	h3->GetYaxis()->SetTitle("Hall probe (au)");
	h3->GetXaxis()->SetTitle("Time (as Entry Number)");
	h3->SetMarkerStyle(3);

	//Here the "L" draws a line through the point on the graph <-want
	//the "C" conects the dots smoothly
	//one needs to place "" in the second place of the draw command (the cuts) part so that
	//root recognizes that that the L is not a cut but a draw option and then root :)
	chain->Draw("Q1HallP:Entry$>>h3","" ,"L");
	h3->Draw("L"); //for some reason ROOT refused to actually draw this line


	//Histogram of the Hall probe measurement over the QTOR current
	c1.cd(4);
	TH2D* h4 = new TH2D ("h4","Hall probe / QTOR current vs Time (as Event Number)",30,1,0, 30,1,0);
	h4->GetYaxis()->SetTitle("Hall probe / QTOR current (au)");
	h4->GetXaxis()->SetTitle("Time (as Entry Number)");
	h4->SetMarkerStyle(3);

	//Here the "L" draws a line through the point on the graph <-want
	//the "C" conects the dots smoothly
	//one needs to place "" in the second place of the draw command (the cuts) part so that
	//root recognizes that that the L is not a cut but a draw option and then root :)
	chain->Draw("Q1HallP/qw_qt_mps_i_dcct:Entry$>>h4","" ,"L");
	h4->Draw("L"); //for some reason ROOT refused to actually draw this line


	//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
	c1.SaveAs(Prefix+"current_vs_time.png");
	c1.SaveAs(Prefix+"current_vs_time.C");

	//write down the QTOR current in a file

	//print to file
	//this file and evrything related to it is fout
	std::ofstream fout;

	//open file
	// open file with outputPrefix+q2.txt which will store the output of the vlaues
	//to a file in a easy way that should be able to be read back into a program if
	//needed
	fout.open(Prefix+"current.txt");
	if (!fout.is_open()) cout << "File not opened" << endl;
	//Prefix will inculed run number which we need.
	fout << h1->GetMean(2) << endl;

	//close the file
	fout.close();

	return;
}


