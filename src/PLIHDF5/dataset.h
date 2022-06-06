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
#include <numeric>
#include <string>
#include <vector>

#include "PLIHDF5/exceptions.h"
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
class Dataset {
 public:
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
  Dataset(const Dataset& otherFile) noexcept;
  /**
   * @brief Construct a new Dataset object
   *
   * Construct a new dataset object by using an existing HDF5 pointer.
   * @param datasetPtr HDF5 pointer to an existing dataset.
   */
  explicit Dataset(hid_t datasetPtr) noexcept;
  /**
   * @brief Open an existing dataset.
   *
   * This method tries to open an existing dataset with the given name. If the
   * dataset does not exist, an exception is thrown.
   * @param parentPtr File or group pointer.
   * @param datasetName Name of the dataset to open.
   * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the parent or
   * dataset pointer is invalid.
   * @throws PLI::HDF5::Exceptions::DatasetNotFoundException If the dataset does
   * not exist.
   */
  void open(const hid_t parentPtr, const std::string& datasetName);
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
   * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the parent or
   * dataset pointer is invalid.
   * @throws PLI::HDF5::Exceptions::DatasetExistsException If the dataset
   * already exists.
   * @throws PLI::HDF5::Exceptions::HDF5RuntimeException If the dataset could
   * not be created.
   */
  template <typename T>
  void create(const hid_t parentPtr, const std::string& datasetName,
              const std::vector<size_t>& dims,
              const std::vector<size_t>& chunkDims = {});

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
   * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the parent or
   * dataset pointer is invalid.
   * @throws PLI::HDF5::Exceptions::DatasetExistsException If the dataset
   * already exists.
   * @throws PLI::HDF5::Exceptions::HDF5RuntimeException If the dataset could
   * not be created.
   */
  void create(
      const hid_t parentPtr, const std::string& datasetName,
      const std::vector<size_t>& dims,
      const std::vector<size_t>& chunkDims = {},
      const PLI::HDF5::Type& dataType = PLI::HDF5::Type::createType<float>());
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
  static bool exists(const hid_t parentPtr, const std::string& datasetName);

  /**
   * @brief This method checks if the dataset is created with chunks enabled.
   * @return true Chunks are enabled.
   * @return false Chunks are disabled.
   */
  bool isChunked() const;

  /**
   * @brief Returns the size of the chunks chosen when creating the dataset. If
   * the dataset is not chunked, an exception is thrown.
   * @return std::vector<size_t> Size of the chunks.
   */
  std::vector<size_t> chunkDims() const;

  /**
   * @brief Closes the dataset if it is valid.
   *
   * If the dataset is valid, it is closed. The dataset pointer is then set to
   * an invalid value (-1) to ensure, that calls will result in an exception
   * afterwards.
   * @throws PLI::HDF5::Exceptions::HDF5RuntimeException If the dataset could
   * not be closed.
   */
  void close();
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
  template <typename T>
  std::vector<T> readFullDataset() const;

