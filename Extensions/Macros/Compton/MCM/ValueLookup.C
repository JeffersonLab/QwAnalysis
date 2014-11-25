#include <fstream>
#include <map>
#include <iostream>
#include <TChain.h>
#include <TString.h>
#include "comptonRunConstants.h"

bool gDipoleCurrentsRead = false;
std::map<UInt_t,Double_t> gDipoleCurrents;

Double_t LookupDipoleCurrent(UInt_t hash, bool &found)
{
  if(!gDipoleCurrentsRead) {
    ifstream in("compton_lookup_dipole.dat",std::ios::in);
    if(in.is_open()) {
      Double_t tmpD;
      UInt_t tmpI;
      while( in >> tmpI &&
          in >> tmpD &&
          !in.eof() ) {
        gDipoleCurrents[tmpI] = tmpD;
      }
      in.close();
      std::cout << "Loaded new lookup table with " << gDipoleCurrents.size()
        << " lookup hashes found." << std::endl;
      gDipoleCurrentsRead = true;
    } else {
      std::cerr << "Could not open file compton_lookup_dipole.dat" << std::endl;
    }
  }

  std::map<UInt_t,Double_t>::iterator it = gDipoleCurrents.find(hash);
  if(it != gDipoleCurrents.end() ) {
    found = true;
    return it->second;
  }

  found = false;
  return -1.0;
}

void GenerateDipoleCurrentLookup()
{
  Double_t startVal = 90.0;
  Double_t endVal = 110.0;
  Double_t step = 0.001;
  ofstream outFile("compton_lookup_dipole.dat");
  for(Double_t val = startVal; val <= endVal; val+=step) {
    TString s(Form("%g",val));
    outFile << s.Hash() << " " << s.Data() << std::endl;
  }
}

void ValueLookup(Int_t runnum, Double_t &dipoleI, Double_t &dipoleIRMS, Double_t &dipoleIEr)
{
  TH1D *hCheck = new TH1D("hCheck","dummy",100,-0.0,0.1);//typical value of maximum beam current
  hCheck->SetBit(TH1::kCanRebin);

  TChain *chain = new TChain("Slow_Tree");
  std::cout << "Found " << chain->Add(Form("$QW_ROOTFILES/Compton_Pass2b_%d.*.root",runnum)) << " files for run " << runnum;
  Int_t num_entries = chain->GetEntries();
  std::cout << " with " << num_entries << " of Slow_Tree entries." << std::endl;

  Double_t branch_value;
  bool found;
  Int_t totalFound = 0;
  Double_t value;
  UInt_t hash;
  //std::cout << "Testing Dipole Current lookup" << std::endl;
  chain->ResetBranchAddresses();
  chain->SetBranchAddress("MCP3P01M",&branch_value);
  for(Int_t entry = 0; entry < num_entries; entry++ ) {
    chain->GetEntry(entry);
    hash = branch_value;
    value = LookupDipoleCurrent(hash,found);
    if(found) {
      //std::cout << "Found hash: " << hash << ", look up value: " <<value << std::endl;
      hCheck->Fill(value);
      totalFound++;
    } else {
      std::cerr << "WARNING!!! Hash not found! : " << hash << std::endl;
    }
  }
  if (totalFound==num_entries) {
    std::cout << "Success! Found " << totalFound << " hashes out of " << num_entries << std::endl;
  } else {
    std::cerr << "ERROR!!!! Missing " << num_entries-totalFound <<" hashes out of " << num_entries << std::endl;
  }

  dipoleI = hCheck->GetMean();
  dipoleIRMS = hCheck->GetRMS();
  dipoleIEr = hCheck->GetMeanError();
  B_dipole = (52.335*dipoleI + 65.660)*1e-4;///Tesla
  B_dipole = B_dipole*0.9986*(124.58/125.0);///correction to field pointed out in Monte Carlo
  if(B_dipole<0.5 || B_dipole>0.6) {
    cout<<red<<"\nresulting dipole field does not make sense hence using default\n"<<normal<<endl;
    B_dipole = B_dipoleDefault;///using the default set value
  }
  cout<<magenta<<"trim coil BDL=-1000Gauss-cm during MCM runs"<<normal<<endl;
  B_dipole = (B_dipole*1e4*125 - 1000)/125.0;///tril coil correction pointed out in https://qweak.jlab.org/elog/Chicane/8 
  B_dipole = B_dipole * 1e-4;///converting back to Tesla
  cout<<blue<<"Dipole filed (T): "<<B_dipole<<normal<<endl;
  ofstream fBeamProp;
  TString file = Form("%s/%s/%sdipoleI.txt",pPath, txt,filePre.Data());
  fBeamProp.open(file);
  if(fBeamProp.is_open()) {
    fBeamProp<<"runnum\tdipoleI\tdipoleIEr\tdipoleIRMS"<<endl;
    fBeamProp<<runnum<<"\t"<<dipoleI<<"\t"<<dipoleIEr<<"\t"<<dipoleIRMS<<endl;
    fBeamProp.close();
    cout<<"wrote dipole info to "<<file<<endl;
  } else cout<<red<<"could not open "<<file<<normal<<endl;

}
