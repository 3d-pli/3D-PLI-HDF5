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
class Group {
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
  explicit Group(const hid_t groupPtr) noexcept;
  /**
   * @brief Opens a group
   *
   * Open an existing group. If the group does not exist, an exception is
   * thrown.
   * @param parentPtr Pointer to the parent group or file.
   * @param groupName Name of the group to open.
   * @throws PLI::HDF5::Exceptions::GroupNotFoundException If the group does not
   * exist.
   * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If opening the
   * group fails or the parentPtr is invalid.
   */
  void open(hid_t parentPtr, const std::string& groupName);
  /**
   * @brief Creates a group
   *
   * Create a new group. If the group already exists, an exception is thrown.
   * @param parentPtr Pointer to the parent group or file.
   * @param groupName Name of the group to create.
   * @throws PLI::HDF5::Exceptions::GroupExistsException If the group already
   * exists.
   * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If creating the
   * group fails or the parentPtr is invalid.
   */
  void create(hid_t parentPtr, const std::string& groupName);
  /**
   * @brief Checks if the group exists
   * @param parentPtr Pointer to the parent group or file
   * @param groupName Name of the group to check.
   * @return true If the group exists.
   * @return false If the group does not exist.
   * @throws PLI::HDF5::Exceptions::IdentifierNotValidException If the parentPtr
   * is invalid.
   */
  static bool exists(hid_t parentPtr, const std::string& groupName);

  /**
   * @brief Closes the group if it is valid.
   *
   * If the dataset is valid, it is closed. The group pointer is then set to
   * an invalid value (-1) to ensure, that calls will result in an exception
   * afterwards.
   * @throws PLI::HDF5::Exceptions::HDF5RuntimeException If the group could
   * not be closed.
   */
  void close();
  /**
   * @brief Get the raw HDF5 pointer of the group.
   *
   * Returns the raw HDF5 pointer of the group. This pointer can be used to
   * access the group using the HDF5 library.
   * @return hid_t Group ID stored in the object.
   */
  hid_t id() const noexcept;
  /**
   * @brief Convert the group to a the raw HDF5 pointer.
   * @return hid_t Group ID stored in the object.
   */
  operator hid_t() const noexcept;

 private:
  hid_t m_id;
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
PLI::HDF5::Group openGroup(hid_t parentPtr, const std::string& groupName);
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
PLI::HDF5::Group createGroup(hid_t parentPtr, const std::string& groupName);
}  // namespace HDF5
}  // namespace PLI
