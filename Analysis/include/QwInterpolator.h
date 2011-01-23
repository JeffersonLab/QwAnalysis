#ifndef _QWINTERPOLATOR_H_
#define _QWINTERPOLATOR_H_

// System headers
#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <stdint.h>

// Qweak headers
#include "QwLog.h"

/// Allowed interpolation methods
enum EQwInterpolationMethod {
  kInterpolationMethodUnknown,
  kMultiLinear,
  kNearestNeighbor
};

// Type of grid coordinates
typedef double coord_t;
// NOTE This could trivially be made into a template argument, but that seems
// hardly worth it considering the small gains in going to different types.

// NOTE The normalized cell-local coordinates are always in double precision,
// and the returned interpolated values are also in double.

/**
 *  \class QwInterpolator
 *  \ingroup QwAnalysis
 *  \brief A multi-dimensional grid of values with interpolation methods
 *
 * This class provides various interpolation methods on a multi-dimensional grid
 * of multi-dimensional values.  Linear interpolation and nearest-neighbor are
 * implemented for all dimensions.
 *
 * The template arguments are the internal storage data type (defaults to float)
 * and the number of dimensions of the stored data at each grid point (defaults
 * to one).  The dimension of the grid itself is set through the constructor.
 * To describe a double vector field with 5 components on a 3-dimensional grid,
 * you would write
 * \code
 * QwInterpolate<float,5> *field = new QwInterpolator<float,5>(3);
 * \endcode
 *
 * The minimum, maximum, and step size of the grid have to be known before
 * the values are filled.
 */
template <class value_t = float, unsigned int value_n = 1>
class QwInterpolator {

  public: // constructors and destructor

    /// Constructor with number of dimensions
    QwInterpolator(const unsigned int ndim) {
      SetDimensions(ndim);
      SetInterpolationMethod(kMultiLinear);
    };
    /// Constructor with minimum, maximum, and step size
    QwInterpolator(const std::vector<coord_t> min, const std::vector<coord_t> max, const std::vector<coord_t> step) {
      SetDimensions(min.size());
      SetInterpolationMethod(kMultiLinear);
      SetMinimumMaximumStep(min,max,step);
    };
    /// Constructor with file name
    QwInterpolator(const std::string filename) {
      ReadBinaryFile(filename);
    };
    /// Destructor
    virtual ~QwInterpolator() { };

  private: // private member fields

    /// Number of dimensions in coordinates
    unsigned int fNDim;

    /// Minimum in each dimension
    std::vector<coord_t> fMin;
    /// Maximum in each dimension
    std::vector<coord_t> fMax;
    /// Step size in each dimension
    std::vector<coord_t> fStep;
    /// Number of points in each dimension
    std::vector<size_t> fSize;
    /// Linear extent between neighbor points in each dimension (e.g. for the
    /// least significant index this will be 1, for the next index the number
    /// of points in the first index, etc...)
    std::vector<size_t> fExtent;

    /// Table with pointers to arrays of values
    std::vector<value_t> fValues[value_n];

    /// Number of values read in
    unsigned int fEntries;
    /// Maximum number of values
    unsigned int fMaximumEntries;

    /// Interpolation method
    EQwInterpolationMethod fInterpolationMethod;

  public: // public methods

