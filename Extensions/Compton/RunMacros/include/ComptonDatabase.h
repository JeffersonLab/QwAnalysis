///////////////////////////////////////////////////////////////////////////////
//  ComptonDatabase.h
//  Summary:  A utility for interacting with the Compton SQLite3 Database
//
///////////////////////////////////////////////////////////////////////////////

#ifndef COMPTONDATABASE_H
#define COMPTONDATABASE_H

#include <sqlite3.h>
#include <TString.h>

class ComptonDatabase {
public:
  // These are default constructors
  ComptonDatabase(TString filename);
  ~ComptonDatabase();

  // Add a runnumber to the database
  Int_t AddRun(Int_t run,Int_t n_mps,Int_t n_qrt);
  Int_t StoreLaserCycle(Int_t runId, Int_t cycle, Int_t mps_start,
      Int_t mps_end, Int_t hel_start, Int_t hel_end);

  // Check if Runnumbe is already in the database
  Bool_t ExistsRun(Int_t run);

  // Clean up
  void Cleanup();

private:
  // Database handler
  sqlite3 *fDB;
};

#endif
