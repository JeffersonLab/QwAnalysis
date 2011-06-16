//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer PostPan
//
//           TaDitCyc.cc
//
// Author:  K.D. Paschke
//
////////////////////////////////////////////////////////////////////////
//
//    Helper class for TaDitAna.  Provides a unit of the dithering
//    analysis, representing a single modulation supercycle.
// 
//      Globally: -macro for spoke plots
//      Needed: 
//              - better calc of averages/slopes
//              - routines to check the quality of data for each monitor in the 
//                supercycle to avoid problems from incomplete data
//              - an overall supercycle quality determination, 
//              - the machinery to calculate errors on slopes, 
//                quality-of-fit for slopes (scale the error on slopes
//                if appropriate)
//              - output information (histograms, text files?) on the slopes,
//                or pass up to TaDitAna for this purpose 
//              - calculate the correction coefficients, errors on coefficients, 
//                quality-of-estimate, diagnostic and status information for 
//                coeff calculation,
//              - relevant output (histograms, text files, correction data 
//                structure for TaDitAna?).
//
//      Laundry List:
//               - quality criteria on individual coils/supercycles
//               - way to flag TaDitAna to accum additional supercyc if
//                 some coils are underfilled.
//               - fix inefficient/unstable matrix inversion with better techniques
//               - Error calculation, including error from fit scaled by chisq?
//
////////////////////////////////////////////////////////////////////////

// #define NOISY

#include "TaDitCyc.hh"
#include "Matrix.hh"

using namespace std;

ClassImp(TaDitCyc)


TaDitCyc::TaDitCyc():
  fSlps_Done(0),fCoeffs_Done(0),
  fNDetUsed(0),fNMonUsed(0),
  fFirstEvNum(-1),fLastEvNum(-1),
  fLostEventCounter(0)
{
  // allocate no memory in default constructor to avoid memory leaks
  for (Int_t i=0; i<fgMaxModObj; i++) fNEvObj[i] =0;

}


TaDitCyc::TaDitCyc(const TaDitCyc &)
{
}


TaDitCyc::~TaDitCyc() {
//   if (fAMat) delete fAMat;
//   if (fRMatInv) delete fRMatInv;
//   if (fSMat) delete fSMat;
}


Bool_t TaDitCyc::IsCompatible(TaDitCyc* compCyc) {
  //**
  // compare coils used
  if (fNDitUsed != compCyc->GetNumDit()) {
    cout << "fNDitUsed " << fNDitUsed << " " << compCyc->GetNumDit() <<endl;
    return kFALSE;
  }
  //**
  // compare number of detectors, monitors
  if (fNDetUsed!=compCyc->GetNumDet()) {
    cout << "mismatch num det " << fNDetUsed << " " << compCyc->GetNumDet() << endl;
    return kFALSE;
  }
  if (fNMonUsed!=compCyc->GetNumMon()) {
    cout << "mismatch num mon " << fNMonUsed << " " << compCyc->GetNumMon() << endl;
    return kFALSE;
  }
  //**
  // compare names of detectors, monitors 
  //          (this is tough, cause TaDitCyc doesn't have these yet!)
  //**
  // check event ranges are contiguous
  if ( (abs(GetFirstEvent()-compCyc->GetLastEvent())>2) 
    && (abs(GetLastEvent()-compCyc->GetFirstEvent())>2) ) return kFALSE;
  //**
  return kTRUE;
}

