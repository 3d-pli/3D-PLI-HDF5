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
#include <mpi.h>
#include <optional>
#include <string>
#include <vector>

#include "PLIHDF5/exceptions.h"
#include "PLIHDF5/type.h"

namespace PLI::HDF5 {
class Group;
class Dataset;

class Object {
  public:
    explicit Object(const std::optional<MPI_Comm> &communicator = {}) noexcept;
    explicit Object(const hid_t id,
                    const std::optional<MPI_Comm> &communicator = {}) noexcept;
    ~Object();

    hid_t id() const noexcept;
    std::optional<MPI_Comm> communicator() const noexcept;
    operator hid_t() const noexcept;

    void close();

  protected:
    std::optional<MPI_Comm> m_communicator;
    hid_t m_id;

    void closeFileObjects(unsigned int types);
};

class Folder : public Object {
  public:
    explicit Folder(const std::optional<MPI_Comm> &communicator = {}) noexcept;
    explicit Folder(const hid_t id,
                    const std::optional<MPI_Comm> &communicator = {}) noexcept;
    /**
     * @brief Opens a group
     * Open an existing group. If the group does not exist, an exception is
     * thrown.
     * @param parentPtr Pointer to the parent group or file.
     * @param groupName Name of the group to open.
     * @return PLI::HDF5::Group Group object, if successful.
     * @throws PLI::HDF5::Exceptions::GroupNotFoundException If the group does
     * not exist.
     * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If opening the
     * group fails or the parentPtr is invalid.
     */
    Group openGroup(const std::string &groupName);
    /**
     * @brief Creates a group
     * Create a new group. If the group already exists, an exception is thrown.
     * @param parentPtr Pointer to the parent group or file.
     * @param groupName Name of the group to create.
     * @return PLI::HDF5::Group Group object, if successful.
     * @throws PLI::HDF5::Exceptions::GroupExistsException If the group already
     * exists.
     * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If creating
     * the group fails or the parentPtr is invalid.
     */
    Group createGroup(const std::string &groupName);

    /**
     * @brief Open an existing dataset.
     *
     * This method tries to open an existing dataset with the given name. If the
     * dataset does not exist, an exception is thrown.
     * @param parentPtr File or group pointer.
     * @param datasetName Name of the dataset to open.
     * @return PLI::HDF5::Dataset Dataset if successful.
     * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the parent
     * or dataset pointer is invalid.
     * @throws PLI::HDF5::Exceptions::DatasetNotFoundException If the dataset
     * does not exist.
     */
    Dataset openDataset(const std::string &datasetName);
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
     * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the parent
     * or dataset pointer is invalid.
     * @throws PLI::HDF5::Exceptions::DatasetExistsException If the dataset
     * already exists.
     * @throws PLI::HDF5::Exceptions::HDF5RuntimeException If the dataset could
     * not be created.
     */
    template <typename T>
    Dataset createDataset(const std::string &datasetName,
                          const std::vector<size_t> &dims,
                          const std::vector<size_t> &chunkDims = {});

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
     * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the parent
     * or dataset pointer is invalid.
     * @throws PLI::HDF5::Exceptions::DatasetExistsException If the dataset
     * already exists.
     * @throws PLI::HDF5::Exceptions::HDF5RuntimeException If the dataset could
     * not be created.
     */
    Dataset createDataset(
        const std::string &datasetName, const std::vector<size_t> &dims,
        const std::vector<size_t> &chunkDims = {},
        const PLI::HDF5::Type &dataType = PLI::HDF5::Type::createType<float>());
};

} // namespace PLI::HDF5
