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

#include "PLIHDF5/file.h"

PLI::HDF5::File PLI::HDF5::createFile(const std::string &fileName,
                                      const bool useMPIFileAccess) {
    PLI::HDF5::File file;
    file.create(fileName, useMPIFileAccess);
    return file;
}

void PLI::HDF5::File::create(const std::string &fileName,
                             const bool useMPIFileAccess) {
    if (PLI::HDF5::File::fileExists(fileName)) {
        throw Exceptions::FileExistsException("File already exists: " +
                                              fileName);
    }

    hid_t fapl_id = createFaplID(useMPIFileAccess);
    hid_t filePtr =
        H5Fcreate(fileName.c_str(), H5F_ACC_EXCL, H5P_DEFAULT, fapl_id);
    checkHDF5Ptr(filePtr, "H5Fcreate");

    this->m_id = filePtr;
    this->m_faplID = fapl_id;
}

PLI::HDF5::File PLI::HDF5::openFile(const std::string &fileName,
                                    const File::OpenState openState,
                                    const bool useMPIFileAccess) {
    PLI::HDF5::File file;
    file.open(fileName, openState, useMPIFileAccess);
    return file;
}

void PLI::HDF5::File::open(const std::string &fileName,
                           const File::OpenState openState,
                           const bool useMPIFileAccess) {
    if (!PLI::HDF5::File::fileExists(fileName)) {
        throw Exceptions::FileNotFoundException("File not found: " + fileName);
    }
    if (!PLI::HDF5::File::isHDF5(fileName)) {
        throw Exceptions::InvalidHDF5FileException("File " + fileName +
                                                   " is not an HDF5 file.");
    }

    hid_t access;
    if (openState == 0) {
        access = H5F_ACC_RDONLY;
    } else {
        access = H5F_ACC_RDWR;
    }

    hid_t fapl_id = createFaplID(useMPIFileAccess);
    hid_t filePtr = H5Fopen(fileName.c_str(), access, fapl_id);
    checkHDF5Ptr(filePtr, "H5Fopen");

    this->m_id = filePtr;
    this->m_faplID = fapl_id;
}

void PLI::HDF5::File::close() {
    if (this->m_id > 0) {
        checkHDF5Call(H5Pclose(this->m_faplID), "H5Pclose");
        checkHDF5Call(H5Fclose(this->m_id), "H5Fclose");
        this->m_id = int64_t(-1);
    }
}

void PLI::HDF5::File::reopen() {
    checkHDF5Ptr(this->m_id, "H5Fopen");
    this->m_id = H5Freopen(this->m_id);
    checkHDF5Ptr(this->m_id, "H5Freopen");
}

void PLI::HDF5::File::flush() {
    checkHDF5Ptr(this->m_id, "H5Fflush");
    checkHDF5Call(H5Fflush(this->m_id, H5F_SCOPE_LOCAL), "H5Fflush");
}

bool PLI::HDF5::File::isHDF5(const std::string &fileName) {
    if (checkMPI()) {
        MPI_Barrier(MPI_COMM_WORLD);
    }
    return H5Fis_hdf5(fileName.c_str()) > 0;
}

bool PLI::HDF5::File::fileExists(const std::string &fileName) {
    if (checkMPI()) {
        MPI_Barrier(MPI_COMM_WORLD);
    }
    return std::filesystem::exists(fileName);
}

hid_t PLI::HDF5::File::id() const { return this->m_id; }

hid_t PLI::HDF5::File::faplID() const { return this->m_faplID; }

PLI::HDF5::File::File() : m_id(-1), m_faplID(-1) {}

PLI::HDF5::File::File(const File &other)
    : m_id(other.id()), m_faplID(other.faplID()) {}

PLI::HDF5::File::File(const hid_t filePtr, const hid_t faplID)
    : m_id(filePtr), m_faplID(faplID) {}

PLI::HDF5::File::operator hid_t() const { return this->m_id; }

PLI::HDF5::File &PLI::HDF5::File::operator=(const File &other) noexcept {
    this->m_id = other.id();
    this->m_faplID = other.faplID();
    return *this;
}

hid_t PLI::HDF5::File::createFaplID(bool useMPIFileAccess) const {
    hid_t fapl_id = H5Pcreate(H5P_FILE_ACCESS);
    checkHDF5Ptr(fapl_id, "H5Pcreate");
    if (useMPIFileAccess && checkMPI()) {
        checkHDF5Call(H5Pset_fapl_mpio(fapl_id, MPI_COMM_WORLD, MPI_INFO_NULL));
    }
    return fapl_id;
}

bool PLI::HDF5::File::checkMPI() {
    int flag;
    MPI_Initialized(&flag);
    return flag;
}
