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

#include "PLIHDF5/group.h"

PLI::HDF5::Group PLI::HDF5::Group::open(hid_t parentPtr,
                                        const std::string& groupName) {
  if (!PLI::HDF5::Group::exists(parentPtr, groupName)) {
    throw GroupNotFoundException("Group not found: " + groupName);
  }
  return PLI::HDF5::Group(H5Gopen(parentPtr, groupName.c_str(), H5P_DEFAULT));
}

PLI::HDF5::Group PLI::HDF5::Group::create(hid_t parentPtr,
                                          const std::string& groupName) {
  if (PLI::HDF5::Group::exists(parentPtr, groupName)) {
    throw GroupExistsException("Group already exists: " + groupName);
  }
  return PLI::HDF5::Group(H5Gcreate(parentPtr, groupName.c_str(), H5P_DEFAULT,
                                    H5P_DEFAULT, H5P_DEFAULT));
}

bool PLI::HDF5::Group::exists(hid_t parentPtr, const std::string& groupName) {
  return H5Lexists(parentPtr, groupName.c_str(), H5P_DEFAULT) == 1;
}

void PLI::HDF5::Group::close() {
  if (this->m_id > 0) {
    H5Gclose(this->m_id);
  }
  this->m_id = -1;
}

hid_t PLI::HDF5::Group::id() const noexcept { return this->m_id; }

PLI::HDF5::Group::Group(const hid_t groupPtr) noexcept : m_id(groupPtr) {}

PLI::HDF5::Group::Group() noexcept : m_id(-1) {}

PLI::HDF5::Group::~Group() {}

PLI::HDF5::Group::operator hid_t() const noexcept { return this->m_id; }
