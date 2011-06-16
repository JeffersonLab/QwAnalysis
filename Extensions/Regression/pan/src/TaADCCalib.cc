//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TaADCCalib.cc   (implementation)
//
// Author:  K. Paschke <http://www.jlab.org/~paschke/>
// @(#)pan/src:$Name:  $:$Id: TaADCCalib.cc,v 1.16 2007/06/19 14:12:10 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//    ADC calibration analysis.  Depending on constructor argument,
//    this class handles analysis of ADC pedestals or DAC noise 
//    pedestals and slopes.
//
////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <fstream>
#include "TaADCCalib.hh"
#include "TaEvent.hh"
#include "TaFileName.hh"
#include "TaRun.hh"
#include "TaString.hh"
#include "TaLabelledQuantity.hh"

#ifndef NODICT
ClassImp(TaADCCalib)
#endif


// Constructors/destructors/operators

TaADCCalib::TaADCCalib():VaAnalysis()
{
  typeFlag = 0;
//  phist = new TH1F* [(ADC_MaxSlot+1)*ADC_MaxChan];
}

TaADCCalib::TaADCCalib(const string& anName)
  :VaAnalysis()
{
  // TaADCCalib constructor; argument specifying calibration type may
  // be "adcped" or "adcdac".

  if (TaString (anName).CmpNoCase("adcped") == 0)
    typeFlag = 1;
  else if (TaString (anName).CmpNoCase("adcdac") == 0)
    typeFlag = 2;
  else
    typeFlag = 0;

  //  clog << "TaADCCalib::TaADCCalib analysis type is " << anName << " and typeflag is "        << typeFlag << endl;

  if (typeFlag == 1) {
    phist = new TH1F* [(ADC_MaxSlot+1)*ADC_MaxChan];
    // initialize sum, sum2 arrays
    fSumX    = vector<Double_t>((size_t) (ADC_MaxSlot+1)*ADC_MaxChan,0);
    fSumX2   = vector<Double_t>((size_t) (ADC_MaxSlot+1)*ADC_MaxChan,0);
    nEntries = vector<Int_t>((size_t) (ADC_MaxSlot+1)*ADC_MaxChan,0);
  }

  if (typeFlag == 2) {
    // initialize sum, sum2 arrays
    fSumX    = vector<Double_t>((size_t) (ADC_MaxSlot+1)*ADC_MaxChan,0);
    fSumX2   = vector<Double_t>((size_t) (ADC_MaxSlot+1)*ADC_MaxChan,0);
    fSumXY   = vector<Double_t>((size_t) (ADC_MaxSlot+1)*ADC_MaxChan,0);
    fSumY    = vector<Double_t>((size_t) (ADC_MaxSlot+1)*ADC_MaxChan,0);
    nEntries = vector<Int_t>((size_t) (ADC_MaxSlot+1)*ADC_MaxChan,0);

    Int_t MaxDACBin = MaxNoiseDACBin;
    dgraphs = new TGraphErrors* [(ADC_MaxSlot+1)*ADC_MaxChan];
    rgraphs = new TGraphErrors* [(ADC_MaxSlot+1)*ADC_MaxChan];
    dEntries =  vector< vector<Int_t> >((size_t) (ADC_MaxSlot+1)*ADC_MaxChan);
    dADCsum  =  vector< vector<Double_t> >((size_t) (ADC_MaxSlot+1)*ADC_MaxChan);
    dADCsum2 =  vector< vector<Double_t> >((size_t) (ADC_MaxSlot+1)*ADC_MaxChan);
    for (Int_t id=0; id< (ADC_MaxSlot+1)*ADC_MaxChan; id++) {
      dEntries[id] = vector<Int_t> ((size_t) MaxDACBin,0);
      dADCsum[id] = vector<Double_t> ((size_t) MaxDACBin,0);
      dADCsum2[id] = vector<Double_t> ((size_t) MaxDACBin,0);
    }
  }   
}



TaADCCalib::~TaADCCalib(){}


// Private member functions

// We should not need to copy or assign an analysis, so copy
// constructor and operator= are private.

