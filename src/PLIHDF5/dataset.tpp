#pragma once

#include "PLIHDF5/dataset.h"

template <typename T>
PLI::HDF5::Dataset PLI::HDF5::createDataset(
    const hid_t parentPtr, const std::string &datasetName,
    const std::vector<size_t> &dims, const std::vector<size_t> &chunkDims) {
  PLI::HDF5::Dataset dataset;
  dataset.create<T>(parentPtr, datasetName, dims, chunkDims);
  return dataset;
}

template <typename T>
void PLI::HDF5::Dataset::create(const hid_t parentPtr,
                                const std::string &datasetName,
                                const std::vector<size_t> &dims,
                                const std::vector<size_t> &chunkDims) {
  PLI::HDF5::Type dataType = PLI::HDF5::Type::createType<T>();
  this->create(parentPtr, datasetName, dims, chunkDims, dataType);
}

template <typename T>
std::vector<T> PLI::HDF5::Dataset::readFullDataset() const {
  checkHDF5Ptr(this->m_id, "Dataset ID");
  hid_t dataspacePtr = H5Dget_space(this->m_id);
  checkHDF5Ptr(dataspacePtr, "H5Dget_space");
  hsize_t sumOfElements;
  sumOfElements = H5Sget_simple_extent_npoints(dataspacePtr);

  std::vector<size_t> _dims = this->dims();
  std::vector<hsize_t> _dims_hdf5(_dims.begin(), _dims.end());
  std::vector<T> returnData;
  returnData.resize(sumOfElements);

  PLI::HDF5::Type returnType = PLI::HDF5::Type::createType<T>();

  hid_t xf_id = createXfID();
  hid_t memspacePtr = H5Screate_simple(_dims_hdf5.size(), _dims_hdf5.data(), nullptr);
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
    const std::vector<size_t> &offset,
    const std::vector<size_t> &count) const {
  // Convert offset and count to hsize_t arrays for H5Sselect_hyperslab
  std::vector<hsize_t> _offset(offset.begin(), offset.end());
  std::vector<hsize_t> _count(count.begin(), count.end());

  checkHDF5Ptr(this->m_id, "Dataset ID");
  hid_t dataspacePtr = H5Dget_space(this->m_id);
  checkHDF5Ptr(dataspacePtr, "H5Dget_space");
  PLI::HDF5::Type returnType = PLI::HDF5::Type::createType<T>();
  hid_t memspacePtr = H5Screate_simple(_count.size(), _count.data(), nullptr);
  checkHDF5Ptr(memspacePtr, "H5Screate_simple");

  size_t numElements = std::accumulate(count.begin(), count.end(), 1ull,
                                        std::multiplies<std::size_t>());
  std::vector<T> returnData;
  returnData.resize(numElements);

  hid_t xf_id = createXfID();
  checkHDF5Call(H5Sselect_hyperslab(dataspacePtr, H5S_SELECT_SET, _offset.data(),
                                    nullptr, _count.data(), nullptr),
                "H5Sselect_hyperslab");
  checkHDF5Call(H5Dread(this->m_id, returnType, memspacePtr, dataspacePtr,
                        xf_id, returnData.data()),
                "H5Dread");

  checkHDF5Call(H5Pclose(xf_id), "H5Pclose");
  checkHDF5Call(H5Sclose(memspacePtr), "H5Sclose");
  checkHDF5Call(H5Sclose(dataspacePtr), "H5Sclose");
  return returnData;
}

template <typename T>
void PLI::HDF5::Dataset::write(const std::vector<T> &data,
                               const std::vector<size_t> &offset,
                               const std::vector<size_t> &dims) {
  this->write<T>(data.data(), offset, dims);
}

template <typename T>
void PLI::HDF5::Dataset::write(const void* data, const std::vector<size_t>& offset,
            const std::vector<size_t>& dims) {
   this->write(data, offset, dims, PLI::HDF5::Type::createType<T>());           
}