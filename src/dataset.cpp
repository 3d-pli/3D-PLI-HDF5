/*
    MIT License

    Copyright (c) 2022 Forschungszentrum Jülich / Jan André Reuter & Felix
   Matuschke.

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.
 */

#include "PLIHDF5/dataset.h"

PLI::HDF5::Dataset PLI::HDF5::Dataset::open(const hid_t parentPtr,
                                            const std::string &datasetName) {
  hid_t datasetPtr = H5Dopen(parentPtr, datasetName.c_str(), H5P_DEFAULT);
  return PLI::HDF5::Dataset(datasetPtr);
}

bool PLI::HDF5::Dataset::exists(const hid_t parentPtr,
                                const std::string &datasetName) {
  return H5LTfind_dataset(parentPtr, datasetName.c_str()) == 1;
}

void PLI::HDF5::Dataset::close() {
  if (this->m_id > 0) {
    H5Dclose(this->m_id);
  }
  this->m_id = -1;
}

const PLI::HDF5::Type PLI::HDF5::Dataset::type() const {
  hid_t typePtr = H5Dget_type(this->m_id);
  return PLI::HDF5::Type(typePtr);
}

int PLI::HDF5::Dataset::ndims() const {
  hid_t dataspace = H5Dget_space(this->m_id);
  int numDims = H5Sget_simple_extent_ndims(dataspace);
  H5Sclose(dataspace);
  if (numDims > 0) {
    return numDims;
  } else {
    // TODO(jreuter): Add exception!
    throw 0;
  }
}

const std::vector<hsize_t> PLI::HDF5::Dataset::dims() const {
  int numDims = this->ndims();
  std::vector<hsize_t> dims;
  dims.resize(numDims);

  hid_t dataspace = H5Dget_space(this->m_id);
  H5Sget_simple_extent_dims(dataspace, dims.data(), nullptr);
  H5Sclose(dataspace);

  return dims;
}

hid_t PLI::HDF5::Dataset::id() const { return this->m_id; }

PLI::HDF5::Dataset::Dataset() : m_id(-1) {}

PLI::HDF5::Dataset::Dataset(hid_t datasetPtr) { this->m_id = datasetPtr; }

PLI::HDF5::Dataset::Dataset(const Dataset &dataset) : m_id(dataset.id()) {}

PLI::HDF5::Dataset::~Dataset() {}

template <typename T>
PLI::HDF5::Dataset PLI::HDF5::Dataset::create(const hid_t parentPtr,
                                              const std::string &datasetName,
                                              const int32_t ndims,
                                              const hsize_t *dims,
                                              const bool chunked) {
  if (PLI::HDF5::Dataset::exists(parentPtr, datasetName)) {
    throw 0;
  }

  hid_t dcpl_id = H5P_DEFAULT;
  if (chunked) {
    // TODO(jreuter): Allow the chunk size to be variable as an input variable.
    std::vector<hsize_t> chunk_dims;
    chunk_dims.resize(ndims);
    for (int32_t i = 0; i < ndims; ++i) {
      if (i < 2) {
        chunk_dims[i] = 256;
      } else {
        chunk_dims[i] = 1;
      }
    }
    dcpl_id = H5Pcreate(H5P_DATASET_CREATE);
    H5Pset_chunk(dcpl_id, ndims, chunk_dims.data());
  }

  hid_t dataspacePtr = H5Screate_simple(ndims, dims, nullptr);
  hid_t datasetPtr = H5Dcreate(parentPtr, datasetName.c_str(), H5T_NATIVE_UCHAR,
                               dataspacePtr, dcpl_id, H5P_DEFAULT, H5P_DEFAULT);
  H5Sclose(dataspacePtr);
  return PLI::HDF5::Dataset(datasetPtr);
}

template <typename T>
std::vector<T> PLI::HDF5::Dataset::readFullDataset() const {
  hid_t dataspacePtr = H5Dget_space(this->m_id);
  hsize_t sumOfElements;
  sumOfElements = H5Sget_simple_extent_npoints(dataspacePtr);

  int numDims;
  numDims = H5Sget_simple_extent_ndims(dataspacePtr);
  std::vector<hsize_t> dims;
  dims.resize(numDims);
  H5Sget_simple_extent_dims(dataspacePtr, dims.data(), nullptr);

  std::vector<T> returnData;
  returnData.resize(sumOfElements);

  PLI::HDF5::Type returnType = PLI::HDF5::Type::createType<T>();

  hid_t xf_id = H5Pcreate(H5P_DATASET_XFER);
  H5Pset_dxpl_mpio(xf_id, H5FD_MPIO_COLLECTIVE);
  hid_t memspacePtr = H5Screate_simple(numDims, dims.data(), nullptr);
  H5Dread(this->m_id, returnType, memspacePtr, dataspacePtr, xf_id,
          returnData.data());

  H5Pclose(xf_id);
  H5Sclose(dataspacePtr);
  H5Sclose(memspacePtr);
  return returnData;
}

template <typename T>
std::vector<T> PLI::HDF5::Dataset::read(
    const std::vector<hsize_t> &offset,
    const std::vector<hsize_t> &count) const {
  hid_t dataspacePtr = H5Dget_space(this->m_id);
  PLI::HDF5::Type returnType = PLI::HDF5::Type::createType<T>();

  size_t numElements = 1;
  std::for_each(
      count.begin(), count.end(),
      [&numElements](const hsize_t dimension) { numElements *= dimension; });
  std::vector<T> returnData;
  returnData.resize(numElements);

  hid_t xf_id = H5Pcreate(H5P_DATASET_XFER);
  H5Pset_dxpl_mpio(xf_id, H5FD_MPIO_COLLECTIVE);
  H5Sselect_hyperslab(dataspacePtr, H5S_SELECT_SET, offset.data(), nullptr,
                      count.data(), nullptr);
  H5Dread(this->m_id, returnType, H5S_ALL, dataspacePtr, xf_id,
          returnData.data());

  H5Pclose(xf_id);
  H5Sclose(dataspacePtr);
  return returnData;
}

template <typename T>
void PLI::HDF5::Dataset::write(const T *data, const int32_t ndims,
                               const hsize_t *dims) {}

PLI::HDF5::Dataset::operator hid_t() const { return this->m_id; }
