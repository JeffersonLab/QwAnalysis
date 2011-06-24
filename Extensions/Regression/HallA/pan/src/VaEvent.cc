//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           VaEvent.cc  (implementation)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: VaEvent.cc,v 1.52 2010/05/21 15:17:04 rom Exp $
//
////////////////////////////////////////////////////////////////////////
//
//    An event of data. Includes methods to get data using keys.  For
//    ADCs and scalers can also get the data by slot number and
//    channel.  Events are loaded from a data source and may have
//    analysis results added to them; they may be checked for cut
//    conditions.
//
////////////////////////////////////////////////////////////////////////

//#define NOISY
//#define DEBUG
//#define QWKINJHEL
//#define UVAHEL

#define SANE_DECODE 1

#include "VaEvent.hh"
#include "TaCutList.hh"
#include "TaLabelledQuantity.hh"
#include "TaRun.hh"
#include "TaString.hh"
#include "TaDevice.hh"
#include "TaDataBase.hh"
#include <iostream>
#include <iomanip>
#include <utility>
#include <cmath>

#ifndef NODICT
ClassImp(VaEvent)
#endif

// Static members
const Int_t ADC_MinDAC = 3000;
const ErrCode_t VaEvent::fgTAEVT_OK = 0;
const ErrCode_t VaEvent::fgTAEVT_ERROR = -1;
VaEvent VaEvent::fgLastEv;
Bool_t VaEvent::fgFirstDecode = true;
Double_t VaEvent::fgLoBeam;
Double_t VaEvent::fgLoBeamC;
Double_t VaEvent::fgBurpCut;
Double_t VaEvent::fgSatCut;
Double_t VaEvent::fgMonSatCut;
Double_t VaEvent::fgPosBurp[fgMaxNumPosMon];
Double_t VaEvent::fgPosBurpE[fgMaxNumPosMonE];
Double_t VaEvent::fgCBurpCut;
Double_t VaEvent::fgAdcxDacBurpCut;
Cut_t VaEvent::fgLoBeamNo;
Cut_t VaEvent::fgLoBeamCNo;
Cut_t VaEvent::fgBurpNo;  
Cut_t VaEvent::fgSatNo;  
Cut_t VaEvent::fgMonSatNo;  
Cut_t VaEvent::fgEvtSeqNo;
Cut_t VaEvent::fgStartupNo;
Cut_t VaEvent::fgPosBurpNo;
Cut_t VaEvent::fgPosBurpENo;
Cut_t VaEvent::fgCBurpNo;  
Cut_t VaEvent::fgAdcxDacBurpNo;
Cut_t VaEvent::fgAdcxBadNo;
Cut_t VaEvent::fgScalerBadNo;
UInt_t VaEvent::fgOversample;
UInt_t VaEvent::fgCurMon;
UInt_t VaEvent::fgCurMonC;
UInt_t VaEvent::fgDetRaw[DETNUM];
UInt_t VaEvent::fgMonRaw[MONNUM];
UInt_t VaEvent::fgNPosMon;
UInt_t VaEvent::fgPosMon[fgMaxNumPosMon];   
UInt_t VaEvent::fgNPosMonE;
UInt_t VaEvent::fgPosMonE[fgMaxNumPosMonE];   
UInt_t VaEvent::fgSizeConst;
UInt_t VaEvent::fgNCuts;
Bool_t VaEvent::fgCalib;
UInt_t VaEvent::fgDetKey[DETNUM];
Double_t VaEvent::fgCombWt[DETCOMBNUM][DETNUM];
Double_t VaEvent::fQPD1Pars[6];
Double_t VaEvent::fLINA1pars[10];
Double_t VaEvent::fCavPars[CAVNUM][2];
Int_t VaEvent::fgDvalue = -1;

VaEvent::VaEvent(): 
  fEvType(0),  
  fEvNum(0),  
  fLastPhyEv(0),  
  fEvLen(0), 
  fFailedACut(false), 
  fHel(UnkHeli),
  fPrevROHel(UnkHeli),
  fPrevHel(UnkHeli)
{
  fEvBuffer = new Int_t[fgMaxEvLen];
  memset(fEvBuffer, 0, fgMaxEvLen*sizeof(Int_t));
  fPrevBuffer = new Int_t[fgMaxEvLen];
  memset(fPrevBuffer, 0, fgMaxEvLen*sizeof(Int_t));
  fPrevAdcxBase = new Double_t[16*ADCXNUM];
  memset(fPrevAdcxBase, 0, 16*ADCXNUM*sizeof(Double_t));
  fData = new Double_t[MAXKEYS];
  memset(fData, 0, MAXKEYS*sizeof(Double_t));
  fN1roc = new Int_t[MAXROC];
  memset(fN1roc, 0, MAXROC*sizeof(Int_t));
  fLenroc = new Int_t[MAXROC];
  memset(fLenroc, 0, MAXROC*sizeof(Int_t));
  fIrn = new Int_t[MAXROC];
  memset(fIrn, 0, MAXROC*sizeof(Int_t));
  if (fgNCuts > 0)
    {
      fCutArray = new Int_t[fgNCuts];
      memset(fCutArray, 0, fgNCuts*sizeof(Int_t));
    }
  else
    fCutArray = 0;
  fResults.clear();
}

VaEvent::~VaEvent() {
  Uncreate();
}

VaEvent::VaEvent(const VaEvent& ev) 
{
  Create (ev);
} 

VaEvent &VaEvent::operator=(const VaEvent &ev)
{
  if ( &ev != this )
    {
      Uncreate();
      Create (ev);
    }
  return *this;
}

VaEvent& 
VaEvent::CopyInPlace (const VaEvent& rhs)
{
  // Like operator=, but copy an event into existing memory, not
  // deleting and reallocating.  This should always be safe, but just
  // to minimize possible problems use this instead of operator= only
  // when pointers must be preserved.

  if ( &rhs != this )
    {
      fEvType = rhs.fEvType;
      fEvNum = rhs.fEvNum;
      fLastPhyEv = rhs.fLastPhyEv;
      fEvLen = rhs.fEvLen;
      fFailedACut = rhs.fFailedACut;
      fResults = rhs.fResults;
      fHel = rhs.fHel;
      fPrevROHel = rhs.fPrevROHel;
      fPrevHel = rhs.fPrevHel;
      memcpy(fEvBuffer, rhs.fEvBuffer, fEvLen*sizeof(Int_t));
      memcpy(fPrevBuffer, rhs.fPrevBuffer, fEvLen*sizeof(Int_t));
      memcpy(fPrevAdcxBase, rhs.fPrevAdcxBase, 16*ADCXNUM*sizeof(Double_t));
      memcpy(fData, rhs.fData, MAXKEYS*sizeof(Double_t));
      if (rhs.fCutArray != 0 && fgNCuts > 0)
	memcpy(fCutArray, rhs.fCutArray, fgNCuts*sizeof(Int_t));
      else
	fCutArray = 0;
    }
  return *this;
};


// Major functions

ErrCode_t
VaEvent::RunInit(const TaRun& run)
{
  // Initialization at start of run.  Get quantities from database
  // which will be needed in event loop, and set other static variables
  // to initial values.
  cout << "VaEvent:: RunInit() " << endl;

  fgFirstDecode = true;

  fgLoBeam = run.GetDataBase().GetCutValue("lobeam");
  fgLoBeamC = run.GetDataBase().GetCutValue("lobeamc");
  fgBurpCut = run.GetDataBase().GetCutValue("burpcut");
  fgSatCut = run.GetDataBase().GetCutValue("satcut");
  fgCBurpCut = run.GetDataBase().GetCutValue("cburpcut");
  fgMonSatCut = run.GetDataBase().GetCutValue("monsatcut");
  fgAdcxDacBurpCut = run.GetDataBase().GetCutValue("adcxdacburpcut");

  vector<TaString> vposmon = run.GetDataBase().GetStringVect("posmon");
  Int_t npm = vposmon.size();
  vector<Double_t> vposbcut = run.GetDataBase().GetCutValueDVector("posburp");
  Int_t npc = vposbcut.size();
  if (npc < npm) vposmon.resize(npc);
  Int_t ic=0;
  vector<Double_t>::iterator ipbc = vposbcut.begin();  
  for(vector<TaString>::iterator iconst = vposmon.begin();
      iconst != vposmon.end(); iconst++) {
    //    clog << " for beam burp, looking for monitor " << *iconst<< endl;
    if ((iconst->size()>2) && (*ipbc)!=0 && run.GetKey(string (*iconst))) {
      // the size requirement is a kludge to get around the fact
      // that GetStringVect returns spaces as elements of the space deliminated
      // array, even if they didn't exist in the database, while 
      // GetCutDVect returns zeros... Beauty.
      fgPosMon[ic] = run.GetKey(string ( *iconst));
      fgPosBurp[ic] = *ipbc;
      ipbc++; ic++;
    }
  } 
  fgNPosMon = ic;

  vector<TaString> vposmonE = run.GetDataBase().GetStringVect("posmone");
  Int_t npmE = vposmonE.size();
  vector<Double_t> vposbcutE = run.GetDataBase().GetCutValueDVector("posburpe");
  Int_t npcE = vposbcutE.size();
  if (npcE < npmE) vposmonE.resize(npcE);
  Int_t icE=0;
  vector<Double_t>::iterator ipbcE = vposbcutE.begin();  
  for(vector<TaString>::iterator iconst = vposmonE.begin();
      iconst != vposmonE.end(); iconst++) {
    //    clog << " for beam burp, looking for monitor " << *iconst<< endl;
    if ((iconst->size()>2) && (*ipbcE)!=0 && run.GetKey(string (*iconst))) {
      // the size requirement is a kludge to get around the fact
      // that GetStringVect returns spaces as elements of the space deliminated
      // array, even if they didn't exist in the database, while 
      // GetCutDVect returns zeros... Beauty.
      fgPosMonE[icE] = run.GetKey(string ( *iconst));
      fgPosBurpE[icE] = *ipbcE;
      ipbcE++; icE++;
    }
  } 
  fgNPosMonE = icE;

  fgNCuts = (UInt_t) run.GetDataBase().GetNumCuts();

  fgLoBeamNo = run.GetDataBase().GetCutNumber ("Low_beam");
  fgLoBeamCNo = run.GetDataBase().GetCutNumber ("Low_beam_c");
  fgBurpNo = run.GetDataBase().GetCutNumber ("Beam_burp");
  fgSatNo = run.GetDataBase().GetCutNumber ("Det_saturate");
  fgMonSatNo = run.GetDataBase().GetCutNumber ("Mon_saturate");
  fgEvtSeqNo = run.GetDataBase().GetCutNumber ("Evt_seq");
  fgStartupNo = run.GetDataBase().GetCutNumber ("Startup");
  fgCBurpNo = run.GetDataBase().GetCutNumber ("C_burp");
  fgPosBurpNo = run.GetDataBase().GetCutNumber ("Pos_burp");
  fgPosBurpENo = run.GetDataBase().GetCutNumber ("Pos_burp_E");
  fgAdcxDacBurpNo = run.GetDataBase().GetCutNumber ("Adcx_DAC_burp");
  fgAdcxBadNo = run.GetDataBase().GetCutNumber ("Adcx_Bad");
  fgScalerBadNo = run.GetDataBase().GetCutNumber ("Scaler_Bad");
  if (fgEvtSeqNo == fgNCuts)
    fgEvtSeqNo = run.GetDataBase().GetCutNumber ("Oversample"); // backward compat
  if (fgLoBeamNo == fgNCuts ||
      fgEvtSeqNo == fgNCuts  )
    {
      cerr << "VaEvent::RunInit ERROR: Following cut(s) are not defined "
	   << "in database and are required:";
      if (fgLoBeamNo == fgNCuts) cerr << " Low_beam";
      if (fgEvtSeqNo == fgNCuts) cerr << " Evt_seq";
      cerr << endl;
      return fgTAEVT_ERROR;
    }
  if (fgBurpNo == fgNCuts ||
      fgSatNo == fgNCuts ||
      fgMonSatNo == fgNCuts ||
      fgLoBeamCNo == fgNCuts ||
      fgStartupNo == fgNCuts ||
      fgCBurpNo == fgNCuts ||
      fgPosBurpENo == fgNCuts ||
      fgPosBurpNo == fgNCuts ||
      fgAdcxDacBurpNo == fgNCuts ||
      fgAdcxBadNo == fgNCuts ||
      fgScalerBadNo == fgNCuts )
    {
      cerr << "VaEvent::RunInit WARNING: Following cut(s) are not defined "
	   << "in database and will not be imposed:";
      if (fgLoBeamCNo == fgNCuts) cerr << " Low_beam_c";
      if (fgBurpNo == fgNCuts) cerr << " Beam_burp";
      if (fgSatNo == fgNCuts) cerr << " Det_saturate";
      if (fgMonSatNo == fgNCuts) cerr << " Mon_saturate";
      if (fgStartupNo == fgNCuts) cerr << " Startup";
      if (fgPosBurpNo == fgNCuts) cerr << " Pos_burp";
      if (fgPosBurpENo == fgNCuts) cerr << " Pos_burp_E";
      if (fgCBurpNo == fgNCuts) cerr << " C_burp";
      if (fgAdcxDacBurpNo == fgNCuts) cerr << " Adcx_DAC_burp";
      if (fgAdcxBadNo == fgNCuts) cerr << " Adcx_Bad";
      if (fgScalerBadNo == fgNCuts) cerr << " Scaler_Bad";
      cerr << endl;
    }

  string scurmon = run.GetDataBase().GetCurMon();
  if (scurmon == "none") scurmon = "bcm1";
  fgCurMon = run.GetKey (scurmon);
  string scurmonc = run.GetDataBase().GetCurMonC();
  if (scurmonc == "none") scurmonc = "bcm10";
  fgCurMonC = run.GetKey (scurmonc);
  fgDetRaw[0] = run.GetKey (string ("det1r"));
  fgDetRaw[1] = run.GetKey (string ("det2r"));
  fgDetRaw[2] = run.GetKey (string ("det3r"));
  fgDetRaw[3] = run.GetKey (string ("det4r"));

  fgMonRaw[0] = run.GetKey (string ("bpm12mx"));
  fgMonRaw[1] = run.GetKey (string ("bpm4amx"));
  fgMonRaw[2] = run.GetKey (string ("bpm4bmx"));
  fgMonRaw[3] = run.GetKey (scurmon+string ("r"));

  fgOversample = run.GetOversample();
  fgLastEv = VaEvent();

  vector<Double_t> qpd1const = run.GetDataBase().GetQpd1Const();
  Int_t ip=0;
  for(vector<Double_t>::iterator iconst = qpd1const.begin();
      iconst != qpd1const.end(); iconst++)  fQPD1Pars[ip++] = *iconst;

  vector<Double_t> cav1const = run.GetDataBase().GetCavConst1();
  ip=0;
  for(vector<Double_t>::iterator iconst = cav1const.begin();
      iconst != cav1const.end(); iconst++)  fCavPars[0][ip++] = *iconst;

  vector<Double_t> cav2const = run.GetDataBase().GetCavConst2();
  ip=0;
  for(vector<Double_t>::iterator iconst = cav2const.begin();
      iconst != cav2const.end(); iconst++)  fCavPars[1][ip++] = *iconst;

  vector<Double_t> cav3const = run.GetDataBase().GetCavConst3();
  ip=0;
  for(vector<Double_t>::iterator iconst = cav3const.begin();
      iconst != cav3const.end(); iconst++)  fCavPars[2][ip++] = *iconst;

  fgCalib = run.GetDataBase().GetCalVar();


  
  fgDetKey[0] = run.GetKey (string ("det1"));
  fgDetKey[1] = run.GetKey (string ("det2"));
  fgDetKey[2] = run.GetKey (string ("det3"));
  fgDetKey[3] = run.GetKey (string ("det4"));


  /*UInt_t combo[DETCOMBNUM][DETNUM] = { { 1, 1, 0, 0 },
				       { 0, 0, 1, 1 },
 				       { 1, 0, 1, 0 },
 				       { 0, 1, 0, 1 },
				       { 1, 1, 1, 1 } };*/
  // Changed by LRM for PREX 4/1/10
  UInt_t combo[DETCOMBNUM][DETNUM] = { { 0, 0, 1, 1 },
				       { 1, 1, 0, 0 },
 				       { 1, 0, 1, 0 },
 				       { 0, 1, 0, 1 },
				       { 1, 1, 1, 1 } };
  // Weight detector combos with decimal values from det wts database line
  vector<Double_t> wtsa(0);      
  wtsa = run.GetDataBase().GetDetWts();
  for (Int_t ic =0; ic<DETCOMBNUM; ic++) {
    for (Int_t id =0; id<DETNUM; id++) {
      if (combo[ic][id]) {
	fgCombWt[ic][id] = wtsa[id];
      } else {
	fgCombWt[ic][id] = 0.0;
      }
    }
  }

  vector<Double_t> lina1const = run.GetDataBase().GetLina1Const();  
  ip=0;  
  for(vector<Double_t>::iterator iconst = lina1const.begin(); 
      iconst != lina1const.end(); iconst++)  
    { 
      fLINA1pars[ip++] = *iconst; 
      //      cout << "  this one : " << fLINA1pars[ip - 1] << endl;
    }
  //  cout << "flina1pars[0] = " <<fLINA1pars[0] << "  " << (int) fLINA1pars[0] << endl;
  
  return fgTAEVT_OK;

}
  