Bool_t TaDitCyc::Merge(TaDitCyc* mergeCyc) {
  // merges two TaDitCycs into one.  Useful when a cyc lacks data
  // for a good fit or necessary precision: TaDitAna can combine
  // sequential supercycles worth of data...
  Bool_t returnFlag = IsCompatible(mergeCyc);
  if (returnFlag) {
    // set first event to earliest event
    SetFirstEvent( (GetFirstEvent() < mergeCyc->GetFirstEvent() ? 
		    GetFirstEvent() : mergeCyc->GetFirstEvent()) );
    // set last sc event to latest event
    SetLastSCEvent( (GetLastSCEvent() > mergeCyc->GetLastSCEvent() ? 
		     GetLastSCEvent() : mergeCyc->GetLastSCEvent()) );
    // set last event to latest event
    SetLastEvent( (GetLastEvent() > mergeCyc->GetLastEvent() ? 
		   GetLastEvent() : mergeCyc->GetLastEvent()) );
    // combine modulation object data arrays:
    cout << "Merged supercycles from Event " << GetFirstEvent() << 
      " to event " << GetLastEvent() << endl;
    cout << "last supercycle included is supercycle " << 
      mergeCyc->GetSuperCycleNumber() << endl;
    for (Int_t ic=0; ic<fNDitUsed; ic++) {
      ObjCyc mergeObj = mergeCyc->fObjCycs[ic];
      Int_t nWinAdd = mergeCyc->fNEvObj[ic];
      cout << "adding " << nWinAdd << " windows to coil " << ic << endl;
      for (Int_t iwin = 0; iwin<nWinAdd; iwin++) {

 	(fObjCycs[ic].sModVal).push_back((mergeObj.sModVal)[iwin]);

 	for (Int_t idet = 0 ; idet<fNDetUsed; idet++) {
#ifdef EVILERRORMESSAGE
	  if ((mergeObj.sDetVal[idet])[iwin]<0.1) {
	    cout << "what the fuck! " <<  mergeCyc->GetSuperCycleNumber() 
		 << endl;
	  }
#endif
 	  (fObjCycs[ic].sDetVal[idet]).push_back((mergeObj.sDetVal[idet])[iwin]);
#ifdef EVILERRORMESSAGE
	  if ((fObjCycs[ic].sDetVal[idet])[fNEvObj[ic]]<0.1) {

 	    cout << "Really what the fuck! " 
 		 <<  " SC: " << mergeCyc->GetSuperCycleNumber() 
 		 <<  " iwin = " << iwin 
 		 <<  " iEVObj: " << fNEvObj[ic] << endl;
	  }
#endif
	}

 	for (Int_t imon = 0 ; imon<fNMonUsed; imon++)
 	  (fObjCycs[ic].sMonVal[imon]).push_back((mergeObj.sMonVal[imon])[iwin]);

	fNEvObj[ic]++;
      }
      cout << "  Added " << nWinAdd << " to coil " << ic 
	    << " for a total of " << fNEvObj[ic]+nWinAdd << " windows " << endl;
    }



    // unset any calculation_completed flags
    fSlps_Done=kFALSE; fCoeffs_Done=kFALSE;
  } else {
    cout <<"Somehow, you have managed to attempt to merge incompatible supercycles";
    cout << "\n\t No merging will be done, and you will probably lose one of these supercycles!" << endl;
  }
  return returnFlag;
}



TaDitCyc* TaDitCyc::Clone() {
  // returns a new TaDitCyc, complete with memory allocations and the
  // same numbers of coils, detectors and monitors used, but with 
  // otherwise empty data members
  //  cout << "cloning a cycle" << endl;
  TaDitCyc* newguy = new TaDitCyc();
  //  newguy->Create();
  //  cout << "setting number of Coils " << fNDitUsed << endl;
  newguy->SetDitNum(fNDitUsed);
  //  cout << "setting number of dets " << fNDetUsed << endl;
  newguy->SetDetNum(fNDetUsed);
  //  cout << "setting number of monitors " << fNMonUsed << endl;
  newguy->SetMonNum(fNMonUsed);
  return newguy;
}