TaADCCalib::TaADCCalib (const TaADCCalib& copy) 
{
}


TaADCCalib& TaADCCalib::operator=( const TaADCCalib& assign) 
{ 
  return *this; 
}


void TaADCCalib::Init(const Bool_t& onl)
{
  // Initialization -- determines which channels to analyze, then
  // calls type-specific initialization

  VaAnalysis::Init(onl);
  //  clog << "TaADCCalib::Init Local init of TaADCCalib needed also..." << endl;

  char charkey[10];
  string key; 
  // loop over slots, 0-9
  //  clog << "TaADCCalib::Init loop over slots" << endl;
  for (Int_t isl=0; isl < ADC_MaxSlot; isl++) {
    //  loop over chans 1-4
    //    clog << "   TaADCCalib::Init loop over channels" << endl;
    for (Int_t ich=0; ich < ADC_MaxChan;  ich++) {
      //  make key ADC<slot>_<chan>
      sprintf(charkey, "adc%i_%i", isl,ich);
      key = charkey;
      //      clog << "      produced key " << key << endl;
      //  check for existence of key  
      //     needs utility in TaEvent to search fKeyDev for the key.
      //     if exists, mark existence on [slot,chan] bool array
      //     for now, just assume it exists
      chanExists[isl][ich] = kTRUE;
    }
  }

  // separate init calls for each type of analysis
  if (typeFlag == 1) 
    InitPed();
  else if (typeFlag == 2)
    InitDAC();
  else {
    cerr << endl;
    cerr << "TaADCCalib::Init ERROR: Invalid ADC Calibration Procedure Selected: typeFlag = "  << typeFlag <<endl;
    cerr << endl;
  }

}

void TaADCCalib::Finish() 
{
  // End of analysis -- mainly just calls type-specific finish routine

  VaAnalysis::Finish();
  //  clog<<"TaADCCalib::Finish Local finish of TaADCCalib needed also..." << endl;


  // separate finish calls for each type of analysis
  if (typeFlag == 1) {
  FinishPed();
  } else if (typeFlag == 2) {
    FinishDAC();
  } else {
    cerr << endl;
    cerr << "TaADCCalib::Finish ERROR: Invalid ADC Calibration Procedure Selected: typeFlag = "  << typeFlag <<endl;
    cerr << endl;
  }


  hfile->Write();
  hfile->Close();

}

