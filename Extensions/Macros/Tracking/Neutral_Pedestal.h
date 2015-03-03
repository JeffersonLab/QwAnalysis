//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Aug 26 19:55:40 2014 by ROOT version 5.34/05
// from TTree event_tree/QwTracking Event-based Tree
// found on file: Qweak_10545.root
//////////////////////////////////////////////////////////

#ifndef Neutral_Pedestal_h
#define Neutral_Pedestal_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include <TObject.h>
#include <TVector3.h>

// Fixed size dimensions of array or collections stored in the TTree if any.

class Neutral_Pedestal {
public :
   TChain         *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain
  
   // User inputs
   Int_t	   run;
   Int_t	   TS_Octant[2];

   // Declaration of leaf types
 //QwEvent         *events;
   UInt_t          fUniqueID;
   UInt_t          fBits;
   Int_t           fNQwHits;
 //vector<QwHit*>  fQwHits;
   Int_t           fNQwTreeLines;
 //vector<QwTrackingTreeLine*> fQwTreeLines;
   Int_t           fNQwPartialTracks;
 //vector<QwPartialTrack*> fQwPartialTracks;
   Int_t           fNQwTracks;
 //vector<QwTrack*> fQwTracks;
   Double_t        fHydrogenEnergyLoss;
   Double_t        fScatteringAngle;
   Double_t        fScatteringVertexZ;
   Double_t        fScatteringVertexR;
   UInt_t          fVertexPosition_fUniqueID;
   UInt_t          fVertexPosition_fBits;
   Double_t        fVertexPosition_fX;
   Double_t        fVertexPosition_fY;
   Double_t        fVertexPosition_fZ;
   UInt_t          fVertexMomentum_fUniqueID;
   UInt_t          fVertexMomentum_fBits;
   Double_t        fVertexMomentum_fX;
   Double_t        fVertexMomentum_fY;
   Double_t        fVertexMomentum_fZ;
   UInt_t          fKin_fUniqueID;
   UInt_t          fKin_fBits;
   Double_t        fKin_fP0;
   Double_t        fKin_fPp;
   Double_t        fKin_fQ2;
   Double_t        fKin_fW2;
   Double_t        fKin_fNu;
   Double_t        fKin_fX;
   Double_t        fKin_fY;
   UInt_t          fKinWithLoss_fUniqueID;
   UInt_t          fKinWithLoss_fBits;
   Double_t        fKinWithLoss_fP0;
   Double_t        fKinWithLoss_fPp;
   Double_t        fKinWithLoss_fQ2;
   Double_t        fKinWithLoss_fW2;
   Double_t        fKinWithLoss_fNu;
   Double_t        fKinWithLoss_fX;
   Double_t        fKinWithLoss_fY;
   UInt_t          fKinElastic_fUniqueID;
   UInt_t          fKinElastic_fBits;
   Double_t        fKinElastic_fP0;
   Double_t        fKinElastic_fPp;
   Double_t        fKinElastic_fQ2;
   Double_t        fKinElastic_fW2;
   Double_t        fKinElastic_fNu;
   Double_t        fKinElastic_fX;
   Double_t        fKinElastic_fY;
   UInt_t          fKinElasticWithLoss_fUniqueID;
   UInt_t          fKinElasticWithLoss_fBits;
   Double_t        fKinElasticWithLoss_fP0;
   Double_t        fKinElasticWithLoss_fPp;
   Double_t        fKinElasticWithLoss_fQ2;
   Double_t        fKinElasticWithLoss_fW2;
   Double_t        fKinElasticWithLoss_fNu;
   Double_t        fKinElasticWithLoss_fX;
   Double_t        fKinElasticWithLoss_fY;
   Double_t        fPrimaryQ2;
   Double_t        CodaEventNumber;
   Double_t        CodaEventType;
   Double_t        Coda_CleanData;
   Double_t        Coda_ScanData1;
   Double_t        Coda_ScanData2;
   Double_t        scifiber_sf_8b_sca;
   Double_t        scifiber_sf_6b_sca;
   Double_t        scifiber_sf_4b_sca;
   Double_t        scifiber_sf_2a_sca;
   Double_t        scifiber_sf_8a_sca;
   Double_t        scifiber_sf_6a_sca;
   Double_t        scifiber_sf_4a_sca;
   Double_t        scifiber_sf_2b_sca;
   Double_t        trigscint_ts2m_adc;
   Double_t        trigscint_ts1m_adc;
   Double_t        trigscint_ts2p_adc;
   Double_t        trigscint_ts1p_adc;
   Double_t        trigscint_ts2m_f1;
   Double_t        trigscint_ts2p_f1;
   Double_t        trigscint_ts1m_f1;
   Double_t        trigscint_ts1p_f1;
   Double_t        trigscint_ts2mt_f1;
   Double_t        trigscint_ts1mt_f1;
   Double_t        trigscint_ts_reftime_f1;
   Double_t        trigscint_ts2m_sca;
   Double_t        trigscint_ts2p_sca;
   Double_t        trigscint_ts1m_sca;
   Double_t        trigscint_ts1p_sca;
   Double_t        trigscint_GEMTrigOut_Naovoo_sca;
   Double_t        trigscint_GEMTrigOut_Urim_sca;
   Double_t        trigscint_ts2mt_sca;
   Double_t        trigscint_ts1mt_sca;
   Double_t        maindet_md1m_adc;
   Double_t        maindet_md2m_adc;
   Double_t        maindet_md3m_adc;
   Double_t        maindet_md4m_adc;
   Double_t        maindet_md5m_adc;
   Double_t        maindet_md6m_adc;
   Double_t        maindet_md7m_adc;
   Double_t        maindet_md8m_adc;
   Double_t        maindet_md1p_adc;
   Double_t        maindet_md2p_adc;
   Double_t        maindet_md3p_adc;
   Double_t        maindet_md4p_adc;
   Double_t        maindet_md5p_adc;
   Double_t        maindet_md6p_adc;
   Double_t        maindet_md7p_adc;
   Double_t        maindet_md8p_adc;
   Double_t        maindet_md1m_f1;
   Double_t        maindet_md2m_f1;
   Double_t        maindet_md3m_f1;
   Double_t        maindet_md4m_f1;
   Double_t        maindet_md5m_f1;
   Double_t        maindet_md6m_f1;
   Double_t        maindet_md7m_f1;
   Double_t        maindet_md8m_f1;
   Double_t        maindet_md1p_f1;
   Double_t        maindet_md2p_f1;
   Double_t        maindet_md3p_f1;
   Double_t        maindet_md4p_f1;
   Double_t        maindet_md_reftime_f1;
   Double_t        maindet_md5p_f1;
   Double_t        maindet_md6p_f1;
   Double_t        maindet_md7p_f1;
   Double_t        maindet_md8p_f1;
   Double_t        maindet_md1bar_sca;
   Double_t        maindet_md2bar_sca;
   Double_t        maindet_md3bar_sca;
   Double_t        maindet_md4bar_sca;
   Double_t        maindet_md5bar_sca;
   Double_t        maindet_md6bar_sca;
   Double_t        maindet_md7bar_sca;
   Double_t        maindet_md8bar_sca;
   Double_t        maindet_md1m_sca;
   Double_t        maindet_md2m_sca;
   Double_t        maindet_md3m_sca;
   Double_t        maindet_md4m_sca;
   Double_t        maindet_md5m_sca;
   Double_t        maindet_md6m_sca;
   Double_t        maindet_md7m_sca;
   Double_t        maindet_md8m_sca;
   Double_t        maindet_md1p_sca;
   Double_t        maindet_md2p_sca;
   Double_t        maindet_md3p_sca;
   Double_t        maindet_md4p_sca;
   Double_t        maindet_md5p_sca;
   Double_t        maindet_md6p_sca;
   Double_t        maindet_md7p_sca;
   Double_t        maindet_md8p_sca;
   Double_t        scanner_PowSupply_VQWK;
   Double_t        scanner_PositionX_VQWK;
   Double_t        scanner_PositionY_VQWK;
   Double_t        scanner_FrontSCA;
   Double_t        scanner_BackSCA;
   Double_t        scanner_CoincidenceSCA;
   Double_t        scanner_FrontADC;
   Double_t        scanner_BackADC;
   Double_t        scanner_FrontTDC;
   Double_t        scanner_BackTDC;
   Double_t        scanner_PositionX_QDC;
   Double_t        scanner_PositionY_QDC;
   Double_t        scanner_front_adc_raw;
   Double_t        scanner_back__adc_raw;
   Double_t        scanner_pos_x_adc_raw;
   Double_t        scanner_pos_y_adc_raw;
   Double_t        scanner_front_f1_raw;
   Double_t        scanner_back__f1_raw;
   Double_t        scanner_coinc_f1_raw;
   Double_t        scanner_ref_t_f1_raw;
   Double_t        scanner_front_sca_raw;
   Double_t        scanner_back__sca_raw;
   Double_t        scanner_coinc_sca_raw;
   Double_t        scanner_randm_sca_raw;
   Double_t        scanner_bg_chrlt_sca_raw;
   Double_t        scanner_bg_wilbr_sca_raw;
   Double_t        scanner_phase_monitor_raw;
   Double_t        scanner_power_vqwk_raw;
   Double_t        scanner_pos_y_vqwk_raw;
   Double_t        scanner_pos_x_vqwk_raw;
   Double_t        beamline_PositionX_ADC;
   Double_t        beamline_PositionY_ADC;
   Double_t        beamline_bpm_3h07a_pos_x;
   Double_t        beamline_bpm_3h07a_pos_y;
   Double_t        beamline_bpm_3h09b_pos_x;
   Double_t        beamline_bpm_3h09b_pos_y;
   Double_t        beamline_raster_posx_adc_raw;
   Double_t        beamline_raster_posy_adc_raw;
   Double_t        beamline_bpm_3h07a_xp_adc_raw;
   Double_t        beamline_bpm_3h07a_xm_adc_raw;
   Double_t        beamline_bpm_3h07a_yp_adc_raw;
   Double_t        beamline_bpm_3h07a_ym_adc_raw;
   Double_t        beamline_bpm_3h09b_xp_adc_raw;
   Double_t        beamline_bpm_3h09b_xm_adc_raw;
   Double_t        beamline_bpm_3h09b_yp_adc_raw;
   Double_t        beamline_bpm_3h09b_ym_adc_raw;
   Double_t        beamline_bcm6_adc_raw;
   Double_t        beamline_sca_bcm1;
   Double_t        beamline_sca_bcm2;
   Double_t        beamline_sca_bcm17;
   Double_t        beamline_sca_4MHz;
   UInt_t          previous_entry_in_Slow_Tree;
   UInt_t          previous_entry_in_Mps_Tree;