    /// Set the number of coordinate dimensions and resize vectors
    void SetDimensions(const unsigned int ndim) {
      if (ndim == 0) {
        QwError << "Dimensions of the grid should be strictly positive!" << QwLog::endl;
        return;
      }
      fNDim = ndim;
      fMin.resize(fNDim); fMax.resize(fNDim); fStep.resize(fNDim);
      fSize.resize(fNDim); fExtent.resize(fNDim+1);
    };
    /// Set minimum, maximum, and step size to single values
    void SetMinimumMaximumStep(const coord_t min, const coord_t max, const coord_t step) {
      SetMinimumMaximumStep(std::vector<coord_t>(fNDim,min), std::vector<coord_t>(fNDim,max), std::vector<coord_t>(fNDim,step));
    };
    /// Set minimum, maximum, and step size to different values
    void SetMinimumMaximumStep(const std::vector<coord_t> min, const std::vector<coord_t> max, const std::vector<coord_t> step) {
      // Set the dimensionality
      if (min.size() != fNDim) SetDimensions(min.size());
      // Check the dimensionality and assign boundaries and step size vectors
      if (min.size() != fNDim || min.size() != fNDim || step.size() != fNDim) return;
      fMin = min; fMax = max; fStep = step;
      fExtent[0] = 1;
      for (size_t i = 0; i < fMin.size(); i++) {
        coord_t int_part; // safer to use modf than a direct static cast
        coord_t frac_part = modf((fMax[i] - fMin[i]) / fStep[i] + 1.0, &int_part);
        fSize[i] = static_cast<unsigned int>(int_part) + (frac_part > 0.5 ? 1 : 0);
        fExtent[i+1] = fExtent[i] * fSize[i];
      }
      // Try resizing to allocate memory and initialize with zeroes
      fMaximumEntries = fExtent[fNDim]; fEntries = 0; // no entries read yet
      for (unsigned int i = 0; i < value_n; i++) {
        try {
          fValues[i].resize(fMaximumEntries,0);
        } catch (std::bad_alloc) {
          QwError << "QwInterpolator could not allocate memory for values!" << QwLog::endl;
        }
      }
    };
    /// Get minimum in dimension
    coord_t GetMinimum(const unsigned int dim) const { return fMin.at(dim); };
    /// Get maximum in dimension
    coord_t GetMaximum(const unsigned int dim) const { return fMax.at(dim); };
    /// Get minimum in dimension
    coord_t GetStepSize(const unsigned int dim) const { return fStep.at(dim); };
    /// Get the maximum number of entries
    unsigned int GetMaximumEntries() const { return fMaximumEntries; };
    /// Get the current number of entries
    unsigned int GetCurrentEntries() const { return fEntries; };


    /// Set the interpolation method
    void SetInterpolationMethod(const EQwInterpolationMethod method)
      { fInterpolationMethod = method; };
    /// Get the interpolation method
    EQwInterpolationMethod GetInterpolationMethod() const
      { return fInterpolationMethod; };


    /// Return true if the coordinate is in bounds
    bool InBounds(const coord_t* coord) const {
      return Check(coord);
    };


    /// \name Functions to write grid values
    // @{
    /// Set a series of single values on a on-dimensional grid
    bool Set(const unsigned int n, const coord_t* r, const value_t* p) {
      if (value_n != 1) return false; // only for one-dimensional values
      if (fNDim != 1)   return false; // only for one-dimensional grids
      for (unsigned int i = 0; i < n; i++)
        if (! Set(r[i],p[i])) return false;
      return true;
    }

    /// Set a single value at a coordinate (false if not possible)
    bool Set(const coord_t& coord, const value_t& value) {
      if (value_n != 1) return false; // only for one-dimensional values
      if (fNDim != 1) return false; // only for one-dimensional grids
      return Set(&coord, &value);
    };
    /// Set a single value at a coordinate (false if not possible)
    bool Set(const coord_t* coord, const value_t& value) {
      if (value_n != 1) return false; // only for one-dimensional values
      return Set(coord, &value);
    };
    /// Set a set of values at a coordinate (false if not possible)
    bool Set(const coord_t* coord, const value_t* value) {
      unsigned int cell_index[fNDim];
      Nearest(coord, cell_index); // nearest cell
      if (! Check(cell_index)) return false; // out of bounds
      unsigned int linear_index = Index(cell_index);
      if (! Check(linear_index)) return false; // out of bounds
      for (unsigned int i = 0; i < value_n; i++)
        fValues[i][linear_index] = value[i];
      fEntries++;
      return true;
    };

    /// Set a single value at a linearized index (false if not possible)
    bool Set(const unsigned int linear_index, const value_t& value) {
      if (value_n != 1) return false; // only for one-dimensional values
      return Set(linear_index, &value);
    };
    /// Set a set of values at a linearized index (false if not possible)
    bool Set(const unsigned int linear_index, const value_t* value) {
      if (! Check(linear_index)) return false; // out of bounds
      for (unsigned int i = 0; i < value_n; i++)
        fValues[i][linear_index] = value[i];
      fEntries++;
      return true;
    };