void TaADCCalib::FinishDAC()
{
  // End of DAC calibration analysis; computes DAC noise pedestals and
  // slopes
  char charkey[10];
  string key; 
  Int_t id;

  vector<Double_t> x0 = vector<Double_t>((size_t) (ADC_MaxSlot+1)*ADC_MaxChan,0);
  vector<Double_t> slope = vector<Double_t>((size_t) (ADC_MaxSlot+1)*ADC_MaxChan,0);
  vector<Double_t> Ex0 = vector<Double_t>((size_t) (ADC_MaxSlot+1)*ADC_MaxChan,0);
  vector<Double_t> Eslope = 
    vector<Double_t>((size_t) (ADC_MaxSlot+1)*ADC_MaxChan,0);
  vector<Bool_t> filledOK = 
    vector<Bool_t>((size_t) (ADC_MaxSlot+1)*ADC_MaxChan,kFALSE);


  // loop over slots, 0-9
  for (Int_t isl=0; isl < ADC_MaxSlot; isl++) {
    //  loop over chans 1-4
    for (Int_t ich=0; ich < ADC_MaxChan;  ich++) {
      id = isl*ADC_MaxChan + ich + 1;
      Double_t delta = nEntries[id]*fSumX2[id] - fSumX[id]*fSumX[id];
      if (delta>0) {
	x0[id] = (fSumX2[id]*fSumY[id] - fSumX[id]*fSumXY[id])/delta;
	slope[id] = (nEntries[id]*fSumXY[id] - fSumX[id]*fSumY[id])/delta;
	Ex0[id] = sqrt( fSumX2[id]/delta);
	Eslope[id] = sqrt( nEntries[id]/delta);
	filledOK[id] = kTRUE;
      } else {
	x0[id] = 0.;
	slope[id] = 0.;
	Ex0[id] = 0.;
	Eslope[id] = 0.;
      }
    }
  }

  cerr << "TaADCCalib::FinishDAC ERROR: The following keys were not found : " << endl;
  // loop over slots, 0-9
  for (Int_t isl=0; isl < ADC_MaxSlot; isl++) {
    //  loop over chans 1-4
    for (Int_t ich=0; ich < ADC_MaxChan;  ich++) {
      //  make key ADC<slot>_<chan>
      sprintf(charkey, "adc%i_%i", isl,ich);
      key = charkey;
      id = isl*ADC_MaxChan + ich + 1;
      if (!chanExists[isl][ich])
	cerr << "  " << key << "  was not found." << endl;
    }
  }
  
  cerr << "TaADCCalib::FinishDAC ERROR: The following keys were found with no or incomplete data : " << endl;
  // loop over slots, 0-9
  for (Int_t isl=0; isl < ADC_MaxSlot; isl++) {
    //  loop over chans 1-4
    for (Int_t ich=0; ich < ADC_MaxChan;  ich++) {
      //  make key ADC<slot>_<chan>
      sprintf(charkey, "adc%i_%i", isl,ich);
      key = charkey;
      id = isl*ADC_MaxChan + ich + 1;
      if (chanExists[isl][ich] && !filledOK[id]) 
	cerr << "  " << key << "  could not be fit." << endl;
    }
  }
  

  cout << "TaADCCalib::FinishDAC Channels found and DAC Noise parameters set" << endl;
  // loop over slots, 0-9
  for (Int_t isl=0; isl < ADC_MaxSlot; isl++) {
    //  loop over chans 1-4
    for (Int_t ich=0; ich < ADC_MaxChan;  ich++) {
      //  make key ADC<slot>_<chan>
      sprintf(charkey, "adc%i_%i", isl,ich);
      key = charkey;
      id = isl*ADC_MaxChan + ich + 1;
      if (chanExists[isl][ich] && filledOK[id]) {
	cout << "  " << key << " : ";
	cout << " Pedestal: " << x0[id] + slope[id]*ADC_MinDAC << " +/- " << Ex0[id] 
	     << "   Slope: " << slope[id] << " +/- " << Eslope[id] << endl;
      }
    }
  }
  
  TaFileName fn = TaFileName("output", "DAC", "txt");
  ofstream ofile(fn.String().c_str(),ios::out);
  TaFileName fndb = TaFileName("output", "DAC", "db");
  ofstream ofiledb(fndb.String().c_str(),ios::out);
  ofiledb << "# dacnoise values, these need to be calibrated" << endl;
  // loop over slots, 0-9
  for (Int_t isl=0; isl < ADC_MaxSlot; isl++) {
    //  loop over chans 1-4
    for (Int_t ich=0; ich < ADC_MaxChan;  ich++) {
      id = isl*ADC_MaxChan + ich + 1;
      if (chanExists[isl][ich] && filledOK[id]) {
	ofile << isl << "  " << ich << "  " << x0[id]+slope[id]*ADC_MinDAC << "  " 
	      << slope[id] << endl;
	ofiledb << "    dacnoise adc " << isl << " chan " << ich
		<< " slope " << slope[id] << endl;
      }
    }
  }
  ofiledb << "# pedestals to subtract from ADCs" << endl;
  for (Int_t isl=0; isl < ADC_MaxSlot; isl++) {
    for (Int_t ich=0; ich < ADC_MaxChan;  ich++) {
      id = isl*ADC_MaxChan + ich + 1;
      if (chanExists[isl][ich] && filledOK[id]) {
	ofiledb	<< "    ped adc " << isl << " chan " << ich 
		<< " value " << x0[id]+slope[id]*ADC_MinDAC << endl;
      }
    }
  }

  ofile.close();
  ofiledb.close();

  
  hfile->cd();
  // loop over slots, 0-9
  for (Int_t isl=0; isl < ADC_MaxSlot; isl++) {
    //  loop over chans 1-4
    for (Int_t ich=0; ich < ADC_MaxChan;  ich++) {
      id = isl*ADC_MaxChan + ich + 1;
      Int_t nGood = 0;
      for (Int_t ib=0; ib<MaxNoiseDACBin; ib++) {
	if (dEntries[id][ib] >=1) {
	  Double_t avg = dADCsum[id][ib]/dEntries[id][ib];
	  Double_t ms = (dADCsum2[id][ib]*dEntries[id][ib] 
			 -dADCsum[id][ib]*dADCsum[id][ib])
	    / (dEntries[id][ib]*dEntries[id][ib]);
	  Double_t rms;
	  Double_t res = avg - x0[id] - slope[id]*ib;
	  if (ms>0) {
	    rms = sqrt(ms);
	    dDACval[nGood] = (Double_t) ib;
	    dEDACval[nGood] = 0.25;
	    dADCavg[nGood] = avg;
	    dADCvar[nGood] = rms/sqrt((Double_t) dEntries[id][ib]);
            dRes[nGood] = res;
	    nGood++;
	  } else if (dEntries[id][ib]==1) {
	    dDACval[nGood] = (Double_t) ib;
	    dEDACval[nGood] = 0.25;
	    dADCavg[nGood] = avg;
	    dADCvar[nGood] = 1.;
            dRes[nGood] = res;
	    nGood++;
	  }
	} 
      }
      // create and fill graphs
      if (nGood>0) {
	char *hid = new char[100];
	char *title = new char[100];
	Int_t ihid = isl*ADC_MaxChan + ich + 1;
	sprintf(hid,"DAC:%d-%d",isl,ich);
	sprintf(title,"DAC Noise: ADC slot %d, channel %d",isl, ich); 
	dgraphs[ihid] = new TGraphErrors(nGood,dDACval,dADCavg,dEDACval,dADCvar);
	dgraphs[ihid]->SetNameTitle(hid,title);
	dgraphs[ihid]->Write();

	sprintf(hid,"RES:%d-%d",isl,ich);
	sprintf(title,"Residual: ADC slot %d, channel %d",isl, ich); 
	rgraphs[ihid] = new TGraphErrors(nGood,dDACval,dRes,dEDACval,dADCvar);
	rgraphs[ihid]->SetNameTitle(hid,title);
	rgraphs[ihid]->Write();
      }
    }
  }
}

