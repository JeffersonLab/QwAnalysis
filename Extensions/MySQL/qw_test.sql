-- MySQL dump 10.11
--
-- Host: localhost    Database: qw_test
-- ------------------------------------------------------
-- Server version	5.0.77

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `analysis`
--

DROP TABLE IF EXISTS `analysis`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `analysis` (
  `analysis_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `run_id` int(10) unsigned NOT NULL COMMENT 'run that was played or replayed',
  `seed_id` int(10) unsigned NOT NULL COMMENT 'ID of seed used for blinding factor calculation',
  `monitor_calibration_id` int(10) unsigned NOT NULL COMMENT 'specifies set of calibrations used to replay the run',
  `cut_id` int(10) unsigned NOT NULL COMMENT 'specifies set of cuts used to replay this run',
  `time` datetime default NULL COMMENT 'time run was played or replayed',
  `bf_checksum` text COMMENT 'blinding factor checksum (NULL if blinding factor calculation not performed)',
  `beam_mode` enum('nbm','cp') NOT NULL COMMENT 'beam motion used to calculate slopes, natural or pulsed',
  `n_mps` int(10) unsigned NOT NULL default '0' COMMENT 'number of good MPSs',
  `n_qrt` int(10) unsigned NOT NULL default '0' COMMENT 'number of good quartets',
  `first_event` int(10) unsigned default NULL COMMENT 'first event replayed',
  `last_event` int(10) unsigned default NULL COMMENT 'last event replayed',
  `segment` int(11) default NULL COMMENT 'run segment replayed (if specified on command line)',
  `slope_calculation` enum('on','off') default NULL COMMENT 'calculation of yield slopes enabled (on/off)',
  `slope_correction` enum('on','off') default NULL COMMENT 'correction of yield slopes enabled (on/off)',
  PRIMARY KEY  (`analysis_id`),
  KEY `fk_analysis_run_id` (`run_id`),
  KEY `fk_analysis_seed_id` (`seed_id`),
  KEY `fk_analysis_monitor_calibration_id` (`monitor_calibration_id`),
  KEY `fk_analysis_cut_id` (`cut_id`),
  CONSTRAINT `fk_analysis_cut_id` FOREIGN KEY (`cut_id`) REFERENCES `cut` (`cut_id`),
  CONSTRAINT `fk_analysis_monitor_calibration_id` FOREIGN KEY (`monitor_calibration_id`) REFERENCES `monitor_calibration` (`monitor_calibration_id`),
  CONSTRAINT `fk_analysis_run_id` FOREIGN KEY (`run_id`) REFERENCES `run` (`run_id`),
  CONSTRAINT `fk_analysis_seed_id` FOREIGN KEY (`seed_id`) REFERENCES `seed` (`seed_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `analysis`
--

LOCK TABLES `analysis` WRITE;
/*!40000 ALTER TABLE `analysis` DISABLE KEYS */;
/*!40000 ALTER TABLE `analysis` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `beam`
--

DROP TABLE IF EXISTS `beam`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `beam` (
  `beam_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `analysis_id` int(10) unsigned NOT NULL COMMENT 'run that was replayed',
  `monitor_id` int(10) unsigned NOT NULL COMMENT 'monitor for this measurement',
  `measurement_type_id` char(4) NOT NULL COMMENT 'type of measurement (yield, asymmetry, etc.)',
  `value` float NOT NULL default '0' COMMENT 'value of measurement',
  `error` float NOT NULL default '0' COMMENT 'error on measurement',
  PRIMARY KEY  (`beam_id`),
  KEY `fk_beam_analysis_id` (`analysis_id`),
  KEY `fk_beam_monitor_id` (`monitor_id`),
  KEY `fk_beam_measurement_type_id` (`measurement_type_id`),
  CONSTRAINT `fk_beam_analysis_id` FOREIGN KEY (`analysis_id`) REFERENCES `analysis` (`analysis_id`),
  CONSTRAINT `fk_beam_measurement_type_id` FOREIGN KEY (`measurement_type_id`) REFERENCES `measurement_type` (`measurement_type_id`),
  CONSTRAINT `fk_beam_monitor_id` FOREIGN KEY (`monitor_id`) REFERENCES `monitor` (`monitor_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `beam`
--

LOCK TABLES `beam` WRITE;
/*!40000 ALTER TABLE `beam` DISABLE KEYS */;
/*!40000 ALTER TABLE `beam` ENABLE KEYS */;
UNLOCK TABLES;

/*!50003 SET @SAVE_SQL_MODE=@@SQL_MODE*/;

DELIMITER ;;
/*!50003 SET SESSION SQL_MODE="" */;;
/*!50003 CREATE */ /*!50017 DEFINER=`qwadmin`@`localhost` */ /*!50003 TRIGGER `summary_qtarg` AFTER INSERT ON `beam` FOR EACH ROW BEGIN

UPDATE summary_by s, monitor m, analysis a, run r
SET s.q_targ_value = NEW.value,
s.q_targ_error = NEW.error
WHERE s.run_number = r.run_number
AND NEW.analysis_id = a.analysis_id
AND a.run_id = r.run_id
AND m.quantity = 'q_targ'
AND NEW.measurement_type_id = 'yq'
AND a.slope_calculation = 'on'
AND NEW.monitor_id = m.monitor_id;

UPDATE summary_by s, monitor m, analysis a, run r
SET s.x_targ_value = NEW.value,
s.x_targ_error = NEW.error
WHERE s.run_number = r.run_number
AND NEW.analysis_id = a.analysis_id
AND a.run_id = r.run_id
AND m.quantity = 'x_targ'
AND NEW.measurement_type_id = 'yp'
AND a.slope_calculation = 'on'
AND NEW.monitor_id = m.monitor_id;

UPDATE summary_by s, monitor m, analysis a, run r
SET s.y_targ_value = NEW.value,
s.y_targ_error = NEW.error
WHERE s.run_number = r.run_number
AND NEW.analysis_id = a.analysis_id
AND a.run_id = r.run_id
AND m.quantity = 'y_targ'
AND NEW.measurement_type_id = 'yp'
AND a.slope_calculation = 'on'
AND NEW.monitor_id = m.monitor_id;

UPDATE summary_by s, monitor m, analysis a, run r
SET s.theta_x_value = NEW.value,
s.theta_x_error = NEW.error
WHERE s.run_number = r.run_number
AND NEW.analysis_id = a.analysis_id
AND a.run_id = r.run_id
AND m.quantity = 'theta_x'
AND NEW.measurement_type_id = 'ya'
AND a.slope_calculation = 'on'
AND NEW.monitor_id = m.monitor_id;

UPDATE summary_by s, monitor m, analysis a, run r
SET s.theta_y_value = NEW.value,
s.theta_y_error = NEW.error
WHERE s.run_number = r.run_number
AND NEW.analysis_id = a.analysis_id
AND a.run_id = r.run_id
AND m.quantity = 'theta_y'
AND NEW.measurement_type_id = 'ya'
AND a.slope_calculation = 'on'
AND NEW.monitor_id = m.monitor_id;

UPDATE summary_by s, monitor m, analysis a, run r
SET s.energy_value = NEW.value,
s.energy_error = NEW.error
WHERE s.run_number = r.run_number
AND NEW.analysis_id = a.analysis_id
AND a.run_id = r.run_id
AND m.quantity = 'energy'
AND NEW.measurement_type_id = 'ye'
AND a.slope_calculation = 'on'
AND NEW.monitor_id = m.monitor_id;

UPDATE summary_ba s, monitor m, analysis a, run r
SET s.q_targ_value = NEW.value,
s.q_targ_error = NEW.error
WHERE s.run_number = r.run_number
AND NEW.analysis_id = a.analysis_id
AND a.run_id = r.run_id
AND m.quantity = 'q_targ'
AND NEW.measurement_type_id = 'aq'
AND a.slope_calculation = 'on'
AND NEW.monitor_id = m.monitor_id;

UPDATE summary_ba s, monitor m, analysis a, run r
SET s.x_targ_value = NEW.value,
s.x_targ_error = NEW.error
WHERE s.run_number = r.run_number
AND NEW.analysis_id = a.analysis_id
AND a.run_id = r.run_id
AND m.quantity = 'x_targ'
AND NEW.measurement_type_id = 'dp'
AND a.slope_calculation = 'on'
AND NEW.monitor_id = m.monitor_id;

UPDATE summary_ba s, monitor m, analysis a, run r
SET s.y_targ_value = NEW.value,
s.y_targ_error = NEW.error
WHERE s.run_number = r.run_number
AND NEW.analysis_id = a.analysis_id
AND a.run_id = r.run_id
AND m.quantity = 'y_targ'
AND NEW.measurement_type_id = 'dp'
AND a.slope_calculation = 'on'
AND NEW.monitor_id = m.monitor_id;

UPDATE summary_ba s, monitor m, analysis a, run r
SET s.theta_x_value = NEW.value,
s.theta_x_error = NEW.error
WHERE s.run_number = r.run_number
AND NEW.analysis_id = a.analysis_id
AND a.run_id = r.run_id
AND m.quantity = 'theta_x'
AND NEW.measurement_type_id = 'da'
AND a.slope_calculation = 'on'
AND NEW.monitor_id = m.monitor_id;

UPDATE summary_ba s, monitor m, analysis a, run r
SET s.theta_y_value = NEW.value,
s.theta_y_error = NEW.error
WHERE s.run_number = r.run_number
AND NEW.analysis_id = a.analysis_id
AND a.run_id = r.run_id
AND m.quantity = 'theta_y'
AND NEW.measurement_type_id = 'da'
AND a.slope_calculation = 'on'
AND NEW.monitor_id = m.monitor_id;

UPDATE summary_ba s, monitor m, analysis a, run r
SET s.energy_value = NEW.value,
s.energy_error = NEW.error
WHERE s.run_number = r.run_number
AND NEW.analysis_id = a.analysis_id
AND a.run_id = r.run_id
AND m.quantity = 'energy'
AND NEW.measurement_type_id = 'de'
AND a.slope_calculation = 'on'
AND NEW.monitor_id = m.monitor_id;


END */;;

DELIMITER ;
/*!50003 SET SESSION SQL_MODE=@SAVE_SQL_MODE*/;

--
-- Table structure for table `bf_test`
--

DROP TABLE IF EXISTS `bf_test`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `bf_test` (
  `bf_test_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `analysis_id` int(10) unsigned NOT NULL COMMENT 'run that was replayed',
  `test_number` int(10) unsigned default NULL COMMENT 'test value identifier',
  `test_value` float default NULL COMMENT 'test value',
  PRIMARY KEY  (`bf_test_id`),
  KEY `fk_bf_test_analysis_id` (`analysis_id`),
  CONSTRAINT `fk_bf_test_analysis_id` FOREIGN KEY (`analysis_id`) REFERENCES `analysis` (`analysis_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `bf_test`
--

LOCK TABLES `bf_test` WRITE;
/*!40000 ALTER TABLE `bf_test` DISABLE KEYS */;
/*!40000 ALTER TABLE `bf_test` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cerenkov`
--

DROP TABLE IF EXISTS `cerenkov`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `cerenkov` (
  `cerenkov_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `analysis_id` int(10) unsigned NOT NULL COMMENT 'run that was replayed',
  `measurement_type_id` char(4) NOT NULL COMMENT 'type of measurement (yield, asymmetry, etc.)',
  `detector_id` int(10) unsigned default NULL COMMENT 'from which detector data was received',
  `value` float NOT NULL default '0' COMMENT 'value of measurement',
  `error` float NOT NULL default '0' COMMENT 'error on measurement',
  `n_qrt` float NOT NULL default '0' COMMENT 'number of good quartets',
  PRIMARY KEY  (`cerenkov_id`),
  KEY `fk_cerenkov_analysis_id` (`analysis_id`),
  KEY `fk_cerenkov_measurement_type_id` (`measurement_type_id`),
  CONSTRAINT `fk_cerenkov_analysis_id` FOREIGN KEY (`analysis_id`) REFERENCES `analysis` (`analysis_id`),
  CONSTRAINT `fk_cerenkov_measurement_type_id` FOREIGN KEY (`measurement_type_id`) REFERENCES `measurement_type` (`measurement_type_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `cerenkov`
--

LOCK TABLES `cerenkov` WRITE;
/*!40000 ALTER TABLE `cerenkov` DISABLE KEYS */;
/*!40000 ALTER TABLE `cerenkov` ENABLE KEYS */;
UNLOCK TABLES;

/*!50003 SET @SAVE_SQL_MODE=@@SQL_MODE*/;

DELIMITER ;;
/*!50003 SET SESSION SQL_MODE="" */;;
/*!50003 CREATE */ /*!50017 DEFINER=`qwadmin`@`localhost` */ /*!50003 TRIGGER `summary_detector` AFTER INSERT ON `cerenkov` FOR EACH ROW BEGIN

UPDATE summary_dy_calc s, analysis a, run r
SET s.value = NEW.value,
s.error = NEW.error
WHERE s.run_number = r.run_number
AND r.run_id = a.run_id
AND s.detector_id = NEW.detector_id
AND NEW.analysis_id = a.analysis_id
AND NEW.measurement_type_id = 'y'
AND a.slope_calculation = 'on';


UPDATE summary_dy_correct s, analysis a, run r
SET s.value = NEW.value,
s.error = NEW.error
WHERE s.run_number = r.run_number
AND r.run_id = a.run_id
AND s.detector_id = NEW.detector_id
AND NEW.analysis_id = a.analysis_id
AND NEW.measurement_type_id = 'y'
AND a.slope_correction = 'on';


UPDATE summary_da_calc s, analysis a, run r
SET s.value = NEW.value,
s.error = NEW.error
WHERE s.run_number = r.run_number
AND r.run_id = a.run_id
AND s.detector_id = NEW.detector_id
AND NEW.analysis_id = a.analysis_id
AND NEW.measurement_type_id = 'a'
AND a.slope_calculation = 'on';


UPDATE summary_da_correct s, analysis a, run r
SET s.value = NEW.value,
s.error = NEW.error
WHERE s.run_number = r.run_number
AND r.run_id = a.run_id
AND s.detector_id = NEW.detector_id
AND NEW.analysis_id = a.analysis_id
AND NEW.measurement_type_id = 'a'
AND a.slope_correction = 'on';

END */;;

DELIMITER ;
/*!50003 SET SESSION SQL_MODE=@SAVE_SQL_MODE*/;

--
-- Table structure for table `compton_data`
--

DROP TABLE IF EXISTS `compton_data`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `compton_data` (
  `compton_data_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `compton_run_id` int(10) unsigned NOT NULL COMMENT 'compton run id for these measurements',
  `measurement_type_id` char(4) NOT NULL COMMENT 'type of measurement (yield, asymmetry, etc.)',
  `particle_type` enum('electron','photon','coincidence') default NULL COMMENT 'type of particle (electron, photon, coincidence)',
  `value` float NOT NULL default '0' COMMENT 'value of measurement',
  `error` float NOT NULL default '0' COMMENT 'error on measurement',
  PRIMARY KEY  (`compton_data_id`),
  KEY `fk_compton_data_compton_run_id` (`compton_run_id`),
  KEY `fk_compton_data_measurment_type_id` (`measurement_type_id`),
  CONSTRAINT `fk_compton_data_compton_run_id` FOREIGN KEY (`compton_run_id`) REFERENCES `compton_run` (`compton_run_id`),
  CONSTRAINT `fk_compton_data_measurment_type_id` FOREIGN KEY (`measurement_type_id`) REFERENCES `measurement_type` (`measurement_type_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `compton_data`
--

LOCK TABLES `compton_data` WRITE;
/*!40000 ALTER TABLE `compton_data` DISABLE KEYS */;
/*!40000 ALTER TABLE `compton_data` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `compton_run`
--

DROP TABLE IF EXISTS `compton_run`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `compton_run` (
  `compton_run_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `run_number` int(10) unsigned default NULL COMMENT 'number of run from CODA',
  `start_time` datetime default NULL COMMENT 'time run began',
  `end_time` datetime default NULL COMMENT 'time run ended',
  `n_mps` int(10) unsigned NOT NULL default '0' COMMENT 'number of MPSs in run',
  `n_qrt` int(10) unsigned NOT NULL default '0' COMMENT 'number of quartets in run',
  PRIMARY KEY  (`compton_run_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `compton_run`
--

LOCK TABLES `compton_run` WRITE;
/*!40000 ALTER TABLE `compton_run` DISABLE KEYS */;
/*!40000 ALTER TABLE `compton_run` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cut`
--

DROP TABLE IF EXISTS `cut`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `cut` (
  `cut_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `start_run_id` int(10) unsigned NOT NULL COMMENT 'run number where cut begins',
  `end_run_id` int(10) unsigned NOT NULL COMMENT 'run number where cut ends',
  `time` datetime default NULL COMMENT 'time cuts were inserted into database',
  `comment` text COMMENT 'user comment',
  PRIMARY KEY  (`cut_id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `cut`
--

LOCK TABLES `cut` WRITE;
/*!40000 ALTER TABLE `cut` DISABLE KEYS */;
INSERT INTO `cut` VALUES (1,1,6600,'2010-02-02 15:39:04','I am a lonely cut');
/*!40000 ALTER TABLE `cut` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cut_data`
--

DROP TABLE IF EXISTS `cut_data`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `cut_data` (
  `cut_data_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `cut_id` int(10) unsigned NOT NULL COMMENT 'set of cuts that this cut belongs to',
  `monitor_id` int(10) unsigned NOT NULL COMMENT 'quantity to cut on',
  `min` float NOT NULL default '0' COMMENT 'minimum value of cut',
  `max` float NOT NULL default '0' COMMENT 'maximum value of cut',
  PRIMARY KEY  (`cut_data_id`),
  KEY `fk_cut_data_cut_id` (`cut_id`),
  KEY `fk_cut_data_monitor_id` (`monitor_id`),
  CONSTRAINT `fk_cut_data_cut_id` FOREIGN KEY (`cut_id`) REFERENCES `cut` (`cut_id`),
  CONSTRAINT `fk_cut_data_monitor_id` FOREIGN KEY (`monitor_id`) REFERENCES `monitor` (`monitor_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `cut_data`
--

LOCK TABLES `cut_data` WRITE;
/*!40000 ALTER TABLE `cut_data` DISABLE KEYS */;
/*!40000 ALTER TABLE `cut_data` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `high_voltage`
--

DROP TABLE IF EXISTS `high_voltage`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `high_voltage` (
  `high_voltage_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `run_id` int(10) unsigned NOT NULL COMMENT 'run for these measurements',
  `high_voltage_file_id` int(10) unsigned NOT NULL COMMENT 'HV file used for this run',
  PRIMARY KEY  (`high_voltage_id`),
  KEY `fk_high_voltage_run_id` (`run_id`),
  KEY `fk_high_voltage_high_voltage_file_id` (`high_voltage_file_id`),
  CONSTRAINT `fk_high_voltage_high_voltage_file_id` FOREIGN KEY (`high_voltage_file_id`) REFERENCES `high_voltage_file` (`high_voltage_file_id`),
  CONSTRAINT `fk_high_voltage_run_id` FOREIGN KEY (`run_id`) REFERENCES `run` (`run_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `high_voltage`
--

LOCK TABLES `high_voltage` WRITE;
/*!40000 ALTER TABLE `high_voltage` DISABLE KEYS */;
/*!40000 ALTER TABLE `high_voltage` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `high_voltage_file`
--

DROP TABLE IF EXISTS `high_voltage_file`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `high_voltage_file` (
  `high_voltage_file_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `name` text COMMENT 'name of high voltage file',
  PRIMARY KEY  (`high_voltage_file_id`)
) ENGINE=InnoDB AUTO_INCREMENT=22 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `high_voltage_file`
--

LOCK TABLES `high_voltage_file` WRITE;
/*!40000 ALTER TABLE `high_voltage_file` DISABLE KEYS */;
INSERT INTO `high_voltage_file` VALUES (1,'high_voltage_0'),(2,'high_voltage_1'),(3,'high_voltage_2'),(4,'high_voltage_3'),(5,'high_voltage_4'),(6,'high_voltage_5'),(7,'high_voltage_6'),(8,'high_voltage_7'),(9,'high_voltage_8'),(10,'high_voltage_9'),(11,'high_voltage_10'),(12,'high_voltage_11'),(13,'high_voltage_12'),(14,'high_voltage_13'),(15,'high_voltage_14'),(16,'high_voltage_15'),(17,'high_voltage_16'),(18,'high_voltage_17'),(19,'high_voltage_18'),(20,'high_voltage_19'),(21,'high_voltage_20');
/*!40000 ALTER TABLE `high_voltage_file` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `high_voltage_reading`
--

DROP TABLE IF EXISTS `high_voltage_reading`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `high_voltage_reading` (
  `high_voltage_reading_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `high_voltage_id` int(10) unsigned NOT NULL COMMENT 'run for these measurements',
  `pmt_id` int(10) unsigned NOT NULL COMMENT 'cerenkov pmt_id',
  `value` float NOT NULL default '0' COMMENT 'high voltage value',
  `error` float NOT NULL default '0' COMMENT 'error on high voltage value measurement',
  PRIMARY KEY  (`high_voltage_reading_id`),
  KEY `fk_high_voltage_reading_high_voltage_id` (`high_voltage_id`),
  KEY `fk_high_voltage_reading_pmt_id` (`pmt_id`),
  CONSTRAINT `fk_high_voltage_reading_high_voltage_id` FOREIGN KEY (`high_voltage_id`) REFERENCES `high_voltage` (`high_voltage_id`),
  CONSTRAINT `fk_high_voltage_reading_pmt_id` FOREIGN KEY (`pmt_id`) REFERENCES `pmt` (`pmt_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `high_voltage_reading`
--

LOCK TABLES `high_voltage_reading` WRITE;
/*!40000 ALTER TABLE `high_voltage_reading` DISABLE KEYS */;
/*!40000 ALTER TABLE `high_voltage_reading` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `high_voltage_setting`
--

DROP TABLE IF EXISTS `high_voltage_setting`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `high_voltage_setting` (
  `high_voltage_setting_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `high_voltage_file_id` int(10) unsigned NOT NULL COMMENT 'name of high voltage file',
  `pmt_id` int(10) unsigned NOT NULL COMMENT 'cerenkov pmt id',
  `setting` float NOT NULL default '0' COMMENT 'high voltage setting',
  `gain` float NOT NULL default '0' COMMENT 'gain value',
  PRIMARY KEY  (`high_voltage_setting_id`),
  KEY `fk_high_voltage_setting_high_voltage_file_id` (`high_voltage_file_id`),
  KEY `fk_high_voltage_setting_pmt_id` (`pmt_id`),
  CONSTRAINT `fk_high_voltage_setting_high_voltage_file_id` FOREIGN KEY (`high_voltage_file_id`) REFERENCES `high_voltage_file` (`high_voltage_file_id`),
  CONSTRAINT `fk_high_voltage_setting_pmt_id` FOREIGN KEY (`pmt_id`) REFERENCES `pmt` (`pmt_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `high_voltage_setting`
--

LOCK TABLES `high_voltage_setting` WRITE;
/*!40000 ALTER TABLE `high_voltage_setting` DISABLE KEYS */;
/*!40000 ALTER TABLE `high_voltage_setting` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `measurement_type`
--

DROP TABLE IF EXISTS `measurement_type`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `measurement_type` (
  `measurement_type_id` char(4) NOT NULL COMMENT 'primary key for table and gives type of measurement (yield, asymmetry, etc.)',
  `units` text NOT NULL COMMENT 'units of measurement (?, ppm)',
  `title` text NOT NULL COMMENT 'axis title (detector yield, detector asymmetry)',
  PRIMARY KEY  (`measurement_type_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `measurement_type`
--

LOCK TABLES `measurement_type` WRITE;
/*!40000 ALTER TABLE `measurement_type` DISABLE KEYS */;
INSERT INTO `measurement_type` VALUES ('a','ppb','Detector Asymmetry'),('abd','ppm','Asymmetry from Differential Buddy (FR)'),('ah','ppm','Halo Asymmetry'),('al','ppm','Lumi Asymmetry'),('aq','ppm','Beam Charge Asymmetry'),('bc','counts/nC','Buddy Current (NA)'),('bd','counts/nC','Differential Buddy (FR)'),('bg','counts/nC','Global Buddy (FR)'),('bp','counts/nC','Buddy Previous (NA)'),('da','urad','Beam Angle Difference'),('dclk','counts','4 MHz Clock Difference'),('de','keV','Beam Energy Difference'),('dp','um','Beam Position Difference'),('p','percent','Beam Polarization'),('r','channel','Raw Signal'),('y','counts/nC','Detector Yield'),('ya','mrad','Beam Angle'),('yclk','counts','4 MHz Clock Yield'),('ye','MeV','Beam Energy'),('yh1','channel','Halo Yield'),('yhp','counts','Halo Yield'),('yl','channel/nC','Lumi Yield'),('yp','mm','Beam Position'),('yq','nC','Beam Charge');
/*!40000 ALTER TABLE `measurement_type` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `monitor`
--

DROP TABLE IF EXISTS `monitor`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `monitor` (
  `monitor_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `quantity` text NOT NULL COMMENT 'name of beam property',
  `title` text NOT NULL COMMENT 'axis title',
  PRIMARY KEY  (`monitor_id`)
) ENGINE=InnoDB AUTO_INCREMENT=126 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `monitor`
--

LOCK TABLES `monitor` WRITE;
/*!40000 ALTER TABLE `monitor` DISABLE KEYS */;
INSERT INTO `monitor` VALUES (1,'qwk_qpdRelX','BPM QWK_QPD Relative X'),(2,'qwk_qpdRelY','BPM QWK_QPD Relative Y'),(3,'qwk_qpdX','BPM QWK_QPD Absolute X'),(4,'qwk_qpdY','BPM QWK_QPD Absolute Y'),(5,'qwk_qpdZ','BPM QWK_QPD Wire Sum'),(6,'empty1','BCM empty1'),(7,'empty2','BCM empty2'),(8,'qwk_batext1','BCM QWK_Batext1'),(9,'qwk_batext2','BCM QWK_Batext2'),(10,'qwk_1i02RelX','BPM QWK_1I02 Relative X'),(11,'qwk_1i02RelY','BPM QWK_1I02 Relative Y'),(12,'qwk_1i02X','BPM QWK_1I02 Absolute X'),(13,'qwk_1i02Y','BPM QWK_1I02 Absolute Y'),(14,'qwk_1i02Z','BPM QWK_1I02 Wire Sum'),(15,'qwk_1i04RelX','BPM QWK_1I04 Relative X'),(16,'qwk_1i04RelY','BPM QWK_1I04 Relative Y'),(17,'qwk_1i04X','BPM QWK_1I04 Absolute X'),(18,'qwk_1i04Y','BPM QWK_1I04 Absolute Y'),(19,'qwk_1i04Z','BPM QWK_1I04 Wire Sum'),(20,'qwk_1i06RelX','BPM QWK_1I06 Relative X'),(21,'qwk_1i06RelY','BPM QWK_1I06 Relative Y'),(22,'qwk_1i06X','BPM QWK_1I06 Absolute X'),(23,'qwk_1i06Y','BPM QWK_1I06 Absolute Y'),(24,'qwk_1i06Z','BPM QWK_1I06 Wire Sum'),(25,'qwk_0i02RelX','BPM QWK_0I02 Relative X'),(26,'qwk_0i02RelY','BPM QWK_0I02 Relative Y'),(27,'qwk_0i02X','BPM QWK_0I02 Absolute X'),(28,'qwk_0i02Y','BPM QWK_0I02 Absolute Y'),(29,'qwk_0i02Z','BPM QWK_0I02 Wire Sum'),(30,'qwk_0i02aRelX','BPM QWK_0I02A Relative X'),(31,'qwk_0i02aRelY','BPM QWK_0I02A Relative Y'),(32,'qwk_0i02aX','BPM QWK_0I02A Absolute X'),(33,'qwk_0i02aY','BPM QWK_0I02A Absolute Y'),(34,'qwk_0i02aZ','BPM QWK_0I02A Wire Sum'),(35,'qwk_0i05RelX','BPM QWK_0I05 Relative X'),(36,'qwk_0i05RelY','BPM QWK_0I05 Relative Y'),(37,'qwk_0i05X','BPM QWK_0I05 Absolute X'),(38,'qwk_0i05Y','BPM QWK_0I05 Absolute Y'),(39,'qwk_0i05Z','BPM QWK_0I05 Wire Sum'),(40,'qwk_0i07RelX','BPM QWK_0I07 Relative X'),(41,'qwk_0i07RelY','BPM QWK_0I07 Relative Y'),(42,'qwk_0i07X','BPM QWK_0I07 Absolute X'),(43,'qwk_0i07Y','BPM QWK_0I07 Absolute Y'),(44,'qwk_0i07Z','BPM QWK_0I07 Wire Sum'),(45,'qwk_0l01RelX','BPM QWK_0L01 Relative X'),(46,'qwk_0l01RelY','BPM QWK_0L01 Relative Y'),(47,'qwk_0l01X','BPM QWK_0L01 Absolute X'),(48,'qwk_0l01Y','BPM QWK_0L01 Absolute Y'),(49,'qwk_0l01Z','BPM QWK_0L01 Wire Sum'),(50,'qwk_0l02RelX','BPM QWK_0L02 Relative X'),(51,'qwk_0l02RelY','BPM QWK_0L02 Relative Y'),(52,'qwk_0l02X','BPM QWK_0L02 Absolute X'),(53,'qwk_0l02Y','BPM QWK_0L02 Absolute Y'),(54,'qwk_0l02Z','BPM QWK_0L02 Wire Sum'),(55,'qwk_0l03RelX','BPM QWK_0L03 Relative X'),(56,'qwk_0l03RelY','BPM QWK_0L03 Relative Y'),(57,'qwk_0l03X','BPM QWK_0L03 Absolute X'),(58,'qwk_0l03Y','BPM QWK_0L03 Absolute Y'),(59,'qwk_0l03Z','BPM QWK_0L03 Wire Sum'),(60,'qwk_0l04RelX','BPM QWK_0L04 Relative X'),(61,'qwk_0l04RelY','BPM QWK_0L04 Relative Y'),(62,'qwk_0l04X','BPM QWK_0L04 Absolute X'),(63,'qwk_0l04Y','BPM QWK_0L04 Absolute Y'),(64,'qwk_0l04Z','BPM QWK_0L04 Wire Sum'),(65,'qwk_0l05RelX','BPM QWK_0L05 Relative X'),(66,'qwk_0l05RelY','BPM QWK_0L05 Relative Y'),(67,'qwk_0l05X','BPM QWK_0L05 Absolute X'),(68,'qwk_0l05Y','BPM QWK_0L05 Absolute Y'),(69,'qwk_0l05Z','BPM QWK_0L05 Wire Sum'),(70,'qwk_0l06RelX','BPM QWK_0L06 Relative X'),(71,'qwk_0l06RelY','BPM QWK_0L06 Relative Y'),(72,'qwk_0l06X','BPM QWK_0L06 Absolute X'),(73,'qwk_0l06Y','BPM QWK_0L06 Absolute Y'),(74,'qwk_0l06Z','BPM QWK_0L06 Wire Sum'),(75,'qwk_0l07RelX','BPM QWK_0L07 Relative X'),(76,'qwk_0l07RelY','BPM QWK_0L07 Relative Y'),(77,'qwk_0l07X','BPM QWK_0L07 Absolute X'),(78,'qwk_0l07Y','BPM QWK_0L07 Absolute Y'),(79,'qwk_0l07Z','BPM QWK_0L07 Wire Sum'),(80,'qwk_0l08RelX','BPM QWK_0L08 Relative X'),(81,'qwk_0l08RelY','BPM QWK_0L08 Relative Y'),(82,'qwk_0l08X','BPM QWK_0L08 Absolute X'),(83,'qwk_0l08Y','BPM QWK_0L08 Absolute Y'),(84,'qwk_0l08Z','BPM QWK_0L08 Wire Sum'),(85,'qwk_0l09RelX','BPM QWK_0L09 Relative X'),(86,'qwk_0l09RelY','BPM QWK_0L09 Relative Y'),(87,'qwk_0l09X','BPM QWK_0L09 Absolute X'),(88,'qwk_0l09Y','BPM QWK_0L09 Absolute Y'),(89,'qwk_0l09Z','BPM QWK_0L09 Wire Sum'),(90,'qwk_0l10RelX','BPM QWK_0L10 Relative X'),(91,'qwk_0l10RelY','BPM QWK_0L10 Relative Y'),(92,'qwk_0l10X','BPM QWK_0L10 Absolute X'),(93,'qwk_0l10Y','BPM QWK_0L10 Absolute Y'),(94,'qwk_0l10Z','BPM QWK_0L10 Wire Sum'),(95,'qwk_0r01RelX','BPM QWK_0R01 Relative X'),(96,'qwk_0r01RelY','BPM QWK_0R01 Relative Y'),(97,'qwk_0r01X','BPM QWK_0R01 Absolute X'),(98,'qwk_0r01Y','BPM QWK_0R01 Absolute Y'),(99,'qwk_0r01Z','BPM QWK_0R01 Wire Sum'),(100,'qwk_0r02RelX','BPM QWK_0R02 Relative X'),(101,'qwk_0r02RelY','BPM QWK_0R02 Relative Y'),(102,'qwk_0r02X','BPM QWK_0R02 Absolute X'),(103,'qwk_0r02Y','BPM QWK_0R02 Absolute Y'),(104,'qwk_0r02Z','BPM QWK_0R02 Wire Sum'),(105,'qwk_0r05RelX','BPM QWK_0R05 Relative X'),(106,'qwk_0r05RelY','BPM QWK_0R05 Relative Y'),(107,'qwk_0r05X','BPM QWK_0R05 Absolute X'),(108,'qwk_0r05Y','BPM QWK_0R05 Absolute Y'),(109,'qwk_0r05Z','BPM QWK_0R05 Wire Sum'),(110,'qwk_0r06RelX','BPM QWK_0R06 Relative X'),(111,'qwk_0r06RelY','BPM QWK_0R06 Relative Y'),(112,'qwk_0r06X','BPM QWK_0R06 Absolute X'),(113,'qwk_0r06Y','BPM QWK_0R06 Absolute Y'),(114,'qwk_0r06Z','BPM QWK_0R06 Wire Sum'),(115,'qwk_bcm0l02','BCM QWK_BCM0L02'),(116,'qwk_batery6','BCM QWK_Batery6'),(117,'qwk_batery7','BCM QWK_Batery7'),(118,'phasemonitor','BCM PhaseMonitor'),(119,'qwk_target_charge','Combined BCM qwk_target_charge'),(120,'qwk_average_charge','Combined BCM qwk_average_charge'),(121,'qwk_target_positionRelX','Combined BPM qwk_target_position Relative X'),(122,'qwk_target_positionRelY','Combined BPM qwk_target_position Relative Y'),(123,'qwk_target_positionX','Combined BPM qwk_target_position Absolute X'),(124,'qwk_target_positionY','Combined BPM qwk_target_position Absolute Y'),(125,'qwk_target_positionZ','Combined BPM qwk_target_position Wire Sum');
/*!40000 ALTER TABLE `monitor` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `monitor_calibration`
--

DROP TABLE IF EXISTS `monitor_calibration`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `monitor_calibration` (
  `monitor_calibration_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `first_run_id` int(10) unsigned NOT NULL COMMENT 'first run of range for which this calibration is valid',
  `last_run_id` int(10) unsigned NOT NULL COMMENT 'last run of range for which this calibration is valid',
  `time` datetime default NULL COMMENT 'time this calibration set was put into the database',
  `comment` text COMMENT 'user comment',
  PRIMARY KEY  (`monitor_calibration_id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `monitor_calibration`
--

LOCK TABLES `monitor_calibration` WRITE;
/*!40000 ALTER TABLE `monitor_calibration` DISABLE KEYS */;
INSERT INTO `monitor_calibration` VALUES (1,0,0,NULL,'test');
/*!40000 ALTER TABLE `monitor_calibration` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `monitor_calibration_data`
--

DROP TABLE IF EXISTS `monitor_calibration_data`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `monitor_calibration_data` (
  `monitor_calibration_data_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `monitor_calibration_id` int(10) unsigned NOT NULL COMMENT 'set to which this calibration belongs',
  `monitor_id` int(10) unsigned NOT NULL COMMENT 'monitor to which this calibration belongs',
  `gain` float NOT NULL default '0' COMMENT 'gain of monitor',
  `offset` float NOT NULL default '0' COMMENT 'offset of monitor',
  PRIMARY KEY  (`monitor_calibration_data_id`),
  KEY `fk_mcd_monitor_calibration_id` (`monitor_calibration_id`),
  KEY `fk_mcd_monitor_id` (`monitor_id`),
  CONSTRAINT `fk_mcd_monitor_calibration_id` FOREIGN KEY (`monitor_calibration_id`) REFERENCES `monitor_calibration` (`monitor_calibration_id`),
  CONSTRAINT `fk_mcd_monitor_id` FOREIGN KEY (`monitor_id`) REFERENCES `monitor` (`monitor_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `monitor_calibration_data`
--

LOCK TABLES `monitor_calibration_data` WRITE;
/*!40000 ALTER TABLE `monitor_calibration_data` DISABLE KEYS */;
/*!40000 ALTER TABLE `monitor_calibration_data` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `pmt`
--

DROP TABLE IF EXISTS `pmt`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `pmt` (
  `pmt_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table/cerenkov PMT ID',
  `detector_id` int(10) unsigned NOT NULL COMMENT 'from which detector data was received',
  `position` enum('l','r') default NULL COMMENT 'left or right',
  PRIMARY KEY  (`pmt_id`)
) ENGINE=InnoDB AUTO_INCREMENT=17 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `pmt`
--

LOCK TABLES `pmt` WRITE;
/*!40000 ALTER TABLE `pmt` DISABLE KEYS */;
INSERT INTO `pmt` VALUES (1,1,'l'),(2,1,'r'),(3,2,'l'),(4,2,'r'),(5,3,'l'),(6,3,'r'),(7,4,'l'),(8,4,'r'),(9,5,'l'),(10,5,'r'),(11,6,'l'),(12,6,'r'),(13,7,'l'),(14,7,'r'),(15,8,'l'),(16,8,'r');
/*!40000 ALTER TABLE `pmt` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `polarized_source`
--

DROP TABLE IF EXISTS `polarized_source`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `polarized_source` (
  `polarized_source_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `run_id` int(10) unsigned NOT NULL COMMENT 'run this data comes from',
  `slow_helicity_plate` enum('in','out') default NULL COMMENT 'state of slow helicity reversal half-wave plate',
  `charge_feedback` enum('on','off') default NULL COMMENT 'charge feedback state',
  `sign_in_neg` smallint(6) NOT NULL default '1' COMMENT 'arbitrary sign to apply such that the measured asymmetry when the half wave plate is IN is negative.',
  PRIMARY KEY  (`polarized_source_id`),
  KEY `fk_polarized_source_run_id` (`run_id`),
  CONSTRAINT `fk_polarized_source_run_id` FOREIGN KEY (`run_id`) REFERENCES `run` (`run_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `polarized_source`
--

LOCK TABLES `polarized_source` WRITE;
/*!40000 ALTER TABLE `polarized_source` DISABLE KEYS */;
/*!40000 ALTER TABLE `polarized_source` ENABLE KEYS */;
UNLOCK TABLES;

/*!50003 SET @SAVE_SQL_MODE=@@SQL_MODE*/;

DELIMITER ;;
/*!50003 SET SESSION SQL_MODE="" */;;
/*!50003 CREATE */ /*!50017 DEFINER=`qwadmin`@`localhost` */ /*!50003 TRIGGER `summary_helicity` AFTER INSERT ON `polarized_source` FOR EACH ROW BEGIN

UPDATE summary_by s, run r
SET s.slow_helicity_plate = NEW.slow_helicity_plate
WHERE s.run_number = r.run_number
AND NEW.run_id = r.run_id;

UPDATE summary_ba s, run r
SET s.slow_helicity_plate = NEW.slow_helicity_plate
WHERE s.run_number = r.run_number
AND NEW.run_id = r.run_id;

UPDATE summary_dy_calc s, run r
SET s.slow_helicity_plate = NEW.slow_helicity_plate
WHERE s.run_number = r.run_number
AND NEW.run_id = r.run_id;

UPDATE summary_dy_correct s, run r
SET s.slow_helicity_plate = NEW.slow_helicity_plate
WHERE s.run_number = r.run_number
AND NEW.run_id = r.run_id;

UPDATE summary_da_calc s, run r
SET s.slow_helicity_plate = NEW.slow_helicity_plate
WHERE s.run_number = r.run_number
AND NEW.run_id = r.run_id;

UPDATE summary_da_correct s, run r
SET s.slow_helicity_plate = NEW.slow_helicity_plate
WHERE s.run_number = r.run_number
AND NEW.run_id = r.run_id;

END */;;

DELIMITER ;
/*!50003 SET SESSION SQL_MODE=@SAVE_SQL_MODE*/;

--
-- Table structure for table `polarized_source_measurement`
--

DROP TABLE IF EXISTS `polarized_source_measurement`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `polarized_source_measurement` (
  `polarized_source_measurement_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `polarized_source_id` int(10) unsigned NOT NULL COMMENT 'run for this set of measurements',
  `polarized_source_monitor_id` int(10) unsigned NOT NULL COMMENT 'monitor for this measurement',
  `average_value` float NOT NULL default '0' COMMENT 'average value of measurement',
  `error` float NOT NULL default '0' COMMENT 'error on average',
  `min_value` float NOT NULL default '0' COMMENT 'minimum value of measurement',
  `max_value` float NOT NULL default '0' COMMENT 'maximum value of measurement',
  PRIMARY KEY  (`polarized_source_measurement_id`),
  KEY `fk_polarized_source_measurement_polarized_source_id` (`polarized_source_id`),
  KEY `fk_polarized_source_measurement_polarized_source_monitor_id` (`polarized_source_monitor_id`),
  CONSTRAINT `fk_polarized_source_measurement_polarized_source_id` FOREIGN KEY (`polarized_source_id`) REFERENCES `polarized_source` (`polarized_source_id`),
  CONSTRAINT `fk_polarized_source_measurement_polarized_source_monitor_id` FOREIGN KEY (`polarized_source_monitor_id`) REFERENCES `polarized_source_monitor` (`polarized_source_monitor_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `polarized_source_measurement`
--

LOCK TABLES `polarized_source_measurement` WRITE;
/*!40000 ALTER TABLE `polarized_source_measurement` DISABLE KEYS */;
/*!40000 ALTER TABLE `polarized_source_measurement` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `polarized_source_monitor`
--

DROP TABLE IF EXISTS `polarized_source_monitor`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `polarized_source_monitor` (
  `polarized_source_monitor_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `quantity` text COMMENT 'name of monitor',
  `units` text COMMENT 'measurement units',
  `title` text COMMENT 'graph axis title',
  PRIMARY KEY  (`polarized_source_monitor_id`)
) ENGINE=InnoDB AUTO_INCREMENT=108 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `polarized_source_monitor`
--

LOCK TABLES `polarized_source_monitor` WRITE;
/*!40000 ALTER TABLE `polarized_source_monitor` DISABLE KEYS */;
INSERT INTO `polarized_source_monitor` VALUES (1,'HC:qint_period','','Charge asymmetry sampling period'),(2,'laser_a_wavelength','nm','Hall A laser wavelength'),(3,'HC:0L03_WSUM','','0L03 Charge Asymmetry 4 Wire Sum'),(4,'psub_cx_pos','','X laser position on cathode'),(5,'SMRPOSB','','Hall B Chopper Slit Readback'),(6,'R00PGSET','','Prebuncher Setting'),(7,'IGLdac2:scale_6.A','V','Hall B TACO Voltage'),(8,'IGL1I00AI27','V','Hall A IA Voltage'),(9,'laser_c_wavelength','nm','Hall C laser wavelength'),(10,'HC:1I04_XDIF','','1I04 X Difference'),(11,'IGL1I00AI17','V','Dummy Helicity Pockels Cell Voltage'),(12,'HC:0L02_DWSUM','','Error on 0L02 Charge Asymmetry 4 Wire Sum'),(13,'HC:DX_DIF','','dX on X Position Difference to MCC'),(14,'SMRPOSC','','Hall C Chopper Slit Readback'),(15,'R00PGMEST','dB','Prebuncher Setting'),(16,'HC:1I04_DXDIF','','Error on 1I04 X Difference'),(17,'HC:Q_ASY','ppm','Charge Asymmetry to MCC'),(18,'HC:1I04_DWSUM','','Error on 1I04 Charge Asymmetry 4 Wire Sum'),(19,'WienAngle','degrees','Wien Filter Angle Setting'),(20,'itov2out','','A2 Slit Setting'),(21,'HC:QPD_XDIF','','QPD X Difference'),(22,'EHCFR_LIPRC','','Master Focus Location:  1=G0, 0=Pivot'),(23,'HC:0L02_WSUM','','0L02 Charge Asymmetry 4 Wire Sum'),(24,'HC:1I04_DYDIF','','Error on 1I04 Y Difference'),(25,'hel_mag_status.D','','Integrated helicity bit difference'),(26,'IBCxxxxCRCUR4','uA','Hall C MPS BCM Current'),(27,'scaler_calc1','nA','Hall B Beam Current (Faraday Cup)'),(28,'psub_cy_pos','','Y laser position on cathode'),(29,'itov5out','','A3 Slit Setting'),(30,'IGL1I00DAC3','','Hall B Seed Level'),(31,'osc_jitter','','Hall A Laser Phase Jitter'),(32,'HC:QPD_DWSUM','','Error on QPD Charge Asymmetry 4 Wire Sum'),(33,'IGL1I00AI24','V','IA Voltage'),(34,'HC:QPD_WSUM','','QPD Charge Asymmetry 4 Wire Sum'),(35,'itov7out','','A4 Slit Setting'),(36,'HC:0L03_DXDIF','','Error on 0L03 X Difference'),(37,'hallb_transmission','%','Hall B Independent Transmission'),(38,'SMRPOSA','','Hall A Chopper Slit Readback'),(39,'HC:0L03_YDIF','','0L03 Y Difference'),(40,'HC:IBCM_DQASY','','Error on Injector BCM Charge Asymmetry'),(41,'HC:1I06_DWSUM','','Error on 1I06 Charge Asymmetry 4 Wire Sum'),(42,'HC:X_DIF','','X Position Difference to MCC'),(43,'HC:0I05_YDIF','','0I05 Y Difference'),(44,'IGL1I00DAC2','','Hall B Control Level'),(45,'HC:1I06_DXDIF','','Error on 1I06 X Difference'),(46,'HC:0I05_DWSUM','','Error on 0I05 Charge Asymmetry 4 Wire Sum'),(47,'HC:QPD_DYDIF','','Error on QPD Y Difference'),(48,'HC:1I06_WSUM','','1I06 Charge Asymmetry 4 Wire Sum'),(49,'psub_ab_pos','','Hall B Laser Attenuator Setting'),(50,'HC:0I05_DYDIF','','Error on 0I05 Y Difference'),(51,'psub_pl_pos','','Rotatable Half Wave Plate Setting'),(52,'hallc_photocurrent','uA','Hall C photocurrent'),(53,'HC:0L02_DYDIF','','Error on 0L02 Y Difference'),(54,'hac_bcm_average','uA','Hall A (?) BCM Current'),(55,'ISL0I04DT','','Master Slit Setting'),(56,'IPM2C24A.IENG','','Hall B Beam Current (2C24A)'),(57,'IBCxxxxCRCUR2','uA','Hall A MPS BCM Current'),(58,'HC:0I05_XDIF','','0I05 X Difference'),(59,'HC:1I02_DYDIF','','Error on 1I02 Y Difference'),(60,'HC:1I02_DXDIF','','Error on 1I02 X Difference'),(61,'IGL1I00AI25','V','Hall A X PZT Voltage'),(62,'HC:QPD_YDIF','','QPD Y Difference'),(63,'HC:DY_DIF','','dY on Y Position Difference to MCC'),(64,'HC:QPD_DXDIF','','Error on QPD X Difference'),(65,'IGL1I00AI8','V','Gun 2 PITA Voltage Negative'),(66,'itov1out','','A1 Slit Setting'),(67,'qe_halla','%','Hall A Quantum Efficiency'),(68,'HC:1I02_DWSUM','','Error on 1I02 Charge Asymmetry 4 Wire Sum'),(69,'hallb_photocurrent','uA','Hall B photocurrent'),(70,'HC:0L02_XDIF','','0L02 X Difference'),(71,'HC:1I02_WSUM','','1I02 Charge Asymmetry 4 Wire Sum'),(72,'hallc_transmission','%','Hall C Independent Transmission'),(73,'HC:IBCM_QASY','','Injector BCM Charge Asymmetry'),(74,'IGL1I00AI23','V','G0 Y PZT Voltage'),(75,'IGL1I00AI22','V','G0 X PZT Voltage'),(76,'IGLdac3:ao_1','V','Y PZT Voltage'),(77,'IGLdac3:ao_0','V','X PZT Voltage'),(78,'HC:Y_DIF','','Y Position Difference to MCC'),(79,'qe_hallb','%','Hall B Quantum Efficiency'),(80,'IGL1I00AI26','V','Hall A Y PZT Voltage'),(81,'laser_b_wavelength','nm','Hall B laser wavelength'),(82,'psub_aa_pos','','Hall A Laser Attenuator Setting'),(83,'HC:1I06_YDIF','','1I06 Y Difference'),(84,'HC:1I02_YDIF','','1I02 Y Difference'),(85,'HC:DQ_ASY','ppm','Charge Asymmetry Uncertainty'),(86,'HC:0L02_YDIF','','0L02 Y Difference'),(87,'IBCxxxxCLOSS','','Machine Energy Loss'),(88,'psub_ac_pos','','Hall C Laser Attenuator Setting'),(89,'IGL1I00AI7','V','Gun 2 PITA Voltage Positive'),(90,'HC:1I04_YDIF','','1I04 Y Difference'),(91,'qe_hallc','%','Hall C Quantum Efficiency'),(92,'HC:0L03_DYDIF','','Error on 0L03 Y Difference'),(93,'HC:0L03_DWSUM','','Error on 0L03 Charge Asymmetry 4 Wire Sum'),(94,'HC:0L03_XDIF','','0L03 X Difference'),(95,'halla_photocurrent','uA','Hall A photocurrent'),(96,'HC:1I06_XDIF','','1I06 X Difference'),(97,'IGL1I00PStrans','','Injector transmission'),(98,'HC:pint_period','','Position difference sampling period'),(99,'HC:0L02_DXDIF','','Error on 0L02 X Difference'),(100,'osc2_jitter','','Hall C Laser Phase Jitter'),(101,'halla_transmission','%','Hall A Independent Transmission'),(102,'HC:1I04_WSUM','','1I04 Charge Asymmetry 4 Wire Sum'),(103,'HC:1I06_DYDIF','','Error on 1I06 Y Difference'),(104,'HC:1I02_XDIF','','1I02 X Difference'),(105,'HC:0I05_WSUM','','0I05 Charge Asymmetry 4 Wire Sum'),(106,'HC:bcm_threshold','uA','BCM threshold cut for charge feedback'),(107,'HC:0I05_DXIF','','Error on 0I05 X Difference');
/*!40000 ALTER TABLE `polarized_source_monitor` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `qtor`
--

DROP TABLE IF EXISTS `qtor`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `qtor` (
  `qtor_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `run_id` int(10) unsigned NOT NULL COMMENT 'run for these measurements',
  PRIMARY KEY  (`qtor_id`),
  KEY `fk_qtor_run_id` (`run_id`),
  CONSTRAINT `fk_qtor_run_id` FOREIGN KEY (`run_id`) REFERENCES `run` (`run_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `qtor`
--

LOCK TABLES `qtor` WRITE;
/*!40000 ALTER TABLE `qtor` DISABLE KEYS */;
/*!40000 ALTER TABLE `qtor` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `qtor_measurement`
--

DROP TABLE IF EXISTS `qtor_measurement`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `qtor_measurement` (
  `qtor_measurement_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `qtor_id` int(10) unsigned NOT NULL COMMENT 'qtor id for these measurements',
  `qtor_monitor_id` int(10) unsigned NOT NULL COMMENT 'qtor monitor id for this measurement',
  `average_value` float NOT NULL default '0' COMMENT 'average value of this measurement',
  `error` float NOT NULL default '0' COMMENT 'error on average',
  `min_value` float NOT NULL default '0' COMMENT 'minimum value of this measurement',
  `max_value` float NOT NULL default '0' COMMENT 'maximum value of this measurement',
  PRIMARY KEY  (`qtor_measurement_id`),
  KEY `fk_qtor_measurement_qtor_id` (`qtor_id`),
  KEY `fk_qtor_measurement_qtor_monitor_id` (`qtor_monitor_id`),
  CONSTRAINT `fk_qtor_measurement_qtor_id` FOREIGN KEY (`qtor_id`) REFERENCES `qtor` (`qtor_id`),
  CONSTRAINT `fk_qtor_measurement_qtor_monitor_id` FOREIGN KEY (`qtor_monitor_id`) REFERENCES `qtor_monitor` (`qtor_monitor_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `qtor_measurement`
--

LOCK TABLES `qtor_measurement` WRITE;
/*!40000 ALTER TABLE `qtor_measurement` DISABLE KEYS */;
/*!40000 ALTER TABLE `qtor_measurement` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `qtor_monitor`
--

DROP TABLE IF EXISTS `qtor_monitor`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `qtor_monitor` (
  `qtor_monitor_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `type` enum('t','v','p','l','i') default NULL COMMENT 'type of monitor (temperature, vacuum, pressure, level, current)',
  `quantity` text COMMENT 'name of monitor',
  `title` text COMMENT 'graph axis title',
  `units` text COMMENT 'measurement units',
  PRIMARY KEY  (`qtor_monitor_id`)
) ENGINE=InnoDB AUTO_INCREMENT=7 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `qtor_monitor`
--

LOCK TABLES `qtor_monitor` WRITE;
/*!40000 ALTER TABLE `qtor_monitor` DISABLE KEYS */;
INSERT INTO `qtor_monitor` VALUES (1,'l','G0:LN2Level','Liquid Nitrogen Reservoir Level','%'),(2,'v','G0:TCCryoBox','Cryobox Vacuum (TC Gauge)','mTorr'),(3,'l','G0:LHeLevel','Liquid Helium Reservoir Level','%'),(4,'t','G0:ROXCoilA','Coil A Temperature (ROX Sensor)','K'),(5,'i','G0:PSCurrent','SMS Power Supply Current','A'),(6,'v','g0smsvac','Cryostat vacuum (CC Gauge)','Torr');
/*!40000 ALTER TABLE `qtor_monitor` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `run`
--

DROP TABLE IF EXISTS `run`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `run` (
  `run_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `run_number` int(10) unsigned NOT NULL COMMENT 'number of run from CODA',
  `run_type` enum('good','bad','junk','spiky') default NULL COMMENT 'quality of this run',
  `start_time` datetime default NULL COMMENT 'time run began',
  `end_time` datetime default NULL COMMENT 'time run ended',
  `n_mps` int(10) unsigned NOT NULL default '0' COMMENT 'number of MPSs in run',
  `n_qrt` int(10) unsigned NOT NULL default '0' COMMENT 'number of quartets in run',
  PRIMARY KEY  (`run_id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `run`
--

LOCK TABLES `run` WRITE;
/*!40000 ALTER TABLE `run` DISABLE KEYS */;
INSERT INTO `run` VALUES (1,753,NULL,NULL,NULL,0,0);
/*!40000 ALTER TABLE `run` ENABLE KEYS */;
UNLOCK TABLES;

/*!50003 SET @SAVE_SQL_MODE=@@SQL_MODE*/;

DELIMITER ;;
/*!50003 SET SESSION SQL_MODE="" */;;
/*!50003 CREATE */ /*!50017 DEFINER=`qwadmin`@`localhost` */ /*!50003 TRIGGER `summary_run_number` AFTER INSERT ON `run` FOR EACH ROW BEGIN



DECLARE i INT DEFAULT 1;
WHILE i<=8 DO

INSERT INTO summary_dy_calc
SET run_number = NEW.run_number,
detector_id = i;

INSERT INTO summary_dy_correct
SET run_number = NEW.run_number,
detector_id = i;

INSERT INTO summary_da_calc
SET run_number = NEW.run_number,
detector_id = i;

INSERT INTO summary_da_correct
SET run_number = NEW.run_number,
detector_id = i;

SET i = i+1;
END WHILE ;

INSERT INTO summary_by
SET run_number = NEW.run_number;

INSERT INTO summary_ba
SET run_number = NEW.run_number;


END */;;

DELIMITER ;
/*!50003 SET SESSION SQL_MODE=@SAVE_SQL_MODE*/;

--
-- Table structure for table `seed`
--

DROP TABLE IF EXISTS `seed`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `seed` (
  `seed_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table/seed identifier',
  `seed` text COMMENT 'seed value in ASCII (hexadecimal representation)',
  `comment` text COMMENT 'user comment',
  PRIMARY KEY  (`seed_id`)
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `seed`
--

LOCK TABLES `seed` WRITE;
/*!40000 ALTER TABLE `seed` DISABLE KEYS */;
INSERT INTO `seed` VALUES (1,'100',NULL),(2,'101',NULL),(3,'102',NULL),(4,'103',NULL),(5,'104',NULL),(6,'105',NULL),(7,'106',NULL);
/*!40000 ALTER TABLE `seed` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `slope`
--

DROP TABLE IF EXISTS `slope`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `slope` (
  `slope_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `analysis_id` int(10) unsigned NOT NULL COMMENT 'run that was replayed',
  `slope_type_id` int(10) unsigned NOT NULL COMMENT 'type of slope',
  `detector_id` int(10) unsigned NOT NULL COMMENT 'detector for this measurement',
  `value` float NOT NULL default '0' COMMENT 'value of slope',
  `error` float NOT NULL default '0' COMMENT 'error on slope',
  PRIMARY KEY  (`slope_id`),
  KEY `fk_slope_analysis_id` (`analysis_id`),
  KEY `fk_slope_slope_type_id` (`slope_type_id`),
  CONSTRAINT `fk_slope_analysis_id` FOREIGN KEY (`analysis_id`) REFERENCES `analysis` (`analysis_id`),
  CONSTRAINT `fk_slope_slope_type_id` FOREIGN KEY (`slope_type_id`) REFERENCES `slope_type` (`slope_type_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `slope`
--

LOCK TABLES `slope` WRITE;
/*!40000 ALTER TABLE `slope` DISABLE KEYS */;
/*!40000 ALTER TABLE `slope` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `slope_type`
--

DROP TABLE IF EXISTS `slope_type`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `slope_type` (
  `slope_type_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table/type of slope',
  `slope` text NOT NULL COMMENT 'name of slope',
  `units` text NOT NULL COMMENT 'units of slope',
  `title` text NOT NULL COMMENT 'axis title',
  PRIMARY KEY  (`slope_type_id`)
) ENGINE=InnoDB AUTO_INCREMENT=7 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `slope_type`
--

LOCK TABLES `slope_type` WRITE;
/*!40000 ALTER TABLE `slope_type` DISABLE KEYS */;
INSERT INTO `slope_type` VALUES (1,'dYdE','counts/nC/MeV','dY/dE'),(2,'dYdty','counts/nC/mr','dY/dtheta_y'),(3,'dYdx','counts/nC/mm','dY/dx'),(4,'dYdq','counts/nC/nC','dY/dq'),(5,'dYdy','counts/nC/mm','dY/dy'),(6,'dYdtx','counts/nC/mr','dY/dtheta_x');
/*!40000 ALTER TABLE `slope_type` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `summary_ba`
--

DROP TABLE IF EXISTS `summary_ba`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `summary_ba` (
  `run_number` int(10) unsigned NOT NULL COMMENT 'displays the run number; primary key for table',
  `slow_helicity_plate` enum('in','out') NOT NULL COMMENT 'status of slow helicity plate',
  `q_targ_value` float default NULL COMMENT 'q_targ value',
  `q_targ_error` float default NULL COMMENT 'q_targ error',
  `x_targ_value` float default NULL COMMENT 'x_targ value',
  `x_targ_error` float default NULL COMMENT 'x_targ value',
  `y_targ_value` float default NULL COMMENT 'y_targ value',
  `y_targ_error` float default NULL COMMENT 'y_targ value',
  `theta_x_value` float default NULL COMMENT 'theta_x value',
  `theta_x_error` float default NULL COMMENT 'theta_x error',
  `theta_y_value` float default NULL COMMENT 'theta_y value',
  `theta_y_error` float default NULL COMMENT 'theta_y_error',
  `energy_value` float default NULL COMMENT 'energy value',
  `energy_error` float default NULL COMMENT 'energy value',
  PRIMARY KEY  (`run_number`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `summary_ba`
--

LOCK TABLES `summary_ba` WRITE;
/*!40000 ALTER TABLE `summary_ba` DISABLE KEYS */;
INSERT INTO `summary_ba` VALUES (753,'in',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
/*!40000 ALTER TABLE `summary_ba` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `summary_by`
--

DROP TABLE IF EXISTS `summary_by`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `summary_by` (
  `run_number` int(10) unsigned NOT NULL COMMENT 'displays the run number; primary key for table',
  `slow_helicity_plate` enum('in','out') NOT NULL COMMENT 'status of slow helicity plate',
  `q_targ_value` float default NULL COMMENT 'q_targ value',
  `q_targ_error` float default NULL COMMENT 'q_targ error',
  `x_targ_value` float default NULL COMMENT 'x_targ value',
  `x_targ_error` float default NULL COMMENT 'x_targ value',
  `y_targ_value` float default NULL COMMENT 'y_targ value',
  `y_targ_error` float default NULL COMMENT 'y_targ value',
  `theta_x_value` float default NULL COMMENT 'theta_x value',
  `theta_x_error` float default NULL COMMENT 'theta_x error',
  `theta_y_value` float default NULL COMMENT 'theta_y value',
  `theta_y_error` float default NULL COMMENT 'theta_y_error',
  `energy_value` float default NULL COMMENT 'energy value',
  `energy_error` float default NULL COMMENT 'energy value',
  PRIMARY KEY  (`run_number`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `summary_by`
--

LOCK TABLES `summary_by` WRITE;
/*!40000 ALTER TABLE `summary_by` DISABLE KEYS */;
INSERT INTO `summary_by` VALUES (753,'in',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
/*!40000 ALTER TABLE `summary_by` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `summary_da_calc`
--

DROP TABLE IF EXISTS `summary_da_calc`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `summary_da_calc` (
  `summary_da_calc_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `run_number` int(10) unsigned default NULL COMMENT 'displays the run number',
  `slow_helicity_plate` enum('in','out') default NULL COMMENT 'status of slow helicity plate',
  `detector_id` int(10) unsigned default NULL COMMENT 'cerenkov detector id',
  `value` float default NULL COMMENT 'detector value',
  `error` float default NULL COMMENT 'detector error',
  PRIMARY KEY  (`summary_da_calc_id`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `summary_da_calc`
--

LOCK TABLES `summary_da_calc` WRITE;
/*!40000 ALTER TABLE `summary_da_calc` DISABLE KEYS */;
INSERT INTO `summary_da_calc` VALUES (1,753,NULL,1,NULL,NULL),(2,753,NULL,2,NULL,NULL),(3,753,NULL,3,NULL,NULL),(4,753,NULL,4,NULL,NULL),(5,753,NULL,5,NULL,NULL),(6,753,NULL,6,NULL,NULL),(7,753,NULL,7,NULL,NULL),(8,753,NULL,8,NULL,NULL);
/*!40000 ALTER TABLE `summary_da_calc` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `summary_da_correct`
--

DROP TABLE IF EXISTS `summary_da_correct`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `summary_da_correct` (
  `summary_da_correct_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `run_number` int(10) unsigned default NULL COMMENT 'displays the run number',
  `slow_helicity_plate` enum('in','out') default NULL COMMENT 'status of slow helicity plate',
  `detector_id` int(10) unsigned default NULL COMMENT 'cerenkov detector id',
  `value` float default NULL COMMENT 'detector value',
  `error` float default NULL COMMENT 'detector error',
  PRIMARY KEY  (`summary_da_correct_id`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `summary_da_correct`
--

LOCK TABLES `summary_da_correct` WRITE;
/*!40000 ALTER TABLE `summary_da_correct` DISABLE KEYS */;
INSERT INTO `summary_da_correct` VALUES (1,753,NULL,1,NULL,NULL),(2,753,NULL,2,NULL,NULL),(3,753,NULL,3,NULL,NULL),(4,753,NULL,4,NULL,NULL),(5,753,NULL,5,NULL,NULL),(6,753,NULL,6,NULL,NULL),(7,753,NULL,7,NULL,NULL),(8,753,NULL,8,NULL,NULL);
/*!40000 ALTER TABLE `summary_da_correct` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `summary_dy_calc`
--

DROP TABLE IF EXISTS `summary_dy_calc`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `summary_dy_calc` (
  `summary_dy_calc_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `run_number` int(10) unsigned default NULL COMMENT 'displays the run number',
  `slow_helicity_plate` enum('in','out') default NULL COMMENT 'status of slow helicity plate',
  `detector_id` int(10) unsigned default NULL COMMENT 'cerenkov detector id',
  `value` float default NULL COMMENT 'detector value',
  `error` float default NULL COMMENT 'detector error',
  PRIMARY KEY  (`summary_dy_calc_id`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `summary_dy_calc`
--

LOCK TABLES `summary_dy_calc` WRITE;
/*!40000 ALTER TABLE `summary_dy_calc` DISABLE KEYS */;
INSERT INTO `summary_dy_calc` VALUES (1,753,NULL,1,NULL,NULL),(2,753,NULL,2,NULL,NULL),(3,753,NULL,3,NULL,NULL),(4,753,NULL,4,NULL,NULL),(5,753,NULL,5,NULL,NULL),(6,753,NULL,6,NULL,NULL),(7,753,NULL,7,NULL,NULL),(8,753,NULL,8,NULL,NULL);
/*!40000 ALTER TABLE `summary_dy_calc` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `summary_dy_correct`
--

DROP TABLE IF EXISTS `summary_dy_correct`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `summary_dy_correct` (
  `summary_dy_correct_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `run_number` int(10) unsigned default NULL COMMENT 'displays the run number',
  `slow_helicity_plate` enum('in','out') default NULL COMMENT 'status of slow helicity plate',
  `detector_id` int(10) unsigned default NULL COMMENT 'cerenkov detector id',
  `value` float default NULL COMMENT 'detector value',
  `error` float default NULL COMMENT 'detector error',
  PRIMARY KEY  (`summary_dy_correct_id`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `summary_dy_correct`
--

LOCK TABLES `summary_dy_correct` WRITE;
/*!40000 ALTER TABLE `summary_dy_correct` DISABLE KEYS */;
INSERT INTO `summary_dy_correct` VALUES (1,753,NULL,1,NULL,NULL),(2,753,NULL,2,NULL,NULL),(3,753,NULL,3,NULL,NULL),(4,753,NULL,4,NULL,NULL),(5,753,NULL,5,NULL,NULL),(6,753,NULL,6,NULL,NULL),(7,753,NULL,7,NULL,NULL),(8,753,NULL,8,NULL,NULL);
/*!40000 ALTER TABLE `summary_dy_correct` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `target`
--

DROP TABLE IF EXISTS `target`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `target` (
  `target_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `run_id` int(10) unsigned NOT NULL COMMENT 'run for this set of measurements',
  `type` enum('h','c','s','l','o','u','r') default NULL COMMENT 'target position (LH2, C12, small hole, large hole, out-of-beam, undefined, radiator)',
  PRIMARY KEY  (`target_id`),
  KEY `fk_target_run_id` (`run_id`),
  CONSTRAINT `fk_target_run_id` FOREIGN KEY (`run_id`) REFERENCES `run` (`run_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `target`
--

LOCK TABLES `target` WRITE;
/*!40000 ALTER TABLE `target` DISABLE KEYS */;
/*!40000 ALTER TABLE `target` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `target_measurement`
--

DROP TABLE IF EXISTS `target_measurement`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `target_measurement` (
  `target_measurements_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `target_id` int(10) unsigned NOT NULL COMMENT 'run for this set of measurements',
  `target_monitor_id` int(10) unsigned NOT NULL COMMENT 'monitor for this measurement',
  `average_value` float NOT NULL default '0' COMMENT 'average value of this measurement',
  `error` float NOT NULL default '0' COMMENT 'error on average',
  `min_value` float NOT NULL default '0' COMMENT 'minimum value of this measurement',
  `max_value` float NOT NULL default '0' COMMENT 'maximum value of this measurement',
  PRIMARY KEY  (`target_measurements_id`),
  KEY `fk_target_measurement_target_id` (`target_id`),
  KEY `fk_target_measurement_target_monitor_id` (`target_monitor_id`),
  CONSTRAINT `fk_target_measurement_target_id` FOREIGN KEY (`target_id`) REFERENCES `target` (`target_id`),
  CONSTRAINT `fk_target_measurement_target_monitor_id` FOREIGN KEY (`target_monitor_id`) REFERENCES `target_monitor` (`target_monitor_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `target_measurement`
--

LOCK TABLES `target_measurement` WRITE;
/*!40000 ALTER TABLE `target_measurement` DISABLE KEYS */;
/*!40000 ALTER TABLE `target_measurement` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `target_monitor`
--

DROP TABLE IF EXISTS `target_monitor`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `target_monitor` (
  `target_monitor_id` int(10) unsigned NOT NULL auto_increment COMMENT 'primary key for table',
  `type` enum('t','v','p','l') default NULL COMMENT 'type of monitor (temperature, vacuum, pressure, level)',
  `quantity` text COMMENT 'name of monitor',
  `units` text COMMENT 'measurement units',
  `title` text COMMENT 'graph axis title',
  PRIMARY KEY  (`target_monitor_id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `target_monitor`
--

LOCK TABLES `target_monitor` WRITE;
/*!40000 ALTER TABLE `target_monitor` DISABLE KEYS */;
INSERT INTO `target_monitor` VALUES (1,'t','gt3','K',''),(2,'p','gpt3','PSIA','');
/*!40000 ALTER TABLE `target_monitor` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2010-03-11 20:24:57
