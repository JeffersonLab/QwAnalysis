#include "ComptonDatabase.h"
#include <iostream>

ComptonDatabase::ComptonDatabase(TString filename)
{
  Int_t rc;
  rc = sqlite3_open_v2(filename.Data(),&fDB,
      SQLITE_OPEN_READWRITE // Defaults to read only if we can't write to it
      //|SQLITE_OPEN_FULLMUTEX // Make it thread safe!
      ,NULL);
}


ComptonDatabase::~ComptonDatabase()
{
}

void ComptonDatabase::Cleanup()
{
  // Let's close up the database
  std::cout << "Closing DB: " << sqlite3_close(fDB) << std::endl;
}



Int_t ComptonDatabase::AddRun(Int_t run,Int_t n_mps, Int_t n_qrt)
{
  Int_t rc;
  sqlite3_stmt *stmt;
  rc = sqlite3_prepare_v2(fDB,
      Form("INSERT INTO run(run_number,n_mps,n_qrt) values(%d,%d,%d);",
        run,n_mps,n_qrt),
      -1,&stmt,NULL);
  rc = sqlite3_step(stmt);
  rc = sqlite3_finalize(stmt);
  return sqlite3_last_insert_rowid(fDB);
}

Bool_t ComptonDatabase::ExistsRun(Int_t run)
{
}


Int_t ComptonDatabase::StoreLaserCycle(Int_t runId, Int_t cycle, Int_t mps_start,
    Int_t mps_end, Int_t hel_start, Int_t hel_end)
{
  Int_t rc;
  sqlite3_stmt *stmt;
  rc = sqlite3_prepare_v2(fDB,
      Form("INSERT INTO laser_cycle(run_id,cycle_number,start_mps,end_mps,"
        "start_hel,end_hel) values(%d,%d,%d,%d,%d,%d);",
        runId,cycle,mps_start,mps_end,hel_start,hel_end),
      -1,&stmt,NULL);
  rc = sqlite3_step(stmt);
  rc = sqlite3_finalize(stmt);
  return sqlite3_last_insert_rowid(fDB);
}
