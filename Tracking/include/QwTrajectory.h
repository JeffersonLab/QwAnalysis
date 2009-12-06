/*! \file   QwTrajectory.h
 *  \author J. Pan
 *  \date   Thu Nov 26 11:44:51 CST 2009
 *  \brief  Raytrace for trajectory in magnetic field.
 *
 *  \ingroup QwTracking
 *
 *   Integrating the equations of motion for electrons in the QTOR.
 *   The 4'th order Runge-Kutta method is used.
 *
 */


#ifndef __QWTRAJECTORY_H__
#define __QWTRAJECTORY_H__

#include <iostream>
#include <vector>
#include <TVector3.h>
#include <TRotation.h>
#include "QwMagneticField.h"

// scale factor of the magnetic field
#define BSCALE 1.04

class QwTrajectory {

 public:
    QwTrajectory();
    ~QwTrajectory(){};

    void SetMagneticField(QwMagneticField *bfield){ B_Field = bfield; };

    double EstimateInitialMomentum(TVector3 direction);

    TVector3 Integrate(double E0,TVector3 startpoint, TVector3 direction,
                            double step, double z_endplane);


    TVector3 GetFieldIntegral() { return TVector3(bdlx,bdly,bdlz); };
    double GetDirectionX(){ return uvx[0]; };
    double GetDirectionY(){ return uvy[0]; };
    double GetDirectionZ(){ return uvz[0]; };

 private:

    QwMagneticField *B_Field;

    double bdlx;
    double bdly;
    double bdlz;

    double xx[2], yy[2], zz[2]; // position
    double uvx[2], uvy[2], uvz[2]; // unit vector

    double momentum;

}; // class QwTrajectory

#endif
