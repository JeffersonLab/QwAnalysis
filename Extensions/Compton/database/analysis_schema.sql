create table if not exists pass_info (
  name TEXT,
  start_time DATETIME,
  end_time DATETIME,
  comment TEXT
);

create table if not exists good_for (
  good_for_id INTEGER PRIMARY KEY ASC,
  type TEXT
);

create table if not exists run_quality (
  run_quality_id INTEGER PRIMARY KEY ASC,
  type TEXT
);

create table if not exists source (
  source_id INTEGER PRIMARY KEY ASC,
  title TEXT
);

create table if not exists run (
  run_id INTEGER PRIMARY KEY ASC,
  run_number INT UNSIGNED,
  slug INT UNSIGNED,
  run_type TEXT,
  good_for_id INT,
  run_quality_id INT,
  start_time DATETIME,
  end_time DATETIME,
  n_mps INT UNSIGNED,
  n_qrt INT UNSIGNED,
  comment TEXT
);

create table if not exists laser_cycle (
  laser_cycle_id INTEGER PRIMARY KEY ASC,
  run_id INT UNSIGNED,
  source_id INT UNSIGNED,
  cycle_number INT UNSIGNED,
  laser_state char(3),
  start INT UNSIGNED,
  end INT UNSIGNED
);

create table if not exists laser_pattern (
  laser_pattern_id INTEGER PRIMARY KEY ASC,
  run_id INT UNSIGNED,
  source_id INT UNSIGNED,
  cycle_number INT UNSIGNED,
  laser_off_left_id  INT UNSIGNED,
  laser_on_id INT UNSIGNED,
  laser_off_right INT UNSIGNED
);

create table if not exists slow_control_detector (
  slow_control_detector_id INTEGER PRIMARY KEY ASC,
  name TEXT,
  units TEXT,
  comment TEXT
);

create table if not exists slow_controls_settings (
  slow_controls_settings_id INTEGER PRIMARY KEY ASC,
  run_id INT UNSIGNED,
  ihwp_1 char(3),
  ihwp_2 char(3),
  laser_pol_direction char(5),
  wien_v float,
  wien_h float,
  wien_phi float
);

create table if not exists slow_controls_data (
  slow_controls_data_id INTEGER PRIMARY KEY ASC,
  slow_controls_detector_id INT UNSIGNED,
  laser_cycle_id INT UNSIGNED,
  value float,
  error float
);

create table if not exists analysis (
  analysis_id INTEGER PRIMARY KEY ASC,
  laser_cycle_id INT UNSIGNED,
  laser_pattern_id INT UNSIGNED,
  type char(3)
);

create table if not exists measurement_type (
  measurement_type_id INTEGER PRIMARY KEY ASC,
  units TEXT,
  title TEXT
);

create table if not exists electron_data (
  electron_data_id INTEGER PRIMARY KEY ASC,
  analysis_id INT UNSIGNED,
  measurement_type_id INT,
  plane UNSIGNED INT,
  strip UNSIGNED INT,
  n UNSIGNED INT,
  value float,
  error float
);

create table if not exists photon_data (
  photon_data_id INTEGER PRIMARY KEY ASC,
  analysis_id INT UNSIGNED,
  monitor_id INT UNSIGNED,
  measurement_type_id INT,
  n INT UNSIGNED,
  value float,
  error float
);

create table if not exists beam (
  beam_id INTEGER PRIMARY KEY ASC,
  analysis_id INT UNSIGNED,
  monitor_id INT UNSIGNED,
  measurement_type_id INT,
  n INT UNSIGNED,
  value float,
  error float
);

create table if not exists monitor (
  monitor_id INTEGER PRIMARY KEY ASC,
  title TEXT
);

create table if not exists slug (
  slug_id INTEGER PRIMARY KEY ASC,
  ihwp1 char(3),
  ihwp2 char(3),
  laser char(3),
  wien char(3)
  number INTEGER
);
