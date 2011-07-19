///////////////////////////////////////////////////////////////
// TaRegression.C
//    - Regression Class for ReD Analysis
//

//#define DEBUG
#define NOISY
// #define NOISY2
// #define NOISY3
#include "TString.h"
#include <string>
#include "TaRegression.hh"
#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TEventList.h"
#include "TaConfFile.hh"
#include "TaInput.hh"
#include "TaStatistics.hh"
#include "Matrix.hh"
#include "TaOResultsFile.hh"

ClassImp(TaRegression);
ClassImp(TaRootRegDef);

// Constructors/destructors/operators

TaRegression::TaRegression():VaAna()
{
#ifdef DEBUG
  cout << "TaRegression::TaRegression()" << endl;
#endif
  //  for (Int_t i=0; i<MaxMONS;i++) {
    //    IVmeans[i] = 0;
    //    DVmeans[i] = 0;
  //  }
}


TaRegression::~TaRegression() {}

// Major functions

Int_t 
TaRegression::Init (TaInput& input)
{
  ///////////////////////////////////////////////////////////////
  // Initialization routine -
  //     Initialize elements common to both algorithms, then
  //     call specific init routines.
  //
#ifdef DEBUG
  cout << "TaRegression::Init()" << endl;
#endif

  //  Call the Common init.
  if (VaAna::Init(input) != 0)
    return 1;

#ifdef DEBUG
  Int_t ivNum = fInput->GetConfig().GetIVNum();
  Int_t dvNum = fInput->GetConfig().GetDVNum();
  Int_t nMons = ivNum + dvNum;
  cout << "\tTotal Number of Monitors = " << nMons << endl;
#endif

  // Grab the Leafs from the Tree.
  vector <TLeaf*> leaf_dvmon;
  vector <TLeaf*> leaf_ivmon;
  vector <TLeaf*> leaf_cut;
  //  TLeaf* leaf_evnum;
  
  // Load up the variable names
  IVnames = fInput->GetConfig().GetIVNames();
  DVnames = fInput->GetConfig().GetDVNames();
  PanCuts = fInput->GetConfig().GetPanCuts();

  //  Int_t ev[4];
  Double_t ev[1];
  fPairTree->SetBranchAddress("pattern_number",ev);
  //  leaf_evnum = (TLeaf*) fPairTree->GetLeaf("evt_ev_num");
  // somehow, this is screwed up now.  I don't get why exactly it is needed.
  // we should keep track of pattern numbers, in part to double check match 
  // of events between the Hel_Tree and the reg tree, but I don't get why 
  // we would need min/max event numbers.  I also don't know if they are 
  //available from the hel tree... who cares, really?

  // Get the DV leafs and check if they exist in the Tree.
  for(UInt_t i=0; i<DVnames.size(); i++) {
    TString* tmpstr = new TString(DVnames[i].Data());
    tmpstr->ReplaceAll(".","/");
    leaf_dvmon.push_back((TLeaf*)fPairTree->GetLeaf(tmpstr->Data()));
  }

  for(UInt_t i=0; i<DVnames.size(); i++) {
    if(!leaf_dvmon[i]) {
      cerr << DVnames[i] << " does not exist in Tree. " 
	   << "Will not include in analysis. " << endl;
      DVnames.erase(DVnames.begin() + i);
      leaf_dvmon.erase(leaf_dvmon.begin() + i);
      i--;
    }
  }
#ifdef NOISY2
  for(UInt_t i=0; i<DVnames.size(); i++) {
    cout << "Dependent variable " << i << ": " << DVnames[i] << endl;
  }
#endif

  // Get the IV leafs and check if they exist in the Tree.
  for(UInt_t i=0; i<IVnames.size(); i++) {
    TString* tmpstr = new TString(IVnames[i].Data());
    tmpstr->ReplaceAll(".","/");
    leaf_ivmon.push_back((TLeaf*)fPairTree->GetLeaf(tmpstr->Data()));
  }
  for(UInt_t i=0; i<IVnames.size(); i++) {
    if(!leaf_ivmon[i]) {
      cerr << IVnames[i] << " does not exist in Tree. " 
	   << "Will not include in analysis." << endl;
      IVnames.erase(IVnames.begin() + i);
      leaf_ivmon.erase(leaf_ivmon.begin() + i);
      i--;
    }
  }
#ifdef NOISY2
  for(UInt_t i=0; i<IVnames.size(); i++) {
    cout << "Independent variable " << i << ": " << IVnames[i] << endl;
  }
#endif

  // Get the CUT leafs and check if they exist in the Tree
  for(UInt_t i=0; i<PanCuts.size(); i++) {
    leaf_cut.push_back((TLeaf*)fPairTree->GetLeaf(PanCuts[i]));
  }

  for(UInt_t i=0; i<PanCuts.size(); i++) {
    if(!leaf_cut[i]) {
      cerr << PanCuts[i] << " does not exist in Tree. " 
	   << "Will not include in analysis." << endl;
      PanCuts.erase(PanCuts.begin() + i);
      leaf_cut.erase(leaf_cut.begin() + i);
      i--;
    }
  }
#ifdef NOISY2
  for(UInt_t i=0; i<PanCuts.size(); i++) {
    cout << "PanCuts " << i << ": " << PanCuts[i] << endl;
  }
#endif


  // Grab the total number of entries
  TotEvents = (Int_t)fPairTree->GetEntries();
  cout << "Obtaining Data from Tree: " << TotEvents << " events "<< endl;

  // Fill the entry vectors

  vector <Double_t> CUTeventvector;
  vector <Double_t> DVeventvector;
  vector <Double_t> IVeventvector;
  CUTeventvector.clear();
  DVeventvector.clear();
  IVeventvector.clear();
  for(UInt_t ient = 0; ient<TotEvents; ient++) {
    fPairTree->GetEntry(ient);
    //    ev = leaf_evnum->GetValue();
    for (UInt_t icut = 0; icut<PanCuts.size(); icut++) {
      Double_t current = leaf_cut[icut]->GetValue();
      CUTeventvector.push_back(current);
    }
    for(UInt_t imon = 0; imon<DVnames.size(); imon++) {
      Double_t current = leaf_dvmon[imon]->GetValue();
      DVeventvector.push_back(current);
    }
    for(UInt_t imon = 0; imon<IVnames.size(); imon++) {
      Double_t current = leaf_ivmon[imon]->GetValue();
      IVeventvector.push_back(current);
    }
    PanCutEntries.push_back(CUTeventvector);
    DVEntries.push_back(DVeventvector);
    DVEntriesOrig.push_back(DVeventvector);
    IVEntries.push_back(IVeventvector);
    // this used to use lowest and highest events of the pattern... 
    //            I don't know why it was done this way. 
//     LEvEntries.push_back( (ev[0]>ev[1]) ? ev[1] : ev[0] );
//     HEvEntries.push_back( (ev[0]>ev[1]) ? ev[0] : ev[1] );
    LEvEntries.push_back( ev[0]);
    HEvEntries.push_back( ev[0] );
    CUTeventvector.clear();
    DVeventvector.clear();
    IVeventvector.clear();
  }
#ifdef NOISY2
  cout << "Entry vectors filled" << endl;
#endif

  // Accumulate Statistics for DVs and IVs (after cuts)
  DVStats = new TaStatistics (DVnames.size(), false);
  IVStats = new TaStatistics (IVnames.size(), false);
  
  // Get the custom cut, if it's there..
  TCut customcut;
  TEventList *regEList=0;
  if(fInput->GetConfig().GetCustomCut().first=="Hel_Tree") {
    cout << "Getting PairTree Entries satisfying customcut.." << endl;
    customcut = fInput->GetConfig().GetCustomCut().second;
    fPairTree->Draw(">>regEList",customcut);
    regEList = (TEventList*)gDirectory->Get("regEList");
    if(regEList) 
      if(regEList->GetN()!=0){
	cout << "\tObtained " << regEList->GetN()
	     << " entries satisfying customcut." << endl;
      } else {
	regEList = 0;
	cout << "\tNo entries obtained for customcut." << endl;
      }
  }

  //  Bool_t goodevent = kFALSE;
  // this used to default to false, but if no pan cuts defined, should be true.
  Bool_t goodevent = kTRUE;
  vector <Double_t> current;
  current.clear();
  GoodEvents.clear();
  nGoodEvents = 0;
  for (UInt_t ient=0; ient<TotEvents; ient++) {
    for (UInt_t icut=0; icut<PanCuts.size(); icut++) {
#ifdef NOISY3
      cout << "PanCutEntries[" << ient << "][" << icut << "] = " 
	   << PanCutEntries[ient][icut] << endl; 
#endif
      if (!PanCutEntries[ient][icut]) {
	goodevent = kFALSE;
	break;
      } else {
	goodevent = kTRUE;
      }
    }
    if(regEList) {
      if(!regEList->Contains(ient)) {
//	cout<<ient<<endl;
	goodevent = kFALSE;
      } else {
      goodevent = kTRUE;
      }
    }
    GoodEvents.push_back(goodevent);
    if (goodevent) {
#ifdef NOISY3
      cout << "Good Event" << endl;
#endif
      nGoodEvents++;
      current = DVEntries[ient];
      DVStats->Update(current);
      current = IVEntries[ient];
      IVStats->Update(current);
    }
  }
  cout << "Total number of initial entries:    " << TotEvents << endl;
  cout << "Entries passing all specified cuts: " << nGoodEvents << endl;


#ifdef NOISY2
  for (UInt_t iDV = 0; iDV<DVnames.size(); iDV++) {
    cout << DVnames[iDV] << " mean = " << DVStats->Mean(iDV) 
	 << " RMS = " <<DVStats->DataRMS(iDV) << endl;
  }
  for (UInt_t iIV = 0; iIV<IVnames.size(); iIV++) {
    cout << IVnames[iIV] << " mean = " << IVStats->Mean(iIV) 
	 << " RMS = " <<IVStats->DataRMS(iIV) << endl;
  }
#endif

  // Initialize the type of algorithm to use.
  if(fInput->GetConfig().GetRegressType() == "linear") LinearInit();
  if(fInput->GetConfig().GetRegressType() == "matrix") MatrixInit();

  return 0;

}


