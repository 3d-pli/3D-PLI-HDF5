#pragma once

#include "PLIHDF5/dataset.h"

#include <limits>
#include <numeric>

#include "PLIHDF5/exceptions.h"

template <typename T>
PLI::HDF5::Dataset
PLI::HDF5::Folder::createDataset(const std::string &datasetName,
                                 const std::vector<size_t> &dims,
                                 const std::vector<size_t> &chunkDims) {
    PLI::HDF5::Dataset dataset;
    dataset.create<T>(*this, datasetName, dims, chunkDims);
    return dataset;
}

template <typename T>
void PLI::HDF5::Dataset::create(const Folder &parentPtr,
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
    hid_t memspacePtr =
        H5Screate_simple(_dims_hdf5.size(), _dims_hdf5.data(), nullptr);
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
std::vector<T>
PLI::HDF5::Dataset::read(const std::vector<size_t> &offset,
                         const std::vector<size_t> &count,
                         const std::vector<size_t> &stride) const {
    // Convert offset and count to hsize_t arrays for H5Sselect_hyperslab
    std::vector<hsize_t> _offset(offset.begin(), offset.end());
    std::vector<hsize_t> _count(count.begin(), count.end());
    std::vector<hsize_t> _stride(offset.size(), 1);
    if (!stride.empty()) {
        _stride = std::vector<hsize_t>(stride.begin(), stride.end());
    }

    size_t numElements = std::accumulate(count.begin(), count.end(), 1ull,
                                         std::multiplies<std::size_t>());
    // Check for possible error with MPI because too many elements are requested
    // to be read at once. Throw an error message for the user and request to
    // use chunk iterators instead.
    if (this->m_communicator.has_value() &&
        numElements > static_cast<size_t>(std::numeric_limits<int>::max())) {
        throw PLI::HDF5::Exceptions::DatasetOperationOverflowException(
            "The requested amount of elements read is not allowed when using "
            "MPI! Consider using chunk iterators or read the dataset in "
            "selected amounts.");
    }

    checkHDF5Ptr(this->m_id, "Dataset ID");
    hid_t dataspacePtr = H5Dget_space(this->m_id);
    checkHDF5Ptr(dataspacePtr, "H5Dget_space");
    PLI::HDF5::Type returnType = PLI::HDF5::Type::createType<T>();
    hid_t memspacePtr = H5Screate_simple(_count.size(), _count.data(), nullptr);
    checkHDF5Ptr(memspacePtr, "H5Screate_simple");

    std::vector<T> returnData;
    returnData.resize(numElements);

    hid_t xf_id = createXfID();
    checkHDF5Call(H5Sselect_hyperslab(dataspacePtr, H5S_SELECT_SET,
                                      _offset.data(), _stride.data(),
                                      _count.data(), nullptr),
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
void PLI::HDF5::Dataset::read(T *const data, const std::vector<size_t> &offset,
                              const std::vector<size_t> &count,
                              const std::vector<size_t> &stride) const {
    // Convert offset and count to hsize_t arrays for H5Sselect_hyperslab
    std::vector<hsize_t> _offset(offset.begin(), offset.end());
    std::vector<hsize_t> _count(count.begin(), count.end());
    std::vector<hsize_t> _stride(offset.size(), 1);
    if (!stride.empty()) {
        _stride = std::vector<hsize_t>(stride.begin(), stride.end());
    }

    size_t numElements = std::accumulate(count.begin(), count.end(), 1ull,
                                         std::multiplies<std::size_t>());
    // Check for possible error with MPI because too many elements are requested
    // to be read at once. Throw an error message for the user and request to
    // use chunk iterators instead.
    if (this->m_communicator.has_value() &&
        numElements > static_cast<size_t>(std::numeric_limits<int>::max())) {
        throw PLI::HDF5::Exceptions::DatasetOperationOverflowException(
            "The requested amount of elements read is not allowed when using "
            "MPI! Consider using chunk iterators or read the dataset in "
            "selected amounts.");
    }

    checkHDF5Ptr(this->m_id, "Dataset ID");
    hid_t dataspacePtr = H5Dget_space(this->m_id);
    checkHDF5Ptr(dataspacePtr, "H5Dget_space");
    PLI::HDF5::Type returnType = PLI::HDF5::Type::createType<T>();
    hid_t memspacePtr = H5Screate_simple(_count.size(), _count.data(), nullptr);
    checkHDF5Ptr(memspacePtr, "H5Screate_simple");

    hid_t xf_id = createXfID();
    checkHDF5Call(H5Sselect_hyperslab(dataspacePtr, H5S_SELECT_SET,
                                      _offset.data(), _stride.data(),
                                      _count.data(), nullptr),
                  "H5Sselect_hyperslab");
    checkHDF5Call(
        H5Dread(this->m_id, returnType, memspacePtr, dataspacePtr, xf_id, data),
        "H5Dread");

    checkHDF5Call(H5Pclose(xf_id), "H5Pclose");
    checkHDF5Call(H5Sclose(memspacePtr), "H5Sclose");
    checkHDF5Call(H5Sclose(dataspacePtr), "H5Sclose");
}

template <typename T>
std::vector<T>
PLI::HDF5::Dataset::read(const PLI::HDF5::Dataset::Hyperslab &hyperslab) const {
    return this->read<T>(hyperslab.offset(), hyperslab.count(),
                         hyperslab.stride());
}

template <typename T>
void PLI::HDF5::Dataset::read(
    T *const data, const PLI::HDF5::Dataset::Hyperslab &hyperslab) const {
    return this->read<T>(data, hyperslab.offset(), hyperslab.count(),
                         hyperslab.stride());
}

template <typename T>
void PLI::HDF5::Dataset::write(const std::vector<T> &data,
                               const std::vector<size_t> &offset,
                               const std::vector<size_t> &dims,
                               const std::vector<size_t> &stride) {
    this->write<T>(data.data(), offset, dims, stride);
}

template <typename T>
void PLI::HDF5::Dataset::write(const void *data,
                               const std::vector<size_t> &offset,
                               const std::vector<size_t> &dims,
                               const std::vector<size_t> &stride) {
    this->write(data, offset, dims, stride, PLI::HDF5::Type::createType<T>());
}

template <typename T>
void PLI::HDF5::Dataset::write(const std::vector<T> &data,
                               const PLI::HDF5::Dataset::Hyperslab &hyperslab) {

    this->write(data.data(), hyperslab.offset(), hyperslab.count(),
                hyperslab.stride(), PLI::HDF5::Type::createType<T>());
}

template <typename T>
void PLI::HDF5::Dataset::write(const void *data,
                               const PLI::HDF5::Dataset::Hyperslab &hyperslab) {

    this->write(data, hyperslab.offset(), hyperslab.count(), hyperslab.stride(),
                PLI::HDF5::Type::createType<T>());
}
