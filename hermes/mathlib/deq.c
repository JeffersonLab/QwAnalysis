#include <float.h>
#include <math.h>
#include "deq.h"


typedef double avector[MAXDIM];
static int deqdim;
static deqfun *fun;
static vector add(vector k, vector x, vector y);
static vector sub(vector k, vector x, vector y);
static vector s_mul(vector k, skalar s, vector x);
static skalar sklpro(vector x, vector y);
static skalar norm (vector x);

method methods[] = {
  { explicit_Euler, 1 },
  { improved_Euler, 2 },
  { simple_Runge, 2 },
  { Heum, 3 },
  { Runge_Kutta, 4 }
};

/* ----------------------------------------------------------------------
 * sets the global ( ;-( ) equation dimension. This function must be called
 * before all tracking
 */

int
deq_setdeqdim( int dim)
{
  deqdim = dim;
  if( deqdim > MAXDIM)
    deqdim = MAXDIM;
  if( deqdim < 1)
    deqdim = 1;
  return deqdim;
}

/* ----------------------------------------------------------------------
 * some simple vector arithmetics functions. The result 
 * is the first argument - the second and third are the operands
 */

#ifdef __GCC__
#define INLINE inline
#else
#define INLINE 
#endif

INLINE
static vector
add(vector k, vector x, vector y)
{
  register int i;
  for ( i = 0; i < deqdim; i++)
    k[i] = x[i] + y[i];
  return k;
}

INLINE
static vector
sub(vector k, vector x, vector y)
{
  register int i;
  for ( i = 0; i < deqdim; i++)
    k[i] = x[i] - y[i];
  return k;
}

INLINE
static vector
s_mul(vector k, skalar s, vector x)
{
  register int i;
  for ( i = 0; i < deqdim; i++) 
    k[i] = s * x[i];
  return k;
}

INLINE
static skalar
sklpro(vector x, vector y)
{
  register int i;
  skalar sum = 0.0;
  for (i = 0; i < deqdim; i++)
    sum += x[i]*y[i];
  return sum;
}

INLINE
static skalar
norm (vector x)
{
  return sqrt(sklpro(x,x));
}


/* ----------------------------------------------------------------------
 * the runge kutta methods for tracking. 5 functions are given for
 * different order
 */

vector
explicit_Euler (vector e, skalar t, vector x, skalar h)
{
  (void) h;
  return fun(e,t,x);
}

vector
improved_Euler (vector r, skalar t, vector x, skalar h)
{
  avector k1,k2,k3,k4,k5;
  fun(k1,t,x);
  return s_mul(r,0.5,add(k2,k1,fun(k3,t+h,add(k4,x,s_mul(k5,h,k1)))));
}

vector
simple_Runge ( vector r, skalar t, vector x ,skalar h)
{
  avector k2,k3,k4;
  return fun(r,t+h/2.0,add(k2,x,s_mul(k3,h/2.0,fun(k4,t,x))));
}

vector
Heum( vector r, skalar t, vector x ,skalar h)
{
  avector k1,k2,k3,k4,k5,k6,k7,k8,k9;
  fun(k2,t,x);
  return add(r,s_mul(k1,0.25,k2),
	     s_mul(k3,0.75,
		   fun(k4,t+2.0/3.0*h,
		       add(k5,x,s_mul(k9,h*2.0/3.0,
				      fun(k6,t+h/3.0,
					  add(k7,x,
					      s_mul(k8,h/3.0,k2))))))));
}

vector
Runge_Kutta ( vector r, skalar t, vector x ,skalar h)
{
  avector k1,k2,k3,k4,k5,k6,k7,k8,k9,k10;
  fun(k1,t,x);
  fun(k2,t+h/2.0,add(k4,x,s_mul(k5,h/2.0,k1)));
  fun(k3,t+h/2.0,add(k4,x,s_mul(k5,h/2.0,k2)));
  fun(k8,t+h,add(k9,x,s_mul(k10,h,k3)));
  add (k4,s_mul(k5,1.0/6.0,k1),s_mul(k6,1.0/3.0,k2));
  add (k5,s_mul(k6,1.0/3.0,k3),s_mul(k7,1.0/6.0,k8));
  return add(r,k4,k5);
}



deqstruct*
deq_awainit( deqstruct *D,
	 skalar maximum_stepsize,
	 skalar minimum_stepsize,
	 skalar initial_stepsize,
	 method *tracking_method,
	 skalar (*ownnorm)(vector))
{
  D->max_step  = maximum_stepsize;
  D->min_step  = minimum_stepsize;
  D->step      = initial_stepsize;
  D->meth      = tracking_method->phi;
  D->ord       = tracking_method->ordnung;
  D->two2ord   = (double) (1 << D->ord);
  D->e_2_m     = 1.0 - 1.0/D->two2ord;
  D->e_2m      = 1.0/(D->two2ord-1.0);
  D->deqnorm   = ownnorm ? ownnorm : norm;
  D->allowdoub = 0;
  return D;
}


skalar
deq_awastep( deqstruct *D, deqfun *deqf,
	 vector r, skalar t, vector x, skalar delta, int use_step) {
  skalar  hn, nstep;
  avector eta, eta1, eta2, k1, k2;
  int breakflag = 0;
  

  fun = deqf;
	
  if( ! use_step) {  /* no step size regulation */
    add( r, x, s_mul(k1, delta, D->meth(k2, t, x, delta) ) );
    return delta;
  }

  /* step size regulation with richardsen extrapolation */

  if( D->allowdoub  && (nstep = D->step * 1.2) < D->max_step ) {
    D->step = nstep; /* Altes H vergroessern */
    D->allowdoub = 0;
  }

	
  for(;;) {
    add( eta1, x,s_mul(k1, D->step    , D->meth(k2,t,x ,D->step    ) ) );
    add( eta , x,s_mul(k1, D->step/2.0, D->meth(k2,t,x ,D->step/2.0) ) );
    add( eta2,eta,s_mul(k1,D->step/2.0,
			D->meth(k2,t+D->step/2.0,eta,D->step/2.0) ) );
    hn = D->deqnorm( sub(k1,eta1,eta2)) * D->e_2m;
      
    if( hn > delta && !breakflag) { /* Schrittweite zu gross ? */
      if( D->step == D->min_step )
	break;
      D->step *= 0.7;
      if( D->step < D->min_step) {
	D->step = D->min_step;
	breakflag = 1;
      }
      continue;
    } else if ( hn < delta * 0.4 )
      D->allowdoub = 1;
    break;
  }

  /* --- Richardsen - Extrapolation --- */
  sub( r,  s_mul( k2, D->two2ord * D->e_2m, eta2), s_mul(k1,D->e_2m,eta1));
  return D->step;
}	
		