void 
VaEvent::Load (const Int_t* buff) 
{
  // Put a raw event into the buffer, and pull out event type and number.

  fFailedACut = false;
  fEvLen = buff[0] + 1;
  if (fEvLen >= fgMaxEvLen) {
      cerr << "VaEvent::Load ERROR  fEvLen = "<<fEvLen; 
      cerr << "  is greater than fgMaxEvLen = "<<fgMaxEvLen<<endl;
      cerr << "(perhaps compile with larger fgMaxEvLen parameter)"<<endl;
      fEvLen = fgMaxEvLen;
  }
  // Load previous event if its a physics event type
  Int_t prev_len = fEvBuffer[0]+1;                 
  Int_t prev_evtype = fEvBuffer[1]>>16;
  if (prev_evtype >= 1 && prev_evtype < 12) {      // Load if physics trig
    memset(fPrevBuffer,0,fgMaxEvLen*sizeof(Int_t));  
    if (prev_len > 0 && prev_len < (Int_t) fgMaxEvLen) {
      memcpy(fPrevBuffer,fEvBuffer,prev_len*sizeof(Int_t));
    }
  }
  memset(fEvBuffer,0,fgMaxEvLen*sizeof(Int_t));
  memcpy(fEvBuffer,buff,fEvLen*sizeof(Int_t));
  fEvType = fEvBuffer[1]>>16;
  fEvNum = 0;
  if ( IsPhysicsEvent() ) {
     fEvNum = fEvBuffer[4];
     fLastPhyEv = fEvNum;
  }
};

void VaEvent::DecodeCook(TaDevice& devices) {
// Determine at start-up which cooked data to add to tree.
// This uses a feature of Decode() but ensures that users of
// this class don't think it is a physics event.

  fEvType = 999;   // Large event type to "trick" Decode().
  Decode(devices);
};

void 
VaEvent::Decode(TaDevice& devices) 
{
// Decodes all the raw data and applies all the calibrations and BPM
// rotations.  Note: This assumes the event structure remains
// constant.  We check this by verifying a constant event length.
// Also note that in order for cooked data to appear in the output,
// everywhere we have fData[cook_key] = function of fData[raw_key], 
// we MUST have a line devices.SetUsed(cook_key) if we want it.

  Int_t i,j,key,idx,ixp,ixm,iyp,iym,ix,iy,icra,jb,ckey;
  Int_t ixpyp,ixpym,ixmyp,ixmym;
  Int_t ila[8],ilac[8];
  Int_t npad;

  Double_t sum,xval,yval;
  memset(fData, 0, MAXKEYS*sizeof(Double_t));
  adcxbad = 0;
  adcxglitch = 0;
  adcxcrlist = 0;
  if ( IsPhysicsEvent() )  {
    if (fgFirstDecode) {
        fgSizeConst = GetEvLength();
        fgFirstDecode = false;
    }
    if (fgCareSize &&  fgSizeConst != (UInt_t)GetEvLength() ) {
        cerr << "VaEvent:: FATAL ERROR: Event structure is changing !!"<<endl;
	cerr << "Size was " << fgSizeConst << " now is " << (Int_t)GetEvLength() << endl;
        cerr << "As a result, decoding will fail."<<endl;
        exit(0);  // This should never happen, but if it does Bob needs to fix some stuff !
    }
  }

  if (DecodeCrates(devices) == 1) {

    if (fPvdisCheckCrate) CheckPvdisCrates();
    for (i = 0; i < devices.GetNumRaw(); i++) {
      key = devices.GetRawKey(i);
      icra = devices.GetCrate(key);     
// EvPointer Convention:  If the crate number is <= 0, then the 
// event pointer is an absolute offset.  If crate > 0, then the 
// pointer is relative to the header for that device and crate.
      Int_t rawd;
      if (icra <= 0) {
#ifdef SANE_DECODE
// This is almost certainly an error, but ... if you know
// better you can turn off the ifdef at top of this code.
        if (devices.GetEvPointer(key) < 2) {
	  cout << "VaEvent::WARNING: Probably wrong datamap entry ";
          cout << "for key = "<<devices.GetKey(key)<<endl;
	}
#endif
        rawd =  GetRawData(key, devices, devices.GetEvPointer(key));
      } else {
        rawd =  GetRawData(key, devices,
          devices.GetOffset(key)+devices.GetEvPointer(key) );
      }

      // ADCX data have to be masked out and checked before conversion
      // to floating point
      if (devices.IsAdcx (key)) {
        if (DECODE_DEBUG) cout << endl << "==============="<<endl<< "adcx key = "<<key<<"    data = 0x"<<rawd<<dec<<endl;
	fData[key] = UnpackAdcx (rawd, key);
        adcxcrlist |= (1 << icra);
      } else if (devices.IsVqwk (key)) {
        if (DECODE_DEBUG) cout << endl << "==============="<<endl<< "Vqwk key = "<<key<<"    data = 0x"<<hex << rawd<<dec<< endl;	
	fData[key] = UnpackVqwk ((UInt_t) rawd, key);
      } else {
	fData[key] = rawd;
      }

      if (DECODE_DEBUG == 1) {
        cout << endl << "------------------" <<endl;
        cout << "Raw Data for key "<< devices.GetKey(key)<<dec<<endl;
        if (icra <= 0) {
          cout << " abs. offset " << devices.GetEvPointer(key);
        } else {
	  cout << " tied to crate " << icra;
          cout << "   at offset " << devices.GetOffset(key);
          cout << "   rel. to offset " << devices.GetEvPointer(key);
        }
        cout << endl;
        cout << " Data word = (dec) "<<dec<<(Int_t)fData[key];
        cout << "   = (hex) "<<hex<<(Int_t)fData[key]<<dec<<endl; 
      }
    }
  }

// Calibrate the ADCs first
  for (i = 0;  i < ADCNUM+ADCXNUM; i++) {
    for (j = 0; j < 4; j++) {
      key = i*4 + j;
      fData[ACCOFF + key] = fData[ADCOFF + key] 
	- devices.GetPedestal(ADCOFF + key)
	- (fData[DACOFF + i]-ADC_MinDAC) * devices.GetDacSlope(key);
      if (devices.IsUsed(ADCOFF+key)) devices.SetUsed(ACCOFF+key);
    }
  }
  // Calibrate VQWKs
  // No way at this time to use oversampled blocks for derived objects
  // Too bad...
  Int_t nsampkey;
  for (i = 0; i < VQWKNUM; i++) { //number of vqwk boards
    for (j = 0; j < 8; j++) { // number of channels

      nsampkey = VQWKOFF+7*(8*i+j);  // first key for each channel
      // If there's no samples... then there's no calibration.
      if (fData[nsampkey] == 0) {
	for (jb = 0; jb < 5; jb++) {  // loop over the integrated words (4 blocks+total)
	  //calibrated vqwk keys 0-4 (of 5) are calibrated integrated blocks 1-4 and total
	  ckey = jb + 5*(j + i*8);
	  // but they correspond to raw vqwk keys 2-6 (of 7)
	  key = jb + 2+ 7*(j + i*8);
	  fData[VQWKCOFF + ckey] = -1e6;
	  if (devices.IsUsed(VQWKOFF+key)) devices.SetUsed(VQWKCOFF+ckey);
	}
      } else {
	// HA! There IS a sample count!
	Double_t nsamp = fData[nsampkey];
	Double_t nsamp4 = nsamp/4;
	// loop over the integrated words (4 blocks+total)
	for (jb = 0; jb < 4; jb++) {   
	  //calibrated vqwk keys 0-3 
	  // are calibrated integrated blocks 1-4
	  ckey = jb + 5*(j + i*8);
	  // but they correspond to raw vqwk keys 2-5 (of 7)
	  key = jb + 2+ 7*(j + i*8);
	  fData[VQWKCOFF + ckey] =
	    (fData[VQWKOFF + key]/nsamp4) - devices.GetPedestal(VQWKOFF + key);
	  if (devices.IsUsed(VQWKOFF+key)) devices.SetUsed(VQWKCOFF+ckey);
	}
	//calibrated vqwk key 4 
	// is calibrated integrated total
	ckey = 4 + 5*(j + i*8);
	// but this corresponds to raw vqwk key 6 (last of 7)
	key = 4 + 2+ 7*(j + i*8);
	fData[VQWKCOFF + ckey] =
	  (fData[VQWKOFF + key] / nsamp) - devices.GetPedestal(VQWKOFF + key);
	if (devices.IsUsed(VQWKOFF+key)) devices.SetUsed(VQWKCOFF+ckey);
// 	cout << endl << "-----------------------" << endl;
// 	cout << Form("VQWK = %1d_%1d \n",i,j);
// 	cout << Form("   raw   = %g \n",fData[VQWKOFF+key]);
// 	cout << Form("   ped   = %g \n",devices.GetPedestal(VQWKOFF+key));
// 	cout << Form("   nsamp = %g \n",nsamp);
// 	cout << Form("   cal   = %g \n",fData[VQWKCOFF+ckey]);
// 	cout << "-----------------------" << endl;
      }
    }
  }  
  
  // Calibrate Scalers for use with v2fs
  // Only one V2F is allowed per scaler.  Sorry.
  Int_t clockkey;

  scalerbad = 0;
  //***************************
  // added to clean data from CH scaler, 18Apr10, rupesh
  // v2f clock is 4Mhz
  // expected_v2f = IT*2.5*4.0
  Int_t expected_v2f = fData[TBDOFF + 8]*2.5*4.0;
  // add +/- 10% of its own value to give us plenty of wiggle room for imprecision
  expected_v2f = expected_v2f + 0.1*expected_v2f;
  //    cout << "VaEvent:: expected_v2f:: " << expected_v2f << endl;
  // **************************

  for (i = 0; i < V2FCLKNUM; i++) {
    clockkey = i + V2FCLKOFF;
    Double_t clockval;

    // For PVDIS we don't care about clock.  Let it be 1.
    // But leave the other devices alone.

    if (devices.GetMinPvdisPKey() != -1 && 
        devices.GetMaxPvdisPKey() != -1 &&
        SCAOFF+i*32 >= devices.GetMinPvdisPKey() && 
        SCAOFF+i*32 <= devices.GetMaxPvdisPKey()) {

           clockval = 1; 

    } else {

           clockval = fData[clockkey];
    }

// If there's no clockval... then there's no calibration.
   
    if (clockval == 0) {
      for (j = 0; j < 32; j++) {
	key = j + i*32;
	fData[SCCOFF + key] = 0;
        if (devices.IsUsed(SCAOFF+key)) devices.SetUsed(SCCOFF+key);
      }
    } else {
      // HA! There IS a clockval!
      for (j = 0; j < 32; j++) {
	key = j + i*32;
	//*******************
	// maxcount for any scaler channel is v2f_clk val
	if(fData[SCAOFF + key]>expected_v2f || clockval>expected_v2f) scalerbad=1;
#ifdef NOISY
	if(scalerbad){
	  if(j==0) cout << "VaEvent:: evnum ::  " << fEvNum << endl;  
	  cout << "VaEvent:: "<< j <<" clockval:: " << clockval << endl;
	  cout << "VaEvent:: "<< j <<" scalerval:: " << fData[SCAOFF + key] << endl;
	  cout << "VaEvent:: "<< j <<" scalerbad:: " << scalerbad << endl;
	}
#endif
	//***************
	fData[SCCOFF + key] = 
	  (fData[SCAOFF + key])/clockval - devices.GetPedestal(SCAOFF + key);
        if (devices.IsUsed(SCAOFF+key)) devices.SetUsed(SCCOFF+key);
      }
    }
  }

// Batteries
  for (i = 0; i < BATNUM; i++) {
    key = BATOFF + i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key);
  }

