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

#pragma once

#include <hdf5.h>
#include <hdf5_hl.h>
#include <mpi.h>

#include <algorithm>
#include <iterator>
#include <numeric>
#include <ostream>
#include <string>
#include <tuple>
#include <vector>

#include "PLIHDF5/exceptions.h"
#include "PLIHDF5/object.h"
#include "PLIHDF5/type.h"

/**
 * @brief The PLI namespace
 */
namespace PLI {
/**
 * @brief The HDF5 namespace
 */
namespace HDF5 {

/**
 * @brief HDF5 Dataset wrapper class.
 * Create and store or read data in an HDF5 dataset.
 */
class Dataset : public Object {
  public:
    // forward declaration
    struct Slice;
    class Slices;
    class Hyperslab;

    /**
     * @brief Construct a new Dataset object
     *
     * Create an empty dataset object. Calling other methods on this object will
     * fail because no pointer to an HDF5 object is set. To open a dataset, use
     * PLI::HDF5::Dataset::open(const hid_t parentPtr, const std::string&
     * datasetName). To create a dataset, use PLI::HDF5::Dataset::create(const
     * hid_t parentPtr, const std::string& datasetName, const
     * std::vector<size_t>& dims, const std::vector<size_t>& chunkDims).
     */
    Dataset() noexcept;
    /**
     * @brief Construct a new Dataset object
     *
     * Construct a new dataset object by using the given pointer of another
     * dataset object.
     * @param otherFile Other dataset object.
     */
    Dataset(const Dataset &otherFile) noexcept;
    /**
     * @brief Construct a new Dataset object
     *
     * Construct a new dataset object by using an existing HDF5 pointer.
     * @param datasetPtr HDF5 pointer to an existing dataset.
     */
    explicit Dataset(hid_t datasetPtr,
                     const std::optional<MPI_Comm> communicator = {}) noexcept;

    /**
     * @brief Open an existing dataset.
     *
     * This method tries to open an existing dataset with the given name. If the
     * dataset does not exist, an exception is thrown.
     * @param parentPtr File or group pointer.
     * @param datasetName Name of the dataset to open.
     * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the parent
     * or dataset pointer is invalid.
     * @throws PLI::HDF5::Exceptions::DatasetNotFoundException If the dataset
     * does not exist.
     */
    void open(const Folder &parentPtr, const std::string &datasetName);
    /**
     * @brief Create a new dataset with the given name.
     *
     * This method tries to create a new dataset with the given name. If the
     * dataset already exists, an exception is thrown.
     * The dimensions of the dataset need to be set during the method call.
     * Optionally, the chunk dimensions can be set.
     * @tparam T Supported data types are: bool, char, unsigned char, short,
     * unsigned short, int, unsigned int, long, unsigned long, long long,
     * unsigned long long, float, double, long double.
     * @param parentPtr Raw HDF5 pointer to a file or group.
     * @param datasetName Name of the dataset to create.
     * @param dims Dimensions of the new dataset.
     * @param chunkDims Chunking dimensions of the dataset. If not set, the
     * chunking is disabled.
     * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the parent
     * or dataset pointer is invalid.
     * @throws PLI::HDF5::Exceptions::DatasetExistsException If the dataset
     * already exists.
     * @throws PLI::HDF5::Exceptions::HDF5RuntimeException If the dataset could
     * not be created.
     */
    template <typename T>
    void create(const Folder &parentPtr, const std::string &datasetName,
                const std::vector<size_t> &dims,
                const std::vector<size_t> &chunkDims = {});