void
TaRegression::LinearInit ()
{
  ///////////////////////////////////////////////////////////////
  // First pass over data to get initial means and sigmas
  //
#ifdef DEBUG
  cout << "TaRegression::LinearInit()" << endl;
#endif
  
  // Get the total number of linear regression cycles
  LinearCycles = fInput->GetConfig().GetRegressCycles();

  delete DVStats;

}

void 
TaRegression::MatrixInit () 
{
  ///////////////////////////////////////////////////////////////
  // Initialization of Matrix Inversion Algorithm
  //     
#ifdef DEBUG
  cout << "TaRegression::MatrixInit()" << endl;
#endif
  
  // Need here to decide whether to Calculate Coefficents, Regress
  // Quantites, or both.  For now... just do both

  UInt_t PairsPerMinirun = fInput->GetConfig().GetRegressMinirun();

  if(PairsPerMinirun == 0) {
    // Produces One minirun for entire run
    cout << "Minirun unspecified or zero." 
	 <<"  Generating result for entire run." << endl;
    minirange.push_back(make_pair(0,TotEvents));
    minirangeEv.push_back(make_pair(LEvEntries[0],HEvEntries[TotEvents-1]));
    for(UInt_t i=0;i<TotEvents;i++)
      minirun.push_back(0);
  }
  
  if(PairsPerMinirun > 0) {
    // Produces miniruns specified by config file
    cout << "Number of pairs per minirun = " << PairsPerMinirun << endl;
    UInt_t NumMiniruns;

    if(PairsPerMinirun > nGoodEvents) {
      // For the rare occassion when the number of good events is
      // less than the number of Pairs per minirun
      cout << "   Number of good events (" 
	   << nGoodEvents << ")" 
	   << " is less than this." << endl
	   <<"   Generating result for entire run." << endl;
      minirange.push_back(make_pair(0,TotEvents));
      minirangeEv.push_back(make_pair(LEvEntries[0],HEvEntries[TotEvents-1]));
      for(UInt_t i=0;i<TotEvents;i++)
	minirun.push_back(0);
    } else {
      if(!fInput->GetConfig().MinirunUsesCuts()) {
	// Minirun is constant, regardless of cuts.
	NumMiniruns = TotEvents/PairsPerMinirun;
	for(UInt_t i = 0; i<NumMiniruns; i++) {
	  minirange.push_back(make_pair(i*PairsPerMinirun,
					(i+1)*PairsPerMinirun));
	  minirangeEv.push_back(make_pair(LEvEntries[i*PairsPerMinirun],
					  HEvEntries[(i+1)*PairsPerMinirun]));
	}
	if(minirange[NumMiniruns-1].second<TotEvents) {
	  minirange.push_back(make_pair(minirange[NumMiniruns-1].second,
					TotEvents));
	  minirangeEv.push_back(make_pair(LEvEntries[minirange[NumMiniruns-1].second],
					  HEvEntries[TotEvents-1]));
	}
	// Fill the minirun vector using the defined ranges
	for (UInt_t imini = 0; imini<minirange.size(); imini++) {
	  for (UInt_t iev=minirange[imini].first; iev<minirange[imini].second;
	       iev++) {
	    minirun.push_back(imini);
	  }
	}
      } else {
	// Minirun is constant, using cuts.
	// Go through the GoodEvents vector to fill the minirange
	UInt_t ncount=0; UInt_t startcount=0; UInt_t imini=0;
	for(UInt_t iev=0; iev<GoodEvents.size(); iev++) {
	  if(GoodEvents[iev] && startcount==0) startcount=iev;
	  if(GoodEvents[iev]) ncount++;
	  minirun.push_back(imini);
	  if(ncount==PairsPerMinirun) {
	    minirange.push_back(make_pair(startcount,iev));
	    minirangeEv.push_back(make_pair(LEvEntries[startcount],
					    HEvEntries[iev]));
	    startcount=0; ncount=0; imini++;
	  }
	}
	if(ncount!=0) {
	  // Combined the pairs left over into the last minirun;
	  pair <UInt_t,UInt_t> lastmin = minirange.back();
	  minirange.pop_back();
	  minirange.push_back(make_pair(lastmin.first,
					TotEvents-1));
	  minirangeEv.pop_back();
	  minirangeEv.push_back(make_pair(LEvEntries[lastmin.first],
					  HEvEntries[TotEvents-1]));
	  for(UInt_t i=lastmin.first; i<TotEvents; i++) {
	    minirun[i] = imini-1;
	  }
	}
      }
      
    }
    
#ifdef NOISY2
    for(UInt_t i = 0; i<minirange.size(); i++) {
      cout << "Minirun[" << i << "] = " << minirange[i].first 
	   << " " << (minirange[i].second) << endl;
    }
#endif
    cout << "Total number of miniruns = " << minirange.size() << endl;
  }
  
  if(PairsPerMinirun < 0) {
    // Uses BMWcycle to define miniruns - not implimented yet
    cout << "Using BMWcycle to define number of pairs per minirun." 
	 << PairsPerMinirun << endl;
    
  }
  
}  
 
  