void TaADCCalib::FinishPed()
{
  // End of pedestal analysis; computes ADC pedestals

  char charkey[10];
  string key; 
  Int_t id;
  vector<Double_t> avg = vector<Double_t>((size_t) (ADC_MaxSlot+1)*ADC_MaxChan,0);
  vector<Double_t> sigma = vector<Double_t>((size_t) (ADC_MaxSlot+1)*ADC_MaxChan,0);

  // loop over slots, 0-9
  for (Int_t isl=0; isl < ADC_MaxSlot; isl++) {
    //  loop over chans 1-4
    for (Int_t ich=0; ich < ADC_MaxChan;  ich++) {
      id = isl*ADC_MaxChan + ich + 1;
      if (nEntries[id]>0 && fSumX2[id]>0) {
	avg[id] = fSumX[id] / nEntries[id];
	sigma[id] = sqrt(fSumX2[id]/nEntries[id] - avg[id]*avg[id]);
      }
    }
  }

  cerr << "TaADCCalib::FinishPed ERROR: The following keys were not found : " << endl;
  // loop over slots, 0-9
  for (Int_t isl=0; isl < ADC_MaxSlot; isl++) {
    //  loop over chans 1-4
    for (Int_t ich=0; ich < ADC_MaxChan;  ich++) {
	//  make key ADC<slot>_<chan>
	sprintf(charkey, "adc%i_%i", isl,ich);
	key = charkey;
        id = isl*ADC_MaxChan + ich + 1;
	if (!chanExists[isl][ich])
	  cerr << "  " << key << "  was not found." << endl;
    }
  }

  cerr << "TaADCCalib::FinishPed ERROR: The following keys were found with no data : " << endl;
  // loop over slots, 0-9
  for (Int_t isl=0; isl < ADC_MaxSlot; isl++) {
    //  loop over chans 1-4
    for (Int_t ich=0; ich < ADC_MaxChan;  ich++) {
	//  make key ADC<slot>_<chan>
	sprintf(charkey, "adc%i_%i", isl,ich);
	key = charkey;
        id = isl*ADC_MaxChan + ich + 1;
	if (chanExists[isl][ich] && (nEntries[id]<=0 || fSumX2[id]<=0)) 
	  cerr << "  " << key << "  returned no data for all events." << endl;
    }
  }

  cout << "TaADCCalib::FinishPed Pedestals found and set" << endl;
  // loop over slots, 0-9
  for (Int_t isl=0; isl < ADC_MaxSlot; isl++) {
    //  loop over chans 1-4
    for (Int_t ich=0; ich < ADC_MaxChan;  ich++) {
	//  make key ADC<slot>_<chan>
	sprintf(charkey, "adc%i_%i", isl,ich);
	key = charkey;
        id = isl*ADC_MaxChan + ich + 1;
	if (chanExists[isl][ich] && nEntries[id]>0 && fSumX2[id]>0) {
	  cout << "  " << key << " : " << avg[id] << " +/- " << sigma[id] << endl;
	  //	  cout << "       " << fSumX[id] << "  " << fSumX2[id] << endl;
	}
    }
  }

  TaFileName fn = TaFileName("output", "Peds", "txt");
  ofstream ofile(fn.String().c_str(),ios::out);
  TaFileName fndb = TaFileName("output", "Peds", "db");
  ofstream ofiledb(fndb.String().c_str(),ios::out);
  ofiledb << "# pedestals to subtract from ADCs" << endl;
  // loop over slots, 0-9
  for (Int_t isl=0; isl < ADC_MaxSlot; isl++) {
    //  loop over chans 1-4
    for (Int_t ich=0; ich < ADC_MaxChan;  ich++) {
        id = isl*ADC_MaxChan + ich + 1;
	if (chanExists[isl][ich] && nEntries[id]>0 && fSumX2[id]>0) {
	  ofile << isl << "  " << ich << "  " << avg[id] << "  " 
		<< sigma[id] << endl;
	  ofiledb	<< "    ped adc " << isl << " chan " << ich 
			<< " value " << avg[id] << endl;
	}
    }
  }
  ofile.close();
  ofiledb.close();

}




