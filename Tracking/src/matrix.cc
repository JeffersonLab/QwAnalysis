#include "matrix.h"

#define DBL_EPSILON 2.22045e-16

/*------------------------------------------------------------------------*//*!

 \fn double UNorm (double *A, int n, int m)

 \brief Calculates the least upper bound norm

*//*-------------------------------------------------------------------------*/
double UNorm (double *A, int n, int m)
{
  int counter;
  double help, Max = 0.0;
  double *B = A;
  while (n--) {
    counter = m;
    while (counter--) {
      B = A + n + counter;
      if (Max < (help = fabs (*(B))))
       Max = help;
      }
  }
  return Max;
}



/*------------------------------------------------------------------------*//*!

 \fn double *M_Cholesky (double *B, double *q, int n)

 \brief Calculates the Cholesky decomposition of the positive-definite matrix B

*//*-------------------------------------------------------------------------*/
double *M_Cholesky (double *B, double *q, int n)
{
  int i, j, k;
  double sum, min;
  double *C = B;
  double A[n][n];
  double p[n],*pp;

  for(i=0;i<n;i++){
    for(j=0;j<n;j++){
      A[i][j]=*C;
      C++;
    }
  }

  min = UNorm (B, n, n)  * n * DBL_EPSILON;
  for( i = 0; i < n; i++ ) {
    for( j = i; j < n; j++ ) {
      sum = A[i][j];
      for( k = i-1; k >= 0; k-- )
       sum -= A[i][k] * A[j][k];
      if( i == j ) {
       if( sum < min ) {
         return 0;
       }
       p[i] = sqrt(sum);
      } else
       A[j][i] = sum/p[i];
    }
  }
  C = B;
  pp=q;
  for(i=0;i<n;i++){
    *p = p[i];
    pp++;
    for(j=0;j<n;j++){
      *C = A[i][j];
      C++;
    }
  }
  C = B;
  pp = q;
  return B;
}



/*------------------------------------------------------------------------*//*!

 \fn void RowMult(double a, double *A, double b, double *B, int n)

 \brief Calculates a*A + b*B, where both A and B are rows of length n

*//*-------------------------------------------------------------------------*/
void RowMult (double a, double *A, double b, double *B, int n)
{
  for (int i = 0; i < n; i++)
    A[i] = a*A[i] + b*B[i];

  return;
}



/*------------------------------------------------------------------------*//*!

 \fn void M_Print (double *A, int n)

 \brief Print matrix A of dimension [n,n] to cout

*//*-------------------------------------------------------------------------*/
void M_Print (double *A, int n)
{
  // Print matrix A
  for (int i = 0; i < n; i++) {
    std::cout << "[";
    for (int j = 0; j < n; j++) {
      std::cout << A[i*n+j] << ' ';
    }
    std::cout << ']' << std::endl;
  }
  std::cout << std::endl;

  return;
}



/*------------------------------------------------------------------------*//*!

 \fn void M_Print (double *A, double *B, int n)

 \brief Print matrices A and B of dimension [n,n] to cout

*//*-------------------------------------------------------------------------*/
void M_Print (double *A, double *B, int n)
{
  // Print matrices A and B
  for (int i = 0; i < n; i++) {
    std::cout << "[";
    for (int j = 0; j < n; j++) {
      std::cout << A[i*n+j] << ' ';
    }
    std::cout << '|' ;
    for (int j = 0; j < n; j++) {
      std::cout << B[i*n+j] << ' ';
    }
    std::cout << ']' << std::endl;
  }
  std::cout << std::endl;

  return;
}



/*------------------------------------------------------------------------*//*!

 \fn double *M_Unit (double *A, int n)

 \brief Creates unit matrix A

*//*-------------------------------------------------------------------------*/
double *M_Unit (double *A, int n)
{
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      A[i*n+j] = 0;
    }
    A[i*n+i] = 1;
  }

  return A;
}



/*------------------------------------------------------------------------*//*!

 \fn double *M_Zero (double *A, int n)

 \brief Creates zero matrix A

*//*-------------------------------------------------------------------------*/
double *M_Zero (double *A, int n)
{
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      A[i*n+j] = 0;
    }
  }

  return A;
}



