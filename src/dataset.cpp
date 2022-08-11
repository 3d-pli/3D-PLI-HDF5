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

#include <iostream>

#include "PLIHDF5/dataset.h"

PLI::HDF5::Dataset PLI::HDF5::Folder::createDataset(
    const std::string &datasetName, const std::vector<size_t> &dims,
    const std::vector<size_t> &chunkDims, const PLI::HDF5::Type &dataType) {
    PLI::HDF5::Dataset dataset;
    dataset.create(*this, datasetName, dims, chunkDims, dataType);
    return dataset;
}

PLI::HDF5::Dataset
PLI::HDF5::Folder::openDataset(const std::string &datasetName) {
    PLI::HDF5::Dataset dataset;
    dataset.open(*this, datasetName);
    return dataset;
}

void PLI::HDF5::Dataset::open(const Folder &parentPtr,
                              const std::string &datasetName) {
    checkHDF5Ptr(parentPtr, "PLI::HDF5::Dataset::open");
    if (!exists(parentPtr, datasetName)) {
        throw PLI::HDF5::Exceptions::DatasetNotFoundException(
            "PLI::HDF5::Dataset::open: Dataset " + datasetName + " not found.");
    }
    hid_t datasetPtr = H5Dopen(parentPtr, datasetName.c_str(), H5P_DEFAULT);
    checkHDF5Ptr(datasetPtr, "H5Dopen");
    this->m_id = datasetPtr;
    this->m_communicator = parentPtr.communicator();
}

bool PLI::HDF5::Dataset::exists(const Folder &parentPtr,
                                const std::string &datasetName) {
    checkHDF5Ptr(parentPtr, "PLI::HDF5::Dataset::exists");
    return H5Lexists(parentPtr, datasetName.c_str(), H5P_DEFAULT) > 0;
}

bool PLI::HDF5::Dataset::isChunked() const {
    checkHDF5Ptr(m_id, "PLI::HDF5::Dataset::isChunked");
    hid_t plist = H5Dget_create_plist(m_id);
    if (plist == H5I_INVALID_HID) {
        return false;
    }
    H5D_layout_t layout = H5Pget_layout(plist);
    checkHDF5Ptr(plist, "H5Pget_layout");
    return layout == H5D_CHUNKED;
}

std::vector<size_t> PLI::HDF5::Dataset::chunkDims() const {
    if (!isChunked()) {
        throw PLI::HDF5::Exceptions::HDF5RuntimeException(
            "PLI::HDF5::Dataset::chunkDims: Dataset is not chunked.");
    }
    checkHDF5Ptr(m_id, "PLI::HDF5::Dataset::chunkDims");
    hid_t plist = H5Dget_create_plist(m_id);
    checkHDF5Ptr(plist, "H5Dget_create_plist");
    std::vector<hsize_t> _chunkDims(this->ndims());
    checkHDF5Call(H5Pget_chunk(plist, this->ndims(), _chunkDims.data()),
                  "H5Pget_chunk");
    return std::vector<size_t>(_chunkDims.begin(), _chunkDims.end());
}

void PLI::HDF5::Dataset::create(const Folder &parentPtr,
                                const std::string &datasetName,
                                const std::vector<size_t> &dims,
                                const std::vector<size_t> &chunkDims,
                                const PLI::HDF5::Type &dataType) {
    std::vector<hsize_t> _dims(dims.begin(), dims.end());
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

        for (size_t i = 0; i < dims.size(); i++) {
            if (dims[i] < chunkDims[i]) {
                throw Exceptions::HDF5RuntimeException(
                    "Chunk dimensions must be smaller than dataset "
                    "dimensions.");
            }
        }

        dcpl_id = H5Pcreate(H5P_DATASET_CREATE);
        checkHDF5Ptr(dcpl_id, "H5Pcreate");
        std::vector<hsize_t> _chunkDims(chunkDims.begin(), chunkDims.end());
        checkHDF5Call(
            H5Pset_chunk(dcpl_id, _chunkDims.size(), _chunkDims.data()),
            "H5Pset_chunk");
        checkHDF5Call(H5Pset_fill_value(dcpl_id, dataType, 0));
        // Disabled because of issues with H5FD_MPIO_INDEPENDENT
        // checkHDF5Call(H5Pset_fletcher32(dcpl_id), "H5Pset_fletcher32");
    }

    hid_t dataspacePtr = H5Screate_simple(_dims.size(), _dims.data(), nullptr);
    checkHDF5Ptr(dataspacePtr, "H5Screate_simple");
    hid_t datasetPtr =
        H5Dcreate(parentPtr, datasetName.c_str(), dataType, dataspacePtr,
                  H5P_DEFAULT, dcpl_id, H5P_DEFAULT);
    checkHDF5Ptr(datasetPtr, "H5Dcreate");
    checkHDF5Call(H5Sclose(dataspacePtr), "H5Sclose");

    this->m_id = datasetPtr;
    this->m_communicator = parentPtr.communicator();
}

