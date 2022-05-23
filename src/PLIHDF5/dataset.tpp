#pragma once

#include "PLIHDF5/dataset.h"

template <typename T>
PLI::HDF5::Dataset createDataset(const hid_t parentPtr, const std::string& datasetName,
                                 const std::vector<hsize_t>& dims,
                                 const std::vector<hsize_t>& chunkDims = {}) {
    PLI::HDF5::Dataset dataset;
    dataset.create<T>(parentPtr, datasetName, dims, chunkDims);
    return dataset;
}

template <typename T>
void PLI::HDF5::Dataset::create(
    const hid_t parentPtr, const std::string &datasetName,
    const std::vector<hsize_t> &dims, const std::vector<hsize_t> &chunkDims) {
  if (PLI::HDF5::Dataset::exists(parentPtr, datasetName)) {
    throw Exceptions::DatasetExistsException("Dataset already exists!");
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
                               dataspacePtr, H5P_DEFAULT, dcpl_id, H5P_DEFAULT);
  checkHDF5Ptr(datasetPtr, "H5Dcreate");
  checkHDF5Call(H5Sclose(dataspacePtr), "H5Sclose");

  this->m_id = datasetPtr;
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
  if (offset.size() != dims.size()) {
    throw Exceptions::HDF5RuntimeException(
        "Chunk dimensions must have the same size as "
        "dataset dimensions.");
  }

  hid_t dataSpacePtr = H5Dget_space(this->m_id);
  checkHDF5Ptr(dataSpacePtr, "H5Dget_space");

  hid_t xf_id = H5Pcreate(H5P_DATASET_XFER);
  checkHDF5Ptr(xf_id, "H5Pcreate");
  checkHDF5Call(H5Pset_dxpl_mpio(xf_id, H5FD_MPIO_COLLECTIVE),
                "H5Pset_dxpl_mpio");
  checkHDF5Call(H5Sselect_hyperslab(dataSpacePtr, H5S_SELECT_SET, offset.data(),
                                    nullptr, dims.data(), nullptr),
                "H5Sselect_hyperslab");

  PLI::HDF5::Type dataType = PLI::HDF5::Type::createType<T>();
  checkHDF5Call(
      H5Dwrite(this->m_id, dataType, H5S_ALL, dataSpacePtr, xf_id, data.data()),
      "H5Dwrite");

  checkHDF5Call(H5Pclose(xf_id), "H5Pclose");
  checkHDF5Call(H5Sclose(dataSpacePtr), "H5Sclose");
}