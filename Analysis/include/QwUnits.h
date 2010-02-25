#ifndef QWUNITS_H
#define QWUNITS_H

namespace Qw {

  // Length units: base unit is mm
  // metric
  static const double mm = 1.0;
  static const double cm = 10.0;
  static const double m = 1.0e3;
  // imperial
  static const double in = 25.4;
  static const double mil = 0.0254;

  // Time units: base unit is ms
  static const double ns = 1.0e-6;
  static const double us = 1.0e-3;
  static const double ms = 1.0;
  static const double sec = 1.0e3; // SI notation is s, but sec avoids ambiguity
  static const double min = 60.0 * sec;
  static const double hour = 60.0 * min;
  static const double day = 24.0 * hour;
  // Frequency
  static const double Hz = 1.0 / sec;
  static const double kHz = 1.0 / ms;
  static const double MHz = 1.0e3 * kHz;

  // Energy: base unit is MeV
  static const double keV = 1.0e-3;
  static const double MeV = 1.0;
  static const double GeV = 1.0e3;

  // Angles: base unit is radian
  static const double pi = 3.14159265;
  static const double deg2rad = pi / 180.0;
  static const double rad2deg = 180.0 / pi;
  static const double rad = 1.0;
  static const double deg = deg2rad;

}; // namespace Qw

#endif // QWUNITS_H
