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

#include <filesystem>
#include <optional>
#include <string>

#include "PLIHDF5/exceptions.h"
#include "PLIHDF5/object.h"

/**
 * @brief The PLI namespace
 */
namespace PLI {
/**
 * @brief The HDF5 namespace
 */
namespace HDF5 {
/**
 * @brief HDF5 File wrapper class.
 * Create and / or open an HDF5 file to store or read data.
 */
class File : public Object {
  public:
    enum OpenState { ReadOnly = 0, ReadWrite = 1 };

    /**
     * @brief Construct a new File object
     *
     * Create an empty file object. Calling other methods on this object will
     * fail because no pointer to an HDF5 object is set. To open a file, use
     * PLI::HDF5::File::open(const std::string& fileName). To create a file, use
     * PLI::HDF5::File::create(const std::string& fileName).
     */
    explicit File(const std::optional<MPI_Comm> communicator = {});
    /**
     * @brief Construct a new File object
     *
     * Construct a new file object by using the given pointer of another file
     * object.
     * @param otherFile Other file object.
     */
    File(const File &otherFile);
    /**
     * @brief Construct a new File object
     *
     * Construct a new file object by using the given pointer of another file
     * object.
     * @param filePtr File pointer.
     * @param faplID File access pointer.
     */
    explicit File(const hid_t filePtr, const hid_t faplID);

    ~File();

    /**
     * @brief Create a new file.
     *
     * Create a new file. Per default, the file is created with
     * MPI file access.
     * @param fileName File name.
     * @param useMPIFileAccess If true, the file will be opened with MPI access.
     * Actions need to be done collectively.
     * @throw PLI::HDF5::Exceptions::FileExistsException If the file already
     * exists.
     * @throw PLI::HDF5::Exceptions::IdentifierNotValidException If the file
     * couldn't be created or the file access token failed.
     * @throw PLI::HDF5::Exceptions::HDF5RuntimeException Error during setting
     * the MPI file access.
     */
    void create(const std::string &fileName,
                const std::optional<MPI_Comm> communicator = {});
    /**
     * @brief Open an existing file.
     *
     * This method tries to open an existing file. If the file doesnt exist, an
     * exception is thrown. The file will be opened with MPI file access. By
     * using the openState variable, one can choose between opening the file in
     * read-only mode or in read-write mode. Setting openState to
     * PLI::HDF5::File::OpenState::ReadOnly will open the file in read-only
     * mode. Setting openState to PLI::HDF5::File::OpenState::ReadWrite will
     * open the file in read-write mode.
     * @param fileName File name which will be opened.
     * @param openState OpenState variable. Available values are
     * PLI::HDF5::File::OpenState::ReadOnly and
     * PLI::HDF5::File::OpenState::ReadWrite.
     * @param useMPIFileAccess If true, the file will be opened with MPI access.
     * Actions need to be done collectively.
     * @throws PLI::HDF5::Exceptions::FileNotFoundException If the file doesn't
     * exist.
     * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the file
     * couldn't be opened or the file access token failed.
     * @throws PLI::HDF5::Exceptions::HDF5RuntimeException Error during setting
     * the MPI file access.
     * @throws PLI::HDF5::Exceptions::InvalidHDF5FileException If the file is
     * not a valid HDF5 file.
     */
    void open(const std::string &fileName, const OpenState openState,
              const std::optional<MPI_Comm> communicator = {});

    /**
     * @brief Check if the file is a valid HDF5 file.
     *
     * This method checks if the file is a valid HDF5 file that can be opened
     * with this library.
     * Warning: This method is not thread-safe.
     * @param fileName Filename of the file.
     * @return true File is a valid HDF5 file.
     * @return false File isn't a valid HDF5 file.
     */
    static bool isHDF5(const std::string &fileName);
    /**
     * @brief Check if the file exists.
     * Warning: This method is not thread-safe.
     * @param fileName Filename of the file.
     * @return true File exists.
     * @return false File doesn't exist.
     */
    static bool fileExists(const std::string &fileName);

    /**
     * @brief Returns if MPI File Access was enabled when creating / opening the
     * file This function will return false if MPI is not available.
     * @return true MPI File Access is enabled
     * @return false MPI File Access is diabled
     */
    bool usesMPIFileAccess() const;

    /**
     * @brief Closes the file if it is valid.
     *
     * If the file is valid, it is closed. The file pointer is then set to
     * an invalid value (-1) to ensure, that calls will result in an exception
     * afterwards.
     * @throws PLI::HDF5::Exceptions::HDF5RuntimeException If the file could
     * not be closed.
     */
    void close();
    /**
     * @brief Reopen the file.
     *
     * This method tries to reopen the file for reading. If the original pointer
     * is invalid or the reopen failed, an exception is thrown.
     * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the file
     * pointer is invalid or the reopen failed.
     */
    void reopen();
    /**
     * @brief Flushes the file content in a local scope.
     * @throws PLI::HDF5::Exceptions::HDF5RuntimeException If the file could not
     * be flushed.
     * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the file
     * pointer is invalid.
     */
    void flush();

    /**
     * @brief Get the file access pointer.
     * @return hid_t File access pointer.
     */
    hid_t faplID() const;

    File &operator=(const PLI::HDF5::File &otherFile) noexcept;

  private:
    hid_t createFaplID() const;
    hid_t m_faplID;
};

/**
 * @brief Create a new file object.
 *
 * Create a new file object. Per default, the file is created with
 * MPI file access.
 * @param fileName File name.
 * @param useMPIFileAccess If true, the file will be opened with MPI access.
 * Actions need to be done collectively.
 * @return PLI::HDF5::File File object, if successful.
 * @throw PLI::HDF5::Exceptions::FileExistsException If the file already
 * exists.
 * @throw PLI::HDF5::Exceptions::IdentifierNotValidException If the file
 * couldn't be created or the file access token failed.
 * @throw PLI::HDF5::Exceptions::HDF5RuntimeException Error during setting the
 * MPI file access.
 */
PLI::HDF5::File createFile(const std::string &fileName,
                           const std::optional<MPI_Comm> communicator = {});

/**
 * @brief Open an existing file object.
 *
 * This method tries to open an existing file. If the file doesnt exist, an
 * exception is thrown. The file will be opened with MPI file access. By using
 * the openState variable, one can choose between opening the file in
 * read-only mode or in read-write mode. Setting openState to
 * PLI::HDF5::File::OpenState::ReadOnly will open the file in read-only mode.
 * Setting openState to PLI::HDF5::File::OpenState::ReadWrite will open the
 * file in read-write mode.
 * @param fileName File name which will be opened.
 * @param openState OpenState variable. Available values are
 * PLI::HDF5::File::OpenState::ReadOnly and
 * PLI::HDF5::File::OpenState::ReadWrite.
 * @param useMPIFileAccess If true, the file will be opened with MPI access.
 * Actions need to be done collectively.
 * @return PLI::HDF5::File File object if successful.
 * @throws PLI::HDF5::Exceptions::FileNotFoundException If the file doesn't
 * exist.
 * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the file
 * couldn't be opened or the file access token failed.
 * @throws PLI::HDF5::Exceptions::HDF5RuntimeException Error during setting
 * the MPI file access.
 * @throws PLI::HDF5::Exceptions::InvalidHDF5FileException If the file is not
 * a valid HDF5 file.
 */
PLI::HDF5::File
openFile(const std::string &fileName,
         const File::OpenState openState = File::OpenState::ReadOnly,
         const std::optional<MPI_Comm> communicator = {});
} // namespace HDF5
} // namespace PLI
