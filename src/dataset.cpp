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

PLI::HDF5::Dataset PLI::HDF5::openDataset(const hid_t parentPtr,
                                          const std::string &datasetName) {
  PLI::HDF5::Dataset dataset;
  dataset.open(parentPtr, datasetName);
  return dataset;
}

void PLI::HDF5::Dataset::open(const hid_t parentPtr,
                              const std::string &datasetName) {
  checkHDF5Ptr(parentPtr, "PLI::HDF5::Dataset::open");
  if (!exists(parentPtr, datasetName)) {
    throw PLI::HDF5::Exceptions::DatasetNotFoundException(
        "PLI::HDF5::Dataset::open: Dataset " + datasetName + " not found.");
  }
  hid_t datasetPtr = H5Dopen(parentPtr, datasetName.c_str(), H5P_DEFAULT);
  checkHDF5Ptr(datasetPtr, "H5Dopen");
  this->m_id = datasetPtr;
}

bool PLI::HDF5::Dataset::exists(const hid_t parentPtr,
                                const std::string &datasetName) {
  checkHDF5Ptr(parentPtr, "PLI::HDF5::Dataset::exists");
  MPI_Barrier(MPI_COMM_WORLD);
  return H5Lexists(parentPtr, datasetName.c_str(), H5P_DEFAULT) > 0;
}

void PLI::HDF5::Dataset::close() {
  if (this->m_id > 0) {
    checkHDF5Call(H5Dclose(this->m_id), "H5Dclose");
  }
  this->m_id = -1;
}

const PLI::HDF5::Type PLI::HDF5::Dataset::type() const {
  checkHDF5Ptr(this->m_id, "PLI::HDF5::Dataset::type");
  hid_t typePtr = H5Dget_type(this->m_id);
  checkHDF5Ptr(typePtr, "H5Dget_type");
  return PLI::HDF5::Type(typePtr);
}

int PLI::HDF5::Dataset::ndims() const {
  if (this->m_id < 0) {
    throw Exceptions::IdentifierNotValidException("Dataset ID is invalid!");
  }
  hid_t dataspace = H5Dget_space(this->m_id);
  checkHDF5Ptr(dataspace, "H5Dget_space");
  int numDims = H5Sget_simple_extent_ndims(dataspace);
  checkHDF5Call(H5Sclose(dataspace), "H5Sclose");
  if (numDims > 0) {
    return numDims;
  } else {
    throw Exceptions::HDF5RuntimeException("Dataset has no dimensions.");
  }
}

const std::vector<hsize_t> PLI::HDF5::Dataset::dims() const {
  checkHDF5Ptr(this->m_id, "Dataset ID");
  int numDims = this->ndims();
  std::vector<hsize_t> dims;
  dims.resize(numDims);

  hid_t dataspace = H5Dget_space(this->m_id);
  checkHDF5Ptr(dataspace, "H5Dget_space");
  H5Sget_simple_extent_dims(dataspace, dims.data(), nullptr);
  checkHDF5Call(H5Sclose(dataspace), "H5Sclose");

  return dims;
}

hid_t PLI::HDF5::Dataset::id() const noexcept { return this->m_id; }

PLI::HDF5::Dataset::Dataset() noexcept : m_id(-1) {}

PLI::HDF5::Dataset::Dataset(hid_t datasetPtr) noexcept {
  this->m_id = datasetPtr;
}

PLI::HDF5::Dataset::Dataset(const Dataset &dataset) noexcept
    : m_id(dataset.id()) {}

PLI::HDF5::Dataset::operator hid_t() const noexcept { return this->m_id; }