void TaRegression::Process () {
  ///////////////////////////////////////////////////////////////
  // Gateway to regression alogorithms
  //  
  //
#ifdef DEBUG
  cout << "TaRegression::Process()" << endl;
#endif
  if(fInput->GetConfig().GetRegressType() == "linear") ProcessLinear();
  if(fInput->GetConfig().GetRegressType() == "matrix") ProcessMatrix();

}


void TaRegression::ProcessLinear () {
  ///////////////////////////////////////////////////////////////
  // Main linear regression alogorithm
  //  
  //
#ifdef DEBUG
  cout << "TaRegression::ProcessLinear()" << endl
       << "\tRegression cycles = " << LinearCycles 
       << "\t #IV = " << IVnames.size() << " #DV = " << DVnames.size() << endl;
#endif

  for(Int_t itimes=0; itimes<LinearCycles; itimes++) {

    // Loop over all DVs
    for(UInt_t iDV=0; iDV<DVnames.size(); iDV++) {
      DVStats = new TaStatistics (1, false);      

      for(UInt_t ient=0; ient<TotEvents; ient++) {
	if(!GoodEvents[ient]) continue; // Dont include events that fail cuts
	DVStats->Update(DVEntries[ient][iDV]);
      }

#ifdef NOISY3
	cout << DVnames[iDV] << " mean = " 
	     << DVStats->Mean(0) << endl;
#endif	
      // Loop over all IVs
      for(UInt_t iIV=0; iIV<IVnames.size(); iIV++) {
	
	Double_t num=0, denom=0, b=0;
	// Calculate regression coefficient b
	for(UInt_t iev = 0; iev<TotEvents; iev++) {
	  if(!GoodEvents[iev]) continue; // Dont include events that fail cuts
	  num += (DVEntries[iev][iDV] - DVStats->Mean(0))*
	    (IVEntries[iev][iIV] - IVStats->Mean(iIV));
	  denom += (IVEntries[iev][iIV] - IVStats->Mean(iIV))*
	    (IVEntries[iev][iIV] - IVStats->Mean(iIV));
	}
	b = num/denom;
#ifdef NOISY2
	cout << "b(" << iIV << ") Before:" << b;
#endif
	
	// Regress IV from DV
	for(UInt_t iev=0;iev<TotEvents; iev++) {
	  DVEntries[iev][iDV] = (DVEntries[iev][iDV]) 
	    - b*(IVEntries[iev][iIV] - IVStats->Mean(iIV));
	}

	// Recalculate DV mean for next IV

	DVStats->Zero();
	for(UInt_t iev=0;iev<TotEvents; iev++) {
	  if(!GoodEvents[iev]) continue; // Dont include events that fail cuts
	  DVStats->Update(DVEntries[iev][iDV]);
	}
#ifdef NOISY3
	cout << DVnames[iDV] << " mean = " 
	     << DVStats->Mean(0) << endl;
#endif	
#ifdef NOISY2
	// Re-Calculate regression coefficient
	num=0; denom=0; b=0;
	for(Int_t iev = 0; iev<TotEvents; iev++) {
	  if(!GoodEvents[iev]) continue; // Dont include events that fail cuts
	  num += (DVEntries[iev][iDV] - DVStats->Mean(0))*
	    (IVEntries[iev][iIV] - IVStats->Mean(iIV));
	  denom += (IVEntries[iev][iIV] - IVStats->Mean(iIV))*
	    (IVEntries[iev][iIV] - IVStats->Mean(iIV));
	}
	b = num/denom;
	cout << " After:" << b << endl;
#endif
      }
      
      cout << itimes+1 << ": Regressed " << DVnames[iDV] 
	   << " mean (" << TotEvents << "): " 
	   << DVStats->Mean(0) << " +/- " << DVStats->DataRMS(0)
	   << endl;
    }
  }
  FillTree();
}

