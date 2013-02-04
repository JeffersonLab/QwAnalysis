#ifndef VCOMPTONMACRO_H
#define VCOMPTONMACRO_H

#include "ComptonSession.h"
#include <TCanvas.h>
#include <map>

/*
 *
 */
class VComptonMacro {
public:
  virtual void init( ComptonSession *session );
  virtual void run() = 0;

protected:
  // Helper functions
  Int_t runnum() {
    return fSession->RunNumber();
  }

  TChain* chain(Int_t type) {
    return chain((Compton::TREE_TYPE_t)type);
  }
  TChain* chain(Compton::TREE_TYPE_t type) {
    return fSession->GetChain(type);
  }

  Compton::LaserCycles cycles(Int_t type) {
    return cycles((Compton::TREE_TYPE_t)type);
  }

  Compton::LaserCycles cycles(Compton::TREE_TYPE_t type) {
    return fCycles[type];
  }

  TString prefix(Int_t type) {
    return prefix(Compton::TREE_TYPE_t(type));
  }

  TString GetStorageDir() {
    return fSession->StorageDir();
  }

  TString prefix(Compton::TREE_TYPE_t type) {
    switch(type) {
      case Compton::HEL_TREE_TYPE:
        return "yield_";
      case Compton::MPS_TREE_TYPE:
      default:
        return "";
    }
  }

  ComptonSession* session() { return fSession; }

  // Save to the web commands
  void SaveToWeb(TCanvas *canvas, TString comment,TString extension = "png");

  // Database commands
  void DBStoreBeam(Int_t tree, TString monitor,
      TString measure, Int_t cycle, Int_t laser_state,Int_t entries,
      Double_t val, Double_t err);

  void DBStorePhoton(Int_t tree, TString monitor,
      TString measure, Int_t cycle, Int_t laser_state,Int_t entries,
      Double_t val, Double_t err);




  void SetName(TString name) {
    fName = name;
  }

private:
  ComptonSession *fSession;
  TString fName;
  Compton::LaserCycles fCycles[2];

  std::map<std::string,Int_t> fMonitorIDs;
  std::map<std::string,Int_t> fMeasurementIDs;
};

#endif