    /// Set a single value at a grid point (false if out of bounds)
    bool Set(const unsigned int* cell_index, const value_t& value) {
      if (value_n != 1) return false; // only for one-dimensional values
      return Set(cell_index, &value);
    };
    /// Set a set of values at a grid point (false if out of bounds)
    bool Set(const unsigned int* cell_index, const value_t* value) {
      if (! Check(cell_index)) return false; // out of bounds
      for (unsigned int i = 0; i < value_n; i++)
        fValues[i][Index(cell_index)] = value[i];
      return true;
    };
    // @}


    /// \name Functions to retrieve interpolated values
    // @{
    /// Get the interpolated value at coordinate (zero if out of bounds)
    value_t GetValue(const coord_t& coord) const {
      if (value_n != 1) return false; // only for one-dimensional values
      if (fNDim != 1)   return false; // only for one-dimensional grids
      value_t value;
      if (GetValue(&coord, &value)) return value;
      else return 0; // interpolation failed
    };
    /// Get the interpolated value at coordinate (zero if out of bounds)
    value_t GetValue(const coord_t* coord) const {
      if (value_n != 1) return 0; // only for one-dimensional values
      value_t value;
      if (GetValue(coord, &value)) return value;
      else return 0; // interpolation failed
    };
    /// Get the interpolated value at coordinate (zero if out of bounds)
    bool GetValue(const coord_t* coord, double& value) const {
      if (value_n != 1) return false; // only for one-dimensional values
      return GetValue(coord, &value);
    };
    /// Get the interpolated value at coordinate (zero if out of bounds)
    bool GetValue(const coord_t* coord, double* value) const {
      for (unsigned int i = 0; i < value_n; i++) value[i] = 0.0; // zero
      if (! Check(coord)) return false; // out of bounds
      value_t result[value_n]; // we need a local copy of type value_t
      switch (fInterpolationMethod) {
        // return interpolation status
        case kMultiLinear:
          if (! Linear(coord, result)) return false;
          break;
        case kNearestNeighbor:
          if (! NearestNeighbor(coord, result)) return false;
          break;
        default: return false;
      }
      for (unsigned int i = 0; i < value_n; i++) value[i] = result[i]; // cast
      return true;
    };
    // @}


    /// \name File reading and writing
    // @{
    /// \brief Write the grid as text
    bool WriteText(std::ostream& stream) const;
    /// Write the grid to text file
    bool WriteTextFile(std::string filename) const {
      std::ofstream file(filename.c_str());
      if (! file.is_open()) return false;
      WriteText(file);
      file.close();
      return true;
    };
    /// Write the grid to screen
    bool WriteTextScreen() const {
      WriteText(std::cout);
      return true;
    };
    /// \brief Read the grid from text
    bool ReadText(std::istream& stream);
    /// Read the grid from text file
    bool ReadTextFile(std::string filename) {
      std::ifstream file(filename.c_str());
      if (! file.is_open()) return false;
      if (! ReadText(file)) return false;
      file.close();
      return true;
    };
    /// \brief Write the grid values to binary file
    bool WriteBinaryFile(std::string filename) const;
    /// \brief Read the grid values from binary file
    bool ReadBinaryFile(std::string filename);
    // @}


    /// \name Indexing functions (publicly available and unchecked)
    // @{
    /// Return the linearized index based on the point coordinates (unchecked)
    unsigned int Index(const coord_t* coord) const {
      unsigned int cell_index[fNDim];
      Cell(coord, cell_index);
      return Index(cell_index);
    };

    /// \brief Return the linearized index based on the cell indices (unchecked)
    unsigned int Index(const unsigned int* cell_index) const;
    /// \brief Return the linearized index based on the cell indices and offset (unchecked)
    unsigned int Index(const unsigned int* cell_index, const unsigned int offset) const;