    /**
     * @brief Create a new dataset with the given name.
     *
     * This method tries to create a new dataset with the given name. If the
     * dataset already exists, an exception is thrown.
     * The dimensions of the dataset need to be set during the method call.
     * Optionally, the chunk dimensions can be set.
     * @tparam T Supported data types are: bool, char, unsigned char, short,
     * unsigned short, int, unsigned int, long, unsigned long, long long,
     * unsigned long long, float, double, long double.
     * @param parentPtr Raw HDF5 pointer to a file or group.
     * @param datasetName Name of the dataset to create.
     * @param dims Dimensions of the new dataset.
     * @param chunkDims Chunking dimensions of the dataset. If not set, the
     * chunking is disabled.
     * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the parent
     * or dataset pointer is invalid.
     * @throws PLI::HDF5::Exceptions::DatasetExistsException If the dataset
     * already exists.
     * @throws PLI::HDF5::Exceptions::HDF5RuntimeException If the dataset could
     * not be created.
     */
    void create(
        const Folder &parentPtr, const std::string &datasetName,
        const std::vector<size_t> &dims,
        const std::vector<size_t> &chunkDims = {},
        const PLI::HDF5::Type &dataType = PLI::HDF5::Type::createType<float>());
    /**
     * @brief Check if the dataset exists.
     *
     * Check if the dataset with the given name exists.
     * @param parentPtr Raw HDF5 pointer to a file or group.
     * @param datasetName Name of the dataset.
     * @return true Dataset exists.
     * @return false Dataset does not exist.
     * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the parent
     * pointer is invalid.
     */
    static bool exists(const Folder &parentPtr, const std::string &datasetName);

    /**
     * @brief This method checks if the dataset is created with chunks enabled.
     * @return true Chunks are enabled.
     * @return false Chunks are disabled.
     */
    bool isChunked() const;

    /**
     * @brief Returns the size of the chunks chosen when creating the dataset.
     * If the dataset is not chunked, an exception is thrown.
     * @return std::vector<size_t> Size of the chunks.
     */
    std::vector<size_t> chunkDims() const;

    /**
     * @brief Read the whole dataset.
     *
     * This method reads the whole dataset and returns the data as a vector of
     * the given type. Here, each MPI process will read the whole dataset.
     * @tparam T Supported data types are: bool, char, unsigned char, short,
     * unsigned short, int, unsigned int, long, unsigned long, long long,
     * unsigned long long, float, double, long double.
     * @return std::vector<T> 1D vector with the data.
     * @throws PLI::HDF5::Exceptions::HDF5RuntimeException If the dataset could
     * not be read.
     * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the dataset
     * pointer is invalid.
     */
    template <typename T> std::vector<T> readFullDataset() const;

    /**
     * @brief Read a sub-dataset.
     *
     * With this method, only a sub-area of the dataset can be read. The
     * dimensions of the sub-dataset are given by the offset in each dimension
     * and the count. The count is the number of elements to read in each
     * dimension. The returned data type is determined by the template
     * parameter. The conversion is handled by the HDF5 library. This method
     * does not check if the selected area is valid. If it is out of bounds, an
     * exception is thrown through an erronous HDF5 call.
     * @tparam T Supported data types are: bool, char, unsigned char, short,
     * unsigned short, int, unsigned int, long, unsigned long, long long,
     * unsigned long long, float, double, long double.
     * @param offset Offset in each dimension.
     * @param count Number of elements to read in each dimension.
     * @param stride Stride between each element
     * @return std::vector<T> 1D vector with the data.
     * @throws PLI::HDF5::Exceptions::HDF5RuntimeException If the dataset could
     * not be read.
     * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the dataset
     * pointer is invalid.
     */
    template <typename T>
    std::vector<T> read(const std::vector<size_t> &offset,
                        const std::vector<size_t> &count,
                        const std::vector<size_t> &stride = {}) const;

    /**
     * @brief Write a sub-dataset.
     *
     * With this method, a selected area of the dataset can be written. The
     * dimensions of the area are given by the offset in each dimension and the
     * count. The count is the number of elements to write in each dimension.
     * The data type is determined by the template parameter. The conversion is
     * handled by the HDF5 library.
     * This method does not check if the selected area is valid. If it is out of
     * bounds, an exception is thrown through an erronous HDF5 call.
     * @tparam T Supported data types are: bool, char, unsigned char, short,
     * unsigned short, int, unsigned int, long, unsigned long, long long,
     * unsigned long long, float, double, long double.
     * @param data Data to write.
     * @param offset Offset in each dimension.
     * @param dims Number of elements to write in each dimension.
     * @param stride Stride between each element
     * @throws PLI::HDF5::Exceptions::HDF5RuntimeException If the dataset could
     * not be written.
     * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the dataset
     * pointer is invalid.
     */
    template <typename T>
    void write(const std::vector<T> &data, const std::vector<size_t> &offset,
               const std::vector<size_t> &dims,
               const std::vector<size_t> &stride = {});