// bmw coil
  for (i = 0; i < BMWCOILNUM; i++) {
      key = BMWCOILOFF + i;
      if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
      idx = devices.GetCalIndex(key);
      if (idx < 0) continue;
      fData[key] = fData[idx];
      if (devices.IsUsed(key)) devices.SetUsed(key);
  }
  
// Quad photodiode
  for (i = 0; i < QPDNUM; i++) {
    key = QPDOFF + 9*i;
    ixpyp = devices.GetCalIndex(key);
    ixpym = devices.GetCalIndex(key+1);
    ixmyp = devices.GetCalIndex(key+2);
    ixmym = devices.GetCalIndex(key+3);
    if (ixpyp < 0 || ixpym < 0 || ixmyp < 0 || ixmym < 0) continue;
    if (i==0) {  // kludgey way to use relative gains for qpd1
       fData[ixpyp] *= fQPD1Pars[0];
       fData[ixpym] *= fQPD1Pars[1];
       fData[ixmyp] *= fQPD1Pars[2];
       fData[ixmym] *= fQPD1Pars[3];
     }
    ix  = key + 4;
    iy  = key + 5;
    sum = fData[ixpyp] + fData[ixpym] + fData[ixmyp] + fData[ixmym];
    fData[key + 6] = sum;
    if (devices.IsUsed(key)) devices.SetUsed(key+6);
    xval = 0;
    yval = 0;
    if ( sum != 0 ) { 
      if (i==0) {
	xval = fQPD1Pars[4]*
	  (fData[ixpyp] + fData[ixpym] - fData[ixmyp]- fData[ixmym])/ sum;
	yval = fQPD1Pars[5]*
	  (fData[ixpyp] - fData[ixpym] + fData[ixmyp]- fData[ixmym])/ sum;
      } else {
	xval = 
	  (fData[ixpyp] + fData[ixpym] - fData[ixmyp]- fData[ixmym])/ sum;
	yval = 
	  (fData[ixpyp] - fData[ixpym] + fData[ixmyp]- fData[ixmym])/ sum;
      }
    }
    if (devices.IsRotated(ix)) {
       fData[ix] = Rotate (xval, yval, 1);
    } else {
       fData[ix] = xval;
    }
    if (devices.IsUsed(key)) devices.SetUsed(ix);
    if (devices.IsRotated(iy)) {
       fData[iy] = Rotate (xval, yval, 2);
    } else {
       fData[iy] = yval;
    }
    if (devices.IsUsed(key)) devices.SetUsed(iy);
  }

// Linear Array
//     for (Int_t ip = 0; ip<10; ip++) 
//       cout << Form("LINA par%d = ",ip) << fLINA1pars[ip]<< endl;
  for (i = 0; i < LINANUM; i++) {
    if (i>0) {
      cout << "VaEvent::WARNING: Invalid number for Linear Array ";
    }
    key = LINAOFF + 21*i;  // device key
    npad = (int) fLINA1pars[0]; // number of pads in use
    //    cout << "Key is " << key << endl;
    if (devices.IsUsed(key)) {      // check pad 1 to see if device is used
      if (npad<2 || npad>8) {
	cout << "VaEvent::WARNING: Invalid number of pads for linear array "<<npad << endl;
      }
      Int_t iErrcd = 0;
      for (Int_t ip = 0; ip<npad; ip++) {
	ila[ip] = devices.GetCalIndex(key+ip);  // pointer to calibrated word from each vqwk
	if (ila[ip]<0) iErrcd =1;
	//	cout << "calibrate index is " <<ila[ip] << " " << devices.GetCalIndex(key+ip) << endl;
      }
      if (iErrcd >0) {
	cout << "LINEAR ARRAY ANALYSIS ERROR " << endl;
	continue;
      }

      // apply relative gains
      for (Int_t ip = 0; ip<npad; ip++) {
	ilac[ip] = key + 8+ip; // key for gain-modified LINA word

	//       	cout << "gain modified key is " << ilac[ip] << endl;
	fData[ ilac[ip] ] = fData[ ila[ip] ]*fLINA1pars[ip+1];  // push gain corrected callibrated data
	devices.SetUsed(ilac[ip]);
	//	cout << "              value is " << fData[ilac[ip]] << endl;
      }

      // Accumulate sum
      Double_t sum=0;

      for (Int_t ip = 0; ip<npad; ip++)
	sum += fData[ilac[ip]];

      fData[key+16]=sum;
      Double_t spc = fLINA1pars[9];
      Double_t xsum=0;
      Double_t xsum2=0;
      Double_t x,rms;
      if (sum>0) {
	for (Int_t ip = 0; ip<npad; ip++)
	  xsum += spc*(ip+0.5)*fData[ilac[ip]];
	x = xsum/sum;
	for (Int_t ip = 0; ip<npad; ip++)
	  xsum2 += pow(spc*(ip+0.5)-x,2)*fData[ilac[ip]];
	if (xsum2>0) {
	  rms = sqrt(xsum2/sum);
	} else {
	  rms = -1e6;
	}
      } else {
	x = -1e6;
	rms = -1e6;
      }

      fData[key+17] = x;
      fData[key+18] = rms;

      // alternative method for x, rms
      if(npad>4 && sum>0) {
	Double_t vx[8];
	Double_t vy[8];
	for (Int_t ip = 0; ip<npad; ip++) {
	  vx[ip] = (ip+0.5)*spc;
	  vy[ip] = fData[ilac[ip]];
	}
	TGraph *g1 = new TGraph(npad,vx,vy);
	g1->Fit("gaus","Q");
	TF1 *f1 = (TF1*) g1->GetFunction("gaus");
	x = f1->GetParameter(1);
	rms = f1->GetParameter(2);
      }

      fData[key+19] = x;
      fData[key+20] = rms;

      devices.SetUsed(key+16);
      devices.SetUsed(key+17);
      devices.SetUsed(key+18);
      devices.SetUsed(key+19);
      devices.SetUsed(key+20);
    }
  }

// Stripline BPMs
  for (i = 0; i < STRNUM; i++) {
    key = STROFF + 10*i;
    ixp = devices.GetCalIndex(key);
    ixm = devices.GetCalIndex(key+1);
    iyp = devices.GetCalIndex(key+2);
    iym = devices.GetCalIndex(key+3);
    if (ixp < 0 || ixm < 0 || iyp < 0 || iym < 0) continue;
    ix  = key + 4;
    iy  = key + 5;
    sum = fData[ixp] + fData[ixm];
    fData[key + 6] = sum;
    if (devices.IsUsed(key)) devices.SetUsed(key+6);
    xval = 0;
    if ( sum > 0 ) xval = 
          fgKappa * (fData[ixp] - fData[ixm])/ sum;
    sum = fData[iyp] + fData[iym]; 
    fData[key + 7] = sum;
    if (devices.IsUsed(key)) devices.SetUsed(key+7);
    yval = 0;
    if ( sum > 0 ) yval = 
          fgKappa * (fData[iyp] - fData[iym])/ sum;
    if (devices.IsRotated(ix)) {
       fData[ix] = Rotate (xval, yval, 1);
    } else {
       fData[ix] = xval;
    }
    if (devices.IsUsed(key)) devices.SetUsed(ix);
    if (devices.IsRotated(iy)) {
       fData[iy] = Rotate (xval, yval, 2);
    } else {
       fData[iy] = yval;
    }
    if (devices.IsUsed(key)) devices.SetUsed(iy);
    fData[key + 8] = fData[key + 6] + fData[key + 7];
    if (devices.IsUsed(key)) devices.SetUsed(key+8);
    Double_t mx = fData[key];
    if (mx < fData[key+1]) mx=fData[key+1];
    if (mx < fData[key+2]) mx=fData[key+2]; // what is the C++
    if (mx < fData[key+3]) mx=fData[key+3]; // "max" function, anyway?
    fData[key + 9] = mx;
    if (devices.IsUsed(key)) devices.SetUsed(key+9);
  }

// Cavity BPM monitors
  for (i = 0; i < CAVNUM; i++) {
    for (j = 0; j < 2; j++) {
       key = CAVOFF + 4*i + j;
       if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
       idx = devices.GetCalIndex(key);
       if (idx < 0) continue;
       //       fData[key+2] = fData[idx];
       Double_t calpar = 1.0;
       if (fCavPars[i][j] != 0.0) calpar = fCavPars[i][j];
       fData[key+2] = fData[idx]*calpar;
       if (devices.IsUsed(key)) devices.SetUsed(key+2);
    }
  }

// Cavity BCM monitors
  for (i = 0; i < CCMNUM; i++) {
    key = CCMOFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }

// Happex-1 era BCMs
  for (i = 0; i < BCMNUM; i++) {
    key = BCMOFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }

// Lumi monitors
  for (i = 0; i < LMINUM; i++) {
    key = LMIOFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
// corrected LUMI data
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }

// Detectors
  for (i = 0; i < DETNUM; i++) {
    key = DETOFF + 3*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    fData[key+2]=0;
    if (fData[key]>=60000) fData[key+2]=1;  // set saturation flag from raw value
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
    if (devices.IsUsed(key)) devices.SetUsed(key+2);
  }

// Detector Combos
  for (Int_t ic=0; ic<DETCOMBNUM; ic++) {
    key = DETCOMBOFF+ic;  
    sum = 0;
    Bool_t use = kTRUE;
    for (Int_t id = 0; id < DETNUM; id++) 
      if ( fgCombWt[ic][id]!=0 && !(devices.IsUsed(fgDetKey[id]))) use=kFALSE;
    if (use) {
      //      cout << "Combo " << ic << " is in use!" << endl;
      for (Int_t id = 0; id < DETNUM; id++)
	if (fgCombWt[ic][id]!=0) sum += fgCombWt[ic][id]*fData[fgDetKey[id]];
      fData[key] = sum;
      devices.SetUsed(key);
    } else {    
      fData[key] = -1.E6;
    }
  }

  // DIS detectors.

  for (i = 0; i < DISTSRNUM; i++) {      //R-HRS showser
    key = DISTSROFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }

  for (i = 0; i < DISTSLNUM; i++) {       //L-HRS shower
    key = DISTSLOFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }

  for (i = 0; i < DISPSRNUM; i++) {       //R-HRS preshower
    key = DISPSROFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }

  for (i = 0; i < DISPSLNUM; i++) {        //L-HRS preshower
    key = DISPSLOFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }

  // DIS triggers

  for (i = 0; i < DISTRIGENRNUM; i++) {          //R-HRS Electron narrow
    key = DISTRIGENROFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }

  for (i = 0; i < DISTRIGENLNUM; i++) {            //L-HRS Electron narrow
    key = DISTRIGENLOFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }

  for (i = 0; i < DISTRIGEWRNUM; i++) {          //R-HRS Electron WIDE
    key = DISTRIGEWROFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }

  for (i = 0; i < DISTRIGEWLNUM; i++) {            //L-HRS Electron WIDE
    key = DISTRIGEWLOFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }


  for (i = 0; i < DISTRIGPWRNUM; i++) {          //R-HRS PION WIDE
    key = DISTRIGPWROFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }

  for (i = 0; i < DISTRIGPWLNUM; i++) {            //L-HRS PION WIDE
    key = DISTRIGPWLOFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }

  for (i = 0; i < DISTRIGPNRNUM; i++) {          //R-HRS PION NARROW
    key = DISTRIGPNROFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }

  for (i = 0; i < DISTRIGPNLNUM; i++) {            //L-HRS PION NARROW
    key = DISTRIGPNLOFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }

  for (i = 0; i < DISTRIGPL1RNUM; i++) {          //R-HRS pileup 1
    key = DISTRIGPL1ROFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }

  for (i = 0; i < DISTRIGPLLNUM; i++) {          //L-HRS pileup
    key = DISTRIGPLLOFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }

  for (i = 0; i < DISTRIGMXD1RNUM; i++) {          //R-HRS MIXED 1 STUFF
    key = DISTRIGMXD1ROFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }

  for (i = 0; i < DISTRIGMXD2RNUM; i++) {          //R-HRS MIXED 2 STUFF
    key = DISTRIGMXD2ROFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }

