#ifndef _QWINTERPOLATOR_H_
#define _QWINTERPOLATOR_H_

// System headers
#include <vector>
#include <fstream>
#include <stdexcept>

// Qweak headers
#include "QwLog.h"

// Type of grid coordinates
typedef double coord_t;
// NOTE This could trivially be made into a template argument, but that seems
// hardly worth it considering the small gains in going to different types.

/**
 *  \class QwInterpolator
 *  \ingroup QwAnalysis
 *  \brief A multi-dimensional grid of values with interpolation methods
 *
 * This class provides various interpolation methods on a multi-dimensional grid
 * of single-dimensional values.  Linear interpolation is implemented for one-
 * dimensional grids, trilinear for three-dimensional grids, multi-linear for
 * multi-dimensional grids.  Nearest-neighbor is implemented for all dimensions.
 *
 * The minimum, maximum, and step size of the grid have to be known at construction.
 */
template <class value_t>
class QwInterpolator {

  public:

    /// Constructor with number of dimensions
    QwInterpolator(const unsigned int ndim) {
      SetDimensions(ndim);
    };
    /// Constructor with minimum, maximum, and step size
    QwInterpolator(const std::vector<coord_t> min, const std::vector<coord_t> max, const std::vector<coord_t> step) {
      SetDimensions(min.size());
      SetMinimumMaximumStep(min,max,step);
    };
    /// Destructor
    virtual ~QwInterpolator() { /* stub */ };


    /// Set the number of coordinate dimensions
    void SetDimensions(const unsigned int ndim) {
      if (ndim > 0) fNDim = ndim;
    };
    /// Set minimum, maximum, and step size
    void SetMinimumMaximumStep(const std::vector<coord_t> min, const std::vector<coord_t> max, const std::vector<coord_t> step) {
      if (min.size() == fNDim && min.size() == fNDim && step.size() == fNDim) {
        fMin = min; fMax = max; fStep = step; fSize.resize(fStep.size());
        fEntries = 1;
        for (size_t i = 0; i < fMin.size(); i++) {
          fSize[i] = static_cast<unsigned int>((fMax[i] - fMin[i]) / fStep[i] + 1);
          fEntries *= fSize[i];
        }
        try {
          fValues.reserve(fEntries);
        } catch (std::bad_alloc) {
          QwError << "Could not allocate memory for object!" << QwLog::endl;
        }
      }
    };
    /// Set minimum, maximum, and step to single values
    void SetMinimumMaximumStep(const coord_t min, const coord_t max, const coord_t step) {
      SetMinimumMaximumStep(std::vector<coord_t>(fNDim,min), std::vector<coord_t>(fNDim,max), std::vector<coord_t>(fNDim,step));
    };


    /// \brief Return the cell index based on the point coordinates (unchecked)
    void GetCellIndex(const coord_t coord, unsigned int& cell_index, double& cell_local, const unsigned int dim) const;
    /// \brief Return the cell index based on the point coordinates (unchecked)
    void GetCellIndex(const coord_t* coord, unsigned int* cell_index, double* cell_local) const;
    /// \brief Return the cell index based on the point coordinates (unchecked)
    void GetCellIndex(const coord_t* coord, unsigned int* cell_index) const;

    /// Return the linearized index based on the point coordinates (unchecked)
    const unsigned int Index(const coord_t* coord) const {
      unsigned int cell_index[fNDim];
      GetCellIndex(coord, cell_index);
      return Index(cell_index);
    };

    /// Return the linearized index based on the cell indices
    const unsigned int GetIndex(const unsigned int* cell_index) const {
      if (! Check(cell_index)) return 0;
      return Index(cell_index);
    };

    /// Set the value at a linearized index (false if zero)
    const bool Set(const unsigned int linear_index, const value_t value) {
      fValues[linear_index] = value;
      return true;
    };

    /// Set the value at a grid point (false if out of bounds)
    const bool Set(const unsigned int* cell_index, const value_t value) {
      if (! Check(cell_index)) return false;
      fValues[Index(cell_index)] = value;
      return true;
    };

    /// Get the interpolated value at coordinate (zero if out of bounds)
    value_t GetValue(const coord_t* coord) {
      if (! Check(coord)) return 0;
      return Linear(coord);
    };

  private:

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

    /// Table with values
    std::vector<value_t> fValues;
    /// Number of values
    unsigned int fEntries;

    /// \brief Return the linearized index based on the cell indices (unchecked)
    const unsigned int Index(const unsigned int* cell_index) const;
    /// \brief Return the linearized index based on the cell indices and offset (unchecked)
    const unsigned int Index(const unsigned int* cell_index, const unsigned int offset) const;

    /// \brief Linear interpolation
    value_t Linear(const coord_t* coord) const;
    /// \brief Nearest-neighbor interpolation
    value_t NearestNeighbor(const coord_t* coord) const;

    /// \brief Check for boundaries with coordinate argument
    const bool Check(const coord_t* coord) const;
    /// \brief Check for boundaries with cell index argument
    const bool Check(const unsigned int* cell_index) const;
    /// \brief Check for boundaries with linearized index argument
    const bool Check(const unsigned int linear_index) const;

    /// Get a value by linearized index (unchecked)
    value_t Get(const unsigned int index) const { return fValues[index]; };

    /// Get a value by cell index (unchecked)
    value_t Get(const unsigned int* cell_index) const {
      return fValues[Index(cell_index)];
    };
};

