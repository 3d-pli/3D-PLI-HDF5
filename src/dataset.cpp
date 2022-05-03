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
  return H5LTfind_dataset(parentPtr, datasetName.c_str()) > 0;
}

void PLI::HDF5::Dataset::close() {
  if (this->m_id > 0) {
    checkHDF5Call(H5Dclose(this->m_id), "H5Dclose");
  }
  this->m_id = -1;
}

const PLI::HDF5::Type PLI::HDF5::Dataset::type() const {
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

PLI::HDF5::Dataset::~Dataset() {}

template <typename T>
PLI::HDF5::Dataset PLI::HDF5::Dataset::create(
    const hid_t parentPtr, const std::string &datasetName,
    const std::vector<hsize_t> &dims, const std::vector<hsize_t> &chunkDims) {
  if (PLI::HDF5::Dataset::exists(parentPtr, datasetName)) {
    throw Exceptions::DatasetExistsException("Dataset alreadt exists!");
  }

  hid_t dcpl_id = H5P_DEFAULT;
  if (!chunkDims.empty()) {
    if (dims.size() != chunkDims.size()) {
      throw Exceptions::HDF5RuntimeException(
          "Chunk dimensions must have the same size as "
          "dataset dimensions.");
    }

    dcpl_id = H5Pcreate(H5P_DATASET_CREATE);
    checkHDF5Ptr(dcpl_id, "H5Pcreate");
    checkHDF5Call(H5Pset_chunk(dcpl_id, chunkDims.size(), chunkDims.data()),
                  "H5Pset_chunk");
  }

  PLI::HDF5::Type dataType = PLI::HDF5::Type::createType<T>();
  hid_t dataspacePtr = H5Screate_simple(dims.size(), dims.data(), nullptr);
  checkHDF5Ptr(dataspacePtr, "H5Screate_simple");
  hid_t datasetPtr = H5Dcreate(parentPtr, datasetName.c_str(), dataType,
                               dataspacePtr, dcpl_id, H5P_DEFAULT, H5P_DEFAULT);
  checkHDF5Ptr(datasetPtr, "H5Dcreate");
  checkHDF5Call(H5Sclose(dataspacePtr), "H5Sclose");
  return PLI::HDF5::Dataset(datasetPtr);
}

template <typename T>
std::vector<T> PLI::HDF5::Dataset::readFullDataset() const {
  checkHDF5Ptr(this->m_id, "Dataset ID");
  hid_t dataspacePtr = H5Dget_space(this->m_id);
  checkHDF5Ptr(dataspacePtr, "H5Dget_space");
  hsize_t sumOfElements;
  sumOfElements = H5Sget_simple_extent_npoints(dataspacePtr);

  std::vector<hsize_t> dims = this->dims();
  std::vector<T> returnData;
  returnData.resize(sumOfElements);

  PLI::HDF5::Type returnType = PLI::HDF5::Type::createType<T>();

  hid_t xf_id = H5Pcreate(H5P_DATASET_XFER);
  checkHDF5Ptr(xf_id, "H5Pcreate");
  checkHDF5Call(H5Pset_dxpl_mpio(xf_id, H5FD_MPIO_COLLECTIVE),
                "H5Pset_dxpl_mpio");
  hid_t memspacePtr = H5Screate_simple(dims.size(), dims.data(), nullptr);
  checkHDF5Ptr(memspacePtr, "H5Screate_simple");
  checkHDF5Call(H5Dread(this->m_id, returnType, memspacePtr, dataspacePtr,
                        xf_id, returnData.data()),
                "H5Dread");

  checkHDF5Call(H5Pclose(xf_id), "H5Pclose");
  checkHDF5Call(H5Sclose(dataspacePtr), "H5Sclose");
  checkHDF5Call(H5Sclose(memspacePtr), "H5Sclose");
  return returnData;
}

template <typename T>
std::vector<T> PLI::HDF5::Dataset::read(
    const std::vector<hsize_t> &offset,
    const std::vector<hsize_t> &count) const {
  checkHDF5Ptr(this->m_id, "Dataset ID");
  hid_t dataspacePtr = H5Dget_space(this->m_id);
  checkHDF5Ptr(dataspacePtr, "H5Dget_space");
  PLI::HDF5::Type returnType = PLI::HDF5::Type::createType<T>();

  size_t numElements = 1;
  std::for_each(
      count.begin(), count.end(),
      [&numElements](const hsize_t dimension) { numElements *= dimension; });
  std::vector<T> returnData;
  returnData.resize(numElements);

  hid_t xf_id = H5Pcreate(H5P_DATASET_XFER);
  checkHDF5Ptr(xf_id, "H5Pcreate");
  checkHDF5Call(H5Pset_dxpl_mpio(xf_id, H5FD_MPIO_COLLECTIVE),
                "H5Pset_dxpl_mpio");
  checkHDF5Call(H5Sselect_hyperslab(dataspacePtr, H5S_SELECT_SET, offset.data(),
                                    nullptr, count.data(), nullptr),
                "H5Sselect_hyperslab");
  checkHDF5Call(H5Dread(this->m_id, returnType, H5S_ALL, dataspacePtr, xf_id,
                        returnData.data()),
                "H5Dread");

  checkHDF5Call(H5Pclose(xf_id), "H5Pclose");
  checkHDF5Call(H5Sclose(dataspacePtr), "H5Sclose");
  return returnData;
}

template <typename T>
void PLI::HDF5::Dataset::write(const std::vector<T> &data,
                               const std::vector<hsize_t> &offset,
                               const std::vector<hsize_t> &dims) {
  hid_t dataSpacePtr = H5Dget_space(this->m_id);
  checkHDF5Ptr(dataSpacePtr, "H5Dget_space");
  // Check if the type matches the type of the dataset
  PLI::HDF5::Type dataType = PLI::HDF5::Type::createType<T>();
  if (dataType != PLI::HDF5::Type(H5Dget_type(this->m_id))) {
    throw Exceptions::HDF5RuntimeException("Datatypes do not match.");
  }

  hid_t xf_id = H5Pcreate(H5P_DATASET_XFER);
  checkHDF5Ptr(xf_id, "H5Pcreate");
  checkHDF5Call(H5Pset_dxpl_mpio(xf_id, H5FD_MPIO_COLLECTIVE),
                "H5Pset_dxpl_mpio");
  checkHDF5Call(H5Sselect_hyperslab(dataSpacePtr, H5S_SELECT_SET, offset.data(),
                                    nullptr, dims.data(), nullptr),
                "H5Sselect_hyperslab");

  checkHDF5Call(
      H5Dwrite(this->m_id, dataType, H5S_ALL, dataSpacePtr, xf_id, data.data()),
      "H5Dwrite");

  checkHDF5Call(H5Pclose(xf_id), "H5Pclose");
  checkHDF5Call(H5Sclose(dataSpacePtr), "H5Sclose");
}

PLI::HDF5::Dataset::operator hid_t() const noexcept { return this->m_id; }