for (i = 0; i < DISTRIGMXD1LNUM; i++) {          //L-HRS MIXED 1 STUFF
    key = DISTRIGMXD1LOFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }
  for (i = 0; i < DISTRIGMXD2LNUM; i++) {          //L-HRS MIXED 2 STUFF
    key = DISTRIGMXD2LOFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }

for (i = 0; i < DISTRIGPL2RNUM; i++) {          //R-HRS pileup 2
    key = DISTRIGPL2ROFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }
  for (i = 0; i < DISTRIGSPRNUM; i++) {          //R-HRS spare channels
    key = DISTRIGSPROFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }

for (i = 0; i < DISTRIGSPLNUM; i++) {          //L-HRS spare channels
    key = DISTRIGSPLOFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }
  for (i = 0; i < DISTRIGBCMRNUM; i++) {          //R-HRS bcm
    key = DISTRIGBCMROFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }
  for (i = 0; i < DISTRIGBCMLNUM; i++) {          //L-HRS bcm
    key = DISTRIGBCMLOFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }

 for (i = 0; i < DISTRIGENCRNUM; i++) {          //R-HRS ELECTRON NARROW COPY
    key = DISTRIGENCROFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }

  for (i = 0; i < DISTRIGTSCLNUM; i++) {            //L-HRS TOTAL SHOWER COPY
    key = DISTRIGTSCLOFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }

  for (i = 0; i < DISTRIGEWCRNUM; i++) {          //R-HRS ELECTRON WIDE COPY
    key = DISTRIGEWCROFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }

  for (i = 0; i < DISTRIGPSCLNUM; i++) {            //L-HRS PRESHOWER COPY
    key = DISTRIGPSCLOFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }
  
  for (i = 0; i < DISTRIGENCLNUM; i++) {            //L-HRS ELECTRON NARROW COPY
    key = DISTRIGENCLOFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }
 
  for (i = 0; i < DISTRIGEWCLNUM; i++) {            //L-HRS ELECTRON WIDE COPY
    key = DISTRIGEWCLOFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }
 

  
    for (i = 0; i < DISTRIGPL1CRNUM; i++) {          //R-HRS pileup 1 copy
    key = DISTRIGPL1CROFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }

   for (i = 0; i < DISTRIGPLCLNUM; i++) {          //L-HRS pileup copy
    key = DISTRIGPLCLOFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }
   
  for (i = 0; i < DISTRIGMXD1CRNUM; i++) {          //R-HRS MIXED 1 STUFF cpoy
    key = DISTRIGMXD1CROFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }

    for (i = 0; i < DISTRIGMXD1CLNUM; i++) {          //L-HRS MIXED 1 COPY STUFF
    key = DISTRIGMXD1CLOFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }
  for (i = 0; i < DISTRIGMXD2CLNUM; i++) {          //L-HRS MIXED 2 COPY STUFF
    key = DISTRIGMXD2CLOFF + 2*i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCalIndex(key);
    if (idx < 0) continue;
    fData[key+1] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(key+1);
  }
  
  // UMass Profile scanners
  for (i = 0; i < PROFNUM; i++) {
     key = PROFOFF + 3*i;
     if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
     idx = devices.GetCalIndex(key);
     if (idx < 0) continue;
     fData[key+2] = fData[idx];
     if (devices.IsUsed(key)) devices.SetUsed(key+2);
  }

  // Helicity signals
  for (i = 0; i < TIRNUM; i++) {
    key = TIROFF + i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;

    //      Standard Configuration
    // Multipletsynch
    fData[MULOFF + i] = (Double_t)(((int)GetData(key) & 0x20) >> 5);
    // Helicity
    fData[HELOFF + i] = (Double_t)(((int)GetData(key) & 0x40) >> 6);
    // Pairsynch
    fData[PAROFF + i] = (Double_t)(((int)GetData(key) & 0x80) >> 7);
#ifdef UVAHEL
    //      For UVa Lab26:
    // Multipletsynch
    fData[MULOFF + i] = (Double_t)(((int)GetData(key) & 0x40) >> 6);
    // Helicity
    fData[HELOFF + i] = (Double_t)(((int)GetData(key) & 0x10) >> 4);
    // Pairsynch
    fData[PAROFF + i] = (Double_t)(((int)GetData(key) & 0x20) >> 5);
#endif
#ifdef QWKINJHEL
    //      For injector ROC31 helicity decoding:
    // Multipletsynch
    fData[MULOFF + i] = (Double_t)(((int)GetData(key) & 0x04) >> 2);
    // Helicity
    fData[HELOFF + i] = (Double_t)(((int)GetData(key) & 0x01));
    // Pairsynch
    fData[PAROFF + i] = (Double_t)(((int)GetData(key) & 0x10) >> 4);
#endif
    if (devices.IsUsed(key)) {
      devices.SetUsed(MULOFF + i);
      devices.SetUsed(HELOFF + i);
      devices.SetUsed(PAROFF + i);
    }
  }

//  // Correction for inverted Pairsynch polarity
//    Int_t fakeps = (((int)GetData(ITIRDATA) & 0x80) >> 7);
//    if(fakeps==1) {
//      fakeps = 0; 
//    }else if (fakeps==0) {
//      fakeps = 1;
//    }
//    fData[IPAIRSYNCH] = (Double_t) fakeps;

  fData[ITIMESLOT] = (Double_t)(((int)GetData(IOVERSAMPLE) & 0xff00) >> 8);
  // kluge to correct new timing boards, with os counting 1,2...(n-1),0
  if (fData[ITIMESLOT]==0) fData[ITIMESLOT] = fgOversample;


//    clog << "VaEvent::Load hel/ps/ms/ts: " 
//         << " " << fData[IHELICITY]
//         << " " << fData[IPAIRSYNCH]
//         << " " << fData[IMULTIPLETSYNCH]
//         << " " << fData[ITIMESLOT] << endl;

  if (devices.IsUsed(IOVERSAMPLE)) devices.SetUsed(ITIMESLOT);

  if(fgCalib) 
    CalibDecode(devices);

  if ( IsPhysicsEvent() ) {
     CheckAdcxDacBurp();
     CheckAdcxBad();
  }

};

void
VaEvent::CalibDecode(TaDevice& devices)
{
  // Analysis to include data not corrected for pedestal.
  //  Can be turned on with fgCalib.
  //  Much of this looks like VaEvent::Decode...
  //   BUT DON'T BE FOOLED!
  Int_t i,j,key,corrkey,idx,ixp,ixm,iyp,iym;
  Int_t ixpyp,ixpym,ixmyp,ixmym;


// Calibrate the ADCs first (no pedestal subtraction here!)
  for (i = 0;  i < ADCNUM+ADCXNUM; i++) {
    for (j = 0; j < 4; j++) {
      key = i*4 + j;
      corrkey = ADCDACSUBOFF + key;
      fData[corrkey] = fData[ADCOFF + key] 
	- (fData[DACOFF + i]-ADC_MinDAC) * devices.GetDacSlope(key);
      if (devices.IsUsed(ADCOFF+key)) devices.SetUsed(corrkey);
    }
  }

  // Calibrate Scalers for use with v2fs (no pedestal subtraction here!)
  // Only one V2F is allowed per scaler.  Sorry.
  Int_t clockkey;

  scalerbad = 0;
  //***************************
  // added to clean data from CH scaler, 18Apr10, rupesh
  // v2f clock is 4Mhz
  // expected_v2f = IT*2.5*4.0
  Int_t expected_v2f = fData[TBDOFF + 8]*2.5*4.0;
  // add +/- 10% of its own value to give us plenty of wiggle room for imprecision
  expected_v2f = expected_v2f + 0.1*expected_v2f;
  //    cout << "VaEvent:: expected_v2f:: " << expected_v2f << endl;
  // **************************

  for (i = 0; i < V2FCLKNUM; i++) {
    clockkey = i + V2FCLKOFF;
    Double_t clockval;

    // For PVDIS we don't care about clock.  Let it be 1.
    // But leave the other devices alone.

    if (devices.GetMinPvdisPKey() != -1 && 
        devices.GetMaxPvdisPKey() != -1 &&
        SCAOFF+i*32 >= devices.GetMinPvdisPKey() && 
        SCAOFF+i*32 <= devices.GetMaxPvdisPKey()) {

           clockval = 1; 

    } else {

           clockval = fData[clockkey];
    }

    // If there's no clockval... then there's no calibration.
    if (clockval == 0) {
      for (j = 0; j < 32; j++) {
	key = j + i*32;
	corrkey = SCACLKDIVOFF + key;
	fData[corrkey] = 0;
        if (devices.IsUsed(SCAOFF+key)) devices.SetUsed(corrkey);
      }
    } else {
      // HA! There IS a clockval!
      for (j = 0; j < 32; j++) {
	key = j + i*32;
	//*******************
	// maxcount for any scaler channel is v2f_clk val
	if(fData[SCAOFF + key]>expected_v2f || clockval>expected_v2f) scalerbad=1; 
	//***************
	corrkey = SCACLKDIVOFF + key;
	fData[corrkey] = (fData[SCAOFF + key])/clockval;
        if (devices.IsUsed(SCAOFF+key)) devices.SetUsed(corrkey);
      }
    }
  }  

// Quad photodiode (no pedestal subtraction here!)
  for (i = 0; i < QPDNUM; i++) {
    key = QPDOFF + 9*i;
    corrkey = QPDCORROFF + 4*i;
    ixpyp = devices.GetCorrIndex(key);
    ixpym = devices.GetCorrIndex(key+1);
    ixmyp = devices.GetCorrIndex(key+2);
    ixmym = devices.GetCorrIndex(key+3);
    if (ixpyp < 0 || ixpym < 0 || ixmyp < 0 || ixmym < 0) continue;
    if (i==0) {  
      fData[corrkey]   = fData[ixpyp]*fQPD1Pars[0];
      fData[corrkey+1] = fData[ixpym]*fQPD1Pars[1];
      fData[corrkey+2] = fData[ixmyp]*fQPD1Pars[2];
      fData[corrkey+3] = fData[ixmym]*fQPD1Pars[3];
    }
    if (devices.IsUsed(key)) {
      devices.SetUsed(corrkey);
      devices.SetUsed(corrkey+1);
      devices.SetUsed(corrkey+2);
      devices.SetUsed(corrkey+3);
    }
  }

//  Stripline BPMs (no pedestal subtraction here!)
    for (i = 0; i < STRNUM; i++) {
      key = STROFF + 10*i;
      corrkey = STRCORROFF + 4*i;
      ixp = devices.GetCorrIndex(key);
      ixm = devices.GetCorrIndex(key+1);
      iyp = devices.GetCorrIndex(key+2);
      iym = devices.GetCorrIndex(key+3);
      if (ixp < 0 || ixm < 0 || iyp < 0 || iym < 0) continue;
      fData[corrkey]   = fData[ixp];
      fData[corrkey+1] = fData[ixm];
      fData[corrkey+2] = fData[iyp];
      fData[corrkey+3] = fData[iym];
      if (devices.IsUsed(key)) {
	devices.SetUsed(corrkey);
	devices.SetUsed(corrkey+1);
	devices.SetUsed(corrkey+2);
	devices.SetUsed(corrkey+3);
      }
    }

// Cavity BPM monitors (no pedestal subtraction here!)
  for (i = 0; i < CAVNUM; i++) {
    for (j = 0; j < 2; j++) {
       key = CAVOFF + 4*i + j;
       corrkey = CAVCORROFF + 2*i;
       if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
       idx = devices.GetCorrIndex(key);
       if (idx < 0) continue;
       fData[corrkey] = fData[idx];
       if (devices.IsUsed(key)) devices.SetUsed(corrkey);
    }
  }

// Cavity BCM monitors (no pedestal subtraction here!)
  for (i = 0; i < CCMNUM; i++) {
    key = CCMOFF + 2*i;
    corrkey = CCMCORROFF + i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCorrIndex(key);
    if (idx < 0) continue;
    fData[corrkey] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(corrkey);
  }

// Happex-1 era BCMs (no pedestal subtraction here!)
  for (i = 0; i < BCMNUM; i++) {
    key = BCMOFF + 2*i;
    corrkey = BCMCORROFF + i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCorrIndex(key);
    if (idx < 0) continue;
    fData[corrkey] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(corrkey);
  }

// Lumi monitors (no pedestal subtraction here!)
  for (i = 0; i < LMINUM; i++) {
    key = LMIOFF + 2*i;
    corrkey = LMICORROFF + i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCorrIndex(key);
    if (idx < 0) continue;
    fData[corrkey] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(corrkey);
  }

// Detectors (no pedestal subtraction here!)
  for (i = 0; i < DETNUM; i++) {
    key = DETOFF + 3*i;
    corrkey = DETCORROFF + i;
    if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
    idx = devices.GetCorrIndex(key);
    if (idx < 0) continue;
    fData[corrkey] = fData[idx];
    if (devices.IsUsed(key)) devices.SetUsed(corrkey);
  }

// UMass Profile scanners (no pedestal subtraction)
  for (i = 0; i < PROFNUM; i++) {
     key = PROFOFF + 3*i;
     corrkey = key + 1;
     if (devices.GetDevNum(key) < 0 || devices.GetChanNum(key) < 0) continue;
     idx = devices.GetCorrIndex(key);
     if (idx < 0) continue;
     fData[corrkey] = fData[idx];
     if (devices.IsUsed(key)) devices.SetUsed(corrkey);
  }

}