void TaRegression::ProcessMatrix () {
  ///////////////////////////////////////////////////////////////
  // Perform Matrix Calculation
  //
#ifdef DEBUG
  cout << "TaRegression::ProcessMatrix()" << endl;
#endif

  for (UInt_t imini = 0; imini<minirange.size(); imini++) {
#ifdef NOISY
    cout << "Minirun = " << imini << "  Events: " << minirange[imini].first 
	 << "-" << (minirange[imini].second) << endl;
#endif
    DVStats->Zero();
    IVStats->Zero();

    UInt_t goodcount=0;
    
    for (UInt_t iev=minirange[imini].first; 
	 iev<=minirange[imini].second; iev++) {
      if (!GoodEvents[iev]) continue;
      vector <Double_t> currentDV = DVEntries[iev];
      vector <Double_t> currentIV = IVEntries[iev];
      DVStats->Update(currentDV);
      IVStats->Update(currentIV);
      goodcount++;
    }
    WriteStats(imini,kFALSE);
    if (goodcount == 0) {
      cout << "  Zero pairs passing defined PAN cuts. " 
	   << "Proceeding to next minirun." << endl;
      // Store empty coefficients
      vector < vector <Double_t> > mini_coeff; // this minirun
      for(UInt_t iDV=0; iDV<DVnames.size(); iDV++) {
	vector <Double_t> dv_coeff; // this DV
	for(UInt_t iIV=0; iIV<IVnames.size(); iIV++) {
	  dv_coeff.push_back(0);
	}
	mini_coeff.push_back(dv_coeff);
      }
      mCoeff.push_back(mini_coeff);
      mErrCoeff.push_back(mini_coeff);
      WriteSlopes(imini);
      WriteStats(imini,kTRUE);
      continue;
    }

    cout << "  Number of good events in this minirun = " << goodcount << endl;

    // Calculate IV matrix (X_ij from talk)
    Matrix X(IVnames.size(),IVnames.size());
    for (UInt_t irow = 0; irow<IVnames.size(); irow++) {
      for (UInt_t icol = irow; icol<IVnames.size(); icol++) {
	Double_t element = 0;
	for (UInt_t iev = minirange[imini].first; iev<minirange[imini].second; 
	     iev++) {
	  if(!GoodEvents[iev]) continue;  // Dont include events that fail cuts
	  element += ((IVEntries[iev][irow] - IVStats->Mean(irow)) *
		      (IVEntries[iev][icol] - IVStats->Mean(icol)));
	  
	}
	X.Assign(irow,icol,element);
	if(icol != irow) X.Assign(icol,irow,element);
      }
    }
#ifdef NOISY2
    cout << "IV Matrix (X_ij)" << endl;
    X.Print();
#endif  
    
    // Calculate DV-IV matrix (Y_i from talk)
    Matrix Y(DVnames.size(),IVnames.size());
    for(UInt_t iDV = 0; iDV<DVnames.size(); iDV++) {
      for(UInt_t iIV = 0; iIV<IVnames.size(); iIV++) {
	Double_t element = 0;
	for (UInt_t iev = minirange[imini].first; iev<minirange[imini].second; 
	     iev++) {
	  if(!GoodEvents[iev]) continue; // Dont include events that fail cuts
	  element += ((IVEntries[iev][iIV] - IVStats->Mean(iIV)) *
		      (DVEntries[iev][iDV] - DVStats->Mean(iDV)));
	}
	Y.Assign(iDV,iIV,element);
      }
    }

#ifdef NOISY2
    cout << "DV-IV Matrix (Y_j)" << endl;
    Y.Print();
#endif  
    
    // Invert IV Matrix
    Matrix Xinv = X.Inverse();
    if (Xinv.NRows() == 0) {
      cout << "\tX Matrix is singular.  Proceeding to next minirun" << endl;
      // Store empty coefficients
      vector < vector <Double_t> > mini_coeff; // this minirun
      for(UInt_t iDV=0; iDV<DVnames.size(); iDV++) {
	vector <Double_t> dv_coeff; // this DV
	for(UInt_t iIV=0; iIV<IVnames.size(); iIV++) {
	  dv_coeff.push_back(0);
	}
	mini_coeff.push_back(dv_coeff);
      }
      mCoeff.push_back(mini_coeff);
      mErrCoeff.push_back(mini_coeff);
      WriteSlopes(imini);
      WriteStats(imini,kTRUE);
      continue;
    }
#ifdef NOISY2
    cout << "IV Matrix Inverse (X_ij)^(-1)" << endl;
    Xinv.Print();
#endif  
    
#ifdef NOISY2
    Matrix CheckUni(IVnames.size(),IVnames.size());
    CheckUni = X.MultiplyBy(Xinv);
    cout << " (X_ij)*(X_ij)^(-1) - Check for Unit Matrix" << endl;
    CheckUni.Print();
#endif  
    
    // Compute matrix of regression coefficients
    Matrix R(DVnames.size(),IVnames.size());
    R = Y.MultiplyBy(Xinv);
    
#ifdef NOISY2
    cout << " Matrix of Regression Coefficients" << endl;
    R.Print();
#endif  
    
#ifdef NOISY2
    // Check to see if you get zero
    Matrix Check(DVnames.size(), IVnames.size());
    Matrix Check2(DVnames.size(), IVnames.size());
    Check2 = R.MultiplyBy(X);
    for(UInt_t irow=0; irow<DVnames.size(); irow++) {
      for(UInt_t icol=0; icol<IVnames.size(); icol++) {
	Check.Assign(irow,icol,Y.Get(irow,icol)-Check2.Get(irow,icol));
      }
    }
    
    cout << "Recalculated Y from R*X" << endl;
    Check2.Print(); cout << endl;
    cout << "Zero check: Y-R*X" << endl;
    Check.Print();
#endif
    
    
#ifdef NOISY
    cout << "  Before Regression" << endl;
    for (UInt_t iDV = 0; iDV<DVnames.size(); iDV++) {
      cout << "   " << DVnames[iDV] << " mean = " << DVStats->Mean(iDV) 
	   << " RMS = " <<DVStats->DataRMS(iDV) << endl;
    }
#endif

    // Store the coefficients
    vector < vector <Double_t> > mini_coeff; // this minirun
    vector < vector <Double_t> > mini_Errcoeff; // this minirun
    for(UInt_t iDV=0; iDV<DVnames.size(); iDV++) {
      vector <Double_t> dv_coeff; // this DV
      vector <Double_t> dv_Errcoeff; // this DV
      for(UInt_t iIV=0; iIV<IVnames.size(); iIV++) {
	dv_coeff.push_back(R.Get(iDV,iIV));
	// Below... just need the variances... 
	dv_Errcoeff.push_back(sqrt(Xinv.Get(iIV,iIV))*DVStats->DataRMS(iDV));
      }
      mini_coeff.push_back(dv_coeff);
      mini_Errcoeff.push_back(dv_Errcoeff);
    }
    mCoeff.push_back(mini_coeff);
    mErrCoeff.push_back(mini_Errcoeff);


    // Regress the IVs from the DVs
    for(UInt_t iev = minirange[imini].first; 
	iev<minirange[imini].second; iev++) {
      if(!GoodEvents[iev]) continue;
      for(UInt_t iDV = 0; iDV<DVnames.size(); iDV++) {
	Double_t RIVsum = 0;
	for (UInt_t iIV = 0; iIV<IVnames.size(); iIV++) {
	  RIVsum += R.Get(iDV,iIV) * 
	    // Changed this line, so that the mean is regressed
	    //	    (IVEntries[iev][iIV] - IVStats->Mean(iIV));
	    (IVEntries[iev][iIV]);
	}
	DVEntries[iev][iDV] = DVEntries[iev][iDV] - RIVsum;
      }
    }
    
    // Recalculate Means and RMS for DVs
    //  DVStats = new TaStatistics (DVnames.size(), false);
    DVStats->Zero();
    
    vector <Double_t> current;
    current.clear();
    for (UInt_t iev=minirange[imini].first; 
	 iev<minirange[imini].second; iev++) {
      if (!GoodEvents[iev]) continue;
      current = DVEntries[iev];
      DVStats->Update(current);
    }
    
#ifdef NOISY
    cout << "  After Regression" << endl;
    for (UInt_t iDV = 0; iDV<DVnames.size(); iDV++) {
      cout << "   " <<  DVnames[iDV] << " mean = " << DVStats->Mean(iDV) 
	   << " RMS = " <<DVStats->DataRMS(iDV) << endl;
    }
    cout << "\tSlopes:\n";
    for(UInt_t iDV = 0; iDV<DVnames.size(); iDV++) {
      for(UInt_t iIV=0; iIV<IVnames.size(); iIV++) {
	cout << "\t\t" << DVnames[iDV] << " vs. ";
	cout << IVnames[iIV] << " = " 
	     << mini_coeff[iDV][iIV]
	     << " +/- " 
	     << mini_Errcoeff[iDV][iIV]
	     << endl;
      }
    }
#endif
    WriteStats(imini,kTRUE);
    WriteSlopes(imini);
  } // End of minirun FOR-loop
  FillTree();
}

