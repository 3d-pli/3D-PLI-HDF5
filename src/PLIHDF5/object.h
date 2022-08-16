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

#include "PLIHDF5/type.h"

namespace PLI::HDF5 {
class Group;
class Dataset;

/**
 * @brief HDF5 Object class
 * Each HDF5 file contains groups, datasets and attributes. Each one can be
 * described through an HDF5 Object (H5O). This class offers a small wrapper
 * to help with other HDF5 functionality.
 */
class Object {
  public:
    ~Object();

    /**
     * @brief Get the raw HDF5 pointer of the object.
     *
     * Returns the object HDF5 pointer of the object. This pointer can be used
     * to access the dataset using the HDF5 library.
     * @return hid_t Object ID stored.
     */
    hid_t id() const noexcept;
    /**
     * @brief Returns the MPI_Communicator set when creating an HDF5 object.
     * Certain HDF5 objects like File and Dataset might need an MPI_Comm to
     * define their transaction. To help with this behaviour, the communicator
     * used for all operations is stored in the Object class and passed to all
     * other created instances.
     * @return MPI_Comm if one is set, else an empty optional.
     */
    std::optional<MPI_Comm> communicator() const noexcept;
    /**
     * @brief Convert the dataset to a the raw HDF5 pointer.
     * @return hid_t Dataset ID stored in the object.
     */
    operator hid_t() const noexcept;
    /**
     * @brief Closes the HDF5 object.
     * Manually closes the HDF5 object. If a file is closed, all other objects
     * in that file are closed as well to prevent issues. Calling the method
     * from a closed instance will do nothing. When closing an HDF5 object which
     * was copied earlier, the object will still be alive until all instances or
     * the file are closed.
     */
    void close();

  protected:
    explicit Object(const std::optional<MPI_Comm> &communicator = {}) noexcept;
    explicit Object(const hid_t id,
                    const std::optional<MPI_Comm> &communicator = {}) noexcept;

    std::optional<MPI_Comm> m_communicator;
    hid_t m_id;

    void closeFileObjects(unsigned int types);
};

class Folder : public Object {
  public:
    /**
     * @brief Opens a group
     * Open an existing group. If the group does not exist, an exception is
     * thrown.
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

  protected:
    explicit Folder(const std::optional<MPI_Comm> &communicator = {}) noexcept;
    explicit Folder(const hid_t id,
                    const std::optional<MPI_Comm> &communicator = {}) noexcept;
};

} // namespace PLI::HDF5