void
VaEvent::CheckEvent(TaRun& run)
{
  // Analysis-independent checks of event quality; updates event's cut array.

  const Int_t WRONGEVNO  = 0x1;
  const Int_t WRONGTMSL  = 0x2;

  Double_t current = GetData(fgCurMon);
  Double_t currentc = GetData(fgCurMonC);


  fFailedACut = false;
  Int_t valLowBeam =0;



  Int_t startupval = 0;
  if ( fgLastEv.GetEvNumber() == 0 )
    {
      // First event, cut it (startup cut)
      startupval = 1;
    }
  else
    {
      // Not the first event, so check event-to-event differences
      
      if ( fgLoBeamNo < fgNCuts)
	{
	  Int_t thisval = 0;
	  if (current < fgLoBeam )
	    {
#ifdef NOISY
	      clog << "Event " << fEvNum << " failed lobeam cut, "
		   << current << " < " << fgLoBeam << endl;
#endif
	      thisval = 1;
	    }
	  valLowBeam=thisval; // flag for later cuts, if needed.
	  AddCut (fgLoBeamNo, thisval);
	  run.UpdateCutList (fgLoBeamNo, thisval, fEvNum);
	}
      
      if ( fgLoBeamCNo < fgNCuts)
	{
	  Int_t thisval = 0;
	  if (currentc < fgLoBeamC )
	    {
#ifdef NOISY
	      clog << "Event " << fEvNum << " failed lobeamc cut, "
		   << currentc << " < " << fgLoBeamC << endl;
#endif
	      thisval = 1;
	    }
	  AddCut (fgLoBeamCNo, thisval);
	  run.UpdateCutList (fgLoBeamCNo, thisval, fEvNum);
	}
      
      if ( fgSatNo < fgNCuts)
	{
	  Int_t thisval = 0;
	  Bool_t saturated = false;
	  UInt_t i;
	  for (i = 0; i < DETNUM && !saturated; i++) 
	    saturated = GetData (fgDetRaw[i]) >= fgSatCut;
	  
	  if (saturated)
	    {
#ifdef NOISY
	      clog << "Event " << fEvNum << " failed saturation cut, det"
		   << i << " raw = " << GetData (fgDetRaw[i-1]) << " > " << fgSatCut << endl;
#endif
	      thisval = 1;
	    }
	  AddCut (fgSatNo, thisval);
	  run.UpdateCutList (fgSatNo, thisval, fEvNum);
	}
      
      if ( fgMonSatNo < fgNCuts)
	{
	  Int_t thisval = 0;
	  Bool_t saturated = false;
	  UInt_t i;
	  for (i = 0; i < MONNUM && !saturated; i++) 
	    saturated = GetData (fgMonRaw[i]) >= fgMonSatCut;
	  
	  if (saturated)
	    {
#ifdef NOISY
	      clog << "Event " << fEvNum << " failed saturation cut, mon"
		   << i << " raw = " << GetData (fgMonRaw[i-1]) << " > " << fgMonSatCut << endl;
#endif
	      thisval = 1;
	    }
	  AddCut (fgMonSatNo, thisval);
	  run.UpdateCutList (fgMonSatNo, thisval, fEvNum);
    }
      
      
      if ( fgAdcxDacBurpNo < fgNCuts)
	{
	  Int_t thisval = 0;
	  
	  if (adcxglitch != 0)
	    {
#ifdef NOISY
	      clog << "Event " << fEvNum << " failed ADCX burp cut" << endl;
#endif
	      thisval = 1;
	    }
	  AddCut (fgAdcxDacBurpNo, thisval);
	  run.UpdateCutList (fgAdcxDacBurpNo, thisval, fEvNum);
	}
      
      
      if ( fgAdcxBadNo < fgNCuts)
	{
	  Int_t thisval = 0;
	  
	  if (adcxbad != 0)
	    {
#ifdef NOISY
	      clog << "Event " << fEvNum << " failed ADCX bad cut " << endl;
#endif
	      thisval = 1;
	    }
	  AddCut (fgAdcxBadNo, thisval);
	  run.UpdateCutList (fgAdcxBadNo, thisval, fEvNum);
	}
      
      // cut out bad scaler events, added 18Apr10, rupesh
  if ( fgScalerBadNo < fgNCuts)
    {
      Int_t thisval = 0;
      
      if (scalerbad != 0)
	{
#ifdef NOISY
	  clog << "Event " << fEvNum << " failed Scaler bad cut " << endl;
#endif
	  thisval = 1;
	}
      AddCut (fgScalerBadNo, thisval);
      run.UpdateCutList (fgScalerBadNo, thisval, fEvNum);
    }
  
      // Beam burp -- current change greater than limit?
      if (fgBurpNo < fgNCuts)
	{
	  Int_t thisval = 0;
	  if (abs (current-fgLastEv.GetData(fgCurMon)) > fgBurpCut)
	    {
#ifdef NOISY
	      clog << "Event " << fEvNum << " failed beam burp cut, "
		   << abs (current-fgLastEv.GetData(fgCurMon)) << " > " << fgBurpCut << endl;
#endif
	      thisval = 1;
	    }
	  AddCut (fgBurpNo, thisval);
	  run.UpdateCutList (fgBurpNo, thisval, fEvNum);
	}


      // C Beam burp -- current change greater than limit?
      // **** Modified so only takes effect if !Low_Beam ****
      if (fgCBurpNo < fgNCuts)
	{
	  Int_t thisval = 0;
	  if (valLowBeam==0 && 
	      abs (currentc-fgLastEv.GetData(fgCurMonC)) > fgCBurpCut)
	    {
#ifdef NOISY
	      clog << "Event " << fEvNum << " failed C-beam burp cut, "
		   << abs (currentc-fgLastEv.GetData(fgCurMonC)) << " > " << fgCBurpCut << endl;
#endif
	      thisval = 1;
	    }
	  AddCut (fgCBurpNo, thisval);
	  run.UpdateCutList (fgCBurpNo, thisval, fEvNum);
	}

      //Check Energy Beam Burp
      if ( fgPosBurpENo < fgNCuts)
	{
	  Int_t thisval = 0;
	  Bool_t posburpE = false;
	  UInt_t i;
	  for (i = 0; !posburpE && i < fgNPosMonE; i++)
	    if (run.GetDevices().IsUsed(fgPosMonE[i]))
	      posburpE = abs( GetData(fgPosMonE[i]) -
			     fgLastEv.GetData(fgPosMonE[i]) ) > fgPosBurpE[i];
	  
	  if (posburpE)
	    {
#ifdef NOISY
	      clog << "Event " << fEvNum 
		   << " failed beam energy burp cut, monitor " << 
	           << i << " = " 
		   << abs( GetData(fgPosMonE[i-1])-fgLastEv.GetData(fgPosMonE[i-1]))
		   << " > " << fgPosBurpE[i-1] << endl;
#endif
	      thisval = 1;
	    }
	  AddCut (fgPosBurpENo, thisval);
	  run.UpdateCutList (fgPosBurpENo, thisval, fEvNum);
	}

      //Check Position Beam Burp
      if ( fgPosBurpNo < fgNCuts)
	{
	  Int_t thisval = 0;
	  Bool_t posburp = false;
	  UInt_t i;
	  for (i = 0; !posburp && i < fgNPosMon; i++)
	    if (run.GetDevices().IsUsed(fgPosMon[i]))
	      posburp = abs( GetData(fgPosMon[i]) -
			     fgLastEv.GetData(fgPosMon[i]) ) > fgPosBurp[i];
	  
	  if (posburp)
	    {
#ifdef NOISY
	      clog << "Event " << fEvNum << " failed beam burp cut, monitor " << 
	           << i << " = " 
		   << abs( GetData(fgPosMon[i-1])-fgLastEv.GetData(fgPosMon[i-1]) ) 
                   << " > " << fgPosBurp[i-1] << endl;
#endif
	      thisval = 1;
	    }
	  AddCut (fgPosBurpNo, thisval);
	  run.UpdateCutList (fgPosBurpNo, thisval, fEvNum);
	}




      // Check event number sequence
      if (fgEvtSeqNo < fgNCuts)
	{
	  Int_t thisval = 0;
	  if (GetEvNumber() != fgLastEv.GetEvNumber() + 1) 
	    {
	      cerr << "VaEvent::CheckEvent ERROR Event " 
		   << GetEvNumber() 
		   << " unexpected event number, last event was " 
		   << fgLastEv.GetEvNumber()
		   << endl;
	      thisval = WRONGEVNO;
	    } 
	  AddCut (fgEvtSeqNo, thisval);
	  run.UpdateCutList (fgEvtSeqNo, thisval, fEvNum);
	}

      // Check time slot sequence
      if ( fgEvtSeqNo < fgNCuts)
	{
	  Int_t thisval = 0;
	  if (fgOversample > 0 )
	    { 
	      if ( GetTimeSlot() != 
		   fgLastEv.GetTimeSlot() % fgOversample + 1 ) 
		{
		  cerr << "VaEvent::CheckEvent ERROR Event " 
		       << GetEvNumber() 
		       << " unexpected timeslot value, went from " 
		       << fgLastEv.GetTimeSlot()
		       << " to " 
		       << GetTimeSlot() 
		       << endl;
		  thisval = WRONGTMSL;
		} 
	    }
	  AddCut (fgEvtSeqNo, thisval);
	  run.UpdateCutList (fgEvtSeqNo, thisval, fEvNum);
	}

    }

  if (fgStartupNo < fgNCuts)
    {
      AddCut (fgStartupNo, startupval);
      run.UpdateCutList (fgStartupNo, startupval, fEvNum);
    }

  fgLastEv = *this;
};


