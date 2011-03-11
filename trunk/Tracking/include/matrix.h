#ifndef MATRIX_H
#define MATRIX_H

// Standard C and C++ headers
#include <fstream>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cassert>
#include <cstring>

// Boost uBLAS (linear algebra) headers
#ifndef BOOST_VERSION
#include <boost/version.hpp>
#endif
#if BOOST_VERSION <= 103200
//    Boost 1.32.00 needs explicit includes of "functional.hpp"
//    and "operation.hpp".
//    They are not needed  for Boost 1.34.01, but don't really hurt
//    if we wanted to just include them unconditionally.
#include <boost/numeric/ublas/config.hpp>
#include <boost/numeric/ublas/storage.hpp>
#include <boost/numeric/ublas/functional.hpp>
#include <boost/numeric/ublas/operation.hpp>
#endif
//
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>


// Matrix inversion routines optimized for 4 x 4 matrices
double UNorm (const double *A, int n, const int m);
double* M_Cholesky (double *B, double *q, const int n);
double* M_InvertPos (double *B, const int n);
double* M_Invert (double *Ap, double *Bp, const int n);
void RowMult (const double a, double *A, const double b, const double *B, const int n);
double* M_A_times_b (double *y, const double *A, const int n, const int m, const double *b);
double* M_Zero (double *A, const int n);
double* M_Unit (double *A, const int n);
void M_Print (const double *A, const int n);
void M_Print (const double *A, const double *B, const int n);

double *V_Zero (double *v, const int n);
double *V_Unit (double *v, const int n, int k);
void V_Print (const double *v, const int n);
void V_Print (const double *v, const double *w, const int n);


/*------------------------------------------------------------------------*//*!

 The following code inverts the matrix input using LU-decomposition with
 backsubstitution of unit vectors.

 References:
 Numerical Recipies in C, 2nd ed., by Press, Teukolsky, Vetterling & Flannery.
 http://www.crystalclearsoftware.com/cgi-bin/boost_wiki/wiki.pl?LU_Matrix_Inversion

 Note:
 This function is relatively slow (around 230 usec on my laptop for a 4 x 4 matrix,
 compared to less than 10 usec using the functions above), but on a total processing
 time per event of 0.1 sec this does not make any difference and greatly improves
 maintainability.  If profiling reveals that this matrix inversion is a bottle neck,
 we should move back to the optimized functions.

*//*-------------------------------------------------------------------------*/

template<class T>
boost::numeric::ublas::matrix<T> invert (boost::numeric::ublas::matrix<T> & input)
{
  using namespace boost::numeric::ublas;

  // Initialize the inverse to a zero matrix
  matrix<T> inverse(zero_matrix<T>(input.size1()));

  // Create a working copy of the input and inverse
  matrix<T> A(input);
  // Create a permutation matrix for the LU-factorization
  permutation_matrix<std::size_t> pm(A.size1());

  // Perform LU-factorization
  int res = lu_factorize(A, pm);
  // Return the zero matrix if this fails
  if (res != 0) return inverse;

  // Create identity matrix of "inverse"
  inverse.assign(identity_matrix<T>(A.size1()));

  // Backsubstitute to get the inverse
  lu_substitute(A, pm, inverse);

  return inverse;
}


#endif // MATRIX_H