    /// \brief Return the cell index and local coordinates in one dimension (unchecked)
    void Cell(const coord_t coord, unsigned int& cell_index, double& cell_local, const unsigned int dim) const;
    /// \brief Return the cell index and local coordinates (unchecked)
    void Cell(const coord_t* coord, unsigned int* cell_index, double* cell_local) const;
    /// \brief Return the cell index (unchecked)
    void Cell(const coord_t* coord, unsigned int* cell_index) const;
    /// \brief Return the cell index based on the linearized index (unchecked)
    void Cell(unsigned int linear_index, unsigned int* cell_index) const;

    /// \brief Return the coordinates based on the cell index (unchecked)
    void Coord(const unsigned int* cell_index, coord_t* coord) const;
    /// \brief Return the coordinates based on the linearized index (unchecked)
    void Coord(const unsigned int linear_index, coord_t* coord) const;
    // @}

  private: // private methods

    /// Return the cell index closest to the coordinate (could be above) (unchecked)
    void Nearest(const coord_t* coord, unsigned int* cell_index) const {
      double cell_local[fNDim];
      Cell(coord, cell_index, cell_local);
      // Loop over all dimensions and add one if larger than 0.5
      for (unsigned int dim = 0; dim < fNDim; dim++)
        if (cell_local[dim] > 0.5) cell_index[dim]++;
    };

    /// \brief Linear interpolation (unchecked)
    bool Linear(const coord_t* coord, value_t* value) const;
    /// \brief Nearest-neighbor (unchecked)
    bool NearestNeighbor(const coord_t* coord, value_t* value) const;

    /// \brief Check for boundaries with coordinate argument
    bool Check(const coord_t* coord) const;
    /// \brief Check for boundaries with cell index argument
    bool Check(const unsigned int* cell_index) const;
    /// \brief Check for boundaries with linearized index argument
    bool Check(const unsigned int linear_index) const;

    /// Get a single value by cell index (unchecked)
    value_t Get(const unsigned int* cell_index) const {
      if (value_n != 1) return 0; // only for one-dimensional values
      return fValues[0][Index(cell_index)];
    };

    /// Get a single value by linearized index (unchecked)
    value_t Get(const unsigned int index) const {
      return fValues[0][index];
    };
    /// Get a vector value by linearized index (unchecked)
    bool Get(const unsigned int index, value_t* value) const {
      for (unsigned int i = 0; i < value_n; i++)
        value[i] = fValues[i][index];
      return true;
    };

}; // class QwInterpolator



/**
 * Perform the multi-dimensional linear interpolation (unchecked)
 * @param coord Point coordinates
 * @param value Interpolated value (reference)
 */
template <class value_t, unsigned int value_n>
inline bool QwInterpolator<value_t,value_n>::Linear(
	const coord_t* coord,
	value_t* value) const
{
  // Get cell and local normalized coordinates
  unsigned int cell_index[fNDim];
  double cell_local[fNDim];
  Cell(coord, cell_index, cell_local);
  // Initialize to zero
  for (unsigned int i = 0; i < value_n; i++) value[i] = 0.0;
  // Calculate the interpolated value
  // by summing the 2^fNDim = 1 << fNDim neighbor points (1U is unsigned one)
  for (unsigned int offset = 0; offset < (1U << fNDim); offset++) {
    value_t neighbor[value_n];
    if (! Get(Index(cell_index,offset), neighbor)) return false;
    // ... with appropriate weighting factors (1 - cell_local or cell_local)
    double fac = 1.0;
    for (unsigned int dim = 0; dim < fNDim; dim++)
      fac *= ((offset >> dim) & 0x1)? cell_local[dim] : (1 - cell_local[dim]);
    // ... for all vector components
    for (unsigned int i = 0; i < value_n; i++)
      value[i] += fac * neighbor[i];
  }
  return true;
}

/**
 * Perform the nearest-neighbor interpolation (unchecked)
 * @param coord Point coordinates
 * @param value Interpolated value (reference)
 */