/*------------------------------------------------------------------------*//*!

 \fn double *M_Invert (double *Ap, double *Bp, int n)

 \brief Matrix inversion of Ap will be stored in Bp

*//*-------------------------------------------------------------------------*/
double *M_Invert (double *Ap, double *Bp, int n)
{
  double *row1, *row2;
  double a, b;

  // Initialize B to unit matrix (wdc: assumed n = 4 here)
  M_Unit (Bp, n);

  // Print matrices Ap and Bp
  //M_Print (Ap, Bp, n);

  // Calculate inverse matrix using row-reduce method
  // (wdc: TODO  faster algs could be useful later)
  if(n == 4) { // this will not be generalized for nxn matrices.

    // get A10 to An0 to be zero.
    for (int i = 1; i < n; i++) {
      a = -(Ap[0]);
      b = Ap[i*n];
      row1 = &(Ap[i*n]);
      row2 = &(Ap[0]);
      RowMult(a, row1, b, row2, 4);
      row1 = &(Bp[i*n]);
      row2 = &(Bp[0]);
      RowMult(a, row1, b, row2, 4);
    }

    // get A21 and A31 to be zero
    for (int i = 2; i < n; i++) {
      a = -(Ap[5]);
      b = Ap[i*n+1];
      row1 = &(Ap[i*n]);
      row2 = &(Ap[4]);
      RowMult(a, row1, b, row2, 4);
      row1 = &(Bp[i*n]);
      row2 = &(Bp[4]);
      RowMult(a, row1, b, row2, 4);
    }

    // get A32 to be zero
    a = -(Ap[10]);
    b = Ap[14];
    row1 = &(Ap[12]);
    row2 = &(Ap[8]);
    RowMult(a, row1, b, row2, 4);
    row1 = &(Bp[12]);
    row2 = &(Bp[8]);
    RowMult(a, row1, b, row2, 4);

    // Now the matrix is upper right triangular.

    // row reduce the 4th row
    a = 1/(Ap[15]);
    row1 = &(Ap[12]);
    RowMult(a, row1, 0, row2, 4);
    row1 = &(Bp[12]);
    RowMult(a, row1, 0, row2, 4);

    // get A03 to A23 to be zero
    for (int i = 0; i < n-1; i++) {
      a = -(Ap[15]);
      b = Ap[i*n+3];
      row1 = &(Ap[i*n]);
      row2 = &(Ap[12]);
      RowMult(a, row1, b, row2, 4);
      row1 = &(Bp[i*n]);
      row2 = &(Bp[12]);
      RowMult(a, row1, b, row2, 4);
    }

    // row reduce the 3rd row
    a = 1/(Ap[10]);
    row1 = &(Ap[8]);
    RowMult(a, row1, 0, row2, 4);
    row1 = &(Bp[8]);
    RowMult(a, row1, 0, row2, 4);

    // get A02 and A12 to be zero
    for (int i = 0; i < 2; i++) {
      a = -(Ap[10]);
      b = Ap[i*n+2];
      row1 = &(Ap[i*n]);
      row2 = &(Ap[8]);
      RowMult(a, row1, b, row2, 4);
      row1 = &(Bp[i*n]);
      row2 = &(Bp[8]);
      RowMult(a, row1, b, row2, 4);
    }

    // row reduce the 2nd row
    a = 1/(Ap[5]);
    row1 = &(Ap[4]);
    RowMult(a, row1, 0, row2, n);
    row1 = &(Bp[4]);
    RowMult(a, row1, 0, row2, n);

    // get A01 to be zero
    a = -(Ap[5]);
    b = Ap[1];
    row1 = &(Ap[0]);
    row2 = &(Ap[4]);
    RowMult(a, row1, b, row2, n);
    row1 = &(Bp[0]);
    row2 = &(Bp[4]);
    RowMult(a, row1, b, row2, n);

    // row reduce 1st row
    a = 1/(Ap[0]);
    row1 = &(Ap[0]);
    RowMult(a, row1, 0, row2, n);
    row1 = &(Bp[0]);
    RowMult(a, row1, 0, row2, n);
  }

  // Print matrices Ap and Bp
  //M_Print (Ap, Bp, n);

  return Bp;
}



/*------------------------------------------------------------------------*//*!

 \fn double *M_InvertPos (double *B, int n)

 \brief Calculates a*A + b*B, where both A and B are rows of length n

*//*-------------------------------------------------------------------------*/
double *M_InvertPos (double *B, int n)
{
  double sum;
  double p[n];
  double q[n];
  double inv[n][n];
  double *pp = NULL;
  double A[n][n],*C;

  // First we find the cholesky decomposition of B
  if (M_Cholesky(B, pp, n)) {

    C = B;
    for (int i = 0; i < n; i++) {
      p[i] = *q;
      for (int j = 0; j < n; j++ ) {
        A[i][j] = *C;
        C++;
      }
    }

    for (int i = 0; i < n; i++) {
      A[i][i] = 1.0 / p[i];
      for (int j = i+1; j < n; j++) {
	sum = 0;
	for (int k = i; k < j; k++)
	  sum -= A[j][k] * A[k][i];
	A[j][i] = sum / p[j];
      }
    }

    for (int i = 0; i < n; i++) {
      for (int j = i; j < n; j++) {
	sum = 0;
	for (int k = j; k < n; k++)
	  sum += A[k][i] * A[k][j];
	inv[i][j] = inv[j][i] = sum;
      }
    }

    C = B;
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
	*C = inv[i][j];
        C++;
      }
    }

  } else {
    B = 0;
    std::cout << "Cholesky decomposition failed!" << std::endl;
  }

  C = B;

  return B;
}



/*------------------------------------------------------------------------*//*!

 \fn double *M_A_times_b (double *y, double *A, int n, int m, double *b)

 \brief Calculates y[n] = A[n,m] * b[m], with dimensions indicated

*//*-------------------------------------------------------------------------*/
double *M_A_times_b (double *y, double *A, int n, int m, double *b)
{
  for (int i = 0; i < n; i++) {
    y[i] = 0;
    for (int j = 0; j < m; j++) {
      y[i] += A[i*n+j] * b[j];
    }
  }
  return y;
}