   // List of branches
   TBranch        *b_events_fUniqueID;   //!
   TBranch        *b_events_fBits;   //!
   TBranch        *b_events_fNQwHits;   //!
   TBranch        *b_events_fNQwTreeLines;   //!
   TBranch        *b_events_fNQwPartialTracks;   //!
   TBranch        *b_events_fNQwTracks;   //!
   TBranch        *b_events_fHydrogenEnergyLoss;   //!
   TBranch        *b_events_fScatteringAngle;   //!
   TBranch        *b_events_fScatteringVertexZ;   //!
   TBranch        *b_events_fScatteringVertexR;   //!
   TBranch        *b_events_fVertexPosition_fUniqueID;   //!
   TBranch        *b_events_fVertexPosition_fBits;   //!
   TBranch        *b_events_fVertexPosition_fX;   //!
   TBranch        *b_events_fVertexPosition_fY;   //!
   TBranch        *b_events_fVertexPosition_fZ;   //!
   TBranch        *b_events_fVertexMomentum_fUniqueID;   //!
   TBranch        *b_events_fVertexMomentum_fBits;   //!
   TBranch        *b_events_fVertexMomentum_fX;   //!
   TBranch        *b_events_fVertexMomentum_fY;   //!
   TBranch        *b_events_fVertexMomentum_fZ;   //!
   TBranch        *b_events_fKin_fUniqueID;   //!
   TBranch        *b_events_fKin_fBits;   //!
   TBranch        *b_events_fKin_fP0;   //!
   TBranch        *b_events_fKin_fPp;   //!
   TBranch        *b_events_fKin_fQ2;   //!
   TBranch        *b_events_fKin_fW2;   //!
   TBranch        *b_events_fKin_fNu;   //!
   TBranch        *b_events_fKin_fX;   //!
   TBranch        *b_events_fKin_fY;   //!
   TBranch        *b_events_fKinWithLoss_fUniqueID;   //!
   TBranch        *b_events_fKinWithLoss_fBits;   //!
   TBranch        *b_events_fKinWithLoss_fP0;   //!
   TBranch        *b_events_fKinWithLoss_fPp;   //!
   TBranch        *b_events_fKinWithLoss_fQ2;   //!
   TBranch        *b_events_fKinWithLoss_fW2;   //!
   TBranch        *b_events_fKinWithLoss_fNu;   //!
   TBranch        *b_events_fKinWithLoss_fX;   //!
   TBranch        *b_events_fKinWithLoss_fY;   //!
   TBranch        *b_events_fKinElastic_fUniqueID;   //!
   TBranch        *b_events_fKinElastic_fBits;   //!
   TBranch        *b_events_fKinElastic_fP0;   //!
   TBranch        *b_events_fKinElastic_fPp;   //!
   TBranch        *b_events_fKinElastic_fQ2;   //!
   TBranch        *b_events_fKinElastic_fW2;   //!
   TBranch        *b_events_fKinElastic_fNu;   //!
   TBranch        *b_events_fKinElastic_fX;   //!
   TBranch        *b_events_fKinElastic_fY;   //!
   TBranch        *b_events_fKinElasticWithLoss_fUniqueID;   //!
   TBranch        *b_events_fKinElasticWithLoss_fBits;   //!
   TBranch        *b_events_fKinElasticWithLoss_fP0;   //!
   TBranch        *b_events_fKinElasticWithLoss_fPp;   //!
   TBranch        *b_events_fKinElasticWithLoss_fQ2;   //!
   TBranch        *b_events_fKinElasticWithLoss_fW2;   //!
   TBranch        *b_events_fKinElasticWithLoss_fNu;   //!
   TBranch        *b_events_fKinElasticWithLoss_fX;   //!
   TBranch        *b_events_fKinElasticWithLoss_fY;   //!
   TBranch        *b_events_fPrimaryQ2;   //!
   TBranch        *b_CodaEventNumber;   //!
   TBranch        *b_CodaEventType;   //!
   TBranch        *b_Coda_CleanData;   //!
   TBranch        *b_Coda_ScanData1;   //!
   TBranch        *b_Coda_ScanData2;   //!
   TBranch        *b_scifiber;   //!
   TBranch        *b_trigscint;   //!
   TBranch        *b_maindet;   //!
   TBranch        *b_scanner;   //!
   TBranch        *b_beamline;   //!
   TBranch        *b_previous_entry_in_Slow_Tree;   //!
   TBranch        *b_previous_entry_in_Mps_Tree;   //!

