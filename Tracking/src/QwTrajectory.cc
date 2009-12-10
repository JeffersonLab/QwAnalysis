
// QwTrajectory.cc
// Author: J. Pan
// Date: Thu Nov 26 12:27:10 CST 2009

#include "QwTrajectory.h"

QwTrajectory::QwTrajectory() {

    for (int i=0;i<2;i++){
      xx[i]=0.0;     // position
      yy[i]=0.0;
      zz[i]=0.0; 
      uvx[i] = 0.0;  // unit vector
      uvy[i] = 0.0;
      uvz[i] = 0.0;
    }

    bdlx = 0.0;
    bdly = 0.0;
    bdlz = 0.0;
};


double QwTrajectory::EstimateInitialMomentum(TVector3 direction){

    double cth = direction.Z();        // cos(theta) = uz/r, where ux,uy,uz form a unit vector
    double wp = 0.93828;              // proton mass [GeV]
    double e0 = 1.165;                // beam energy [GeV]
    double e = e0/(1.0+e0/wp*(1.0-cth)) - 0.012; //kinematics for e+p scattering, target energy loss ~12 MeV
    return e;

};

//************************************************************
//     RK4 integration for trajectory and field integral.
//     linear dimensions must be in m and b in tesla
//     beta=qc/e=-0.2998/e(GeV) [coul.(m/s)/j]
//*************************************************************
//     the coupled differential equations are :
//         dx/ds=vx
//         dy/ds=vy
//         dz/ds=vz
//         dvx/ds=beta*(vy*bz-vz*by)
//         dvy/ds=beta*(vz*bx-vx*bz)
//         dvz/ds=beta*(vx*by-vy*bx)
//     where ds is the displacement along the trajectory (=h, the int. step)
//
//input: values of the cordinates (x0,y0,z0), slopes (vx0,vy0,vz0) at starting point
//output: coordinates of endpoint, direction at endpoint and field integral
//*************************************************************

// If the endpoint is at upstream and startpoint is at downstream,
// the electron will swim backward

