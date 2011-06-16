void column_average(void)
{

  // Store constants for connecting to database (fill in blanks as
  // appropriate)
  const Char_t *dbusername = "dbmanager";  // username for DB access
  const Char_t *dbpasswd = "parity";    // password for DB access
  const Char_t *dbname = "pandb";      // name of DB
  const Char_t *dbhostname = "alquds.jlab.org";  // hostname of DB server computer

  // Connect to MySQL server
  Char_t *dburl = new Char_t[50];
  sprintf(dburl,"mysql://%s/%s", dbhostname, dbname);
  TSQLServer *db = TSQLServer::Connect(dburl, dbusername, dbpasswd);
  delete []dburl;
  dburl=NULL;

  // Get name of dbtablename 
  Char_t * dbtablename= new Char_t[50];
  const Char_t *inputtable = "Please enter desired table:  ";
  cout << inputtable;
  cin.getline(dbtablename, 49);
  cout << dbtablename << endl;

  // Get name of column to average over
  Char_t *column = new Char_t[50];
  const Char_t *inputcolumn = "Please enter desired column:  ";
  cout << inputcolumn;
  cin.getline(column, 49);
  cout << column << endl;

//  // Get selection criteria
//  Char_t *query = new Char_t[4096];
//  const Char_t *inputmessage = "Please enter desired selection "
//    "criteria:  ";
//  cout << inputmessage;
//  cin.getline(query, 4095);
//  cout << query << endl;

  // Construct query
  Char_t *sql = new Char_t[4200];
  sprintf(sql, "SELECT %s FROM %s", column, dbtablename);
  cout << sql << endl;

  // start timer
  TStopwatch timer;
  timer.Start();

  // Submit query to server
  TSQLRow *row;
  TSQLResult *res;
  res = db->Query(sql);

  // Process results
  Int_t nrows = res->GetRowCount();
  cout << "Got " << nrows << " rows in result." << endl;

  Int_t nfields = res->GetFieldCount();
  cout << "Got " << nfields << " fields in result." << endl;
  if (nfields != 1) {
    cerr << "Select only one column to average over!" << endl;
    exit;
  }

  Float_t average = 0;

  for (Int_t i = 0; i < nrows; i++) {
    row = res->Next();
    //    average += row->GetField(0);
    Float_t wtf = atof(row->GetField(0));
    //    cout << wtf << endl;
    average += wtf;
    //    cout << row->GetField(0) << " : " << average << endl;
    delete row;
  }

  delete res;

  average /= nrows;

  cout << "The average value of column " << column << " is ";
  cout << average << endl;

  // stop timer and print results
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();

  printf("\nRealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);

  // Clean up
  delete []sql;
  delete []column;
  delete []dbtablename;

  delete db;
  db = NULL;
}