void PLI::HDF5::Dataset::write(const void *data,
                               const std::vector<size_t> &offset,
                               const std::vector<size_t> &dims,
                               const std::vector<size_t> &stride,
                               const PLI::HDF5::Type &type) {
    std::vector<hsize_t> _dims(dims.begin(), dims.end());
    std::vector<hsize_t> _offset(offset.begin(), offset.end());
    std::vector<hsize_t> _stride(offset.size(), 1);
    if (!stride.empty()) {
        _stride = std::vector<hsize_t>(stride.begin(), stride.end());
    }

    if (offset.size() != dims.size()) {
        throw Exceptions::HDF5RuntimeException(
            "Offset dimensions must have the same size as "
            "dims dimensions.");
    }

    hid_t dataSpacePtr = H5Dget_space(this->m_id);
    checkHDF5Ptr(dataSpacePtr, "H5Dget_space");

    hid_t xf_id = createXfID();
    checkHDF5Call(H5Sselect_hyperslab(dataSpacePtr, H5S_SELECT_SET,
                                      _offset.data(), _stride.data(),
                                      _dims.data(), nullptr),
                  "H5Sselect_hyperslab");

    hid_t memspacePtr = H5Screate_simple(_dims.size(), _dims.data(), nullptr);
    checkHDF5Ptr(memspacePtr, "H5Screate_simple");
    checkHDF5Call(
        H5Dwrite(this->m_id, type, memspacePtr, dataSpacePtr, xf_id, data),
        "H5Dwrite");

    checkHDF5Call(H5Pclose(xf_id), "H5Pclose");
    checkHDF5Call(H5Sclose(dataSpacePtr), "H5Sclose");
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

const std::vector<size_t> PLI::HDF5::Dataset::dims() const {
    checkHDF5Ptr(this->m_id, "Dataset ID");
    int numDims = this->ndims();
    std::vector<hsize_t> dims;
    dims.resize(numDims);

    hid_t dataspace = H5Dget_space(this->m_id);
    checkHDF5Ptr(dataspace, "H5Dget_space");
    H5Sget_simple_extent_dims(dataspace, dims.data(), nullptr);
    checkHDF5Call(H5Sclose(dataspace), "H5Sclose");

    return std::vector<size_t>(dims.begin(), dims.end());
}

PLI::HDF5::Dataset::Dataset() noexcept : Object() {}

PLI::HDF5::Dataset::Dataset(
    hid_t datasetPtr, const std::optional<MPI_Comm> communicator) noexcept {
    this->m_id = datasetPtr;
    this->m_communicator = communicator;
}

PLI::HDF5::Dataset::Dataset(const Dataset &dataset) noexcept
    : Object(dataset.id(), dataset.communicator()) {}

PLI::HDF5::Dataset &
PLI::HDF5::Dataset::operator=(const Dataset &dataset) noexcept {
    this->m_id = dataset.id();
    checkHDF5Call(H5Iinc_ref(dataset.id()), "H5Iinc_ref");
    this->m_communicator = dataset.communicator();
    return *this;
}

hid_t PLI::HDF5::Dataset::createXfID() const {
    hid_t xf_id = H5Pcreate(H5P_DATASET_XFER);
    checkHDF5Ptr(xf_id, "H5Pcreate");

    if (m_communicator) {
        checkHDF5Call(H5Pset_dxpl_mpio(xf_id, H5FD_MPIO_INDEPENDENT),
                      "H5Pset_dxpl_mpio");
    }
    return xf_id;
}

bool PLI::HDF5::Dataset::Slice::operator==(
    const PLI::HDF5::Dataset::Slice &slice) const {
    return slice.start == start && slice.stop == stop && slice.step == step;
}

bool PLI::HDF5::Dataset::Slice::operator!=(
    const PLI::HDF5::Dataset::Slice &slice) const {
    return slice.start != start || slice.stop != stop || slice.step != step;
}

// ??? why not possible in cpp?
// std::ostream &operator<<(std::ostream &out,
//                          const PLI::HDF5::Dataset::Slice &slice) {
//     out << "[" << slice.start << ":" << slice.stop << ":" << slice.step <<
//     "]"; return out;
// }

std::tuple<std::vector<size_t>, std::vector<size_t>, std::vector<size_t>>
PLI::HDF5::Dataset::toOffsetAndDim(
    const PLI::HDF5::Dataset::Slices &slices) noexcept {
    std::vector<size_t> offset(slices.size(), 0);
    std::vector<size_t> dim(slices.size(), 0);
    std::vector<size_t> stride(slices.size(), 0);

    for (size_t i = 0; i < slices.size(); i++) {
        offset[i] = slices[i].start;
        dim[i] = (slices[i].stop - slices[i].start) / slices[i].step;
        stride[i] = slices[i].step;
    }
    return std::make_tuple(offset, dim, stride);
}

PLI::HDF5::Dataset::Slices PLI::HDF5::Dataset::toSlices(
    const std::vector<size_t> &offset, const std::vector<size_t> &dim,
    std::optional<std::vector<size_t>> stride) noexcept {
    PLI::HDF5::Dataset::Slices slice(offset.size());

    if (offset.size() != dim.size())
        std::runtime_error("invalid argument dimension");
    if (stride.has_value())
        if (offset.size() != stride.value().size())
            std::runtime_error("invalid argument dimension");
    const auto stride_ = stride.value_or(std::vector<size_t>(offset.size(), 1));

    for (size_t i = 0; i < offset.size(); i++) {
        slice[i].start = offset[i];
        slice[i].stop = offset[i] + dim[i] * stride_[i];
        slice[i].step = stride_[i];
    }
    return slice;
}

std::vector<PLI::HDF5::Dataset::Slices> PLI::HDF5::Dataset::getChunkSlices() {
    return this->getChunkSlices(this->chunkDims());
}

std::vector<PLI::HDF5::Dataset::Slices>
PLI::HDF5::Dataset::getChunkSlices(const std::vector<size_t> &chunkDims) {
    return slicesFromDimensions(this->dims(), chunkDims);
}

std::vector<PLI::HDF5::Dataset::Slices>
PLI::HDF5::Dataset::slicesFromDimensions(
    const std::vector<size_t> &dataDims, const std::vector<size_t> &chunkDims,
    std::optional<const std::vector<size_t>> chunkOffset) {

    std::vector<PLI::HDF5::Dataset::Slices> result;

    if (dataDims.size() != chunkDims.size())
        throw PLI::HDF5::Exceptions::DimensionMismatchException(
            "Dimension size does not match");

    if (chunkOffset.has_value())
        if (dataDims.size() != chunkOffset.value().size())
            throw PLI::HDF5::Exceptions::DimensionMismatchException(
                "Dimension size does not match");

    const auto chunkOffset_ =
        chunkOffset.value_or(std::vector<size_t>(dataDims.size(), 0));
    std::vector<size_t> offset = chunkOffset_;

    // increment offset, beginning with last element, and save valid slices
    while (offset[0] < dataDims[0]) {
        // set current offset chunk dimension
        auto dim = chunkDims;
        for (std::size_t i = 0; i < dataDims.size(); i++) {
            if (dim[i] + offset[i] > dataDims[i])
                dim[i] = dataDims[i] - offset[i];
        }

        // save offset/dim element
        PLI::HDF5::Dataset::Slices chunk;
        for (size_t i = 0; i < offset.size(); i++)
            chunk.push_back(
                PLI::HDF5::Dataset::Slice(offset[i], offset[i] + dim[i], 1));
        result.push_back(chunk);

        // increment offset, beginning with last
        for (auto i = static_cast<int64_t>(dataDims.size()) - 1; i >= 0; i--) {
            offset[i] += chunkDims[i];
            if (offset[i] >= dataDims[i] && i > 0) {
                offset[i] = chunkOffset_[i];
            } else {
                break;
            }
        }
    }
    return result;
}