  /**
   * @brief Read a sub-dataset.
   *
   * With this method, only a sub-area of the dataset can be read. The
   * dimensions of the sub-dataset are given by the offset in each dimension
   * and the count. The count is the number of elements to read in each
   * dimension. The returned data type is determined by the template parameter.
   * The conversion is handled by the HDF5 library.
   * This method does not check if the selected area is valid. If it is out of
   * bounds, an exception is thrown through an erronous HDF5 call.
   * @tparam T Supported data types are: bool, char, unsigned char, short,
   * unsigned short, int, unsigned int, long, unsigned long, long long,
   * unsigned long long, float, double, long double.
   * @param offset Offset in each dimension.
   * @param count Number of elements to read in each dimension.
   * @return std::vector<T> 1D vector with the data.
   * @throws PLI::HDF5::Exceptions::HDF5RuntimeException If the dataset could
   * not be read.
   * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the dataset
   * pointer is invalid.
   */
  template <typename T>
  std::vector<T> read(const std::vector<size_t>& offset,
                      const std::vector<size_t>& count) const;

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
   * @throws PLI::HDF5::Exceptions::HDF5RuntimeException If the dataset could
   * not be written.
   * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the dataset
   * pointer is invalid.
   */
  template <typename T>
  void write(const std::vector<T>& data, const std::vector<size_t>& offset,
             const std::vector<size_t>& dims);

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
   * @throws PLI::HDF5::Exceptions::HDF5RuntimeException If the dataset could
   * not be written.
   * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the dataset
   * pointer is invalid.
   */
  template <typename T>
  void write(const void* data, const std::vector<size_t>& offset,
             const std::vector<size_t>& dims);

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
   * @param type Datatype of the data.
   * @throws PLI::HDF5::Exceptions::HDF5RuntimeException If the dataset could
   * not be written.
   * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the dataset
   * pointer is invalid.
   */
  void write(const void* data, const std::vector<size_t>& offset,
             const std::vector<size_t>& dims, const PLI::HDF5::Type& type);

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
   * @return const std::vector<size_t> The number of elements of the dataset in
   * each dimension.
   * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the dataset
   * is not valid.
   * @throws PLI::HDF5::Exceptions::HDF5RuntimeException Error during closing
   * the dataspace of the dataset.
   */
  const std::vector<size_t> dims() const;
  /**
   * @brief Get the raw HDF5 pointer of the dataset.
   *
   * Returns the raw HDF5 pointer of the dataset. This pointer can be used to
   * access the dataset using the HDF5 library.
   * @return hid_t Dataset ID stored in the object.
   */
  hid_t id() const noexcept;

  /**
   * @brief Convert the dataset to a the raw HDF5 pointer.
   * @return hid_t Dataset ID stored in the object.
   */
  operator hid_t() const noexcept;
  Dataset& operator=(const PLI::HDF5::Dataset& other) noexcept;

 private:
  hid_t createXfID() const;
  hid_t m_id;
};

/**
 * @brief Open an existing dataset.
 *
 * This method tries to open an existing dataset with the given name. If the
 * dataset does not exist, an exception is thrown.
 * @param parentPtr File or group pointer.
 * @param datasetName Name of the dataset to open.
 * @return PLI::HDF5::Dataset Dataset if successful.
 * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the parent or
 * dataset pointer is invalid.
 * @throws PLI::HDF5::Exceptions::DatasetNotFoundException If the dataset does
 * not exist.
 */
PLI::HDF5::Dataset openDataset(const hid_t parentPtr,
                               const std::string& datasetName);
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
 * @return PLI::HDF5::Dataset New dataset object if successful.
 * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the parent or
 * dataset pointer is invalid.
 * @throws PLI::HDF5::Exceptions::DatasetExistsException If the dataset
 * already exists.
 * @throws PLI::HDF5::Exceptions::HDF5RuntimeException If the dataset could
 * not be created.
 */
template <typename T>
PLI::HDF5::Dataset createDataset(const hid_t parentPtr,
                                 const std::string& datasetName,
                                 const std::vector<size_t>& dims,
                                 const std::vector<size_t>& chunkDims = {});

/**
 * @brief Create a new dataset with the given name.
 *
 * This method tries to create a new dataset with the given name. If the
 * dataset already exists, an exception is thrown.
 * The dimensions of the dataset need to be set during the method call.
 * Optionally, the chunk dimensions can be set.
 * @param parentPtr Raw HDF5 pointer to a file or group.
 * @param datasetName Name of the dataset to create.
 * @param dims Dimensions of the new dataset.
 * @param chunkDims Chunking dimensions of the dataset. If not set, the
 * chunking is disabled.
 * @param dataType Datatype of the dataset. Default = float.
 * @return PLI::HDF5::Dataset New dataset object if successful.
 * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the parent or
 * dataset pointer is invalid.
 * @throws PLI::HDF5::Exceptions::DatasetExistsException If the dataset
 * already exists.
 * @throws PLI::HDF5::Exceptions::HDF5RuntimeException If the dataset could
 * not be created.
 */
PLI::HDF5::Dataset createDataset(
    const hid_t parentPtr, const std::string& datasetName,
    const std::vector<size_t>& dims, const std::vector<size_t>& chunkDims = {},
    const PLI::HDF5::Type& dataType = PLI::HDF5::Type::createType<float>());

}  // namespace HDF5
}  // namespace PLI

#include "PLIHDF5/dataset.tpp"