Int_t VaEvent::DecodeCrates(TaDevice& devices) {
// Decoding of crate structure.  
// A crate is also called a "ROC" in CODA parlance.
// return code:
//    0 = no decoding done (error, or wrong event type)
//    1 = fine.
  int iroc, lentot, n1, ipt, istart, istop;
  uint fFragLength, fSubbankTag, fSubbankType, fSubbankNum;
// Cannot decode non-physics triggers  
  if(fEvType < 0 || fEvType > 12) return 0; 
  if(DECODE_DEBUG) RawDump();
  memset (fN1roc, 0, MAXROC*sizeof(Int_t));
  memset (fLenroc, 0, MAXROC*sizeof(Int_t));
  memset (fIrn, 0, MAXROC*sizeof(Int_t));
  numroc = 0;
  lentot = 0;
// Split up the ROCs 
  for (iroc=0; iroc < MAXROC; iroc++) {
// n1 = pointer to first word of ROC
    if(iroc==0) {
       n1 = fEvBuffer[2]+3;
    } else {
       n1 = fN1roc[fIrn[iroc-1]]+fLenroc[fIrn[iroc-1]]+1;
    }
    if( (unsigned int)(n1+1) >= fEvLen ) break;// fIrn = ROC number 
    fIrn[iroc]=(fEvBuffer[n1+1]&0xff0000)>>16;
// This error might mean you need a bigger MAXROC parameter,
// or it might mean corruption of the raw data.
    if(fIrn[iroc]<0 || fIrn[iroc]>=MAXROC) {
       cout << "ERROR in VaEvent::DecodeCrates():";
       cout << "  illegal ROC number " <<dec<<fIrn[iroc]<<endl;
       return 0;
    }
    if (iroc == 0) {
       fN1roc[fIrn[iroc]] = n1;
       fLenroc[fIrn[iroc]] = fEvBuffer[n1];
       lentot = n1 + fEvBuffer[n1];
    } else {
       fN1roc[fIrn[iroc]]=
          fN1roc[fIrn[iroc-1]]+fLenroc[fIrn[iroc-1]]+1;
       fLenroc[fIrn[iroc]] = fEvBuffer[fN1roc[fIrn[iroc]]];
       lentot  = lentot + fLenroc[fIrn[iroc]] + 1;
    } 
    numroc++;
    if(DECODE_DEBUG) {
        cout << "Roc ptr " <<dec<<numroc<<"  "<<iroc+1;
        cout << "  "<<fIrn[iroc]<<"  "<<fN1roc[fIrn[iroc]];
        cout << "  "<<fLenroc[fIrn[iroc]];
        cout << "  "<<lentot<<"  "<<fEvLen<<endl;
    }
    if ((unsigned int)lentot >= fEvLen) break;
  }
// Find device headers in each ROC
  for (iroc=0; iroc < numroc; iroc++) {
    // MUST use special case for ROC 31 - injector ROC utilizing QWeak readout buffer.
    if (fIrn[iroc]==31) {
      istart = fN1roc[fIrn[iroc]];
      istop = fN1roc[fIrn[iroc]]+fLenroc[fIrn[iroc]];

      //debugging output
      //       for (ipt = istart; ipt< 30; ipt++) {
      //        	cout << hex << fEvBuffer[ipt] << endl;
      //       }

      // 
      ipt = istart;
      fFragLength = fEvBuffer[ipt] -1; // This is the number of words in the data block
                                       // minus the ID word which follows
      fSubbankTag   = (fEvBuffer[ipt+1]&0xFFFF0000)>>16; // Bits 16-31
      fSubbankType  = (fEvBuffer[ipt+1]&0xFF00)>>8;      // Bits 8-15
      fSubbankNum   = (fEvBuffer[ipt+1]&0xFF);           // Bits 0-7
 
      // debugging output
      //       cout << Form("istop is %x", istop) << endl;
      //       cout << Form("fFragLength is %x",fFragLength) << endl;
      //       cout << Form("fSubbankTag is %x",fSubbankTag) << endl;
      //       cout << Form("fSubbankType is %x",fSubbankType) << endl;
      //       cout << Form("fSubbankNum is %x",fSubbankNum) << endl;

      if (fSubbankTag<=31){
	//  Subbank tags between 0 and 31 indicate this is
	//  a ROC bank.
	//	fROC        = fSubbankTag;
	fSubbankTag = 0;
	ipt = istart+2;	// get pointer to first subblock
      }
      
      while (ipt < istop) {

	fFragLength = fEvBuffer[ipt] -1; // This is the number of words in the data block, 
                                         // minus the ID word which follows
	fSubbankTag   = (fEvBuffer[ipt+1]&0xFFFF0000)>>16; // Bits 16-31
	fSubbankType  = (fEvBuffer[ipt+1]&0xFF00)>>8;      // Bits 8-15
	fSubbankNum   = (fEvBuffer[ipt+1]&0xFF);           // Bits 0-7
	// debugging output
	// 	cout << Form("  fFragLength2 is %x",fFragLength) << endl;
	// 	cout << Form("  fSubbankTag2 is %x",fSubbankTag) << endl;
	// 	cout << Form("  fSubbankType2 is %x",fSubbankType) << endl;
	// 	cout << Form("  fSubbankNum2 is %x",fSubbankNum) << endl;	

      // locate subblock tag 3101 - scalers
	if (fSubbankTag == 0x3101 ) {
	}
      // locate subblock tag 3102 - vqwk
	if (fSubbankTag == 0x3102 ) {
	  if ((fFragLength%48)==2) {
	    // test of frag length, if frag length is 2+(integer*6*8), then we start with a padding word
	    //  step header by one more word
	    devices.FindHeaders(fIrn[iroc], ipt+2, devices.GetVqwkHeader() );  // kludge as if VQWK header is here
	  } else {
	    devices.FindHeaders(fIrn[iroc], ipt+1, devices.GetVqwkHeader() );  // kludge as if VQWK header is here
	  }
	}
	// locate subblock tag 3103 - fio + other stuff
	if (fSubbankTag == 0x3103 ) {
	  devices.FindHeaders(fIrn[iroc], ipt+1, devices.GetTirHeader() ); // kludge to point to data w/o header
	  //	  cout << Form("  tirdata is %x",fEvBuffer[ipt+2]) << endl;	
	  // handles first word of 3103 subblock only.
	}
	ipt += fFragLength+2;

      }

    } else {    
      // loop through every word to identify header start for data types in that ROC
      istart = fN1roc[fIrn[iroc]]+1;
      istop = fN1roc[fIrn[iroc]]+fLenroc[fIrn[iroc]];
      ipt = istart; 
      while (ipt++ < istop) {
	devices.FindHeaders(fIrn[iroc], ipt, fEvBuffer[ipt]);
      }
    }
  }
  if (DECODE_DEBUG) devices.PrintHeaders();
  return 1;
}

void VaEvent::AddCut (const Cut_t cut, const Int_t val)
{
  // Store information about cut conditions passed or failed by this event.
  // Lobeamc does not affect fFailedACut.

  if (fCutArray != 0)
    fCutArray[(unsigned int) cut] = val;
  if (cut != fgLoBeamCNo && val != 0)
    fFailedACut = true;
};


void VaEvent::AddResult( const TaLabelledQuantity& result)
{
  // Store a result from analysis of this event.

  fResults.push_back (result);
};


// Data access functions

Int_t VaEvent::GetRawData(Int_t index) const {
  // Return an item from the event buffer.

  if (index >= 0 && (UInt_t)index < fgMaxEvLen) 
    return fEvBuffer[index];
  else
    {
      cerr << "VaEvent::GetRawData ERROR: index " << index 
	   << "out of range 0 to " << fgMaxEvLen;
      return 0;
    }
};

Int_t VaEvent::GetRawData(Int_t key, TaDevice &devices, Int_t index) const {

  // Return an item from the event buffer.
  // This routine has a trick needed by PVDIS to align
  // the datastreams, but only if fgPvdisPhaseShift != 0
  // If 'key' is from among the PVDIS devices, we use 
  // the present event.
  // Otherwise we use the previous event !

  Int_t which_event = FindEventPhase(key, devices);

  
  if (index >= 0 && (UInt_t)index < fgMaxEvLen) 
    if (which_event == 0) {
      return fEvBuffer[index];
    } else {
      return fPrevBuffer[index];
    }
  else
    {
      cerr << "VaEvent::GetRawData ERROR: index " << index 
	   << "out of range 0 to " << fgMaxEvLen;
      return 0;
    }
};


Int_t VaEvent::FindEventPhase(Int_t key, TaDevice &devices) const {

  // To rapidly handle the event-phase shift necessary for PVDIS.

  // Returns 0 if the event belongs to this data buffer
  // Returns 1 if we need the previous event buffer.

  if ( !fPvdisPhaseShift ) return 0;

#ifdef DUMB_SLOW
  for (Int_t i=0; i < devices.GetNumPvdisDev(); i++) {
    if (key == devices.GetPvdisDev(i)) {
      return 0;
    }
  }
#endif

  if (devices.GetMinPvdisPKey() == -1 || devices.GetMaxPvdisPKey() == -1) {
    cout << "ERROR: Pvdis primary keys min,max not found "<<endl;
  }
  if (devices.GetMinPvdisDKey() == -1 || devices.GetMaxPvdisDKey() == -1) {
    cout << "ERROR: Pvdis derived keys min,max not found "<<endl;
  }

  if (  key >= devices.GetMinPvdisPKey() && 
        key <= devices.GetMaxPvdisPKey()) return 0;

  if (  key >= devices.GetMinPvdisDKey() && 
        key <= devices.GetMaxPvdisDKey()) return 0;

  return 1;

}


Bool_t VaEvent::CutStatus() const {
  // Return true iff event failed one or more cut conditions 

  return (fFailedACut);
};

Bool_t 
VaEvent::BeamCut() const
{
  // Return true iff event failed low beam cut

  return (CutCond(fgLoBeamNo) != 0);
}

Bool_t 
VaEvent::BeamCCut() const
{
  // Return true iff event failed low beam C cut

  return (CutCond(fgLoBeamCNo) != 0);
}

UInt_t 
VaEvent::GetNCuts () const
{
  // Return size of cut array

  return fgNCuts;
}

Int_t 
VaEvent::CutCond (const Cut_t c) const
{
  // Return value of cut condition c

  return (c < fgNCuts ? fCutArray[(unsigned int) c] : 0);
}

Bool_t VaEvent::IsPrestartEvent() const {
  return (fEvType == 17);
};

Bool_t VaEvent::IsPhysicsEvent() const {
  return (fEvType >= 1 && fEvType < 12);
};

Bool_t VaEvent::IsEpicsEvent() const {
  return (fEvType == fgEpicsType);
};

EventNumber_t VaEvent::GetEvNumber() const {
  return fEvNum;
};

Int_t VaEvent::GetLastPhyEv() const {
  return fLastPhyEv;
};

UInt_t VaEvent::GetEvLength() const {
  return fEvLen;
};

UInt_t VaEvent::GetEvType() const {
  return fEvType;
};

SlotNumber_t VaEvent::GetTimeSlot() const {
  // Return oversampling timeslot for this event.

  return (SlotNumber_t)GetData(ITIMESLOT);
};

void VaEvent::SetHelicity(EHelicity h)
{
  // Fill in the true helicity value for an event.  We use this to
  // associated a delayed helicity with the earlier event it applies to.

  fHel = h;
}

EHelicity VaEvent::GetROHelicity() const 
{
  // Return readout helicity as RightHeli or LeftHeli.  (WARNING: This
  // is the helicity stored in the data stream for this event, which
  // in general is *not* the helicity to use in analysis of this
  // event!  See GetHelicity().  Note also that this is the helicity
  // bit from the source and does not reflect half wave plate state,
  // g-2 precession, etc.)

  Double_t val = GetData(IHELICITY);
  if (val == 0)
    return RightHeli;
  else
    return LeftHeli;
}

EHelicity VaEvent::GetHelicity() const 
{
  // Return true helicity as RightHeli or LeftHeli.  (WARNING: This is
  // the helicity to use in analysis of this event, which in general
  // is *not* the helicity stored in the data stream for this event!
  // See GetROHelicity().  Note also that this is the helicity bit
  // from the source and does not reflect half wave plate state, g-2
  // precession, etc.)

  return fHel;
}

void VaEvent::SetPrevROHelicity(EHelicity h)
{
  // Fill in the readout helicity value for the previous event.

  fPrevROHel = h;
}

void VaEvent::SetPrevHelicity(EHelicity h)
{
  // Fill in the true helicity value for the previous event.

  fPrevHel = h;
}

EHelicity VaEvent::GetPrevROHelicity() const 
{
  // Return readout helicity of previous event as RightHeli or LeftHeli.

  return fPrevROHel;
}

EHelicity VaEvent::GetPrevHelicity() const {
  // Return true helicity of previous event as RightHeli or LeftHeli.

  return fPrevHel;
}

EPairSynch VaEvent::GetPairSynch() const {
  // Return pairsynch (aka realtime) for this event as FirstPS or
  // SecondPS, tagging this as an event from the first or second
  // window, repectively, of a helicity window pair.

  Double_t val = GetData(IPAIRSYNCH);
  if (val == 1) 
    return FirstPS;
  else 
    return SecondPS;
};

EMultipletSynch VaEvent::GetMultipletSynch() const {
  // Return multipletsynch for this event as FirstMS or
  // OtherMS, tagging this as an event from the first or later
  // window, repectively, of a helicity window multiplet.

  Double_t val = GetData(IMULTIPLETSYNCH);
  if (val == 1) 
    return FirstMS;
  else 
    return OtherMS;
};

const vector < TaLabelledQuantity > & VaEvent::GetResults() const 
{ 
  // Return event analysis results stored in this event.

  return fResults; 
};

void VaEvent::RawDump() const {
// Diagnostic dump of raw data for debugging purposes

   cout << "\n\n==========  Raw Data Dump  ==========" << hex << endl;
   cout << "\n Event number  " << dec << GetEvNumber();
   cout << "  length " << GetEvLength() << "  type " << GetEvType() << endl;
   cout << "\n Hex (0x) | Dec (d) format for each data" << endl;
   UInt_t ipt = 0;
   for (UInt_t j = 0; j < GetEvLength()/5; j++) {
       cout << dec << "\n fEvBuffer[" << ipt << "] = ";
       for (UInt_t k = j; k < j+5; k++) {
	 cout << hex << " 0x"<< GetRawData(ipt);
	 cout << " = (d)"<< dec << GetRawData(ipt) << " |";
         ipt++;
       }
   }
   if (ipt < GetEvLength()) {
      cout << dec << "\n fEvBuffer[" << ipt << "] = ";
      for (UInt_t k = ipt; k < GetEvLength(); k++) {
	cout << hex << " 0x"<< GetRawData(ipt);
	cout << " = (d)"<< dec << GetRawData(ipt) << " |";
        ipt++;
      }
      cout << endl;
   }
   cout << "--------------------------------\n\n";
};

void VaEvent::RawPrevDump() const {
// Diagnostic dump of raw data for debugging purposes
// For Previous Buffer.

   cout << "\n\n==========  Previous Raw Data Dump  ==========" << hex << endl;
   UInt_t pevlen = fPrevBuffer[0]+1;
   Int_t ptype = fPrevBuffer[1]>>16;
   cout << "Length "<<pevlen<<"   Type "<<ptype<<endl;
   UInt_t ipt = 0;
   for (UInt_t j = 0; j < pevlen/5; j++) {
       cout << dec << "\n fPrevBuffer[" << ipt << "] = ";
       for (UInt_t k = j; k < j+5; k++) {
	 cout << hex << " 0x"<< fPrevBuffer[ipt];
	 cout << " = (d)"<< dec << fPrevBuffer[ipt] << " |";
         ipt++;
       }
   }
   if (ipt < pevlen) {
      cout << dec << "\n fPrevBuffer[" << ipt << "] = ";
      for (UInt_t k = ipt; k < pevlen; k++) {
	 cout << hex << " 0x"<< fPrevBuffer[ipt];
	 cout << " = (d)"<< dec << fPrevBuffer[ipt] << " |";
         ipt++;
      }
      cout << endl;
   }
   cout << "--------------------------------\n\n";
};


