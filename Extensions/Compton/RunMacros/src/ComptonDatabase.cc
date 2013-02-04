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


Int_t ComptonDatabase::StoreLaserCycle(Int_t runId, Int_t sourceID, Int_t cycle,
    Int_t state,Int_t start, Int_t end)
{
  Int_t rc;
  sqlite3_stmt *stmt;
  rc = sqlite3_prepare_v2(fDB,
      Form("INSERT INTO laser_cycle(run_id,source_id,cycle_number,laser_state,"
        "start,end)  values(%d,%d,%d,'%s',%d,%d);",
        runId,sourceID,cycle,(state==0? "off":"on"),start,end),
      -1,&stmt,NULL);
  rc = sqlite3_step(stmt);
  rc = sqlite3_finalize(stmt);
  return sqlite3_last_insert_rowid(fDB);
}

Int_t ComptonDatabase::StoreLaserPattern(Int_t runId, Int_t sourceID,
    Int_t cycle, Int_t leftID, Int_t onID, Int_t rightID )
{
  Int_t rc;
  sqlite3_stmt *stmt;
  rc = sqlite3_prepare_v2(fDB,
      Form("INSERT INTO laser_pattern(run_id,source_id,cycle_number,"
        "laser_off_left_id,laser_on_id,laser_off_right)"
        " values(%d,%d,%d,%d,%d,%d);",
        runId,sourceID,cycle,leftID,onID,rightID),
      -1,&stmt,NULL);
  rc = sqlite3_step(stmt);
  rc = sqlite3_finalize(stmt);
  return sqlite3_last_insert_rowid(fDB);
}


Int_t ComptonDatabase::GetSourceID(TString source)
{
  Int_t rc;
  Int_t result = 0;
  sqlite3_stmt *stmt;
  rc = sqlite3_prepare_v2(fDB,
      Form("SELECT source_id FROM source where title='%s';",source.Data()),
      -1, &stmt, NULL);
  rc = sqlite3_step(stmt);
  if(rc==SQLITE_ROW)
    result = sqlite3_column_int(stmt,0);

  rc = sqlite3_finalize(stmt);

  return result;
}


Int_t ComptonDatabase::CreateAnalysisIfNotExist(Int_t id, TString type)
{
  Int_t rc;
  Int_t result;
  sqlite3_stmt *stmt;
  TString col;
  if(type.CompareTo("cyc") == 0 ) {
    col = "laser_cycle_id";
  } else if ( type.CompareTo("pat") ==0 ) {
    col = "laser_pattern_id";
  } else {
    return 0;
  }
  rc = sqlite3_prepare_v2(fDB,Form("SELECT analysis_id FROM analysis where "
        "%s=%d",col.Data(),id),-1,&stmt,NULL);
  rc = sqlite3_step(stmt);
  if(rc == SQLITE_ROW) {
    result = sqlite3_column_int(stmt,0);
  } else {
    sqlite3_stmt *stmt2;
    rc = sqlite3_prepare_v2(fDB,
        Form("INSERT INTO analysis(%s,type) values(%d,\"%s\");",
          col.Data(),id,type.Data()),-1,&stmt2,NULL);
    sqlite3_step(stmt2);
    result = sqlite3_last_insert_rowid(fDB);
    rc = sqlite3_finalize(stmt2);
  }

  // Clean up
  rc = sqlite3_finalize(stmt);

  return result;
}


std::map<std::string, Int_t> ComptonDatabase::GetIDMapOf(TString table,
    TString match)
{
  std::map<std::string,Int_t> map;
  Int_t rc;
  sqlite3_stmt *stmt;
  Int_t id;
  char *pt = NULL;
  rc = sqlite3_prepare_v2(fDB,Form("SELECT %s_id,%s FROM %s; ",
        table.Data(),match.Data(),table.Data()),-1,&stmt,NULL);
  rc = sqlite3_step(stmt);
  while(rc == SQLITE_ROW) {
    id = sqlite3_column_int(stmt,0);
    pt = (char*)sqlite3_column_text(stmt,1);
    if(pt != NULL ) {
      map[std::string(pt)] = id;
      //m->insert(std::pair<std::string,Int_t>(std::string(pt),id));
    }
    pt = NULL;
    rc = sqlite3_step(stmt);
  }

  // Clean up
  rc = sqlite3_finalize(stmt);

  return map;
}


Int_t ComptonDatabase::StoreBeam(Int_t anaID,Int_t monID,Int_t measID,
    Int_t n,Double_t v,Double_t e)
{
  Int_t rc;
  sqlite3_stmt *stmt;
  rc = sqlite3_prepare_v2(fDB,
      Form("INSERT INTO beam(analysis_id,monitor_id,measurement_type_id,"
        "n,value,error) values(%d,%d,%d,%d,%f,%f);",
        anaID,monID,measID,n,v,e),
      -1,&stmt,NULL);
  rc = sqlite3_step(stmt);
  rc = sqlite3_finalize(stmt);
  return sqlite3_last_insert_rowid(fDB);
}

Int_t ComptonDatabase::StorePhoton(Int_t anaID,Int_t monID,Int_t measID,
    Int_t n,Double_t v,Double_t e)
{
  Int_t rc;
  sqlite3_stmt *stmt;
  rc = sqlite3_prepare_v2(fDB,
      Form("INSERT INTO photon_data(analysis_id,monitor_id,measurement_type_id,"
        "n,value,error) values(%d,%d,%d,%d,%f,%f);",
        anaID,monID,measID,n,v,e),
      -1,&stmt,NULL);
  rc = sqlite3_step(stmt);
  rc = sqlite3_finalize(stmt);
  return sqlite3_last_insert_rowid(fDB);
}