void TaRegression::WriteStats(UInt_t imini, Bool_t regressed) {
  // Write out the statistics for the DV
  if(!regressed) {
    (*fTxtOut) << "# Dependent variable statistics for minirun " << imini 
	       << " ===================" << endl;  
  }
  for (UInt_t iDV = 0; iDV < DVnames.size(); iDV++) {
    TString regname = "minirun_";
    regname += imini;
    if(regressed) regname += "_regressed";
    regname += "_";
    regname += DVnames[iDV];
    regname += "_";
    fTxtOut->WriteNextLine (regname + "mean",
			    DVStats->Mean(iDV),
			    DVStats->MeanErr(iDV),
			    minirange[imini].first,
			    minirange[imini].second-1,"","");
    fTxtOut->WriteNextLine (regname + "RMS",
			    DVStats->DataRMS(iDV),
			    0,
			    minirange[imini].first,
			    minirange[imini].second-1,"","");
    fTxtOut->WriteNextLine (regname + "Neff",
			    DVStats->Neff(iDV),
			    0,
			    minirange[imini].first,
			    minirange[imini].second-1,"","");
  }
}

void TaRegression::WriteSlopes(UInt_t imini) {
  // Write out the matrix regression slopes to the results file.

  (*fTxtOut) << "# Minirun slopes for minirun " << imini 
	     << " ===================" << endl;  
  for (UInt_t iDV = 0; iDV < DVnames.size(); iDV++) {
    TString slopename = "minirun_";
    slopename += imini;
    slopename += "_";
    slopename += DVnames[iDV];
    slopename += "_vs_";
    for (UInt_t iIV = 0; iIV < IVnames.size(); iIV++) {
      TString name_end = slopename;
      name_end += IVnames[iIV];
      name_end += "_slope";
      fTxtOut->WriteNextLine (name_end, 
			      mCoeff[imini][iDV][iIV], 
			      mErrCoeff[imini][iDV][iIV], 
			      minirange[imini].first, 
			      minirange[imini].second-1, "", "");
    }
  }    
}