void VaEvent::DeviceDump() const {
// Diagnostic dump of device data for debugging purposes.


  static int bpmraw[] = {  IBPM8XP, IBPM8XM, IBPM8YP, IBPM8YM, 
                           IBPM10XP, IBPM10XM, IBPM10YP, IBPM10YM, 
                           IBPM12XP, IBPM12XM, IBPM12YP, IBPM12YM, 
			   IBPM14XP, IBPM14XM, IBPM14YP, IBPM14YM, 
                           IBPM4AXP, IBPM4AXM, IBPM4AYP, IBPM4AYM, 
                           IBPM4BXP, IBPM4BXM, IBPM4BYP, IBPM4BYM };
  static int bpmcalib[] = { IBPM8X, IBPM8Y, IBPM10X, IBPM10Y, IBPM14X, IBPM14Y, 
                           IBPM4AX, IBPM4AY, IBPM4BX, IBPM4BY };
  static int profile[] = { ILPROF, ILPROFX, ILPROFY,
                           IRPROF, IRPROFX, IRPROFY };
  static int scalers[] = { ISCALER0_0, ISCALER0_1, ISCALER0_2, ISCALER0_3 };
  cout << "\n============   Device Data for Event "<<dec<<GetEvNumber()<<"  ======== "<<endl;
  cout << "Raw BCM1 = 0x"<<hex<<(Int_t)GetData(IBCM1R);
  cout <<"    BCM2 = 0x"<<(Int_t)GetData(IBCM2R)<<endl;
  cout << "Calibrated BCM1 = "<<GetData(IBCM1)<<"  BCM2 = "<<GetData(IBCM2)<<endl; 
  cout << "Raw BPM antenna:   (hex Int_t)"<<endl;
  for (int i = 0; i < 20; i++) {
    cout << dec << bpmraw[i] << "  = 0x"<<hex<<(Int_t)GetData(bpmraw[i])<<"   ";
    if (i > 0 && (((i+1) % 4) == 0)) cout << endl;
  }
  cout << "Calibrated BPM positions:  (Double_t) "<<endl;
  for (int i = 0; i < 8; i++) {
    cout << bpmcalib[i] << " = "<<GetData(bpmcalib[i])<<"    ";
    if (i > 0 && (((i+1) % 2) == 0)) cout << endl;
  }
  cout << "UMass profile scanner "<<endl;
  for (int i = 0; i < 6; i++) {
    cout << profile[i] << "  =  " << GetData(profile[i])<<"   ";
  }
  cout << "Scalers: "<<endl;
  for (int i = 0; i < 4; i++) {
    cout << dec << scalers[i] << "  = 0x"<<hex<<(Int_t)GetData(scalers[i])<<"   ";
  }
  cout <<endl;
  cout << "tirdata = 0x"<<hex<<(Int_t)GetData(ITIRDATA);  
  cout << "   readout helicity = "<<dec<<(Int_t)GetData(IHELICITY);
  cout << "   timeslot = "<<(Int_t)GetData(ITIMESLOT);
  cout << "   pairsynch = "<<(Int_t)GetData(IPAIRSYNCH)<<endl;
  cout << "Data by ADC "<<endl;
  for (int adc = 0; adc < 10; adc++) {
    cout << " -> ADC "<<dec<<adc<<endl;
    for (int chan = 0; chan < 4; chan++) {
      cout << "  chan "<<  chan;
      cout << " raw= 0x" << hex << (Int_t)GetRawADCData(adc,chan)<<dec;
      cout << "  cal= " << GetCalADCData(adc,chan);
      if (chan == 0 || chan == 2) cout << "  |   ";
      if (chan == 1) cout << endl;
    }
    cout << endl;
  }
  cout << dec;
}

void 
VaEvent::MiniDump() const 
{
// Diagnostic dump of selected data on one line for debugging purposes.

  cout << "Event " << dec << setw(7) << GetEvNumber()
       << " h(ro)/h/t/p/q =" << dec 
       << " " << setw(2) << (Int_t) GetData (IHELICITY)
       << "/" << setw(2) << (Int_t) GetHelicity()
       << "/" << setw(2) << (Int_t) GetData (ITIMESLOT)
       << "/" << setw(2) << (Int_t) GetData (IPAIRSYNCH)
       << "/" << setw(2) << (Int_t) GetData (IMULTIPLETSYNCH)
       << " Cuts = " << setw(2) << CutCond(0)
       << setw(2) << CutCond(1)
       << setw(2) << CutCond(2)
       << setw(2) << CutCond(3)
       << setw(2) << CutCond(4)
       << " BCM = " << GetData (fgCurMon)
       << endl; 
}

Double_t VaEvent::GetData( Int_t key ) const { 
// To find a value corresponding to a data key 
  return fData[Idx(key)];
}; 

Double_t 
VaEvent::GetDataSum (vector<Int_t> keys, vector<Double_t> wts) const
{
  // Get weighted sum of quantities corresponding to set of keys

  Double_t sum = 0;

  if (wts.size() == 0)
    for (vector<Int_t>::const_iterator p = keys.begin();
	 p != keys.end();
	 ++p)
      sum += fData[Idx(*p)];
  else if (wts.size() != keys.size())
    cerr << "VaEvent::GetDataSum ERROR: Weight and key vector sizes differ" << endl;
  else
    for (size_t i = 0; i < keys.size(); ++i)
      sum += wts[i] * fData[Idx(keys[i])];

  return sum;
}

Int_t VaEvent::Idx(const Int_t& index) const {
  if (index >= 0 && index < MAXKEYS) return index;
  return 0;
};

Double_t VaEvent::GetRawADCData( Int_t adc, Int_t chan ) const {
// Data raw data for adc # 0, 1, 2...  and channel # 0,1,2,3
  return GetData(ADCOFF + 4*adc + chan);
}; 

Double_t VaEvent::GetCalADCData( Int_t adc, Int_t chan ) const {
// Data calibrated data for adc # 0, 1, 2...and channel # 0,1,2,3
  return GetData(ACCOFF + 4*adc + chan);
}; 

Double_t VaEvent::GetScalerData( Int_t scaler, Int_t chan ) const {
// Data from scaler # 1,2,3..  and channel # 1,2,3...
  return GetData(SCAOFF + 32*scaler + chan);
}; 

void 
VaEvent::AddToTree (TaDevice& devices, 
		    const TaCutList& cutlist,
		    TTree& rawtree ) 
{
  // Add the data of this device to the raw data tree (root output)
  // Called by TaRun::InitDevices()
    DecodeCook(devices);
    Int_t bufsize = 5000;
    char tinfo[20];
    Int_t key;
    map<string, Int_t> fKeyToIdx = devices.GetKeyList();
    for (map<string, Int_t>::iterator ikey = fKeyToIdx.begin(); 
      ikey != fKeyToIdx.end(); ikey++) {
        string keystr = ikey->first;
        key = devices.GetKey(keystr);
        if (key <= 0) continue;
        if (key >= MAXKEYS) {
          cerr << "VaEvent::AddToTree::ERROR:  Attempt to add a key = "<<key;
          cerr << " larger than array size MAXKEYS = "<<MAXKEYS<<endl;
          cerr << "Compile with a bigger MAXKEYS value."<<endl;
          continue;
        }
// Dont add to tree unless in datamap
        if ( !devices.IsUsed(key) ) continue;  
        strcpy(tinfo,keystr.c_str());  strcat(tinfo,"/D");
  	rawtree.Branch(keystr.c_str(), &fData[key], tinfo, bufsize);
    }


    for (Cut_t icut = Cut_t (0); icut < cutlist.GetNumCuts(); ++icut)
      {
	TaString cutstr = "cond_" + cutlist.GetName(icut);
	cutstr = cutstr.ToLower();
        strcpy (tinfo, cutstr.c_str());  strcat(tinfo,"/I");
  	rawtree.Branch(cutstr.c_str(), 
		       &fCutArray[(unsigned int) icut], 
		       tinfo, 
		       bufsize);
      }	
};

void
VaEvent::SetCalib(Bool_t flag)
{
  fgCalib = flag;
};

// Private methods

Double_t VaEvent::Rotate(Double_t x, Double_t y, Int_t xy) {
// Rotation to get X or Y depending on xy flag
   Double_t result = 0;
   Double_t root2 = sqrt(2.0);
   if (xy == 2) {
      result = ( x + y ) / root2;
   } else {
      result = ( x - y ) / root2;
   } 
   return result;
};

void VaEvent::Create(const VaEvent& rhs)
{
  // Utility routine used by copy constructor and assignment.

 fEvType = rhs.fEvType;
 fEvNum = rhs.fEvNum;
 fEvLen = rhs.fEvLen;
 fFailedACut = rhs.fFailedACut;
 fResults = rhs.fResults;
 fHel = rhs.fHel;
 fPrevROHel = rhs.fPrevROHel;
 fPrevHel = rhs.fPrevHel;
 fEvBuffer = new Int_t[fgMaxEvLen];
 memset (fEvBuffer, 0, fgMaxEvLen*sizeof(Int_t));
 memcpy(fEvBuffer, rhs.fEvBuffer, fEvLen*sizeof(Int_t));
 fPrevBuffer = new Int_t[fgMaxEvLen];
 memset(fPrevBuffer, 0, fgMaxEvLen*sizeof(Int_t));
 UInt_t pevlen = fPrevBuffer[0]+1;
 memcpy(fPrevBuffer, rhs.fPrevBuffer, pevlen*sizeof(Int_t));
 fData = new Double_t[MAXKEYS];
 memcpy(fData, rhs.fData, MAXKEYS*sizeof(Double_t));
 fPrevAdcxBase = new Double_t[16*ADCXNUM];
 memset(fPrevAdcxBase, 0, 16*ADCXNUM*sizeof(Double_t));
 memcpy(fPrevAdcxBase, rhs.fPrevAdcxBase, 16*ADCXNUM*sizeof(Double_t));
 // We don't need to copy these pointers because they get
 // filled and used only in decoding.
 fN1roc = new Int_t[MAXROC];
 fIrn = new Int_t[MAXROC];
 fLenroc = new Int_t[MAXROC];
 if (rhs.fCutArray != 0 && fgNCuts > 0)
   {
     fCutArray = new Int_t[fgNCuts];
     memcpy(fCutArray, rhs.fCutArray, fgNCuts*sizeof(Int_t));
   }
 else
   fCutArray = 0;
};

void VaEvent::Uncreate()
{
  // Utility routine used by destructor and assignment.

  delete [] fEvBuffer;
  delete [] fPrevBuffer;
  delete [] fPrevAdcxBase;
  delete [] fData;
  delete [] fN1roc;
  delete [] fLenroc;
  delete [] fIrn;
  if (fCutArray != 0)
    delete [] fCutArray;
};

