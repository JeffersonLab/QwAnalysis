#include<iostream>
#include "tree.h"



/* This class is to be used to convert the un-orthogonal(word?) u and v coordinates 
   of the Region 2 and 3 drift chamber wire planes into x and y coordinates.
*/
class Uv2xy{
   friend class treecombine;
   friend int r2_TrackFit(int Num,Hit **Hit,double *fit,double *cov,double *chi);

  public:
    Uv2xy();
    ~Uv2xy();
    
    double uv2x(double u, double v, enum ERegion region);
    double uv2y(double u, double v, enum ERegion region);
    double xy2u(double x, double y, enum ERegion region);
    double xy2v(double x, double y, enum ERegion region);

  private:
    double R2_uv[2][2];
    double R2_xy[2][2];
    double R3_uv[2][2];
    double R3_xy[2][2];

    double R2_offset[2];
    double R2_wirespacing;

};