TVector3 QwTrajectory::Integrate(double E0,
                                TVector3 startpoint,
                                TVector3 direction,
                                double step,
                                double z_endplane) {

// local variables
    double x,y,z,x1,y1,z1;
    double vx,vy,vz,vx1,vy1,vz1;
    double dx1,dy1,dz1;
    double dx2,dy2,dz2;
    double dx3,dy3,dz3;
    double dx4,dy4,dz4;
    double dvx1,dvy1,dvz1;
    double dvx2,dvy2,dvz2;
    double dvx3,dvy3,dvz3;
    double dvx4,dvy4,dvz4;

    double bx,by,bz;  // B field components
    double Point[3];
    double Bfield[3];

    double beta = -0.2998/E0;
    bdlx=0.0;
    bdly=0.0;
    bdlz=0.0;

// convert linear dimensions cm -> m
    step = step*0.01;
    z_endplane = z_endplane*0.01;

    xx[0]=startpoint.X()*0.01;
    yy[0]=startpoint.Y()*0.01;
    zz[0]=startpoint.Z()*0.01;

    //reverse coordinates for backward swiming
    if (startpoint.Z()>z_endplane) {
       xx[0]=-xx[0];
       zz[0]=-zz[0];
       uvx[0]=direction.X();
       uvy[0]=-direction.Y();
       uvz[0]=direction.Z();
       z_endplane=-z_endplane;
    }
    else {  //forward swiming
       uvx[0]=direction.X();
       uvy[0]=direction.Y();
       uvz[0]=direction.Z();
    }

    while ( fabs(zz[0]-z_endplane)>=step ) {  // integration loop

        //values of the cordinates, unit vector and field at start of interval
        x1=xx[0];
        y1=yy[0];
        z1=zz[0];
        vx1=uvx[0];
        vy1=uvy[0];
        vz1=uvz[0];

        Point[0]=x1*100.0;
        Point[1]=y1*100.0;
        Point[2]=z1*100.0;
        B_Field->GetFieldValue(Point, Bfield, BSCALE);
        bx = Bfield[0];
        by = Bfield[1];
        bz = Bfield[2];

//          std::cout<<"x1, y1, z1, bx, by, bz :    "
//                    <<", "<<Point[0]<<", "<<Point[1]<<", "<<Point[2]<<",      "
//                    <<bx*10000.0<<", "<<by*10000.0<<", "<<bz*10000.0<<std::endl;

//     first approximation to the changes in the variables for step h (k1)
        dx1=step*vx1;
        dy1=step*vy1;
        dz1=step*vz1;
        dvx1=step*beta*(vy1*bz-vz1*by);
        dvy1=step*beta*(vz1*bx-vx1*bz);
        dvz1=step*beta*(vx1*by-vy1*bx);

//     next approximation to the values of the variables for step h/2
        x=x1+dx1/2.0;
        y=y1+dy1/2.0;
        z=z1+dz1/2.0;
        vx=vx1+dvx1/2.0;
        vy=vy1+dvy1/2.0;
        vz=vz1+dvz1/2.0;
        Point[0]=x*100.0;
        Point[1]=y*100.0;
        Point[2]=z*100.0;
        B_Field->GetFieldValue(Point, Bfield, BSCALE);
        bx = Bfield[0];
        by = Bfield[1];
        bz = Bfield[2];

//     second approximation to the changes in the variables for step h (k2)
        dx2=step*vx;
        dy2=step*vy;
        dz2=step*vz;
        dvx2=step*beta*(vy*bz-vz*by) ;
        dvy2=step*beta*(vz*bx-vx*bz);
        dvz2=step*beta*(vx*by-vy*bx);

//     next approximation to the values of the variables for step h/2
        x=x1+dx2/2.0;
        y=y1+dy2/2.0;
        z=z1+dz2/2.0;
        vx=vx1+dvx2/2.0;
        vy=vy1+dvy2/2.0;
        vz=vz1+dvz2/2.0;
        Point[0]=x*100.0;
        Point[1]=y*100.0;
        Point[2]=z*100.0;
        B_Field->GetFieldValue(Point, Bfield, BSCALE);
        bx = Bfield[0];
        by = Bfield[1];
        bz = Bfield[2];

//    third approximation to the changes in the variables for step h (k3)
        dx3=step*vx;
        dy3=step*vy;
        dz3=step*vz;
        dvx3=step*beta*(vy*bz-vz*by);
        dvy3=step*beta*(vz*bx-vx*bz);
        dvz3=step*beta*(vx*by-vy*bx);

// next approximation to the values of the variables for step h, not h/2
        x=x1+dx3;
        y=y1+dy3;
        z=z1+dz3;
        vx=vx1+dvx3;
        vy=vy1+dvy3;
        vz=vz1+dvz3;
        Point[0]=x*100.0;
        Point[1]=y*100.0;
        Point[2]=z*100.0;
        B_Field->GetFieldValue(Point, Bfield, BSCALE);
        bx = Bfield[0];
        by = Bfield[1];
        bz = Bfield[2];

//evaluate the path integral (b x dl)
        bdlx=bdlx+step*(vz*by-vy*bz);
        bdly=bdly+step*(vx*bz-vz*bx);
        bdlz=bdlz+step*(vy*bx-vx*by);

//fourth approximation to the changes in the variables for step h (k4)
        dx4=step*vx;
        dy4=step*vy;
        dz4=step*vz;
        dvx4=step*beta*(vy*bz-vz*by);
        dvy4=step*beta*(vz*bx-vx*bz);
        dvz4=step*beta*(vx*by-vy*bx);

//final estimates of trajectory
        xx[1]=xx[0]+(dx1+2.0*dx2+2.0*dx3+dx4)/6.0;
        yy[1]=yy[0]+(dy1+2.0*dy2+2.0*dy3+dy4)/6.0;
        zz[1]=zz[0]+(dz1+2.0*dz2+2.0*dz3+dz4)/6.0;
        uvx[1]=uvx[0]+(dvx1+2.0*dvx2+2.0*dvx3+dvx4)/6.0;
        uvy[1]=uvy[0]+(dvy1+2.0*dvy2+2.0*dvy3+dvy4)/6.0;
        uvz[1]=uvz[0]+(dvz1+2.0*dvz2+2.0*dvz3+dvz4)/6.0;

        xx[0]=xx[1];
        yy[0]=yy[1];
        zz[0]=zz[1];
        uvx[0]=uvx[1];
        uvy[0]=uvy[1];
        uvz[0]=uvz[1];

//          std::cout<<"xx, yy, zz, bx, by, bz :    "
//                    <<", "<<xx[0]*100.0<<", "<<yy[0]*100.0<<", "<<zz[0]*100.0<<",      "
//                    <<bx*10000.0<<", "<<by*10000.0<<", "<<bz*10000.0<<std::endl;

    } //end of while loop

    //reverse coordinates for backward swiming
    if (startpoint.Z()>z_endplane) {
       xx[0]=-xx[0];
       zz[0]=-zz[0];
       uvy[0]=-uvy[0];
    }

    return TVector3(xx[0]*100.0,yy[0]*100.0,zz[0]*100.0);
}