   Neutral_Pedestal(TTree *tree=0);
   Neutral_Pedestal(Int_t Run=13709, Int_t TS1_oct=5, Int_t TS2_oct=1);
   virtual ~Neutral_Pedestal();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TChain *chain);
   virtual void     Loop(Int_t cleanData = 0);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef Neutral_Pedestal_cxx
Neutral_Pedestal::Neutral_Pedestal(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.

   const char* location = "/home/mjmchugh/qweak/rootfiles/tracking_pass3b";
   std::cout << "Enter Run Number:" << std::endl;
   cin >> run;
   std::cout << "Enter TS1 Octant:" << std::endl;
   cin >> TS_Octant[0];
   std::cout << "Enter TS2 Octant:" << std::endl;
   cin >> TS_Octant[1];
 
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->
                 GetListOfFiles()->
                 FindObject(Form("/lustre/expphy/volatile/hallc/qweak/mjmchugh/tracking-pass3b/Qweak_%d.root",run));
      if (!f || !f->IsOpen()) {
         f = new TFile(Form("/lustre/expphy/volatile/hallc/qweak/mjmchugh/tracking-pass3b/Qweak_%d.root",run));
      }
      f->GetObject("event_tree",tree);

   }
   Init(tree);
}

Neutral_Pedestal::Neutral_Pedestal(Int_t Run, Int_t TS1_oct, Int_t TS2_oct) : fChain(0) {

   run = Run;
   TS_Octant[0] = TS1_oct;
   TS_Octant[1] = TS2_oct;

   TChain* chain = new TChain("event_tree");
   chain->Add(Form("/lustre/expphy/volatile/hallc/qweak/mjmchugh/tracking-pass3b/Qweak_%d.root",run));
 
   Init(chain);

}

