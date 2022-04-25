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

PLI::HDF5::File PLI::HDF5::File::create(const std::string& fileName) {
  if (PLI::HDF5::File::fileExists(fileName)) {
    // TODO(jreuter): Write Exceptions!
    throw 0;
  }

  hid_t fapl_id = H5Pcreate(H5P_FILE_ACCESS);
  H5Pset_fapl_mpio(fapl_id, MPI_COMM_WORLD, MPI_INFO_NULL);

  hid_t filePtr =
      H5Fcreate(fileName.c_str(), H5F_ACC_EXCL, H5P_DEFAULT, fapl_id);
  if (filePtr <= 0) {
    // TODO(jreuter): Write Exceptions!
    throw 0;
  }
  return PLI::HDF5::File(filePtr, fapl_id);
}

PLI::HDF5::File PLI::HDF5::File::open(const std::string& fileName,
                                      const unsigned openState) {
  if (!PLI::HDF5::File::fileExists(fileName)) {
    // TODO(jreuter): Write Exceptions!
    throw 0;
  }
  if (!PLI::HDF5::File::isHDF5(fileName)) {
    // TODO(jreuter): Write Exceptions!
    throw 0;
  }

  hid_t access;
  if (openState == 0) {
    access = H5F_ACC_RDWR;
  } else {
    access = H5F_ACC_RDONLY;
  }

  hid_t fapl_id = H5Pcreate(H5P_FILE_ACCESS);
  H5Pset_fapl_mpio(fapl_id, MPI_COMM_WORLD, MPI_INFO_NULL);

  // TODO(jreuter): Check the openState variable!
  hid_t filePtr = H5Fopen(fileName.c_str(), access, fapl_id);
  if (filePtr <= 0) {
    // TODO(jreuter): Write Exceptions!
    throw 0;
  }
  return PLI::HDF5::File(filePtr, fapl_id);
}

void PLI::HDF5::File::close() {
  if (this->m_id > 0) {
    H5Pclose(this->m_faplID);
    H5Fclose(this->m_id);
    this->m_id = int64_t(-1);
  }
}

void PLI::HDF5::File::reopen() { this->m_id = H5Freopen(this->m_id); }

void PLI::HDF5::File::flush() {
  if (H5Fflush(this->m_id, H5F_SCOPE_LOCAL) <= 0) {
    // TODO(jreuter): Write Exceptions!
    throw 0;
  }
}

bool PLI::HDF5::File::isHDF5(const std::string& fileName) {
  return H5Fis_hdf5(fileName.c_str()) > 0;
}

bool PLI::HDF5::File::fileExists(const std::string& fileName) {
  return std::filesystem::exists(fileName);
}

hid_t PLI::HDF5::File::id() const { return this->m_id; }

hid_t PLI::HDF5::File::faplID() const { return this->m_faplID; }

PLI::HDF5::File::File() : m_id(-1) {}

PLI::HDF5::File::File(const File& other) : m_id(other.id()) {}

PLI::HDF5::File::File(const hid_t filePtr, const hid_t faplID)
    : m_id(filePtr), m_faplID(faplID) {}

PLI::HDF5::File::~File() {}

PLI::HDF5::File::operator hid_t() const { return this->m_id; }
