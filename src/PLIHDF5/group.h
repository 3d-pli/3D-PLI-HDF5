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
 * @brief HDF5 Group wrapper class.
 * Create and / or open an HDF5 group.
 */
class Group : public Object {
  public:
    /**
     * @brief Construct a new Group object
     *
     * Create an empty group object. Calling other methods on this object will
     * fail because no pointer to an HDF5 object is set. To open a group, use
     * PLI::HDF5::Group::open(hid_t parentPtr, const std::string& groupName). To
     * create a grouop, use PLI::HDF5::Group::create(hid_t parentPtr, const
     * std::string& groupName).
     */
    Group() noexcept;
    /**
     * @brief Construct a new Group object
     *
     * Construct a new group object by using the given HDF5 pointer of another
     * group object.
     * @param otherGroup Other group object.
     */
    explicit Group(const hid_t groupPtr,
                   const std::optional<MPI_Comm> communicator = {}) noexcept;
    /**
     * @brief Construct a new Group object
     *
     * Construct a new group object by using another
     * group object.
     * @param otherGroup Other group object.
     */
    Group(const Group &otherGroup) noexcept;

    ~Group();
    /**
     * @brief Opens a group
     *
     * Open an existing group. If the group does not exist, an exception is
     * thrown.
     * @param parentPtr Pointer to the parent group or file.
     * @param groupName Name of the group to open.
     * @throws PLI::HDF5::Exceptions::GroupNotFoundException If the group does
     * not exist.
     * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If opening the
     * group fails or the parentPtr is invalid.
     */
    void open(const Object &parentPtr, const std::string &groupName);
    /**
     * @brief Creates a group
     *
     * Create a new group. If the group already exists, an exception is thrown.
     * @param parentPtr Pointer to the parent group or file.
     * @param groupName Name of the group to create.
     * @throws PLI::HDF5::Exceptions::GroupExistsException If the group already
     * exists.
     * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If creating
     * the group fails or the parentPtr is invalid.
     */
    void create(const Object &parentPtr, const std::string &groupName);
    /**
     * @brief Checks if the group exists
     * @param parentPtr Pointer to the parent group or file
     * @param groupName Name of the group to check.
     * @return true If the group exists.
     * @return false If the group does not exist.
     * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the
     * parentPtr is invalid.
     */
    static bool exists(const Object &parentPtr, const std::string &groupName);
    Group &operator=(const PLI::HDF5::Group &otherGroup) noexcept;
};

/**
 * @brief Opens a group
 * Open an existing group. If the group does not exist, an exception is
 * thrown.
 * @param parentPtr Pointer to the parent group or file.
 * @param groupName Name of the group to open.
 * @return PLI::HDF5::Group Group object, if successful.
 * @throws PLI::HDF5::Exceptions::GroupNotFoundException If the group does not
 * exist.
 * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If opening the
 * group fails or the parentPtr is invalid.
 */
PLI::HDF5::Group openGroup(const Object &parentPtr,
                           const std::string &groupName);
/**
 * @brief Creates a group
 * Create a new group. If the group already exists, an exception is thrown.
 * @param parentPtr Pointer to the parent group or file.
 * @param groupName Name of the group to create.
 * @return PLI::HDF5::Group Group object, if successful.
 * @throws PLI::HDF5::Exceptions::GroupExistsException If the group already
 * exists.
 * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If creating the
 * group fails or the parentPtr is invalid.
 */
PLI::HDF5::Group createGroup(const Object &parentPtr,
                             const std::string &groupName);
} // namespace HDF5
} // namespace PLI