template <class value_t, unsigned int value_n>
inline bool QwInterpolator<value_t,value_n>::NearestNeighbor(
	const coord_t* coord,
	value_t* value) const
{
  // Get nearest cell
  unsigned int cell_index[fNDim];
  Nearest(coord, cell_index);
  return Get(Index(cell_index), value);
}

/**
 * Check whether the point is inside the valid region
 * @param coord Point coordinates
 * @return True if the coordinates are in the valid region
 */
template <class value_t, unsigned int value_n>
inline bool QwInterpolator<value_t,value_n>::Check(const coord_t* coord) const
{
  for (unsigned int dim = 0; dim < fNDim; dim++)
    if (coord[dim] < fMin[dim] || coord[dim] > fMax[dim])
      return false;
  // Otherwise
  return true;
}

/**
 * Check whether the cell index is inside the valid region
 * @param cell_index Cell index
 * @return True if the cell index is in the valid region
 */
template <class value_t, unsigned int value_n>
inline bool QwInterpolator<value_t,value_n>::Check(const unsigned int* cell_index) const
{
  for (unsigned int dim = 0; dim < fNDim; dim++)
    if (cell_index[dim] >= fSize[dim])
      return false;
  // Otherwise
  return true;
}

/**
 * Check whether the linearized index is inside the valid region
 * @param linear_index Linearized index
 * @return True if the cell index is in the valid region
 */
template <class value_t, unsigned int value_n>
inline bool QwInterpolator<value_t,value_n>::Check(const unsigned int linear_index) const
{
  if (linear_index >= fMaximumEntries)
    return false;
  // Otherwise
  return true;
}

/**
 * Return the linearized index based on the cell indices (unchecked)
 * @param cell_index Index in each dimension
 * @return Linearized index
 */
template <class value_t, unsigned int value_n>
inline unsigned int QwInterpolator<value_t,value_n>::Index(
	const unsigned int* cell_index) const
{
  unsigned int linear_index = 0;
  // Loop over dimensions
  for (unsigned int dim = 0; dim < fNDim; dim++)
    // ... and use the stored extents for every dimensions
    linear_index += fExtent[dim] * cell_index[dim];
  return linear_index;
}

/**
 * Return the linearized index based on the cell indices and offset (unchecked)
 * @param cell_index Index in each dimension
 * @param pattern Bit pattern with offsets in each dimension
 * @return Linearized index
 */
template <class value_t, unsigned int value_n>
inline unsigned int QwInterpolator<value_t,value_n>::Index(
	const unsigned int* cell_index,
	const unsigned int pattern) const
{
  unsigned int linear_index = 0;
  // Loop over dimensions
  for (unsigned int dim = 0; dim < fNDim; dim++) {
    // Add offset of one based on the bit at position dim
    unsigned int offset = (pattern >> dim) & 0x1;
    linear_index += fExtent[dim] * (cell_index[dim] + offset);
  }
  return linear_index;
}

/**
 * Return the cell index and local coordinates in one dimension (unchecked)
 * @param coord Point coordinate in one dimension
 * @param cell_index Cell index of the point (reference)
 * @param cell_local Local coordinates in cell (reference)
 * @param dim Dimension
 */
template <class value_t, unsigned int value_n>
inline void QwInterpolator<value_t,value_n>::Cell(
	const coord_t coord,
	unsigned int &cell_index,
	double &cell_local,
	const unsigned int dim) const
{
  // Normalize coordinate (positive, with integers on grid points)
  double norm_coord = (coord - fMin[dim]) / fStep[dim];
  // Split in fractional and integer part
  double frac_part, int_part;
  frac_part = modf(norm_coord, &int_part);
  cell_local = frac_part;
  cell_index = static_cast<int>(int_part); // cast to integer
}

/**
 * Return the cell index and local coordinates (unchecked)
 * @param coord Point coordinates
 * @param cell_index Cell index of the point (reference)
 * @param cell_local Local coordinates in cell (reference)
 */
template <class value_t, unsigned int value_n>
inline void QwInterpolator<value_t,value_n>::Cell(
	const coord_t* coord,
	unsigned int* cell_index,
	double* cell_local) const
{
  // Get cell index and local coordinates in each dimension
  for (unsigned int dim = 0; dim < fNDim; dim++)
    Cell(coord[dim], cell_index[dim], cell_local[dim], dim);
}

