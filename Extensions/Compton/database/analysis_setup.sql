/** Setup the sources **/
INSERT INTO source(title) values("qrt");
INSERT INTO source(title) values("mps");

/** Setup Measurement Types **/
INSERT INTO measurement_type(units,title) values( "mm","Distance");
INSERT INTO measurement_type(units,title) values( "uA","Current");

INSERT INTO measurement_type(units,title) values( "mm","DiffDistance");
INSERT INTO measurement_type(units,title) values( "uA","DiffCurrent");

/** Setup the BeamLine Elements **/
INSERT INTO monitor(title) values("bcm1");
INSERT INTO monitor(title) values("bcm2");
INSERT INTO monitor(title) values("bcm6");

INSERT INTO monitor(title) values("3p02aX");
INSERT INTO monitor(title) values("3p02aY");

INSERT INTO monitor(title) values("3p02bX");
INSERT INTO monitor(title) values("3p02bY");

INSERT INTO monitor(title) values("3p03aX");
INSERT INTO monitor(title) values("3p03aY");