void TaRegression::FillTree() {
#ifdef DEBUG
  cout << "TaRegression::FillTree()" << endl;
#endif

  cout << "Filling reg tree with regressed variables" << endl;

  TTree reg("reg","regression tree");
  TTree regcoeffs("regcoeffs","regression coefficients tree");
  regcoeffs.SetMarkerStyle(20);
  Double_t iv[IVnames.size()], dv[DVnames.size()], dvorig[DVnames.size()],
    cuts;
  //  Int_t ev,first_ev,last_ev;
  Double_t ev,first_ev,last_ev;
  Int_t mini;
  Double_t coeff[DVnames.size()][IVnames.size()];
  Double_t Err_coeff[DVnames.size()][IVnames.size()];
  TString buff, buff2;

  reg.Branch("ok_cut",&cuts,"ok_cut/D");
  reg.Branch("reg_mini_first_ev",&first_ev,"reg_mini_first_ev/I");
  reg.Branch("reg_mini_last_ev",&last_ev,"reg_mini_last_ev/I");
  regcoeffs.Branch("reg_mini_first_ev",&first_ev,"reg_mini_first_ev/I");
  regcoeffs.Branch("reg_mini_last_ev",&last_ev,"reg_mini_last_ev/I");

  for(UInt_t k = 0; k<IVnames.size(); k++) {
    buff = IVnames[k];
    buff2 = IVnames[k] + "/D";
    reg.Branch(buff,&iv[k],buff2);
  }
  for(UInt_t k = 0; k<DVnames.size(); k++) {
    buff = "reg_" + DVnames[k];
    buff2 = "reg_" + DVnames[k] + "/D";
    reg.Branch(buff,&dv[k],buff2);
  }
  for(UInt_t k = 0; k<DVnames.size(); k++) {
    buff = DVnames[k];
    buff2 = DVnames[k] + "/D";
    reg.Branch(buff,&dvorig[k],buff2);
  }

  reg.Branch("ev",&ev,"ev/I");

  // Create a minirun and coeffs branch only if we're using the Matrix
  // algorithm
//   if((fInput->GetConfig().GetRegressType() == "matrix") &&
//      (fInput->GetConfig().GetRegressMinirun() != 0)) {
  if (fInput->GetConfig().GetRegressType() == "matrix") {
    reg.Branch("minirun",&mini,"minirun/I");
    regcoeffs.Branch("minirun",&mini,"minirun/I");
    TString Err_coeffBuff;
    TString coeffBuff = "coeff[";
    coeffBuff += DVnames.size();
    coeffBuff += "][";
    coeffBuff += IVnames.size();
    coeffBuff += "]/D";
    Err_coeffBuff = coeffBuff;
    Err_coeffBuff.Prepend("err_");
//     cout << coeffBuff << endl;
    regcoeffs.Branch("coeff",&coeff,coeffBuff);
    regcoeffs.Branch("err_coeff",&Err_coeff,Err_coeffBuff);
    rootdef = new TaRootRegDef();
    for (UInt_t i=0; i<IVnames.size(); i++) {
      rootdef->PutIV(IVnames[i]);
    }
    for (UInt_t i=0; i<DVnames.size(); i++) {
      rootdef->PutDV(DVnames[i]);
    }
    rootdef->Write();
  }

  // fill the tree (pairs first, then slopes)
  for(UInt_t i=0; i<GoodEvents.size(); i++) {
    for(UInt_t jj=0; jj<DVnames.size(); jj++) {
      dv[jj] = DVEntries[i][jj];
      dvorig[jj] = DVEntriesOrig[i][jj];
    }
    for(UInt_t j=0; j<IVnames.size(); j++) {
      iv[j] = IVEntries[i][j];
    }
//     if((fInput->GetConfig().GetRegressType() == "matrix") &&
//        (fInput->GetConfig().GetRegressMinirun() != 0)) {
    if (fInput->GetConfig().GetRegressType() == "matrix") {
      mini = minirun[i];
    }
    ev = i;
    cuts = GoodEvents[i];
    first_ev = minirangeEv[mini].first;
    last_ev = minirangeEv[mini].second;
    reg.Fill();
  }
  reg.Write();

  if (fInput->GetConfig().GetRegressType() == "matrix") {
    for(UInt_t imini=0; imini<minirange.size(); imini++) {
    mini = imini;
    first_ev = minirangeEv[imini].first;
    last_ev = minirangeEv[imini].second;
    for(UInt_t idv=0;idv<DVnames.size();idv++) 
      for(UInt_t iiv=0;iiv<IVnames.size();iiv++) {
	coeff[idv][iiv] = mCoeff[imini][idv][iiv];
	Err_coeff[idv][iiv] = mErrCoeff[imini][idv][iiv];	  
      }
    regcoeffs.Fill();
    }
  }
  regcoeffs.Write();
}