/**
 * Return the cell index (unchecked)
 * @param coord Point coordinates
 * @param cell_index Cell index of the point (reference)
 */
template <class value_t, unsigned int value_n>
inline void QwInterpolator<value_t,value_n>::Cell(
	const coord_t* coord,
	unsigned int* cell_index) const
{
  // Get cell index and ignore local coordinates
  double cell_local[fNDim];
  Cell(coord, cell_index, cell_local);
}

/**
 * Return the cell index based on the linearized index (unchecked)
 * @param linear_index Linearized index
 * @param cell_index Cell index (reference)
 */
template <class value_t, unsigned int value_n>
inline void QwInterpolator<value_t,value_n>::Cell(
	unsigned int linear_index,
	unsigned int* cell_index) const
{
  // This does not work with unsigned int, because that is always >= 0 and wraps
  for (int dim = fNDim-1; dim >= 0; dim--) {
    cell_index[dim] = linear_index / fExtent[dim];
    linear_index -= cell_index[dim] * fExtent[dim];
  }
}

/**
 * Return the coordinates based on the cell index (unchecked)
 * @param cell_index Cell index
 * @param coord Point coordinates (reference)
 */
template <class value_t, unsigned int value_n>
inline void QwInterpolator<value_t,value_n>::Coord(
	const unsigned int* cell_index,
	coord_t* coord) const
{
  for (unsigned int dim = 0; dim < fNDim; dim++)
    coord[dim] = fMin[dim] + cell_index[dim] * fStep[dim];
}

/**
 * Return the coordinates based on the linearized index (unchecked)
 * @param linear_index Linearized index
 * @param coord Point coordinates (reference)
 */
template <class value_t, unsigned int value_n>
inline void QwInterpolator<value_t,value_n>::Coord(
	const unsigned int linear_index,
	coord_t* coord) const
{
  unsigned int cell_index[fNDim];
  Cell(linear_index,cell_index);
  Coord(cell_index,coord);
}

/**
 * Write the grid values to a text stream
 * @param stream Output stream
 */
template <class value_t, unsigned int value_n>
inline bool QwInterpolator<value_t,value_n>::WriteText(std::ostream& stream) const
{
  // Write the dimensions
  stream << fNDim << "\t" << value_n << std::endl;
  // Write the grid parameters
  for (unsigned int dim = 0; dim < fNDim; dim++)
    stream << dim << "\t" << fMin[dim] << "\t" << fMax[dim] << "\t" << fStep[dim] << std::endl;
  // Write the values
  stream << fValues[0].size() << std::endl;
  for (unsigned int index = 0; index < fValues[0].size(); index++) {
    for (unsigned int i = 0; i < value_n; i++)
      stream << fValues[i][index] << "\t";
    stream << std::endl;
  }
  stream << "end" << std::endl;
  return true;
}

/**
 * Read the grid values from a text stream
 * @param stream Input stream
 * @return True if successfully read all values
 */
template <class value_t, unsigned int value_n>
inline bool QwInterpolator<value_t,value_n>::ReadText(std::istream& stream)
{
  // Read the dimensions
  unsigned int n;
  stream >> fNDim >> n;
  if (n != value_n) return false; // not same dimensionality
  SetDimensions(fNDim);
  // Read the grid parameters
  for (unsigned int dim = 0; dim < fNDim; dim++)
    stream >> dim >> fMin[dim] >> fMax[dim] >> fStep[dim];
  SetMinimumMaximumStep(fMin, fMax, fStep);
  // Read the grid values
  unsigned int entries;
  stream >> entries;
  for (unsigned int index = 0; index < entries; index++)
    for (unsigned int i = 0; i < value_n; i++)
      stream >> fValues[i][index];
  QwMessage << QwLog::endl;
  // Check for end of file
  std::string end;
  stream >> end;
  if (end == "end") return true;
  else return false;
}

