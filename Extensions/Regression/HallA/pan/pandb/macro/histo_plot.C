void histo_plot(void)
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

  // Get name of column to plot
  Char_t *column = new Char_t[50];
  const Char_t *inputcolumn = "Please enter desired column:  ";
  cout << inputcolumn;
  cin.getline(column, 49);
  cout << column << endl;

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

  // Get histogram ranges
  Char_t *lowrangechar = new Char_t[10];
  const Char_t *lowrangemessage = "Please enter lower limit of "
    "the histogram range:  ";
  cout << lowrangemessage;
  cin.getline(lowrangechar, 9);
  Float_t lowrangefloat = atof(lowrangechar);
  
  Char_t *highrangechar = new Char_t[10];
  const Char_t *highrangemessage = "Please enter upper limit of "
    "the histogram range:  ";
  cout << highrangemessage;
  cin.getline(highrangechar, 9);
  Float_t highrangefloat = atof(highrangechar);
  
  // Create histogram
  TH1F *histo = new TH1F("histo", column, 50, lowrangefloat,
			 highrangefloat);

  // Fill histogram
  for (Int_t i = 0; i<nrows; i++) {
    row = res->Next();
    Float_t field = atof(row->GetField(0));
    delete row;
    histo->Fill(field);
  }

  histo->Draw();

  // stop timer and print results
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();

  printf("\nRealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);

  // Clean up
  delete []sql;
  delete []dbtablename;
  delete []column;
  delete []lowrangechar;
  delete []highrangechar;

  delete res;
  delete db;

}