Neutral_Pedestal::~Neutral_Pedestal()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t Neutral_Pedestal::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t Neutral_Pedestal::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void Neutral_Pedestal::Init(TChain *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("fUniqueID", &fUniqueID, &b_events_fUniqueID);
   fChain->SetBranchAddress("fBits", &fBits, &b_events_fBits);
   fChain->SetBranchAddress("fNQwHits", &fNQwHits, &b_events_fNQwHits);
   fChain->SetBranchAddress("fNQwTreeLines", &fNQwTreeLines, &b_events_fNQwTreeLines);
   fChain->SetBranchAddress("fNQwPartialTracks", &fNQwPartialTracks, &b_events_fNQwPartialTracks);
   fChain->SetBranchAddress("fNQwTracks", &fNQwTracks, &b_events_fNQwTracks);
   fChain->SetBranchAddress("fHydrogenEnergyLoss", &fHydrogenEnergyLoss, &b_events_fHydrogenEnergyLoss);
   fChain->SetBranchAddress("fScatteringAngle", &fScatteringAngle, &b_events_fScatteringAngle);
   fChain->SetBranchAddress("fScatteringVertexZ", &fScatteringVertexZ, &b_events_fScatteringVertexZ);
   fChain->SetBranchAddress("fScatteringVertexR", &fScatteringVertexR, &b_events_fScatteringVertexR);
   fChain->SetBranchAddress("fVertexPosition.fUniqueID", &fVertexPosition_fUniqueID, &b_events_fVertexPosition_fUniqueID);
   fChain->SetBranchAddress("fVertexPosition.fBits", &fVertexPosition_fBits, &b_events_fVertexPosition_fBits);
   fChain->SetBranchAddress("fVertexPosition.fX", &fVertexPosition_fX, &b_events_fVertexPosition_fX);
   fChain->SetBranchAddress("fVertexPosition.fY", &fVertexPosition_fY, &b_events_fVertexPosition_fY);
   fChain->SetBranchAddress("fVertexPosition.fZ", &fVertexPosition_fZ, &b_events_fVertexPosition_fZ);
   fChain->SetBranchAddress("fVertexMomentum.fUniqueID", &fVertexMomentum_fUniqueID, &b_events_fVertexMomentum_fUniqueID);
   fChain->SetBranchAddress("fVertexMomentum.fBits", &fVertexMomentum_fBits, &b_events_fVertexMomentum_fBits);
   fChain->SetBranchAddress("fVertexMomentum.fX", &fVertexMomentum_fX, &b_events_fVertexMomentum_fX);
   fChain->SetBranchAddress("fVertexMomentum.fY", &fVertexMomentum_fY, &b_events_fVertexMomentum_fY);
   fChain->SetBranchAddress("fVertexMomentum.fZ", &fVertexMomentum_fZ, &b_events_fVertexMomentum_fZ);
   fChain->SetBranchAddress("fKin.fUniqueID", &fKin_fUniqueID, &b_events_fKin_fUniqueID);
   fChain->SetBranchAddress("fKin.fBits", &fKin_fBits, &b_events_fKin_fBits);
   fChain->SetBranchAddress("fKin.fP0", &fKin_fP0, &b_events_fKin_fP0);
   fChain->SetBranchAddress("fKin.fPp", &fKin_fPp, &b_events_fKin_fPp);
   fChain->SetBranchAddress("fKin.fQ2", &fKin_fQ2, &b_events_fKin_fQ2);
   fChain->SetBranchAddress("fKin.fW2", &fKin_fW2, &b_events_fKin_fW2);
   fChain->SetBranchAddress("fKin.fNu", &fKin_fNu, &b_events_fKin_fNu);
   fChain->SetBranchAddress("fKin.fX", &fKin_fX, &b_events_fKin_fX);
   fChain->SetBranchAddress("fKin.fY", &fKin_fY, &b_events_fKin_fY);
   fChain->SetBranchAddress("fKinWithLoss.fUniqueID", &fKinWithLoss_fUniqueID, &b_events_fKinWithLoss_fUniqueID);
   fChain->SetBranchAddress("fKinWithLoss.fBits", &fKinWithLoss_fBits, &b_events_fKinWithLoss_fBits);
   fChain->SetBranchAddress("fKinWithLoss.fP0", &fKinWithLoss_fP0, &b_events_fKinWithLoss_fP0);
   fChain->SetBranchAddress("fKinWithLoss.fPp", &fKinWithLoss_fPp, &b_events_fKinWithLoss_fPp);
   fChain->SetBranchAddress("fKinWithLoss.fQ2", &fKinWithLoss_fQ2, &b_events_fKinWithLoss_fQ2);
   fChain->SetBranchAddress("fKinWithLoss.fW2", &fKinWithLoss_fW2, &b_events_fKinWithLoss_fW2);
   fChain->SetBranchAddress("fKinWithLoss.fNu", &fKinWithLoss_fNu, &b_events_fKinWithLoss_fNu);
   fChain->SetBranchAddress("fKinWithLoss.fX", &fKinWithLoss_fX, &b_events_fKinWithLoss_fX);
   fChain->SetBranchAddress("fKinWithLoss.fY", &fKinWithLoss_fY, &b_events_fKinWithLoss_fY);
   fChain->SetBranchAddress("fKinElastic.fUniqueID", &fKinElastic_fUniqueID, &b_events_fKinElastic_fUniqueID);
   fChain->SetBranchAddress("fKinElastic.fBits", &fKinElastic_fBits, &b_events_fKinElastic_fBits);
   fChain->SetBranchAddress("fKinElastic.fP0", &fKinElastic_fP0, &b_events_fKinElastic_fP0);
   fChain->SetBranchAddress("fKinElastic.fPp", &fKinElastic_fPp, &b_events_fKinElastic_fPp);
   fChain->SetBranchAddress("fKinElastic.fQ2", &fKinElastic_fQ2, &b_events_fKinElastic_fQ2);
   fChain->SetBranchAddress("fKinElastic.fW2", &fKinElastic_fW2, &b_events_fKinElastic_fW2);
   fChain->SetBranchAddress("fKinElastic.fNu", &fKinElastic_fNu, &b_events_fKinElastic_fNu);
   fChain->SetBranchAddress("fKinElastic.fX", &fKinElastic_fX, &b_events_fKinElastic_fX);
   fChain->SetBranchAddress("fKinElastic.fY", &fKinElastic_fY, &b_events_fKinElastic_fY);
   fChain->SetBranchAddress("fKinElasticWithLoss.fUniqueID", &fKinElasticWithLoss_fUniqueID, &b_events_fKinElasticWithLoss_fUniqueID);
   fChain->SetBranchAddress("fKinElasticWithLoss.fBits", &fKinElasticWithLoss_fBits, &b_events_fKinElasticWithLoss_fBits);
   fChain->SetBranchAddress("fKinElasticWithLoss.fP0", &fKinElasticWithLoss_fP0, &b_events_fKinElasticWithLoss_fP0);
   fChain->SetBranchAddress("fKinElasticWithLoss.fPp", &fKinElasticWithLoss_fPp, &b_events_fKinElasticWithLoss_fPp);
   fChain->SetBranchAddress("fKinElasticWithLoss.fQ2", &fKinElasticWithLoss_fQ2, &b_events_fKinElasticWithLoss_fQ2);
   fChain->SetBranchAddress("fKinElasticWithLoss.fW2", &fKinElasticWithLoss_fW2, &b_events_fKinElasticWithLoss_fW2);
   fChain->SetBranchAddress("fKinElasticWithLoss.fNu", &fKinElasticWithLoss_fNu, &b_events_fKinElasticWithLoss_fNu);
   fChain->SetBranchAddress("fKinElasticWithLoss.fX", &fKinElasticWithLoss_fX, &b_events_fKinElasticWithLoss_fX);
   fChain->SetBranchAddress("fKinElasticWithLoss.fY", &fKinElasticWithLoss_fY, &b_events_fKinElasticWithLoss_fY);
   fChain->SetBranchAddress("fPrimaryQ2", &fPrimaryQ2, &b_events_fPrimaryQ2);
   fChain->SetBranchAddress("CodaEventNumber", &CodaEventNumber, &b_CodaEventNumber);
   fChain->SetBranchAddress("CodaEventType", &CodaEventType, &b_CodaEventType);
   fChain->SetBranchAddress("Coda_CleanData", &Coda_CleanData, &b_Coda_CleanData);
   fChain->SetBranchAddress("Coda_ScanData1", &Coda_ScanData1, &b_Coda_ScanData1);
   fChain->SetBranchAddress("Coda_ScanData2", &Coda_ScanData2, &b_Coda_ScanData2);
   fChain->SetBranchAddress("scifiber", &scifiber_sf_8b_sca, &b_scifiber);
   fChain->SetBranchAddress("trigscint", &trigscint_ts2m_adc, &b_trigscint);
   fChain->SetBranchAddress("maindet", &maindet_md1m_adc, &b_maindet);
   fChain->SetBranchAddress("scanner", &scanner_PowSupply_VQWK, &b_scanner);
   fChain->SetBranchAddress("beamline", &beamline_PositionX_ADC, &b_beamline);
   fChain->SetBranchAddress("previous_entry_in_Slow_Tree", &previous_entry_in_Slow_Tree, &b_previous_entry_in_Slow_Tree);
   fChain->SetBranchAddress("previous_entry_in_Mps_Tree", &previous_entry_in_Mps_Tree, &b_previous_entry_in_Mps_Tree);
   Notify();
}

Bool_t Neutral_Pedestal::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void Neutral_Pedestal::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t Neutral_Pedestal::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef Neutral_Pedestal_cxx