/**
 * Check whether the point is inside the valid region
 * @param coord Point coordinates
 * @return True if the coordinates are in the valid region
 */
template<class value_t>
inline const bool QwInterpolator<value_t>::Check(const coord_t* coord) const
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
template<class value_t>
inline const bool QwInterpolator<value_t>::Check(const unsigned int* cell_index) const
{
  for (unsigned int dim = 0; dim < fNDim; dim++)
    if (cell_index[dim] < 0 || cell_index[dim] >= fSize[dim])
      return false;
  // Otherwise
  return true;
}

/**
 * Check whether the linearized index is inside the valid region
 * @param linear_index Linearized index
 * @return True if the cell index is in the valid region
 */
template<class value_t>
inline const bool QwInterpolator<value_t>::Check(const unsigned int linear_index) const
{
  if (linear_index < 0 || linear_index >= fEntries)
    return false;
  // Otherwise
  return true;
}

/**
 * Perform the multi-dimensional linear interpolation
 * @param coord Point coordinates
 * @return Interpolated value
 */
template<class value_t>
inline value_t QwInterpolator<value_t>::Linear(const coord_t* coord) const
{
  // Get cell and local normalized coordinates
  unsigned int cell_index[fNDim];
  double cell_local[fNDim];
  GetCellIndex(coord, cell_index, cell_local);
  // Calculate the interpolated value
  value_t sum = 0;
  // ... by summing the 2^fNDim = 1 << fNDim closest points
  for (unsigned int offset = 0; offset < (1UL << fNDim); offset++) {
    double val = Get(Index(cell_index,offset));
    // ... with appropriate weighting factors (1 - cell_local or cell_local)
    for (unsigned int dim = 0; dim < fNDim; dim++)
      val *= ((offset >> dim) & 0x1)? cell_local[dim] : 1 - cell_local[dim];
    sum += val;
  }
  return sum;
}

/**
 * Perform the nearest-neighbor interpolation
 * @param coord Point coordinates
 * @return Interpolated value
 */
template<class value_t>
inline value_t QwInterpolator<value_t>::NearestNeighbor(const coord_t* coord) const
{
  // Get cell and local normalized coordinates
  unsigned int cell_index[fNDim];
  double cell_local[fNDim];
  GetCellIndex(coord, cell_index, cell_local);
  // Loop over all dimensions and add one if larger than 0.5
  for (unsigned int dim = 0; dim < fNDim; dim++)
    if (cell_local[dim] > 0.5) cell_index[dim]++;
  return Get(Index(cell_index));
}

/**
 * Return the linearized index based on the cell indices (unchecked)
 * @param cell_index Index in each dimension
 * @return Linearized index
 */
template<class value_t>
inline const unsigned int QwInterpolator<value_t>::Index(
	const unsigned int* cell_index) const
{
  unsigned int linear_index = 0;
  unsigned int linear_extent = 1;
  // Loop over dimensions
  for (unsigned int dim = 0; dim < fNDim; dim++) {
    linear_index += linear_extent * cell_index[dim];
    // Keep the linear extent in each dimension (NOTE could be stored)
    linear_extent *= fSize[dim];
  }
  return linear_index;
}

/**
 * Return the linearized index based on the cell indices and offset (unchecked)
 * @param cell_index Index in each dimension
 * @param pattern Bit pattern with offsets in each dimension
 * @return Linearized index
 */
template<class value_t>
inline const unsigned int QwInterpolator<value_t>::Index(
	const unsigned int* cell_index,
	const unsigned int pattern) const
{
  unsigned int linear_index = 0;
  unsigned int linear_extent = 1;
  // Loop over dimensions
  for (unsigned int dim = 0; dim < fNDim; dim++) {
    // Add offset of one based on the bit at position dim
    unsigned int offset = (pattern >> dim) & 0x1;
    linear_index += linear_extent * (cell_index[dim] + offset);
    // Keep the linear extent in each dimension (NOTE could be stored)
    linear_extent *= fSize[dim];
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
template<class value_t>
inline void QwInterpolator<value_t>::GetCellIndex(
	const coord_t coord,
	unsigned int &cell_index,
	double &cell_local,
	const unsigned int dim) const
{
  // Normalize coordinate
  double norm_coord = (coord - fMin[dim]) / fStep[dim];
  // Split in fractional and integer part
  double frac_part, int_part;
  frac_part = modf(norm_coord, &int_part);
  cell_local = frac_part;
  cell_index = int(int_part); // cast
}

/**
 * Return the cell index and local coordinates (unchecked)
 * @param coord Point coordinates
 * @param cell_index Cell index of the point (reference)
 * @param cell_local Local coordinates in cell (reference)
 */
template<class value_t>
inline void QwInterpolator<value_t>::GetCellIndex(
	const coord_t* coord,
	unsigned int* cell_index,
	double* cell_local) const
{
  // Get cell index and local coordinates in each dimension
  for (unsigned int dim = 0; dim < fNDim; dim++)
    GetCellIndex(coord[dim], cell_index[dim], cell_local[dim], dim);
}

/**
 * Return the cell index (unchecked)
 * @param coord Point coordinates
 * @param cell_index Cell index of the point (reference)
 */
template<class value_t>
inline void QwInterpolator<value_t>::GetCellIndex(
	const coord_t* coord,
	unsigned int* cell_index) const
{
  // Get cell index and ignore local coordinates
  double cell_local[fNDim];
  GetCellIndex(coord, cell_index, cell_local);
}

#endif // _QWINTERPOLATOR_H_