    /**
     * @brief Write a sub-dataset.
     *
     * With this method, a selected area of the dataset can be written. The
     * dimensions of the area are given by the offset in each dimension and the
     * count. The count is the number of elements to write in each dimension.
     * The data type is determined by the template parameter. The conversion is
     * handled by the HDF5 library.
     * This method does not check if the selected area is valid. If it is out of
     * bounds, an exception is thrown through an erronous HDF5 call.
     * @tparam T Supported data types are: bool, char, unsigned char, short,
     * unsigned short, int, unsigned int, long, unsigned long, long long,
     * unsigned long long, float, double, long double.
     * @param data Data to write.
     * @param offset Offset in each dimension.
     * @param dims Number of elements to write in each dimension.
     * @param stride Stride between each element
     * @throws PLI::HDF5::Exceptions::HDF5RuntimeException If the dataset could
     * not be written.
     * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the dataset
     * pointer is invalid.
     */
    template <typename T>
    void write(const void *data, const std::vector<size_t> &offset,
               const std::vector<size_t> &dims,
               const std::vector<size_t> &stride = {});

    /**
     * @brief Write a sub-dataset.
     *
     * With this method, a selected area of the dataset can be written. The
     * dimensions of the area are given by the offset in each dimension and the
     * count. The count is the number of elements to write in each dimension.
     * The data type is determined by the template parameter. The conversion is
     * handled by the HDF5 library.
     * This method does not check if the selected area is valid. If it is out of
     * bounds, an exception is thrown through an erronous HDF5 call.
     * @param data Data to write.
     * @param offset Offset in each dimension.
     * @param dims Number of elements to write in each dimension.
     * @param stride Stride between each element
     * @param type Datatype of the data.
     * @throws PLI::HDF5::Exceptions::HDF5RuntimeException If the dataset could
     * not be written.
     * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the dataset
     * pointer is invalid.
     */
    void write(const void *data, const std::vector<size_t> &offset,
               const std::vector<size_t> &dims,
               const std::vector<size_t> &stride, const PLI::HDF5::Type &type);

    /**
     * @brief Get the type of the dataset.
     *
     * @return const PLI::HDF5::Type Type of the dataset
     * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the dataset
     * is not valid or the type doesn't exist.
     */
    const PLI::HDF5::Type type() const;
    /**
     * @brief Returns the number of dimensions of the dataset.
     * @return int The number of dimensions of the dataset.
     * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the dataset
     * is not valid.
     */
    int ndims() const;
    /**
     * @brief Returns the number of elements of the dataset.
     * @return const std::vector<size_t> The number of elements of the dataset
     * in each dimension.
     * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the dataset
     * is not valid.
     * @throws PLI::HDF5::Exceptions::HDF5RuntimeException Error during closing
     * the dataspace of the dataset.
     */
    const std::vector<size_t> dims() const;

    Dataset &operator=(const PLI::HDF5::Dataset &other) noexcept;

    /**
     * @brief Returns a vector of PLI::HDF5::Dataset::OffsetDim of the
     * dataset.
     * @return vector of PLI::HDF5::Dataset::OffsetDim of the dataset from
     * the dataset chunk.
     * @throws PLI::HDF5::Exceptions::HDF5RuntimeException if the
     * dataset is not chunked.
     */
    std::vector<Hyperslab> getChunks() const;

    /**
     * @brief Returns a vector of PLI::HDF5::Dataset::OffsetDim of the
     * dataset.
     * @return vector of PLI::HDF5::Dataset::OffsetDim of the dataset.
     * @param chunkDims user defined chunk dimension of the data.
     * @throws PLI::HDF5::Exceptions::DimensionMismatchException if the
     * the arguments dimensions are mismatched.
     */
    std::vector<Hyperslab>
    getChunks(const std::vector<size_t> &chunkDims) const;

