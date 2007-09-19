#include "mathstd.h"

/*
 * ----------------------------------------
 *         line crosses a plane
 * ----------------------------------------
 * plane: (x-p)*n = 0
 * line : t*s + q = x
 * dimenstion is assumed to be 3 (for now)
 * returns the result in result if
 * returncode == 1. If return == 0 there
 * is no solution. If return == -1 there
 * is an infinite number of solutions.
 * ----------------------------------------
 */

int
V_PlaneXLine( vektor n, vektor p, vektor s, vektor q, vektor result)
{
  double a, b;
  const int dim = 3;
  double diff[3];

  V_Sub( diff, q, p, dim);
  
  a = V_SkalarProdukt(s,n,dim);
  b = V_SkalarProdukt(diff,n,dim);

  if( a ) 
    b /= -a;
  else if( b ) 
    return 0;
  else
    return -1;

  V_S_Produkt(diff, b, s, dim);
  V_Add( result, diff, q, dim);
  return 1;
}
