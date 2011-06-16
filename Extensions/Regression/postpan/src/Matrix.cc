// Based on Peter Mastromarino's Matrix class
// Inverse returns zero-size matrix if singular

#include "Matrix.hh"

#include <stdio.h>
#include <iostream>
#include <math.h>

using namespace std;

Matrix::Matrix(size_t nrows, size_t ncols)
  : fNRows(nrows)
  , fNCols(ncols)
{
  // Try to allocate the memory for the matrix.
  fA = new double[nrows * ncols];
}

Matrix::Matrix(size_t nrows)
  : fNRows(nrows)
  , fNCols(1)
{
  // Try to allocate the memory for the matrix.
  fA = new double[nrows];
}

Matrix::Matrix()
  : fA(0),
    fNRows(0),
    fNCols(0)
{
}

Matrix::Matrix (const Matrix& m)
{
  fNRows = m.fNRows;
  fNCols = m.fNCols;
  if (m.fA != 0)
    {
      fA = new double[fNRows * fNCols];
      for (size_t i = 0; i < fNRows; ++i)
	for (size_t j = 0; j < fNCols; ++j)
	  Assign (i, j, m.Get (i, j));
    }
  else
    fA = 0;
}

Matrix::~Matrix( )
{
  // Free up the memory block.
  delete [] fA;
}

Matrix&
Matrix::operator= (const Matrix& m)
{
  if (&m != this)
    {
      delete [] fA;
      fNRows = m.fNRows;
      fNCols = m.fNCols;
      if (m.fA != 0)
	{
	  fA = new double[fNRows * fNCols];
	  for (size_t i = 0; i < fNRows; ++i)
	    for (size_t j = 0; j < fNCols; ++j)
	      Assign (i, j, m.Get (i, j));
	}
    }
  else
    fA = 0;
  return *this;
}

Matrix
Matrix::Inverse(void) const
{
  Matrix inv;
  
  // This method only works on NxN matrices!
  if (fNRows != fNCols) {
    cerr << " Will not invert a non-square matrix!" << endl;
    return inv;
  }
  
  // Get the L and U matrices.
  Matrix l (fNRows, fNCols);
  Matrix u (fNRows, fNCols);
  
  bool stat = GetLUMatrices(l, u);

  if (!stat) {
    cerr << " Could not invert this matrix.  Returning NULL." << endl;
    return inv;
  }
  
  Matrix col(fNCols);
  
  inv = Matrix(fNRows, fNCols);
  
  for (size_t j = 0; j < fNCols; j++) {
    for (size_t i = 0; i < fNRows; i++) {
      col.Assign (i, 0.0);
    }
    col.Assign (j, 1.0);
    
    Matrix sol = LUBackSub(l, u, col);

    if (sol.NRows() == 0)
      {
	cerr << "Inverse ERROR: Matrix is singular" << endl;
	return Matrix();
      }

    for (size_t i = 0; i < fNRows; i++) {
      double solval = sol.Get(i);
      inv.Assign(i, j, solval);
    }
  }
  
  return inv;
}

bool
Matrix::GetLUMatrices(Matrix& l, Matrix& u) const
{
  if (l.NRows() != u.NRows()) {
    cerr << " Uhhh, we've got a problem here..." << endl;
    return false;
  }

  size_t N = l.NRows();

  for (size_t j = 0; j < N; j++) {
    l.Assign(j, j, 1.0);

    for (size_t i = 0; i <= j; i++) {
      double sum = 0.0;
      for (size_t k = 0; k < i; k++) {
	double a = l.Get(i, k);
	double b = u.Get(k, j);
	sum += a * b;
      }

      sum = fA[i*N + j] - sum;
      u.Assign(i, j, sum);
    }

    for (size_t i = j+1; i < N; i++) {
      double sum = 0.0;
      for (size_t k = 0; k < j; k++) {
	double a = l.Get(i, k);
	double b = u.Get(k, j);
	sum += a * b;
      }

      sum = fA[i*N + j] - sum;
      sum = sum / u.Get(j, j);
      l.Assign(i, j, sum);
    }
  }

  return true;
}

Matrix
Matrix::LUBackSub(Matrix& l, Matrix& u, const Matrix& col) const
{
  size_t N = l.NRows();

  for (size_t i = 0; i < N; i++) 
    {
      if (u.Get (i, i) == 0)
	{
	  cerr << "LUBackSub ERROR: Zero on diagonal" << endl;
	  return Matrix();
	}
    }

  double* y = new double[N];

  for (size_t i = 0; i < N; i++) {
    double sum = 0.0;
    for (size_t j = 0; j < i; j++) {
      double prod = l.Get(i, j) * y[j];
      sum += prod;
    }

    sum = col.Get(i) - sum;
    double yval = sum / l.Get(i, i);
    y[i] = yval;
  }

  Matrix sol(N);

  for (int id = N-1; id >= 0; id--) {
    double sum = 0.0;
    size_t i = id;
    for (size_t j = i+1; j < N; j++) {
      double prod = u.Get(i, j) * sol.Get(j);
      sum += prod;
    }

    sum = y[i] - sum;
    double solval = sum / u.Get(i, i);
    sol.Assign (i, solval);
  }

  delete [] y;
  return sol;
}

Matrix
Matrix::MultiplyBy(const Matrix& b) const
{
  if (b.NRows() != fNCols) {
    cerr << " cannot multiply a " << fNRows << "x" << fNCols << " matrix by a " << b.NRows() << "x" << b.NCols() << " matrix!" << endl;
    return Matrix();
  }

  size_t nrows = fNRows;
  size_t ncols = b.NCols();
  Matrix prod (nrows, ncols);

  for (size_t i = 0; i < nrows; i++) {
    for (size_t j = 0; j < ncols; j++) {
      double sum = 0.0;
      for (size_t k = 0; k < fNCols; k++) {
	double aval = fA[i*fNCols + k];
	double bval = b.Get(k, j);
	sum += aval * bval;
      }
      prod.Assign(i, j, sum);
    }
  }

  return prod;
}

Matrix
Matrix::Transpose() const
{
  size_t ncols = fNRows;
  size_t nrows = fNCols;
  Matrix trans (nrows, ncols);

  for (size_t i = 0; i < nrows; i++)
    for (size_t j = 0; j < ncols; j++)
      trans.Assign(i, j, Get(j,i));
  
  return trans;
}

double
Matrix::Get(const size_t i, const size_t j) const
{
  if (i >= fNRows || j >= fNCols) {
    return -1.0;
  }

  return fA[i * fNCols + j];
}

double
Matrix::Get(const size_t i) const
{
  if (i >= fNRows) {
    return -1.0;
  }

  return fA[i * fNCols];
}

void
Matrix::Assign(const size_t i, const size_t j, double val)
{
  if (i >= fNRows || j >= fNCols) {
    return;
  }

  fA[i * fNCols + j] = val;
}

void
Matrix::Assign(const size_t i, double val)
{
  if (i >= fNRows) {
    return;
  }

  fA[i * fNCols] = val;
}

void
Matrix::Print(void) const
{
  for (size_t i = 0; i < fNRows; i++) {
    for (size_t j = 0; j < fNCols; j++) {
      printf("%f  ", Get(i,j));
    }
    printf("\n");
  }
}
