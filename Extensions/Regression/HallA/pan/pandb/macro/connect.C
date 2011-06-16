void connect(void)
{
  gSystem->Load("/apps/lib/libz");
  gSystem->Load("libMySQL");

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

  // Print server information
  printf("Server info: %s\n", db->ServerInfo());

  // Clean up

  delete db;
  db = NULL;
}