/**
 * Write the grid values to binary file (should be 64-bit safe, untested)
 *
 * Integer data types can be stored differently on 32-bit and 64-bit systems.
 * Fixed-length types uint32_t and u_int32_t are provided in stdint.h and
 * sys/types.h, respectively.  The floating point types float and double will
 * always have a length of 32 and 64 bit, per the IEEE convention.
 *
 * @param filename File name
 * @return True if written successfully
 */
template <class value_t, unsigned int value_n>
inline bool QwInterpolator<value_t,value_n>::WriteBinaryFile(std::string filename) const
{
  std::ofstream file(filename.c_str(), std::ios::binary);
  if (! file.is_open()) return false;
  // Write template parameters
  uint32_t n = value_n; // uint32_t has length of 32 bits on any system
  uint32_t size = sizeof(value_t);
  file.write(reinterpret_cast<const char*>(&n), sizeof(n));
  file.write(reinterpret_cast<const char*>(&size), sizeof(size));
  // Write grid parameters
  uint32_t ndim = fNDim;
  file.write(reinterpret_cast<const char*>(&ndim), sizeof(ndim));
  for (unsigned int dim = 0; dim < fNDim; dim++) {
    file.write(reinterpret_cast<const char*>(&fMin[dim]),sizeof(fMin[dim]));
    file.write(reinterpret_cast<const char*>(&fMax[dim]),sizeof(fMax[dim]));
    file.write(reinterpret_cast<const char*>(&fStep[dim]),sizeof(fStep[dim]));
  }
  uint32_t entries = fValues[0].size();
  file.write(reinterpret_cast<const char*>(&entries),sizeof(entries));
  for (unsigned int index = 0; index < fValues[0].size(); index++) {
    for (unsigned int i = 0; i < value_n; i++) {
      value_t value = fValues[i][index];
      file.write(reinterpret_cast<const char*>(&value),sizeof(value));
    }
  }
  file.close();
  return true;
}

/**
 * Read the grid values from binary file (should be 64-bit safe, untested)
 * @param filename File name
 * @return True if read successfully
 */
template <class value_t, unsigned int value_n>
inline bool QwInterpolator<value_t,value_n>::ReadBinaryFile(std::string filename)
{
  std::ifstream file(filename.c_str(), std::ios::binary);
  if (! file.is_open()) return false;
  // Go to end and store length (could also use std::ios::ate)
  file.seekg(0, std::ios::end); unsigned int length = file.tellg();
  // Go to begin and start reading template parameters
  file.seekg(0, std::ios::beg);
  uint32_t n, size;
  file.read(reinterpret_cast<char*>(&n), sizeof(n));
  file.read(reinterpret_cast<char*>(&size), sizeof(size));
  if (n != value_n) return false; // not same dimensionality
  if (size != sizeof(value_t)) return false; // not same type
  // Read grid parameters
  uint32_t ndim;
  file.read(reinterpret_cast<char*>(&ndim), sizeof(ndim));
  SetDimensions(ndim);
  // Read sizes
  for (unsigned int dim = 0; dim < fNDim; dim++) {
    file.read(reinterpret_cast<char*>(&fMin[dim]),sizeof(fMin[dim]));
    file.read(reinterpret_cast<char*>(&fMax[dim]),sizeof(fMax[dim]));
    file.read(reinterpret_cast<char*>(&fStep[dim]),sizeof(fStep[dim]));
  }
  SetMinimumMaximumStep(fMin, fMax, fStep); // calculates fMaximumEntries
  // Read values
  uint32_t maximum_entries;
  file.read(reinterpret_cast<char*>(&maximum_entries),sizeof(maximum_entries));
  if (maximum_entries != fMaximumEntries) return false; // not expected number of entries
  int value_size = sizeof(value_t);
  for (unsigned int index = 0; index < fValues[0].size(); index++)
    for (unsigned int i = 0; i < value_n; i++)
      file.read((char*)(&fValues[i][index]),value_size);
  QwMessage << QwLog::endl;
  file.close();
  return true;
}


#endif // _QWINTERPOLATOR_H_