    /**
     * @brief HDF5 Dataset Slice object.
     * Slice object inspired by python.
     */
    struct Slice {
        Slice() = default;
        Slice(size_t start_, size_t stop_, size_t step_)
            : start(start_), stop(stop_), step(step_) {}

        Hyperslab toHyperslab() const;

        bool operator==(const Slice &slice) const;
        bool operator!=(const Slice &slice) const;

        friend std::ostream &operator<<(std::ostream &out, const Slice &slice) {
            out << "[" << slice.start << ":" << slice.stop << ":" << slice.step
                << "]";
            return out;
        }

        size_t start{0};
        size_t stop{0};
        size_t step{0};
    };

    /**
     * @brief HDF5 Dataset Slices object.
     * Slices object inspired by python slice.
     */
    class Slices {
      public:
        Slices() = default;
        explicit Slices(const std::vector<Slice> &slices);
        Slices(const std::initializer_list<Slice> &slices);

        std::vector<Slice>::iterator begin();
        std::vector<Slice>::iterator end();
        std::vector<Slice>::const_iterator cbegin() const;
        std::vector<Slice>::const_iterator cend() const;

        bool operator==(const Slices &slices) const;
        bool operator!=(const Slices &slices) const;

        Slice operator[](size_t i) const;
        Slice &operator[](size_t i);

        void push_back(const Slice &slice);
        void clear();
        size_t size() const;

        Hyperslab toHyperslab() const;

        friend std::ostream &operator<<(std::ostream &out,
                                        const Slices &slices) {
            for (size_t i = 0; i < slices.size(); i++) {
                out << slices[i] << ", ";
            }
            // ??? why does this not work
            // for (auto const slice : slices) {
            //     out << slice << ", ";
            // }
            out << "\b\b"; // remove list ", "
            return out;
        }

      private:
        std::vector<Slice> m_slices;
    };

    /**
     * @brief HDF5 Dataset Hyperslap object for chunking and slicing.
     * Hyperslap object for HDF5 for chunking and slicing.
     */
    class Hyperslab {
      public:
        Hyperslab() = default;
        Hyperslab(std::vector<size_t> offset, std::vector<size_t> count,
                  std::vector<size_t> stride = {});

        const std::vector<size_t> &offset() const { return m_offset; }
        const std::vector<size_t> &count() const { return m_count; }
        const std::vector<size_t> &stride() const { return m_stride; }

        void push_back(size_t offset, size_t count, size_t stride);
        void clear();
        size_t size() const;

        Slices toSlices() const;

        bool operator==(const Hyperslab &hyperslab) const;
        bool operator!=(const Hyperslab &hyperslab) const;
        friend std::ostream &operator<<(std::ostream &out,
                                        const Hyperslab &hyperslab) {
            out << "[";
            std::copy(hyperslab.offset().cbegin(), hyperslab.offset().cend(),
                      std::ostream_iterator<char>(out, ", "));
            out << "\b\b], "; // \b\b -> remove last ", "
            std::copy(hyperslab.count().cbegin(), hyperslab.count().cend(),
                      std::ostream_iterator<char>(out, ", "));
            out << "\b\b], "; // \b\b -> remove last ", "
            std::copy(hyperslab.stride().cbegin(), hyperslab.stride().cend(),
                      std::ostream_iterator<char>(out, ", "));
            out << "\b\b]\n"; // \b\b -> remove last ", "
            return out;
        }

      private:
        std::vector<size_t> m_offset;
        std::vector<size_t> m_count;
        std::vector<size_t> m_stride;
    };

    static std::vector<PLI::HDF5::Dataset::Hyperslab>
    chunkTensor(const std::vector<size_t> &tensorDims,
                const PLI::HDF5::Dataset::Hyperslab &chunk_hyperslab);

  private:
    hid_t createXfID() const;
};
} // namespace HDF5
} // namespace PLI

#include "PLIHDF5/dataset.tpp"
