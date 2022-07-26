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
#include <iostream>

void PLI::HDF5::Group::open(const Folder &parentPtr,
                            const std::string &groupName) {
    checkHDF5Ptr(parentPtr, "Group::open");
    if (!PLI::HDF5::Group::exists(parentPtr, groupName)) {
        throw Exceptions::GroupNotFoundException("Group not found: " +
                                                 groupName);
    }
    hid_t groupPtr = H5Gopen(parentPtr, groupName.c_str(), H5P_DEFAULT);
    checkHDF5Ptr(groupPtr);
    this->m_id = groupPtr;
}

void PLI::HDF5::Group::create(const Folder &parentPtr,
                              const std::string &groupName) {
    checkHDF5Ptr(parentPtr, "Group::create");
    if (PLI::HDF5::Group::exists(parentPtr, groupName)) {
        throw Exceptions::GroupExistsException("Group already exists: " +
                                               groupName);
    }
    hid_t groupPtr = H5Gcreate(parentPtr, groupName.c_str(), H5P_DEFAULT,
                               H5P_DEFAULT, H5P_DEFAULT);
    checkHDF5Ptr(groupPtr, "H5Gcreate");
    this->m_id = groupPtr;
}

bool PLI::HDF5::Group::exists(const Folder &parentPtr,
                              const std::string &groupName) {
    checkHDF5Ptr(parentPtr, "Group::exists");
    return H5Lexists(parentPtr, groupName.c_str(), H5P_DEFAULT) > 0;
}

PLI::HDF5::Group::Group(const hid_t groupPtr,
                        const std::optional<MPI_Comm> communicator) noexcept
    : PLI::HDF5::Folder(groupPtr, communicator) {}

PLI::HDF5::Group::Group(const Group &group) noexcept
    : PLI::HDF5::Folder(group.id(), group.communicator()) {}

PLI::HDF5::Group::Group() noexcept : PLI::HDF5::Folder() {}

PLI::HDF5::Group &
PLI::HDF5::Group::operator=(const PLI::HDF5::Group &otherGroup) noexcept {
    this->close();

    this->m_id = otherGroup.id();
    checkHDF5Call(H5Iinc_ref(m_id), "H5Iinc_ref");
    this->m_communicator = otherGroup.communicator();
    return *this;
}

PLI::HDF5::Group PLI::HDF5::Folder::createGroup(const std::string &groupName) {
    Group group;
    group.create(*this, groupName);
    return group;
}
PLI::HDF5::Group PLI::HDF5::Folder::openGroup(const std::string &groupName) {
    Group group;
    group.open(*this, groupName);
    return group;
}
