/** Setup the sources **/
INSERT INTO source(title) values("qrt");
INSERT INTO source(title) values("mps");

/** Setup Measurement Types **/
INSERT INTO measurement_type(units,title) values( "mm","Distance");
INSERT INTO measurement_type(units,title) values( "uA","Current");

INSERT INTO measurement_type(units,title) values( "mm","DiffDistance");
INSERT INTO measurement_type(units,title) values( "uA","DiffCurrent");

/** Photon Measurement types **/
INSERT INTO measurement_type(units,title) values( "percent", "Asymmetry");
INSERT INTO measurement_type(units,title) values( "iadc", "BackgroundSubtracted");
INSERT INTO measurement_type(units,title) values( "iadc", "Background");
INSERT INTO measurement_type(units,title) values( "counts", "LaserPower");
INSERT INTO measurement_type(units,title) values( "counts", "ScalersLaserOn");
INSERT INTO measurement_type(units,title) values( "counts", "ScalersLaserOff");


INSERT INTO measurement_type(units,title) values( "iadc", "DiffBackgroundSubtracted");
INSERT INTO measurement_type(units,title) values( "iadc", "DiffBackground");

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

/** Setup Photon Elements **/
INSERT INTO monitor(title) values("accum0");

/** Setup the Goodfor **/
INSERT INTO good_for(type) values("junk");
INSERT INTO good_for(type) values("qrt");
INSERT INTO good_for(type) values("mps");
INSERT INTO good_for(type) values("both");
INSERT INTO good_for(type) values("transverse");
