///////////////////////////////////////////////////////////////////////////////
//  ComptonDatabase.h
//  Summary:  A utility for interacting with the Compton SQLite3 Database
//
///////////////////////////////////////////////////////////////////////////////

#ifndef COMPTONDATABASE_H
#define COMPTONDATABASE_H

#include <sqlite3.h>
#include <TString.h>
#include <map>

class ComptonDatabase {
public:
  // These are default constructors
  ComptonDatabase(TString filename);
  ~ComptonDatabase();

  // An utility function that returns a map of ids to the matched column
  std::map<std::string,Int_t> GetIDMapOf(TString table,TString match);

  // Add a runnumber to the database
  Int_t AddRun(Int_t run,Int_t n_mps,Int_t n_qrt);
  Int_t StoreLaserCycle(Int_t runId,Int_t sourceId, Int_t cycle, Int_t state,
      Int_t start, Int_t end);
  Int_t StoreLaserPattern(Int_t runId,Int_t sourceId, Int_t cycle,
      Int_t leftID, Int_t onID, Int_t rightID );

  // Check if Runnumbe is already in the database
  Bool_t ExistsRun(Int_t run);

  // Retrieve various ID's
  Int_t GetSourceID(TString source);

  // Clean up
  void Cleanup();

  // Analysis DB functions
  Int_t CreateAnalysisIfNotExist(Int_t id, TString type);


  // Storing values
  Int_t StoreBeam(Int_t anaID,Int_t monID,Int_t measID,Int_t n,Double_t v,
      Double_t e);

  Int_t StorePhoton(Int_t anaID,Int_t monID,Int_t measID,Int_t n,Double_t v,
      Double_t e);

private:
  // Database handler
  sqlite3 *fDB;
};

#endif