void TaADCCalib::InitPed()
{
  // Pedestal analysis initialization -- sets up pedestal root file

  clog << "TaADCCalib::InitPed Initializing ADC Pedestal analysis" << endl;

  // Set up ROOT.  Define output file.
  TaFileName fn = TaFileName("root", "Peds");
  hfile = new TFile(fn.String().c_str(),"RECREATE","Pedestal calibration file");

  char *hid = new char[100];
  char *title = new char[100];

  // loop over slots, 0-9
  for (Int_t isl=0; isl < ADC_MaxSlot; isl++) {
    //  loop over chans 1-4
    for (Int_t ich=0; ich < ADC_MaxChan;  ich++) {
      if ( chanExists[isl][ich] ) {
	//  if exists, Book 1D histo for ADC value	
	Int_t ihid = isl*ADC_MaxChan + ich + 1;
	sprintf(hid,"Ped:%d-%d",isl,ich);
	sprintf(title,"ADC slot %d, channel %d",isl, ich); 
	phist[ihid] = new TH1F(hid,title,25000,0,25000);
	// initialize sum, sum2 arrays
	fSumX[ihid]   = 0.0;
	fSumX2[ihid]  = 0.0;
	nEntries[ihid]= 0;
      }
    }
  }
}

void TaADCCalib::InitDAC()
{
  // DAC calibration analysis initialization -- sets up DAC root file

  clog << "TaADCCalib::InitDAC Initializing ADC noise DAC calibration analysis" << endl;

  // Set up ROOT.  Define output file.
  TaFileName fn = TaFileName("root", "DAC");
  hfile = new TFile(fn.String().c_str(),"RECREATE","Noise DAC calibration file");

  char *hid = new char[100];
  char *title = new char[100];

  // loop over slots, 0-9
  for (Int_t isl=0; isl < ADC_MaxSlot; isl++) {
    //  loop over chans 1-4
    for (Int_t ich=0; ich < ADC_MaxChan;  ich++) {
      //  check existence of key from array filled in Init()
      if ( chanExists[isl][ich] ) {
	//     if exists, Book 2D histo for ADC value vs DAC value
	Int_t ihid = isl*ADC_MaxChan + ich + 1;
	sprintf(hid,"hDAC-%d-%d",isl,ich);
	sprintf(title,"Noise DAC, ADC slot %d, channel %d",isl, ich); 
	// initialize arrays
	fSumX[ihid]   = 0.0;
	fSumX2[ihid]  = 0.0;
	fSumXY[ihid]  = 0.0;
	fSumY[ihid]   = 0.0;
	nEntries[ihid]= 0;
      }
    }
  }
}