Double_t 
VaEvent::UnpackAdcx (Int_t rawd, Int_t key)
{
  // Data from ADCX are unpacked and dealt with
  // If the key is within one of the data blocks used for an ADCX
  // signal, we check for compatibility between the key and the
  // data.  If not (presumably meaning it's a device tied to an ADCX)
  // we do no checking.  If there's a problem it should be caught in
  // checking the ADC signal.

  Int_t lprint=0;

  Int_t mask31x   = 0x80000000;   // Header bit mask
  Int_t mask3029x = 0x60000000;   // Channel number mask
  Int_t mask2625x = 0x06000000;   // Divider value mask
  Int_t mask2422x = 0x01c00000;   // Data type mask
  Int_t mask21x   = 0x00200000;   // Data type 0 value sign mask
  Int_t mask200x  = 0x001fffff;   // Data type 0 value field mask
  Int_t mask2118x = 0x003c0000;   // Data types 1-2 sample number mask
  Int_t mask170x  = 0x0003ffff;   // Data types 1-2 value field mask
  Int_t mask150x  = 0x0000ffff;   // Data type 4 value field mask

  // "Actual" values from data word
  Int_t act_dtype  = (rawd & mask2422x) >> 22;
  Int_t act_chan   = act_dtype != 4 ? ((rawd & mask3029x) >> 29) : 0;
  Int_t act_dvalue = (rawd & mask2625x) >> 25;
  Int_t act_snum   = act_dtype == 1 || act_dtype == 2 ?
    ((rawd & mask2118x) >> 18) : 0;

  // To check the data we need to see what kind of data are expected
  // based on where in the data array it's going to go (i.e. value of key)

  Bool_t checking = true;  // should we check?
  Int_t keyo;  // key minus offset
  Int_t adcx;  // ADC number
  Int_t exp_chan;  // Expected ADC channel number
  Int_t exp_dtype; // Expected data type
  Int_t exp_snum;  // Expected sample number
  if (key >= ADCXOFF && key < ADCXOFF + 4 * ADCXNUM)
    {
      // Raw integrated signal
      keyo = key - ADCXOFF;
      adcx = keyo / 4;
      exp_chan = keyo % 4;
      exp_dtype = 0;
    }
  else if (key >= ADCXBSOFF && key < ADCXBSOFF + 4 * 4 * ADCXNUM)
    {
      // Baseline sample
      keyo = key - ADCXBSOFF;
      adcx = keyo / 16;
      exp_chan = (keyo % 16) / 4;
      exp_dtype = 1;
      exp_snum = keyo % 4 + 1;
    }
  else if (key >= ADCXPSOFF && key < ADCXPSOFF + 4 * 4 * ADCXNUM)
    {
      // Peak sample
      keyo = key - ADCXPSOFF;
      adcx = keyo / 16;
      exp_chan = (keyo % 16) / 4;
      exp_dtype = 2;
      exp_snum = keyo % 4 + 1;
    }
  else if (key >= DACXOFF && key < DACXOFF + DACXNUM)
    {
      // DAC value
      keyo = key - DACXOFF;
      adcx = keyo;
      exp_dtype = 4;
    }
  else if (key >= CSRXOFF && key < CSRXOFF + CSRXNUM)
    {
      // CSR value -- nothing to mask or check
      return rawd;
    }
  else
    {
      // Must be a tied device, so we won't do any checking
      checking = false;
    }

  Double_t retval; 

  // Here are checks common to all (or almost all) data types

  if (lprint) {
   // RawDump();
    cout << "Ev num "<<fEvNum<<endl;
    cout << "ADCX  "<<key<<"   "<<(key-ADCXOFF)/4<<endl;
    cout << "Raw Data:  0x"<<hex<<rawd<<"   dvalue = "<<dec<<act_dvalue<<endl;
    cout << "Seen :     type "<<dec<<act_dtype<<"   chan "<<act_chan;
    cout << endl << "sample  "<<act_snum<<endl;
    cout << "Expected : type "<<dec<<exp_dtype<<"   chan "<<exp_chan;
    cout << endl << "sample  "<<exp_snum<<endl;
  }

  if (checking)
    {
      // If this is a header word we're doomed already
      if ((rawd & mask31x) != 0)
	{
          adcxbad |= 0x1;
	  cout << "VaEvent::UnpackAdcx: ERROR -- Header found where data expected for ADCX"
	       << adcx;
	  if (exp_dtype != 4) cout << " channel " << exp_chan;
	  cout << endl;
	  //	  exit(0);
	}
      
      if (exp_dtype != 4)
	{
	  // Check channel number
	  if (act_chan != exp_chan)
	    {
              adcxbad |= 0x2;
	      cout << "VaEvent::UnpackAdcx: ERROR -- Unexpected channel number " 
		   << act_chan << " for ADCX"
		   << adcx << " channel = " << exp_chan << endl;
	      //    exit(0);
	    }
	}

      // Check the data type
      if (act_dtype != exp_dtype )
	{
          adcxbad |= 0x4;
	  cout << " VaEvent::UnpackAdcx:  ERROR -- Unexpected data type, expected " 
	       << exp_dtype << ", got " << act_dtype
	       << " for ADCX" << adcx;
	  if (exp_dtype != 4) cout << " channel " << exp_chan;
	  cout << endl; 
	  //	  exit(0);
	}
    }
  
  // Done with general checking.  Here we check (if needed) and unpack
  // values for types 0, 1 or 2, and 4 separately

  if (act_dtype == 0) // Regular signal data
    {
      if (checking)
	{
	  // Check divider value to see if it changes
	  // act_dvalue = log_2 (number of samples)
	  if (fgDvalue != -1 && fgDvalue != act_dvalue)
	    {
              adcxbad |= 0x8;
	      cout << "VaEvent::UnpackAdcx:  ERROR -- Number of ADCX samples changes during the run from " 
		   << fgDvalue << " to " << act_dvalue 
		   << "  for ADCX" << adcx << " channel " << exp_chan << endl; 
	      //	      exit (0);
	    }
	  fgDvalue = act_dvalue;
	}

      // Now get the value, fix the sign, and divide by 2^act_dvalue
      Int_t iretval = rawd & mask200x;
      if (rawd & mask21x) // Is sign bit set?
	iretval = -((~iretval & 0x1fffff) + 1); // -(2's complement)
      retval = (Double_t) iretval / (1 << act_dvalue);
    }
  else if (act_dtype == 1 || act_dtype == 2) // Baseline or peak sample data
    {
      if (checking)
	{
	  // Check sample number (sample number is not the "number of samples") to see if it is as expected
	  if (act_snum != exp_snum)
	    {
              adcxbad |= 0x10;
	      cout <<  "VaEvent::UnpackAdcx:  ERROR -- unexpected sample value " 
		   << act_snum		
		   << " expected value " << exp_snum << " for ADCX" << adcx << " channel "
		   << exp_chan << endl;
	      //	      exit (0);
	    }
	  if (act_dvalue != 0)
	    {
              adcxbad |= 0x20;
	      cout <<  "VaEvent::UnpackAdcx:  ERROR --  dvalue != 0 for data type 1 or 2 for ADCX"
		   << adcx << " channel " << exp_chan << endl;
	      //	      exit (0);
	    }
	}

       //Now get the value
       retval = rawd & mask170x;
    }
  else if (act_dtype == 4)
    {
      if (act_dvalue != 0)
	{
          adcxbad |= 0x40;
	  cout <<  "VaEvent::UnpackAdcx:  ERROR --  dvalue != 0 for data type 4 for ADCX" << adcx
	       << " channel " << exp_chan << endl;
	  //	  exit (0);
	}
      //Now get the return value
      retval = rawd & mask150x;
    }
  return retval;
}


Double_t 
VaEvent::UnpackVqwk (UInt_t rawd, Int_t key)
{
  // Data from Vqwk are unpacked.  
  // The 6th word for each channel requires care.
  // Since linked words don't support oversampling, 
  // a more fundamental rewrite is necessary to accomodate in-window oversampling

//   /*  Permanent change in the structure of the 6th word of the ADC readout.   
//    *  The upper 16 bits are the number of samples, and the upper 8 of the     
//    *  lower 16 are the sequence number.  This matches the structure of        
//    *  the ADC readout in block read mode, and now also in register read mode. 
//    *  P.King, 2007sep04.      
//    */
//   for (size_t i=0; i<4; i++) fBlock[i] = Double_t(localbuf[i]);
//   fHardwareBlockSum = Double_t(localbuf[4]);
//   fSequenceNumber   = (localbuf[5]>>8)  & 0xFF;
//   fNumberOfSamples  = (localbuf[5]>>16) & 0xFFFF;

  Int_t keyo;  // key minus offset
  Int_t chwd; // word in channel readout
  Long_t lrawd;
//   Int_t vqwk;  // vqwk number
//   Int_t exp_chan;  // Expected ADC channel number
  Double_t retval; 
  lrawd = rawd;
  if (key >= VQWKOFF && key < VQWKOFF + 8 * 7 * VQWKNUM) { // vqwk device
    // Raw integrated signal or possibly number of samples
    keyo = key - VQWKOFF;
    // There are 6 words per channel, 
    // and the 6th word needs special treatment
    // this corresponds to the 1st and 2nd (of 7) entry keys for each vqwk channel
    chwd = keyo % 7;
    //      vqwk = keyo / (6*8);        // might be useful, eventually
    //      exp_chan = keyo/6 - 8*vqwk; // for using oversample blocks
    if (chwd==0) {
      // combined word, number of samples
      Int_t numsamp = (lrawd>>16) & 0xFFFF;
      retval = numsamp;
    } else if (chwd==1) {
      // combined word: sequence number
      retval = (lrawd>>8) & 0xFF;
    } else {
      retval =  Double_t(lrawd);
    }
  } else {  // tied device
    // this should be pointed to integrated data word
    retval =  Double_t(lrawd);
  }
        
  return retval;
}


void
VaEvent::CheckAdcxDacBurp () {
// Purpose: To check if any ADCX base signal had a glitch due to the DAC.

  static Int_t first_time = 1;
  Int_t ldebug = 0;

  if (ldebug) {
    cout << "CheckAdcxDacBurp "<<dec<<fgAdcxDacBurpCut;
    cout << "   evnum "<<GetEvNumber()<<endl;
  }

  if (fgAdcxDacBurpCut <= 0) return;  // do nothing b/c don't care.
  
  Int_t idx;
  Double_t diff;

  for (Int_t key = ADCXBSOFF; key < ADCXBSOFF + 16 * ADCXNUM; key++) {
 
    idx = key-ADCXBSOFF;

    if (!first_time) {
       diff = fData[key] - fPrevAdcxBase[idx];
       if (diff < 0) diff = -1.*diff;
       if (diff > fgAdcxDacBurpCut) {
	  adcxglitch = 1;
       }
       if (ldebug && (fData[key] != 0 || fPrevAdcxBase[idx] != 0) ) {
	 cout << "glitch ? "<<idx << "   "<<fData[key]<<"  "<<fPrevAdcxBase[idx];
         cout << "  "<<diff<<"  "<<fgAdcxDacBurpCut;
         if (diff > fgAdcxDacBurpCut) cout << "   DAC GLITCH ! "<<endl;
	 cout << endl;
       }
    }

    fPrevAdcxBase[idx] = fData[key];

  }

  if (first_time) first_time = 0;

}

void 
VaEvent::CheckAdcxBad() {
// To look explicitly for the "bad" flags for 18-bit ADC.
// This does overlap mostly with the other sources 
// of adcxbad in UnpackAdcx.

  static int ldebug=0;

  if (ldebug) {
    cout << "CheckAdcxBad ";
    cout << "   evnum "<<GetEvNumber()<<endl;
    cout << "crateslist 0x"<<hex<<adcxcrlist<<dec<<endl;
  }

  if (adcxcrlist == 0) return;   // no crates with ADCX

  Int_t nroc=31;
  if (nroc > MAXROC) nroc = MAXROC;

  Int_t bad0, bad1;

  bad0 = 0;  bad1 = 0;

  for (Int_t iroc = 1; iroc<nroc; iroc++) {

    int btst = (1 << iroc);
    if ((adcxcrlist & btst)>>iroc) {  // crate with ADCX in it

      for (Int_t j = fN1roc[iroc]; j < fN1roc[iroc]+fLenroc[iroc]; j++) {
        UInt_t rawd1 = GetRawData(j);
        UInt_t rawd2 = GetRawData(j+9);
        UInt_t rawd3 = GetRawData(j+24);

        if ( (rawd1 == 0xfb0b4000) &&
             (rawd2 == 0xfa180bad) ) bad0 = 1;
        if ( (rawd1 == 0xfb0b4000) &&
             (rawd3 == 0xfa18bad1) ) bad1 = 1;

        if (ldebug) {
             cout << "chk ADCX bad "<<dec<<iroc<<"  rawd ";
             cout << hex << rawd1 << "  "<<rawd2<<"  "<<rawd3<<dec<<endl;
	}

        if (bad0 || bad1) {
          adcxbad |= 0x80;
          if (ldebug) cout << "Found bad flag !! "<<endl;
          if (!ldebug) return;   // for efficiency, return immediately
	}

      }
    }
  }

}

Int_t
VaEvent::CheckPvdisCrates()
{
  Int_t fL,fR,found;

  static Int_t first_time=1;
  static Int_t badL, badR;
  Int_t crate_left = 29;
  Int_t crate_right = 28;
  Int_t badL_tolerance = 20;
  Int_t badR_tolerance = 20;

  fL=0;   fR=0;  found=0;

  static int dodump  = 0;  // dump the raw fEvBuffer
  static int debug   = 0;  // debug the decoding
  static int verbose = 1;  // to warn verbosely

  if (first_time) {
    first_time = 0;
    badL = 0;  badR = 0;
  }

  if (dodump) { // Event dump
    cout << "\n Private check for PVDIS ---- "<<endl<<endl;
    cout << "\n\n Event number " << dec << fEvNum;
    cout << " length " << fEvLen << " type " << fEvType << endl;
    int ipt = 0;
    for (int j = 0; j < (Int_t)(fEvLen/5); j++) {
      cout << dec << "\n evbuffer[" << ipt << "] = ";
      for (int k=j; k<j+5; k++) {
    	 cout << hex << fEvBuffer[ipt++] << " ";
      }
      cout << endl;
    }
    if (ipt < (Int_t)fEvLen) {
        cout << dec << "\n evbuffer[" << ipt << "] = ";
        for (int k=ipt; k<(Int_t)fEvLen; k++) {
	   cout << hex << fEvBuffer[ipt++] << " ";
        }
        cout << endl;
    }
  }

  if (fEvType == 1) {

    for (Int_t iroc = 0; iroc < numroc; iroc++) {
      if(debug) {
        cout << "Roc ptr " <<dec<<iroc;
        cout << "  "<<fIrn[iroc]<<"  "<<fN1roc[fIrn[iroc]];
        cout << "  "<<fLenroc[fIrn[iroc]]<<endl;
      }
      if (fIrn[iroc] == crate_left) fL=1;
      if (fIrn[iroc] == crate_right) fR=1;
    }

    found = 0;
    if ( (fL==1) && (fR==1) ) found=1;

    if (found == 1) {
      if (debug) cout << "\nFOUND the PVDIS crates !\n"<<endl;
    } else {
      if (verbose && ((fEvNum%100)==0)) 
         cout << "WARNING: No PVDIS crates "<<endl<<endl;
      return -1;
    }


  }

  Int_t crate;

  for (Int_t icra=0; icra<2; icra++) {  // loop over PVDIS crates

    crate = crate_left;
    if (icra == 1) crate = crate_right;

    if (debug) cout << "Crate "<<dec<<crate<<endl;

    for (int j = fN1roc[crate]; j < fN1roc[crate]+fLenroc[crate]; j++) {

    if (debug) printf("fEvBuffer[%d] = 0x%x = (dec) %d \n",j,fEvBuffer[j],fEvBuffer[j]);
 
      if ((fEvBuffer[j] & 0xff380100) == 0xff380100) {
          
	if (debug) cout << "Found header = 0x"<<hex<<fEvBuffer[j]<<dec<<endl;

        if ((fEvBuffer[j] & 0x10) != 0) {
          if (crate==crate_left) badL++;
          if (crate==crate_right) badR++;
	}

      }
    }
  }

  if(debug) cout << "Num bad scalers "<<badL<<"   "<<badR<<endl;

  if ( (badL > badL_tolerance) || (badR > badR_tolerance) ) {

    cout << "\n\nERROR :  PVDIS :  Scaler data are out of synch !!"<<endl;
    cout << "  Fatal.  Cannot run this way."<<endl;
    cout << "  Exiting.  Call expert. "<<endl;
    exit(0);

  }


  return 0;

}
