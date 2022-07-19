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
#include <iostream>

PLI::HDF5::File
PLI::HDF5::createFile(const std::string &fileName,
                      const std::optional<MPI_Comm> communicator) {
    PLI::HDF5::File file;
    file.create(fileName, communicator);
    return file;
}

void PLI::HDF5::File::create(const std::string &fileName,
                             const std::optional<MPI_Comm> communicator) {
    if (PLI::HDF5::File::fileExists(fileName)) {
        throw Exceptions::FileExistsException("File already exists: " +
                                              fileName);
    }
    this->m_communicator = communicator;
    hid_t fapl_id = createFaplID();
    hid_t filePtr =
        H5Fcreate(fileName.c_str(), H5F_ACC_EXCL, H5P_DEFAULT, fapl_id);
    checkHDF5Ptr(filePtr, "H5Fcreate");

    this->m_id = filePtr;
    this->m_faplID = fapl_id;
}

PLI::HDF5::File
PLI::HDF5::openFile(const std::string &fileName,
                    const File::OpenState openState,
                    const std::optional<MPI_Comm> communicator) {
    PLI::HDF5::File file;
    file.open(fileName, openState, communicator);
    return file;
}

void PLI::HDF5::File::open(const std::string &fileName,
                           const File::OpenState openState,
                           const std::optional<MPI_Comm> communicator) {
    if (!PLI::HDF5::File::fileExists(fileName)) {
        throw Exceptions::FileNotFoundException("File not found: " + fileName);
    }
    if (!PLI::HDF5::File::isHDF5(fileName)) {
        throw Exceptions::InvalidHDF5FileException("File " + fileName +
                                                   " is not an HDF5 file.");
    }
    this->m_communicator = communicator;

    hid_t access;
    if (openState == 0) {
        access = H5F_ACC_RDONLY;
    } else {
        access = H5F_ACC_RDWR;
    }

    hid_t fapl_id = createFaplID();
    hid_t filePtr = H5Fopen(fileName.c_str(), access, fapl_id);
    checkHDF5Ptr(filePtr, "H5Fopen");

    this->m_id = filePtr;
    this->m_faplID = fapl_id;
}

void PLI::HDF5::File::close() {
    if (this->m_id > 0) {
        checkHDF5Call(H5Idec_ref(this->m_faplID), "H5Idec_ref");
        checkHDF5Call(H5Idec_ref(this->m_id), "H5Idec_ref");
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
    return H5Fis_hdf5(fileName.c_str()) > 0;
}

bool PLI::HDF5::File::fileExists(const std::string &fileName) {
    return std::filesystem::exists(fileName);
}

hid_t PLI::HDF5::File::faplID() const { return this->m_faplID; }

PLI::HDF5::File::File(const std::optional<MPI_Comm> communicator)
    : Object(communicator), m_faplID(-1) {}

PLI::HDF5::File::File(const File &other)
    : Object(other.id(), other.communicator()), m_faplID(other.faplID()) {}

PLI::HDF5::File::~File() {
    std::cerr << __func__ << std::endl;
    close();
}

PLI::HDF5::File::File(const hid_t filePtr, const hid_t faplID)
    : Object(filePtr), m_faplID(faplID) {
    // Determine the MPI state through our file access property list.
    MPI_Comm communicator;
    MPI_Info info;
    H5Pget_fapl_mpio(m_faplID, &communicator, &info);

    this->m_communicator = communicator;
}

PLI::HDF5::File &PLI::HDF5::File::operator=(const File &other) noexcept {
    this->m_id = other.id();
    checkHDF5Call(H5Iinc_ref(this->m_id), "H5Iinc_ref");
    this->m_faplID = other.faplID();
    checkHDF5Call(H5Iinc_ref(this->m_faplID), "H5Iinc_ref");
    return *this;
}

hid_t PLI::HDF5::File::createFaplID() const {
    hid_t fapl_id = H5Pcreate(H5P_FILE_ACCESS);
    checkHDF5Ptr(fapl_id, "H5Pcreate");
    if (m_communicator) {
        checkHDF5Call(
            H5Pset_fapl_mpio(fapl_id, m_communicator.value(), MPI_INFO_NULL));
    }
    return fapl_id;
}

bool PLI::HDF5::File::usesMPIFileAccess() const {
    return m_communicator.has_value();
}
