//
// C++ Interface: enum
//
// Description: Header file that defines the enumeration data types
//              for upper/lower, detector region, detector type and
//              wire orientation.
//
// Author: Wouter Deconinck <wdconinc@mit.edu>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef QWTRACKINGENUM_H
#define QWTRACKINGENUM_H

/*! \enum EUppLow
    Upper or lower part of the detector
 */
enum EUppLow {
  w_nowhere = -1,	/*!< Undefined part of the detector */
  w_upper,		/*!< Upper detector part */
  w_lower		/*!< Lower detector part */
};

/*! \enum ERegion
    Detector region
 */
enum ERegion {
  r1 = 1,	/*!< Region 1 */
  r2,		/*!< Region 2 */
  r3		/*!< Region 3 */
};

/*! \enum Etype
    Detector type
 */
enum Etype {
  d_drift,	/*!< Drift chamber */
  d_gem,	/*!< GEM detector */
  d_trigscint,	/*!< Trigger scintillator */
  d_cerenkov	/*!< Cerenkov detector */
};

/*! \enum Edir
    Chamber wire orientation
 */
enum Edir {
  null_dir = 0,	/*!< Undefined direction */
  u_dir,	/*!< U direction */
  v_dir,	/*!< V direction */
  x_dir,	/*!< X direction */
  y_dir		/*!< Y direction */
};

#endif // QWTRACKINGENUM_H
