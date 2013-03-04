/** Table of Run Numbers with basic info for each run here **/
create table if not exists runs(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  period INTEGER,
  week INTEGER,
  runnum INTEGER,
  segments INTEGER,
  good_for INTEGER,
  comment TEXT
);

/** This table will hold technical info about the pass **/
create table if not exists passes(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  pass TEXT,
  comment TEXT
);

/** This table holds the details of the pass **/
create table if not exists pass_details(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  run_id INTEGER,
  pass_id INTEGER,
  rootfile_status TEXT,
  macro_status TEXT,
  comment TEXT,
  request_id INTEGER
);
