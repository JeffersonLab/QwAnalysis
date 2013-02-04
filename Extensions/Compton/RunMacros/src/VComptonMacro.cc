#include "VComptonMacro.h"
#include "ComptonDatabase.h"
#include <iostream>

void VComptonMacro::init( ComptonSession *session )
{
  fSession = session;
  fCycles[0] = session->GetLaserCycles(Compton::MPS_TREE_TYPE);
  fCycles[1] = session->GetLaserCycles(Compton::HEL_TREE_TYPE);

  // Get maps
  fMonitorIDs = session->db()->GetIDMapOf("monitor","title");
  fMeasurementIDs = session->db()->GetIDMapOf("measurement_type","title");
}

void VComptonMacro::SaveToWeb(TCanvas *canvas, TString comment,
    TString extension )
{
  TString tmp = comment.IsNull() ? "" : TString("_") + comment;
  canvas->SaveAs(fSession->WebDir() + Form("/%s_%d%s.%s",
        fName.Data(),runnum(),tmp.Data(),extension.Data()));
}

void VComptonMacro::DBStorePhoton(Int_t tree, TString monitor, TString measure,
   Int_t cycle, Int_t laser_state, Int_t entries,
   Double_t val, Double_t err)
{
  Int_t source_id;
  switch(tree) {
    case 0:
      source_id = fSession->MpsID();
      break;
    case 1:
    default:
      source_id = fSession->HelID();
      break;
  }
  Int_t analysis_id;
  Int_t laser_id;
  TString tmp;
  switch(laser_state) {
    case 0:
      laser_id = fCycles[tree].off_id[cycle];
      tmp = "cyc";
      break;
    case 1:
      laser_id = fCycles[tree].on_id[cycle];
      tmp = "cyc";
      break;
    case 2:
      laser_id = fCycles[tree].pat_id[cycle];
      tmp = "pat";
      break;
    default:
      return;
  }
  analysis_id = session()->db()->CreateAnalysisIfNotExist(laser_id,tmp);

  Int_t monitor_id = fMonitorIDs[monitor.Data()];
  Int_t measurement_id = fMeasurementIDs[measure.Data()];
  session()->db()->StorePhoton(analysis_id,monitor_id,measurement_id,entries,
      val,err);

}

void VComptonMacro::DBStoreBeam(Int_t tree, TString monitor, TString measure,
   Int_t cycle, Int_t laser_state, Int_t entries,
   Double_t val, Double_t err)
{
  Int_t source_id;
  switch(tree) {
    case 0:
      source_id = fSession->MpsID();
      break;
    case 1:
    default:
      source_id = fSession->HelID();
      break;
  }
  Int_t analysis_id;
  Int_t laser_id;
  TString tmp;
  switch(laser_state) {
    case 0:
      laser_id = fCycles[tree].off_id[cycle];
      tmp = "cyc";
      break;
    case 1:
      laser_id = fCycles[tree].on_id[cycle];
      tmp = "cyc";
      break;
    case 2:
      laser_id = fCycles[tree].pat_id[cycle];
      tmp = "pat";
      break;
    default:
      return;
  }
  analysis_id = session()->db()->CreateAnalysisIfNotExist(laser_id,tmp);

  Int_t monitor_id = fMonitorIDs[monitor.Data()];
  Int_t measurement_id = fMeasurementIDs[measure.Data()];
  session()->db()->StoreBeam(analysis_id,monitor_id,measurement_id,entries,
      val,err);
}