TaDitCyc* TaDitCyc::Copy() {
  // returns a new TaDitCyc, complete with memory allocations and the
  // same numbers of coils, detectors and monitors used, and with
  // all data copied as well
  //  cout << "copying a cycle" << endl;
  TaDitCyc* newguy = Clone();

    newguy->SetFirstEvent(GetFirstEvent());
    newguy->SetLastSCEvent(GetLastSCEvent());
    newguy->SetLastEvent(GetLastEvent());
    newguy->SetSuperCycleNumber(GetSuperCycleNumber());

    // Copy modulation object data arrays:
     for (Int_t ic=0; ic<fNDitUsed; ic++) {
       ObjCyc newObj = newguy->fObjCycs[ic];
       Int_t nWinAdd = fNEvObj[ic];
       for (Int_t iwin = 0; iwin<nWinAdd; iwin++) {
	 (newObj.sModVal).push_back(fObjCycs[ic].sModVal[iwin]);
	 for (Int_t idet = 0 ; idet<fNDetUsed; idet++) 
	   (newObj.sDetVal[idet]).push_back(fObjCycs[ic].sDetVal[idet][iwin]);
	 for (Int_t imon = 0 ; imon<fNMonUsed; imon++) 
	   (newObj.sMonVal[imon]).push_back(fObjCycs[ic].sMonVal[imon][iwin]);
       }
       newguy->fNEvObj[ic] = fNEvObj[ic];
     }

    // unset any calculation_completed flags
    newguy->fSlps_Done=kFALSE; fCoeffs_Done=kFALSE;

  return newguy;
}

void TaDitCyc::CalcSlopes() 
{
  fSlps_Done=kFALSE;

  // calculate slopes for detector and monitors vs. Coils, 
  // along with errors and goodness-of-fit
  for (Int_t ic=0; ic<fNDitUsed; ic++) {
    // need to check for goodness of num this cycle, to avoid blowing up
    // I should have better checks,and also do something intelligent 
    // in TaDitAna if this fails for insufficient datab
    if (fNEvObj[ic]<15) {
      cout << "Can't Calculate Slopes, Cycle Underpopulated for item: " << ic
	   << " in cycle " << fSuperCycNum << endl;
      return;
    }
    vector <Double_t> x;
    x = fObjCycs[ic].sModVal;

    // calculate x-mean
    Double_t xmean=0;
    for (Int_t iev = 0; iev<fNEvObj[ic]; iev++) xmean += x[iev];
    xmean /= fNEvObj[ic];

    // for each detector
    for (Int_t idet = 0 ; idet<fNDetUsed; idet++) {
      vector<Double_t> y;
      y = fObjCycs[ic].sDetVal[idet];

      Double_t ymean=0, Sxx=0, Syy=0, Sxy=0, b=0, sigb=0;
      // calculate y-mean
      for (Int_t iev = 0; iev<fNEvObj[ic]; iev++) ymean += y[iev];
      ymean /= fNEvObj[ic];
      
      for (Int_t iev = 0; iev<fNEvObj[ic]; iev++) {
	Syy += (y[iev]-ymean)*(y[iev]-ymean);
	Sxy += (y[iev]-ymean)*(x[iev]-xmean);
	Sxx += (x[iev]-xmean)*(x[iev]-xmean);
      }
      b = Sxy/Sxx;
      sigb = (Syy - Sxy*Sxy/Sxx)/(Sxx*(fNEvObj[ic]-2));
      if (sigb>0) sigb = sqrt(sigb);
      else {
	cout << "Error in calculating slope for Detector " << idet 
	     << " in cycle " << fSuperCycNum << endl;
	cout << "      Xmean = " << xmean << endl;
	cout << "      Ymean = " << ymean << endl;
	cout << "      Syy = " << Syy << endl;
	cout << "      Sxy = " << Sxy << endl;
	cout << "      Sxx = " << Sxx << endl;
	cout << "      Number of ev = " << fNEvObj[ic] << endl;
	cout << "      b = " << b << endl;
	cout << "      sigb = " << sigb << endl;
	sigb = 1.E6;
	fSlps_Done=kFALSE;
	return;	
    }
      // scale slope by mean detector value to get fractional change
//       fDetDSlp[ic][idet] = b/abs(ymean);
//       fDetDSlpE[ic][idet] = sigb/abs(ymean);
      fDetDSlp[ic][idet] = (b*1e6)/(2*abs(ymean));
      fDetDSlpE[ic][idet] = (sigb*1e6)/(2*abs(ymean));
      // cout << "I fit a detector slope of " << fDetDSlp[ic][idet] << endl;
    }
    // for each monitor
    for (Int_t imon = 0 ; imon<fNMonUsed; imon++) {
      vector <Double_t> y;
      y = fObjCycs[ic].sMonVal[imon];
      
      Double_t ymean=0, Sxx=0, Syy=0, Sxy=0, b=0, sigb=0;
      // calculate y-mean
      for (Int_t iev = 0; iev<fNEvObj[ic]; iev++) ymean += y[iev];
      ymean /= fNEvObj[ic];
      
      for (Int_t iev = 0; iev<fNEvObj[ic]; iev++) {
	Syy += (y[iev]-ymean)*(y[iev]-ymean);
	Sxy += (y[iev]-ymean)*(x[iev]-xmean);
	Sxx += (x[iev]-xmean)*(x[iev]-xmean);
      }
      b = Sxy/Sxx;
      sigb = (Syy - Sxy*Sxy/Sxx)/(Sxx*(fNEvObj[ic]-2));
      if (sigb>0) sigb = sqrt(sigb);
      else {
	sigb = 1.E6;
	cout << "Error in calculating slope for Monitor " << imon 
	     << " in cycle " << fSuperCycNum << endl;
	fSlps_Done=kFALSE;
	return;
      }

      // for each monitor, calculate dB/dC
      fMonDSlp[ic][imon] = b*1e3;  // 1e3 to convert to micron
      // for each monitor, calculate error on dB/dC
      fMonDSlpE[ic][imon] = sigb*1e3;
      // for each monitor, calculate goodness-of-fit for dB/dC, scale errors?
      // output info if goodness-of-fit is very bad
      // ... or do all that that later ...	
//       cout << "For Monitor " << imon << " and Coil " << ic <<
// 	" I fit a monitor slope of " << fMonDSlp[ic][imon] << endl;


    }
    // check for dominance of dD/dC errors over dB/dC... which is the operating
    // assumption for the next step.  Comparison will involve some default
    // or assumed value for dD/dB, in order to scale between the two measures.
    //   ??? I have no idea what the hell I was thinking when I wrote the 
    //       above comment.  Apparently, I had some insight into error calculation
    //       that I no longer possess, dammit.  I'll do some more thinking about 
    //       this ...
    
    // set some flag to verify good calculation of slopes.  
    // It'd be reasonable to do that here...    
  }    

  fSlps_Done=kTRUE;
}