ErrCode_t
TaADCCalib::ProcessRun()
{
  // Main analysis routine -- this is the event loop, override this
  // from VaAnalysis because it is such a pain to deal with helicity,
  // which isn't necessary in calibration runs.
  while ( fRun->NextEvent() ) 
    {
      fRun->GetEvent().CheckEvent(*fRun);
      *fEvt = fRun->GetEvent();
      EventAnalysis();
      if (fRun->GetEvent().GetEvNumber() >= fMaxNumEv)
	break;
    }  
  return VaAnalysis::fgVAANA_OK;
}



void TaADCCalib::EventAnalysis()
{
  // put everything needed to analyze one event 
  //
  // when looping over all devices, make keys (strings like adc0_1, first
  // number slot, second number channel), and check whether keys exist.
  // use GetData(key) to get data?
  // maybe init phase can be used to check for all valid keys?


  //  clog << "TaADCCalib::EventAnalysis Processing Event Analaysis for ADC Calib" << endl;
  string key; 
  Int_t id;
  Double_t dataX;
  Double_t dataY;
  // loop over slots, 0-9
  for (Int_t isl=0; isl < ADC_MaxSlot; isl++) {
    //  loop over chans 1-4
    if (typeFlag == 2) {
      dataX = fEvt->GetData(DACOFF + isl);
    }
    for (Int_t ich=0; ich < ADC_MaxChan;  ich++) {
      if (chanExists[isl][ich]) {
	id = isl*ADC_MaxChan + ich + 1;
  	dataY = fEvt->GetRawADCData(isl,ich);
	//	clog << "TaADCCalib::EventAnalysis Data for " << key << "  Channel  " << ich << " : " 
	//	     <<dataY << endl;
	if (typeFlag == 1) {
	  // PEDESTAL analysis
	  // accumulate sums for ADC pedestal averages, widths
	  fSumX[id]  += dataY;
	  fSumX2[id] += dataY*dataY;
	  nEntries[id]++;
	  // fill each 1D pedestal histo  
	  phist[id]->Fill(dataY);
	  //	  clog << "TaADCCalib::EventAnalysis   Filling histo for " << key << endl;
	} else if (typeFlag == 2) {
	  // Noise DAC analysis
	  // Accumulate sums for ADC (y) and DAC (x) values
 	  fSumX[id]  += dataX;
	  fSumX2[id] += dataX*dataX;
 	  fSumY[id]  += dataY;
	  fSumXY[id] += dataX*dataY;
	  nEntries[id]++;
	  // fill vectors for graphing of data
	  Int_t iDAC = (Int_t) dataX;
	  dADCsum[id][iDAC]  += dataY;
	  dADCsum2[id][iDAC] += dataY*dataY;
	  dEntries[id][iDAC]++;
	}

      }
    }
  }
}

void TaADCCalib::PairAnalysis()
{ 
  // no pair analysis for ADC calib runs, leave this empty
}


void TaADCCalib::InitChanLists ()
{
  // not using any external framework for output in root files, etc. 
  // so this routine is empty.
}
