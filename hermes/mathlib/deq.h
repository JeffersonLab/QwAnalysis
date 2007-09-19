#define MAXDIM 10

typedef double skalar, *vector;

typedef struct {
	double *(*phi)(vector r, skalar t, vector x, skalar h);
	int ordnung;
} method;

extern method methods[];

typedef struct {
  skalar step, e_2_m, e_2m, two2ord, max_step, min_step;
  int ord;
  skalar (*deqnorm)(vector);
  double *(*meth)(vector r, skalar t, vector x, skalar h);
  int allowdoub;
} deqstruct;


typedef vector deqfun( vector r, skalar t, vector x);
int deq_setdeqdim( int dim);
vector explicit_Euler (vector r, skalar t, vector x, skalar h);
vector improved_Euler (vector r, skalar t, vector x, skalar h);
vector simple_Runge ( vector r, skalar t, vector x ,skalar h)	;
vector Heum	( vector r, skalar t, vector x ,skalar h);
vector Runge_Kutta ( vector r, skalar t, vector x ,skalar h);
deqstruct* deq_awainit( deqstruct *D,
		    skalar maximum_stepsize,
		    skalar minimum_stepsize,
		    skalar initial_stepsize,
		    method *tracking_method,
		    skalar (*ownnorm)(vector));
skalar deq_awastep( deqstruct *D, deqfun *fun,
		vector r, skalar t, vector x, skalar h,int stepsizecorrection);