void TaDitCyc::CalcCoeffs() 
{
  // calculate the correction coefficients and covariance matrix
  // using results of CalcSlopes
  if (!fSlps_Done) {
    cout << "Can't calculate Coeffs because slopes aren't calculated!" << endl;
    return;
  }


  // create matrix A_lj = Sum_i dD_l/dC_i*dB_j/dC_i for each detector D_l
  // create matrix R_kj = Sum_i dB_k/dC_i*dB_j/dC_i 
  //**
  // create matrix A_lj = Sum_i dD_l/dC_i*dB_j/dC_i for each detector D_l
  // **
  Double_t element;
  Matrix AMat(fNDetUsed,fNMonUsed);
  for (Int_t id = 0; id<fNDetUsed; id++) {
    for (Int_t im = 0; im<fNMonUsed; im++) {
      element =0;
      for (Int_t ic = 0; ic<fNDitUsed; ic++)
	element += fDetDSlp[ic][id]*fMonDSlp[ic][im];
      AMat.Assign(id,im,element);
    }
  }
#ifdef NOISY
  cout << "AMat  = " << endl;
  AMat.Print();
#endif
  //**
  // create matrix R_kj = Sum_i dB_k/dC_i*dB_j/dC_i 
  // **
  Matrix RMat(fNMonUsed,fNMonUsed);
  for (Int_t im1 = 0; im1<fNMonUsed; im1++) {
    for (Int_t im2 = im1; im2<fNMonUsed; im2++) {
      element = 0;
      for (Int_t ic = 0; ic<fNDitUsed; ic++) 
	element += fMonDSlp[ic][im1]*fMonDSlp[ic][im2];
      RMat.Assign(im1,im2,element);
      if (im1!=im2) RMat.Assign(im2,im1,element);
    }
  }
#ifdef NOISY
  cout << "RMat  = " << endl;
  RMat.Print();
#endif
  // invert R_kj
  //  Double_t RDet = 1;
  Matrix RMatInv = RMat.Inverse();
#ifdef NOISY
  cout << "RMatInv  = " << endl;
  RMatInv.Print();
#endif
  if (RMatInv.NRows() == 0) {
    cout << "\tX Matrix is singular.  Proceeding to next supercycle" << endl;
    return;
  }

  //  fRMatInv->Invert(&Rdet);
  //  cout << "inverted matrix and got determinant = " << Rdet << endl;
  // if fail, print error (other diagnostics such as order mismatch or SVD?)
  // verify that inverse has been found.
#ifdef NOISY
  Matrix CheckUni(fNMonUsed,fNMonUsed);
  CheckUni = RMat.MultiplyBy(RMatInv);
  cout << " (R_ij)*(R_ij)^(-1) - Check for Unit Matrix" << endl;
  CheckUni.Print();
//   ident *= (TMatrixD) *fRMatInv;
//   TMatrixD unit(fNMonUsed,fNMonUsed);
//   unit.UnitMatrix();
//   VerifyMatrixIdentity(unit,ident);
#endif

  // **
  //calculate matrix S_lk= dD_l/dB_k = A_lj (R_kj)^(-1)  for each detector D_l
  // **
  Matrix SMat(fNDetUsed,fNMonUsed);
  SMat = AMat.MultiplyBy(RMatInv);

  //  fSMat = new TMatrixD(*fRMatInv,TMatrixD::kMult,*fAMat);
  // Calculate chisq = Sum_i  (1/sig^2_i)*[dD/dC_i - Sum_j R_j*dB_j/dC_i]^2 for each
  //  detector.  This provides the only goodness-of-fit measure!

  // Also store covariance matrix for S, V= R^(-1)  for each detector. 
  //    This is your error estimate!


  // Store Coefficients
  for (Int_t id = 0; id<fNDetUsed; id++) {
    for (Int_t im = 0; im<fNMonUsed; im++) {
      fDetMonCoef[id][im] = SMat.Get(id,im);
      fDetMonCoefE[id][im] = -1.0;  // no ideas here yet...
    }
  }

  fCoeffs_Done=kTRUE;

}


void TaDitCyc::LoadData(Int_t iCoil, Double_t value, Double_t DetVal[],
			Double_t MonVal[])
{
  // Accepts data on a single helicity window and loads it into 
  // arrays, sorted by modulation object
  if (iCoil==-1) {
    // no object is currently modulated. Probably data from end of supercycle.
    // Data could be stored for each coil (with value=0), but for right now
    // data is ignored
    return;
  } else if (iCoil>=fNDitUsed || iCoil<-1) {
    cout << "TaDitCyc::LoadData -- Given Bad Modulation Object Index: "
	 << iCoil << ",    Max Index = " << fNDitUsed << endl;
    return;
  }
//  #ifdef NOISY
//   cout << "TaDitCyc:: Loading Data, iCoil "<< iCoil << endl;
//  #endif

  (fObjCycs[iCoil].sModVal).push_back(value);
  for (Int_t i = 0; i<fNDetUsed; i++) 
    fObjCycs[iCoil].sDetVal[i].push_back(DetVal[i]);
  for (Int_t i = 0; i<fNMonUsed; i++) 
    fObjCycs[iCoil].sMonVal[i].push_back(MonVal[i]);
  fNEvObj[iCoil]++;

}

