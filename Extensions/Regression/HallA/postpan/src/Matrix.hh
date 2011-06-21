// Based on Peter Mastromarino's Matrix class
// Inverse returns zero-size matrix if singular

#include <stddef.h>

class Matrix {

public:
  // Constructors

  // default constructor
  Matrix(size_t rows, size_t ncols);
  Matrix(size_t rows);
  Matrix();
  Matrix (const Matrix& m);

  // Destructor
  ~Matrix( );

  // Operator
  Matrix& operator= (const Matrix& m);

  // Operating functions
  Matrix Inverse(void) const;
  bool GetLUMatrices(Matrix& l, Matrix& u) const;
  Matrix LUBackSub(Matrix& l, Matrix& u, const Matrix& col) const;
  Matrix MultiplyBy(const Matrix& b) const;
  Matrix Transpose() const;

  // accessors to data items
  inline size_t NRows(void) const {return fNRows;}
  inline size_t NCols(void) const {return fNCols;}
  double Get(const size_t i, const size_t j) const;
  double Get(const size_t i) const;
  void Assign(const size_t i, const size_t j, double val);
  void Assign(const size_t i, double val);

  // display function
  void Print(void) const;

private:
  double* fA;
  size_t fNRows;
  size_t fNCols;
};

